#ifndef GUARD_general_typedefs_hpp
#define GUARD_general_typedefs_hpp

/** \file general_typedefs.hpp
 *
 * \brief Header file containing typedefs for use in Phatbooks
 * application code.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "sqloxx/persistent_object.hpp"

namespace phatbooks
{

/**
 * Id is the type used for primary keys in the database, for
 * Phatbooks objects such as Entry and Commodity objects stored in
 * the database.
 *
 * It is a requirement that no rows in the database will have Id(0)
 * in the primary key column.
 */
typedef sqloxx::PersistentObject::Id Id;


}  // namespace phatbooks

#endif  // GUARD_general_typedefs_hpp
