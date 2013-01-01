#ifndef GUARD_phatbooks_persistent_object_hpp
#define GUARD_phatbooks_persistent_object_hpp

#include "phatbooks_database_connection.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>

/**
 * @file phatbooks_database_connection.hpp
 *
 * Contains base classes for "business classes" in phatbooks,
 * where these business
 * classes share an interface in involving their persistence to a database,
 * and share implementation details in that they are all wrappers
 * around implementation classes inherited from
 * sqloxx::PersistentObject<PhatbooksDatabaseConnection, ...>.
 *
 * Each business class B should inherit \e virtually and publically from
 * PhatbooksPersistentObjectBase (for the interface),
 * and \e privately from PhatbooksPersistentObjectDetail<B, Impl>
 * (for the implementation).
 * The PhatbooksPersistentObjectDetail template should be instantiated with
 * a specific
 * implementation class for Impl,
 * which should in turn inherit from
 * sqloxx::PersistentObject<PhatbooksDatabaseConnection, Impl>.
 *
 * Note there is a reason these are two separate classes, namely,
 * to support the PersistentJournal class as an abstract class
 * between Journal and DraftJournal/OrdinaryJournal in the hierarchy.
 * This architecture makes this possible.
 */

namespace phatbooks
{

class PhatbooksPersistentObjectBase
{
public:
	typedef sqloxx::Id Id;

	void save();
	Id id() const;
	PhatbooksDatabaseConnection& database_connection() const;
	bool has_id() const;
	void remove();
	void ghostify();

private:
	// Note the implementations for these are provided by
	// PhatbooksPersistentObjectDetail<...> and should \e not be redefined
	// by the business classes.
	virtual
	void do_save() = 0;

	virtual
	Id do_get_id() const = 0;

	virtual
	PhatbooksDatabaseConnection& do_get_database_connection() const = 0;

	virtual
	bool does_have_id() const = 0;

	virtual
	void do_remove() = 0;
	
	virtual
	void do_ghostify() = 0;
};



template <typename Impl>
class PhatbooksPersistentObjectDetail:
	virtual private PhatbooksPersistentObjectBase
{
protected:

	typedef sqloxx::Id Id;
	PhatbooksPersistentObjectDetail
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	PhatbooksPersistentObjectDetail
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);


	Impl& impl()
	{
		return *m_impl;
	}
	Impl& impl() const
	{
		return *m_impl;
	}
	PhatbooksPersistentObjectDetail(sqloxx::Handle<Impl> const& p_handle):
		m_impl(p_handle)
	{
	}

private:

	void do_save();
	Id do_get_id() const;
	PhatbooksDatabaseConnection& do_get_database_connection() const;
	bool does_have_id() const;
	void do_remove();
	void do_ghostify();
	sqloxx::Handle<Impl> m_impl;
};


template <typename Impl>
PhatbooksPersistentObjectDetail<Impl>::PhatbooksPersistentObjectDetail
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl(sqloxx::Handle<Impl>(p_database_connection))
{
}

template <typename Impl>
PhatbooksPersistentObjectDetail<Impl>::PhatbooksPersistentObjectDetail
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl(sqloxx::Handle<Impl>(p_database_connection, p_id))
{
}

template <typename Impl>
void
PhatbooksPersistentObjectDetail<Impl>::do_save()
{
	m_impl->save();
	return;
}

template <typename Impl>
typename sqloxx::Id
PhatbooksPersistentObjectDetail<Impl>::do_get_id() const
{
	return m_impl->id();
}

template <typename Impl>
PhatbooksDatabaseConnection&
PhatbooksPersistentObjectDetail<Impl>::do_get_database_connection() const
{
	return m_impl->database_connection();
}

template <typename Impl>
bool
PhatbooksPersistentObjectDetail<Impl>::does_have_id() const
{
	return m_impl->has_id();
}

template <typename Impl>
void
PhatbooksPersistentObjectDetail<Impl>::do_remove()
{
	m_impl->remove();
	return;
}

template <typename Impl>
void
PhatbooksPersistentObjectDetail<Impl>::do_ghostify()
{
	m_impl->ghostify();
	return;
}










}  // namespace phatbooks

#endif  // GUARD_phatbooks_persistent_object_hpp
