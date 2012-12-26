#include "text_session.hpp"
#include "text_session.hpp"
#include "consolixx_exceptions.hpp"
#include "get_input.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
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


			


TextSession::~TextSession()
{
}


void
TextSession::Menu::add_item(shared_ptr<MenuItem const> const& item)
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


TextSession::Menu::Menu(string const& p_prompt):
	m_selection_record(1),
	m_prompt(p_prompt)
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
			cout << "\n" << m_prompt;
			string input = get_user_input();
			cout << endl;

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


shared_ptr<TextSession::MenuItem>
TextSession::MenuItem::provide_menu_exit(string const& message)
{
	shared_ptr<MenuItem> const ret
	(	new MenuItem
		(	message,
			do_nothing,
			false,
			"x"
		)
	);
	return ret;
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


