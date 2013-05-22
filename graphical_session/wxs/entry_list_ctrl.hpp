// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_list_ctrl_hpp
#define GUARD_entry_list_ctrl_hpp


#include "entry_reader.hpp"
#include <wx/listctrl.h>
#include <wx/wx.h>

namespace phatbooks
{

class PhatbooksDatabaseConnection;

namespace gui
{

class EntryListCtrl:
	public wxListCtrl
{
public:
		
	/**
	 * @returns a pointer to a heap-allocated EntryListCtrl, listing
	 * all and only the \e actual (non-budget) OrdinaryEntry's stored in \e
	 * dbc. The client does not need to take care of the memory - the memory
	 * is taken care of by the parent window.
	 */
	static EntryListCtrl* create_actual_ordinary_entry_list
	(	wxWindow* parent,
		PhatbooksDatabaseConnection& dbc
	);

private:
	
	EntryListCtrl
	(	wxWindow* p_parent,
		EntryReader const& p_reader,
		PhatbooksDatabaseConnection& p_database_connection
	);
	PhatbooksDatabaseConnection& m_database_connection;
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_ctrl_hpp
