/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_gridded_scrolled_panel_hpp_8020584330903638
#define GUARD_gridded_scrolled_panel_hpp_8020584330903638

#include <jewel/decimal_fwd.hpp>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// end forward declarations

/**
 * Represents a panel for which the sizing is controlled by a wxGridBagSizer,
 * for which has scrolling is enabled, and which is associated with a
 * PhatbooksDatabaseConnection. The class will always have
 * phatbooks::gui::standard_gap() incorporated into the sizing scheme
 * (see "sizing.hpp").
 *
 * This is a general purpose widget class which will typically be inherited
 * by more customized widget classes.
 */
class GriddedScrolledPanel: public wxScrolledWindow
{
public:

	/**
	 * Vertical scrolling is always enabled. Horizontal scrolling
	 * is only enabled if \e p_horizontal_scrolling_enabled is passed
	 * \e true.
	 */
	GriddedScrolledPanel
	(	wxWindow* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		bool p_horizontal_scrolling_enabled = false
	);

	GriddedScrolledPanel(GriddedScrolledPanel const&) = delete;
	GriddedScrolledPanel(GriddedScrolledPanel&&) = delete;
	GriddedScrolledPanel& operator=(GriddedScrolledPanel const&) = delete;
	GriddedScrolledPanel& operator=(GriddedScrolledPanel&&) = delete;
	virtual ~GriddedScrolledPanel();

protected:

	wxGridBagSizer& top_sizer();

	/**
	 * An instance of GriddedScrolledPanel will keep track of a
	 * "current row" for purposes of positioning widgets on its sizing
	 * grid.
	 */
	int current_row() const;
	void increment_row();
	void decrement_row();
	void set_row(int p_row);

	/**
	 * Display wxString using a wxStaticText at the position given
	 * by current_row() and \e p_column. This both creates the text
	 * and adds it to the sizer.
	 *
	 * @returns a pointer to the displayed wxStaticText. (As always in
	 * wxWidgets, this points to memory managed by the parent window,
	 * which in this case is the GriddedScrolledPanel.)
	 */
	wxStaticText* display_text
	(	wxString const& p_text,
		int p_column,
		int p_alignment_flags = wxALIGN_LEFT
	);

	/**
	 * Display a jewel::Decimal using a wxStaticText at the position
	 * given by current_row() and \e p_column. This both creates the
	 * text and adds it to the sizer.
	 *
	 * @returns a pointer to the displayed wxStaticText. (As always in
	 * wxWidgets, this points to memory managed by the parent window,
	 * which in this case is the GriddedScrolledPanel.)
	 */
	wxStaticText* display_decimal
	(	jewel::Decimal const& p_decimal,
		int p_column,
		bool p_dash_for_zero = true
	);

	PhatbooksDatabaseConnection& database_connection();
	PhatbooksDatabaseConnection const& database_connection() const;

private:
	
	int m_current_row;
	wxGridBagSizer* m_top_sizer;
	PhatbooksDatabaseConnection& m_database_connection;

};  // class GriddedScrolledPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_gridded_scrolled_panel_hpp_8020584330903638
