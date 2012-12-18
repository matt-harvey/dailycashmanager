#ifndef GUARD_table_hpp
#define GUARD_table_hpp


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
class Table
{
public:
	typedef std::vector<std::string> Row;
	typedef boost::shared_ptr<Row> RowPtr;
	typedef boost::function< RowPtr(T const&) > MakeRow;
	typedef std::list<RowPtr> RowList;

	/**
	 * Iter must be such that when dereferenced it produces an instance of T.
	 *
	 * Construct a table from instances of T delimited by \e p_beg and \e
	 * p_end,
	 * use applying \e p_make_row to each to construct each Row
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
	template <typename T, typename Iter>
	Table
	(	Iter p_beg,
		Iter p_end,
		boost::function< RowPtr(T const&) > p_make_row,
		Row const& p_headings,
		std::vector<alignment::Flag> p_alignments,
		Row::size_type p_padding = 1
	);

	std::ostream& print_aux(std::ostream& os);

private:
	boost::scoped_ptr<RowList> m_data;
	Row m_headings;
	std::vector<alignment::Flag> m_alignments;
	std::vector<std::string::size_type> m_widths;
	Row::size_type m_padding;
};



template <typename T, typename Iter>
Table::Table
(	Iter p_beg,
	Iter p_end,
	MakeRow p_make_row,
	Row const& p_headings,
	std::vector<alignment::Flag> const& p_alignments;
	Row::size_type p_padding
):
	m_data(new RowList),
	m_headings(p_headings),
	m_alignments(p_alignments),
	m_padding(p_padding)
{
	Row::size_type const columns = m_headings.size();
	m_widths = std::vector(columns, 0);
	for (Row::size_type i = 0; i != columns; ++i)
	{
		m_widths[i] = m_headings[i].size();
	}
	for (Iter it = p_beg; it != p_end; ++it)
	{
		RowPtr row_ptr = p_make_row(*it);
		Row::const_iterator row_end = row_ptr->end();
		for
		(	Row::const_iterator jt = row_ptr->begin(),
				Row::const_iterator row_end = row_ptr->end(),
				std::vector<std::string::size_type>::const_iterator
					width_iter = m_widths.begin();
			jt != row_end;
			++jt
		)
		{
			*width_iter = (jt->size() > width_iter? jt->size(): width_iter);
		}
		m_data->push_back(row_ptr);
	}
}


template <typename T>




			
			



	




#endif  // GUARD_table_hpp
