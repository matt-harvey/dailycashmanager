#ifndef GUARD_reader_hpp
#define GUARD_reader_hpp

#include "database_connection.hpp"
#include "general_typedefs.hpp"
#include "handle.hpp"
#include "sqloxx_exceptions.hpp"
#include "sql_statement.hpp"
#include <boost/noncopyable.hpp>
#include <string>

namespace sqloxx
{

/**
 * Class template each instantiation of which represents a class
 * of iterator-like "reader" objects that wrap an SQL "select" statement,
 * where the field being selected is the primary key field for type T as
 * it is stored in the database. By calling the value() method of
 * a Reader, a Handle<T> instance is returned. Because the Reader selects
 * from a table that is physically in the database, the implementation
 * of Reader can use the fast, unchecked_get_handle(...) function to
 * get the Handle, in the knowledge that the Id is valid as it has
 * just been read from the physical database.
 * Reader classes also provide a higher-level, cleaner interface for
 * client code wishing to traverse a database table without having to
 * use SQL directly.
 * Each instantiation of Reader represents wraps a particular "select"
 * statement.
 *
 * @todo Documentation and testing.
 */
template <typename T, typename Connection>
class Reader:
	private boost::noncopyable
{
public:

	Reader
	(	Connection& p_database_connection,
		std::string const& p_selector =
		(	"select " + T::primary_key_name() +
			" from " + T::primary_table_name()
		)
	);
	
	bool read();
	
	Handle<T> value() const;

private:
	Connection& m_database_connection;
	SQLStatement mutable m_statement;
	bool m_is_valid;

};  // Reader


template <typename T, typename Connection>
Reader<T, Connection>::Reader
(	Connection& p_database_connection,
	std::string const& p_selector
):
	m_database_connection(p_database_connection),
	m_statement(p_database_connection, p_selector),
	m_is_valid(true)
{
}

template <typename T, typename Connection>
inline
bool
Reader<T, Connection>::read()
{
	// Assignment here is intentional
	return m_is_valid = m_statement.step();
}

template <typename T, typename Connection>
Handle<T>
Reader<T, Connection>::value() const
{
	if (m_is_valid)
	{
		// Warning Id used here not T::Id. Can we
		// compile with the latter?
		return unchecked_get_handle<T>
		(	m_database_connection,
			m_statement.extract<Id>(0)
		);
	}
	assert (!m_is_valid);
	throw InvalidReader
	(	"Reader is beyond the final result row."
	);
}

	

}  // namespace sqloxx


#endif  // GUARD_reader_hpp
