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
 * Each business class B should inherit publically from
 * from PhatbooksPersistentObject<B, Impl>.
 * The PhatbooksPersistentObject template should be instantiated with
 * a specific
 * implementation class for Impl,
 * which should in turn inherit from
 * sqloxx::PersistentObject<PhatbooksDatabaseConnection, Impl>.
 * 
 * Note there is a reason why PhatbooksPersistentObjectBase and
 * PhatbooksPersistentObject are two separate classes, namely,
 * to support the PersistentJournal class as an abstract class
 * between Journal and DraftJournal/OrdinaryJournal in the hierarchy.
 * This architecture makes this possible. In this case, PersistentJournal
 * inherits from PhatbooksPersistentObjectBase, but not from
 * PhatbooksPersistentObject<...>, as there is no instantiation of
 * the latteer from which it could inherit.
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
	// PhatbooksPersistentObject<...> and should \e not be redefined
	// by the business classes.
	virtual void do_save() = 0;
	virtual Id do_get_id() const = 0;
	virtual PhatbooksDatabaseConnection& do_get_database_connection() const=0;
	virtual bool does_have_id() const = 0;
	virtual void do_remove() = 0;
	virtual void do_ghostify() = 0;
};



template <typename Impl>
class PhatbooksPersistentObject:
	virtual public PhatbooksPersistentObjectBase
{
public:
	bool operator==(PhatbooksPersistentObject const& rhs) const;
	bool operator!=(PhatbooksPersistentObject const& rhs) const;
	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);
	static bool none_saved
	(	PhatbooksDatabaseConnection& p_database_connection
	);

protected:
	PhatbooksPersistentObject
	(	PhatbooksDatabaseConnection& p_database_connection
	);
	PhatbooksPersistentObject
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);
	PhatbooksPersistentObject(sqloxx::Handle<Impl> const& p_handle);
	Impl& impl();
	Impl& impl() const;

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
PhatbooksPersistentObject<Impl>::PhatbooksPersistentObject
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl(sqloxx::Handle<Impl>(p_database_connection))
{
}

template <typename Impl>
PhatbooksPersistentObject<Impl>::PhatbooksPersistentObject
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl(sqloxx::Handle<Impl>(p_database_connection, p_id))
{
}

template <typename Impl>
inline
PhatbooksPersistentObject<Impl>::PhatbooksPersistentObject
(	sqloxx::Handle<Impl> const& p_handle
):
	m_impl(p_handle)
{
}


template <typename Impl>
inline
bool
PhatbooksPersistentObject<Impl>::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Impl::exists(p_database_connection, p_id);
}

template <typename Impl>
inline
bool
PhatbooksPersistentObject<Impl>::none_saved
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return Impl::none_saved(p_database_connection);
}

template <typename Impl>
inline
Impl&
PhatbooksPersistentObject<Impl>::impl()
{
	return *m_impl;
}

template <typename Impl>
inline
Impl&
PhatbooksPersistentObject<Impl>::impl() const
{
	return *m_impl;
}

template <typename Impl>
inline
bool
PhatbooksPersistentObject<Impl>::operator==
(	PhatbooksPersistentObject<Impl> const& rhs
) const
{
	return m_impl == rhs.m_impl;
}

template <typename Impl>
inline
bool
PhatbooksPersistentObject<Impl>::operator!=
(	PhatbooksPersistentObject<Impl> const& rhs
) const
{
	return m_impl != rhs.m_impl;
}

template <typename Impl>
void
PhatbooksPersistentObject<Impl>::do_save()
{
	m_impl->save();
	return;
}

template <typename Impl>
typename PhatbooksPersistentObject<Impl>::Id
PhatbooksPersistentObject<Impl>::do_get_id() const
{
	return m_impl->id();
}

template <typename Impl>
PhatbooksDatabaseConnection&
PhatbooksPersistentObject<Impl>::do_get_database_connection() const
{
	return m_impl->database_connection();
}

template <typename Impl>
bool
PhatbooksPersistentObject<Impl>::does_have_id() const
{
	return m_impl->has_id();
}

template <typename Impl>
void
PhatbooksPersistentObject<Impl>::do_remove()
{
	m_impl->remove();
	return;
}

template <typename Impl>
void
PhatbooksPersistentObject<Impl>::do_ghostify()
{
	m_impl->ghostify();
	return;
}




}  // namespace phatbooks

#endif  // GUARD_phatbooks_persistent_object_hpp
