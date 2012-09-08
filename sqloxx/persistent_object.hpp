#ifndef GUARD_persistent_object_hpp
#define GUARD_persistent_object_hpp

#include "database_connection.hpp"
#include <jewel/decimal.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


namespace sqloxx
{

template <typename Id = int>
class PersistentObject
{
public:


	PersistentObject
	(	DatabaseConnection& p_database_connection,
		Id p_id
	);
	
	virtual ~PersistentObject();

	void load();

	void save();

protected:

	DatabaseConnection& database_connection();

	Id id();

private:

	
	virtual void do_load_all() = 0;

	virtual void do_load_partial() = 0;

	virtual void do_save_all() = 0;

	virtual void do_save_partial() = 0;

	enum LoadingStatus
	{
		ghost,
		loading,
		loaded
	};

	// Data members

	LoadingStatus m_loading_status;

	DatabaseConnection& m_database_connection;
	Id m_id;


};


template <typename Id>
inline
PersistentObject<Id>::PersistentObject
(	DatabaseConnection& p_database_connection,
	Id p_id
):
	m_database_connection(p_database_connection),
	m_id(p_id),
	m_loading_status(ghost)
{
	do_load_partial();
}


template <typename Id>
inline
PersistentObject<Id>::~PersistentObject()
{
}


template <typename Id>
inline
void
PersistentObject<Id>::load()
{
	if (m_loading_status == ghost)
	{
		m_loading_status = loading;
		do_load_all();
		m_loading_status = loaded;
	}
	return;
}


template <typename Id>
inline
void
PersistentObject<Id>::save()
{
	start:
	switch (m_loading_status)
	{
	case loaded:
		do_save_all();
		break;
	case ghost:
		do_save_partial();
		break;
	case loading:
		goto start;
		break;
	default:
		throw std::logic_error;
	}
	return;
}


template <typename Id>
inline
DatabaseConnection&
PersistentObject<Id>::database_connection()
{
	return m_database_connection;
}


template <typename Id>
inline
Id
PersistentObject<Id>::id()
{
	return m_id;
}



}  // namespace sqloxx



#endif  // GUARD_persistent_object





