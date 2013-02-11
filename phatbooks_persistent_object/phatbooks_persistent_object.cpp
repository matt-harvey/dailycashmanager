#include "phatbooks_persistent_object.hpp"
#include "phatbooks_database_connection.hpp"

namespace phatbooks
{

void
PhatbooksPersistentObjectBase::save()
{
	do_save();
	return;
}

PhatbooksPersistentObjectBase::Id
PhatbooksPersistentObjectBase::id() const
{
	return do_get_id();
}

PhatbooksDatabaseConnection&
PhatbooksPersistentObjectBase::database_connection() const
{
	return do_get_database_connection();
}

bool
PhatbooksPersistentObjectBase::has_id() const
{
	return does_have_id();
}

void
PhatbooksPersistentObjectBase::remove()
{
	do_remove();
	return;
}

void
PhatbooksPersistentObjectBase::ghostify()
{
	do_ghostify();
	return;
}





}  // namespace phatbooks
