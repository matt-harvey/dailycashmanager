#ifndef consolixx_exceptions_hpp
#define consolixx_exceptions_hpp

/** \file consolixx_exceptions.hpp
 *
 * \brief Header file containing declarations and definitions
 * of exception classes specific to Consolixx (a text user interface module). 
 *
 * \author Matthew Harvey
 * \date 13 August 2012.
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

/**
 * Exception to be thrown when a Menu object is queried for some previous
 * user choice, but there is no history stored in the Menu (generally because
 * the user has not yet made any choices from that Menu).
 */
JEWEL_DERIVED_EXCEPTION(NoMenuHistoryException, TextUserInterfaceException);


}  // namespace consolixx

#endif  // consolixx_exceptions.hpp
