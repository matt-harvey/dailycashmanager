#ifndef GUARD_table_hpp
#define GUARD_table_hpp

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/output_aux.hpp>
#include <cassert>
#include <iostream>
#include <ostream>
#include <sstream>
#include <list>
#include <string>
#include <vector>


namespace consolixx
{

namespace alignment
{
	enum Flag
	{
		left,
		right
	};
}  // namespace alignment

/**
 * Class representing a printable table showing data for items of
 * type T.
 */
template <typename T>
class Table
{
public:
	typedef std::vector<std::string> Row;
	typedef boost::shared_ptr<Row> RowPtr;
	typedef boost::function< RowPtr(T const&) > MakeRow;
	typedef std::list<RowPtr> RowList;

	/**
	 * Iter must be such that when dereferenced it produces an instance of T
	 * (or a reference to such an instance).
	 *
	 * Construct a table from instances of T delimited by \e p_beg and \e
	 * p_end,
	 * applying \e p_make_row to each instance to construct each Row
	 * with p_headings being the headings for each column, and with
	 * a minimum of \e p_padding spaces between adjacent columns when printed.
	 *
	 * p_alignments indicates for each row whether it should be aligned to the
	 * left or the right.
	 *
	 * Preconditions:\e
	 * The number of elements in each Row returned by p_make_row must be equal
	 * to the number of elements in p_headings, and must also be equal to
	 * the number of elements in p_alignments; or undefined behaviour will
	 * result.
	 */
	template <typename Iter>
	Table
	(	Iter p_beg,
		Iter p_end,
		MakeRow p_make_row,
		Row const& p_headings,
		std::vector<alignment::Flag> const& p_alignments,
		Row::size_type p_padding = 1
	);

	template <typename S>
	friend
	void output_table_aux(std::ostream& os, Table<S> const& table);

private:
	boost::scoped_ptr<RowList> m_data;
	bool m_has_headings;
	std::vector<alignment::Flag> m_alignments;
	std::vector<std::string::size_type> m_widths;
	Row::size_type m_padding;
	Row::size_type m_columns;
};


template <typename T>
std::ostream&
operator<<(std::ostream& os, Table<T> const& table);



template <typename T>
template <typename Iter>
Table<T>::Table
(	Iter p_beg,
	Iter p_end,
	MakeRow p_make_row,
	Row const& p_headings,
	std::vector<alignment::Flag> const& p_alignments,
	Row::size_type p_padding
):
	m_data(new RowList),
	m_has_headings(false),
	m_alignments(p_alignments),
	m_padding(p_padding),
	m_columns(p_headings.size())
{
	m_widths = std::vector<std::string::size_type>(m_columns, 0);
	for (Row::size_type i = 0; i != m_columns; ++i)
	{
		std::string::size_type const heading_size =
			p_headings[i].size();
		if (heading_size != 0)
		{
			assert (heading_size > 0);
			m_has_headings = true;
		}
		m_widths[i] = heading_size;
	}
	if (m_has_headings)
	{
		RowPtr headings_ptr(new Row(p_headings));
		m_data->push_back(headings_ptr);
		RowPtr blank_row_ptr(new Row(m_columns, ""));
		m_data->push_back(blank_row_ptr);
	}
	for (Iter it = p_beg; it != p_end; ++it)
	{
		RowPtr row_ptr = p_make_row(*it);
		std::vector<std::string::size_type>::iterator
			width_iter = m_widths.begin();
		for
		(	Row::const_iterator jt = row_ptr->begin(),
				row_end = row_ptr->end();
			jt != row_end;
			++jt, ++width_iter
		)
		{
			*width_iter = (jt->size() > *width_iter? jt->size(): *width_iter);
		}
		m_data->push_back(row_ptr);
	}
}


template <typename S>
void
output_table_aux(std::ostream& os, Table<S> const& table)
{
	// TODO Factor out the repeated code here
	typedef Table<S> Table;
	typedef typename Table::Row Row;
	typedef typename Table::RowList RowList;
	using std::string;
	for
	(	typename RowList::const_iterator it = table.m_data->begin(),
			end = table.m_data->end();
		it != end;
		++it
	)
	{
		Row const& row = **it;
		for (typename Row::size_type j = 0; j != table.m_columns; ++j)
		{
			string const padder
			(	table.m_widths[j] + table.m_padding - row[j].size(),
				' '
			);
			switch (table.m_alignments[j])
			{
			case alignment::left:
				os << row[j] << padder;
				break;		
			case alignment::right:
				os << padder << row[j];
				break;
			default:
				assert (false);  // Execution should never reach here
			}
		}
		os << std::endl;
	}
	return;
}

template <typename S>
std::ostream&
operator<<(std::ostream& os, Table<S> const& table)
{
	jewel::output_aux(os, table, output_table_aux);
	return os;
}


}  // namespace consolixx		



#endif  // GUARD_table_hpp
