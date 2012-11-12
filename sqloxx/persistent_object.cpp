#include "persistent_object.hpp"
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <stdexcept>
#include <string>

using boost::optional;
using boost::shared_ptr;
using std::logic_error;

namespace sqloxx
{


typedef PersistentObject::Id Id;


PersistentObject::PersistentObject
(	boost::shared_ptr<DatabaseConnection> p_database_connection,
	Id p_id
):
	m_database_connection(p_database_connection),
	m_id(p_id),
	m_loading_status(ghost)
{
}


PersistentObject::PersistentObject
(	boost::shared_ptr<DatabaseConnection> p_database_connection
):
	m_database_connection(p_database_connection),
	m_loading_status(ghost)
{
}


PersistentObject::~PersistentObject()
{
}


void
PersistentObject::load()
{
	if (m_loading_status == ghost && has_id())
	{
		m_loading_status = loading;
		try
		{
			m_database_connection->begin_transaction();
		}
		catch (TransactionNestingException&)
		{
			clear_loading_status();
			throw;
		}
		catch (InvalidConnection&)
		{
			clear_loading_status();
			throw;
		}
		try
		{
			do_load_all();
		}
		catch (std::exception&)
		{
			clear_loading_status();
			throw;
		}
		try
		{
			m_database_connection->end_transaction();
			// Note this can't possibly throw TransactionNestingException
			// here, unless do_load_all() has done something perverse.
		}
		catch (InvalidConnection&)
		{
			// As do_load_all has already completed, the object in
			// memory should be non-corrupt and fully loaded. The fact that
			// the database connection is now invalid only affects the
			// database, not the in-memory object. The invalidity of the
			// database connection will presumably be detected and dealt with
			// the next time it is accessed. We therefore do NOT rethrow
			// here.
			//
			// WARNING Am I really comfortable with this?
		}
		m_loading_status = loaded;
	}
	return;
}


void
PersistentObject::save_existing()
{
	if (!has_id())
	{
		throw logic_error
		(	"Method save_existing() called on an instance of PersistentObject"
			" that does not correspond with an existing database record."
		);
	}
	start:
	switch (m_loading_status)
	{
	case loaded:
		m_database_connection->begin_transaction();
		do_save_existing_all();
		m_database_connection->end_transaction();
		break;
	case loading:
		goto start;  // WARNING This sucks
		assert (false);  // Execution never reaches here.
	case ghost:
		throw IncompleteObjectException
		(	"Attempted to re-save already-persisted object in incomplete "
			"state."
		);
		assert (false);  // Execution never reaches here.
	default:
		assert (false);  // Execution never reaches here.
	}
	return;
}


Id
PersistentObject::prospective_key() const
{
	if (has_id())
	{
		throw std::logic_error
		(	"Object already has id so prospective_key does not apply."
		);
	}
	return do_calculate_prospective_key();
}


Id
PersistentObject::do_calculate_prospective_key() const
{	
	return database_connection()->next_auto_key<Id>
	(	do_get_table_name()
	);
}


void
PersistentObject::save_new()
{
	m_database_connection->begin_transaction();
	Id key = prospective_key();
	do_save_new_all();
	m_database_connection->end_transaction();
	set_id(key);
	return;
}


boost::shared_ptr<DatabaseConnection>
PersistentObject::database_connection() const
{
	return m_database_connection;
}


Id
PersistentObject::id() const
{
	return jewel::value(m_id);
}

void
PersistentObject::set_id(Id p_id)
{
	if (has_id())
	{
		throw logic_error("Object already has id.");
	}
	assert (!has_id());
	m_id = p_id;
	return;
}


bool
PersistentObject::has_id() const
{
	// Relies on the fact that m_id is a boost::optional<Id>, and
	// will convert to true if and only if it has been initialized.
	return m_id;
}


void
PersistentObject::clear_loading_status()
{
	m_loading_status = ghost;
	return;
}


PersistentObject::PersistentObject(PersistentObject const& rhs):
	m_database_connection(rhs.m_database_connection),
	m_id(rhs.m_id),
	m_loading_status(rhs.m_loading_status)
{
}
		

void
PersistentObject::swap_base_internals(PersistentObject& rhs)
{
	shared_ptr<DatabaseConnection> temp_dbc = rhs.m_database_connection;
	optional<Id> temp_id = rhs.m_id;
	LoadingStatus temp_loading_status = rhs.m_loading_status;

	rhs.m_database_connection = m_database_connection;
	rhs.m_id = m_id;
	rhs.m_loading_status = m_loading_status;

	m_database_connection = temp_dbc;
	m_id = temp_id;
	m_loading_status = temp_loading_status;

	return;
}




}  // namespace sqloxx
