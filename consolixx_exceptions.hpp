#ifndef consolixx_exceptions_hpp
#define consolixx_exceptions_hpp

/** \file consolixx_exceptions.hpp
 *
 * \brief Header file containing declarations and definitions
 * of exception classes specific to Consolixx (a text user interface module). 
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include <jewel/exception.hpp>

namespace consolixx
{


/**
 * Base exception class for exceptions involving textual / console-based
 * user interfaces.
 */
JEWEL_DERIVED_EXCEPTION(TextUserInterfaceException, jewel::Exception);

/**
 * Exception to be thrown in contexts involving the "banner" (user-facing 
 * description) of menu items.
 */
JEWEL_DERIVED_EXCEPTION(MenuItemBannerException, TextUserInterfaceException);

/**
 * Exception to be thrown in contexts involving the "label" (brief
 * alphanumeric label entered by user to select the item) of menu items.
 */
JEWEL_DERIVED_EXCEPTION(MenuItemLabelException, TextUserInterfaceException);



}  // namespace consolixx

#endif  // consolixx_exceptions.hpp
