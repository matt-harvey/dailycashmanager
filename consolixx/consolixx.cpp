
/** \file consolixx.cpp
 *
 * \brief Source file for \c consolixx - tools for creating
 * text\console based user interfaces
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "consolixx.hpp"
#include "consolixx_exceptions.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/function.hpp>
#include <boost/exception/all.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <cassert>
#include <istream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using jewel::Decimal;
using jewel::DecimalFromStringException;
using jewel::DecimalRangeException;
using boost::bind;
using boost::function;
using boost::lexical_cast;
using boost::regex;
using boost::regex_match;
using boost::shared_ptr;
using std::cin;
using std::cout;
using std::istream;
using std::endl;
using std::getline;
using std::max;
using std::ostream;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace consolixx
{


string
get_user_input
(	string const& error_message,
	istream& is,
	ostream& os
)
{
	string input;
	while (!getline(is, input))
	{
		assert (!is);
		is.clear();
		assert (is);
		os << error_message;
		os.flush();
	}
	return input;
}


string
get_constrained_user_input
(	function< bool(string const&) > criterion,
	string const& message_on_invalid_input,
	bool user_can_escape,
	function< bool(string const&) > escape_criterion,
	string const& escape_message,
	string const& return_string_on_escape,
	string const& message_on_unsuccessful_read,
	istream& is,
	ostream& os
)
{
	do
	{
		string input = get_user_input(message_on_unsuccessful_read, is, os);
		if (user_can_escape && escape_criterion(input))
		{
			os << escape_message;
			os.flush();
			return return_string_on_escape;
		}
		if (criterion(input))
		{
			return input;
		}
		assert (!criterion(input));
		os << message_on_invalid_input;
		os.flush();
	}
	while (true);
}


Decimal get_decimal_from_user()
{
	Decimal ret("0");
	for (bool input_is_valid = false; !input_is_valid; )
	{
		try
		{
			ret = Decimal(get_user_input());
			input_is_valid = true;
		}
		catch (DecimalFromStringException&)
		{
			assert (!input_is_valid);
			cout << "Please try again, entering a decimal number, "
			     << "(e.g. \"1.343\"): ";
		}
		catch (DecimalRangeException&)
		{
			assert (!input_is_valid);
			cout << "The number you entered is too large or too small to be "
			     << "safely handled. Please try again: ";
		}
	}
	return ret;
}

boost::gregorian::date
get_date_from_user(string const& error_prompt)
{
	boost::gregorian::date ret = boost::gregorian::day_clock::local_day();
	regex const validation_pattern
	(	"^[0-9][0-9][0-9][0-9][0-1][0-9][0-3][0-9]$"
	);
	while (true)
	{
		string input = get_user_input();
		if (input.empty())
		{
			break;
		}
		if (regex_match(input, validation_pattern))
		{
			try
			{
				ret = boost::gregorian::date_from_iso_string(input);
				break;
			}
			catch (boost::exception&)
			{
			}
		}
		cout << error_prompt;
	}
	return ret;
}
		
			


TextSession::~TextSession()
{
}


void
TextSession::Menu::add_item(shared_ptr<MenuItem const> item)
{
	for
	(	ItemContainer::const_iterator it = m_items.begin();
		it != m_items.end();
		++it
	)
	{
		if (item->banner() == (*it)->banner())
		{
			throw MenuItemBannerException
			(	"MenuItem with this banner already in Menu."
			);
		}
		if
		(	item->has_special_label() && (*it)->has_special_label() &&
			item->special_label() == (*it)->special_label()
		)
		{
			throw MenuItemLabelException
			(	"MenuItem with this special label already in Menu."
			);
		}
	}
	m_items.push_back(item);
	return;
}


TextSession::Menu::Menu():
	m_selection_record(1)
{
}


boost::shared_ptr<TextSession::MenuItem const>
TextSession::Menu::last_choice() const
{
	if (m_selection_record.empty())
	{
		throw NoMenuHistoryException
		(	"User has yet to make any selection from this Menu."
		);
	}
	assert (!m_selection_record.empty());
	// Do this rather than going straight to begin(), so that this
	// won't rely on m_last_choice_ctr having a capacity of 1.
	History::const_iterator it = m_selection_record.end();
	return *(--it);
}





void
TextSession::Menu::present_to_user()
{
	typedef vector<string>::size_type vec_sz;
	bool replay_menu = false;

	do
	{
		// Determine how to label each menu item
		int item_num = 1;
		string::size_type max_label_length = 0;
		vector<string> label_vec;
		ItemContainer::iterator it;
		for (it = m_items.begin(); it != m_items.end(); ++it)
		{
			if ((*it)->has_special_label())
			{
				label_vec.push_back((*it)->special_label());
			}
			else
			{
				label_vec.push_back(lexical_cast<string>(item_num));
				++item_num;
			}
			max_label_length = max(label_vec.back().size(), max_label_length);
		}

		// Print the menu
		cout << endl;
		it = m_items.begin();
		for (vec_sz i = 0; i != label_vec.size(); ++i, ++it)
		{
			assert (it < m_items.end());
			cout << label_vec[i]
				 << string(max_label_length + 1 - label_vec[i].size(), ' ')
				 << (*it)->banner()
				 << endl;
		}

		// Receive and process user input
		for (bool successful = false; !successful; )
		{
			cout << "\nEnter an option from the above menu: ";
			string input = get_user_input();

			// See whether input corresponds to any of the item labels,
			// and invoke the item if it does.
			// This simple linear search is fast enough for all but
			// ridiculously large user menus.
			it = m_items.begin();
			for (vec_sz i = 0; i != label_vec.size(); ++i, ++it)
			{
				assert (it < m_items.end());
				if (input == label_vec[i])
				{
					m_selection_record.push_back(*it);  // Remember choice
					(*it)->invoke();
					replay_menu = (*it)->repeats_menu();
					successful = true;
					break;
				}
			}
			if (!successful) cout << endl << "Please try again.";
		}
	}
	while (replay_menu);

	return;
}



TextSession::MenuItem::MenuItem
(	string const& p_banner,
	MenuItem::CallbackType p_callback,
	bool p_repeats_menu,
	string const& p_special_label
):
	m_banner(string()),
	m_callback(p_callback),
	m_repeats_menu(p_repeats_menu),
	m_special_label(string())
{
	if (p_banner.empty())
	{
		throw MenuItemBannerException("Menu item banner is empty.");
	}
	assert (p_banner != "");
	m_banner = p_banner;
	if (!p_special_label.empty())
	{
		if (p_special_label.find_first_not_of("0123456789") == string::npos)
		{
			// The special label is all digits
			throw MenuItemLabelException
			(	"Special label cannot contain only digits."
			);
		}
		m_special_label = p_special_label;
	}
}


string
TextSession::MenuItem::banner() const
{
	return m_banner;
}


string
TextSession::MenuItem::special_label() const
{
	if (!has_special_label())
	{
		throw MenuItemLabelException("Item does not have special label.");
	}
	assert (has_special_label());
	return m_special_label;
}

bool
TextSession::MenuItem::repeats_menu() const
{
	return m_repeats_menu;
}

bool
TextSession::MenuItem::has_special_label() const
{
	return !m_special_label.empty();
}

void
TextSession::MenuItem::invoke() const
{
	m_callback();
	return;
}


}  // namespace consolixx

