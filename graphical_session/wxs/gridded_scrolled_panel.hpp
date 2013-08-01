#ifndef GUARD_gridded_scrolled_panel_hpp
#define GUARD_gridded_scrolled_panel_hpp

#include <jewel/decimal_fwd.hpp>
#include <boost/noncopyable.hpp>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
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
class GriddedScrolledPanel:
	public wxScrolledWindow,
	private boost::noncopyable
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

	/**
	 * Display wxString using a wxStaticText at the position given
	 * by current_row() and \e p_column.
	 */
	void display_text
	(	wxString const& p_text,
		int p_column,
		int p_alignment_flags = wxALIGN_LEFT
	);

	/**
	 * Display a jewel::Decimal using a wxStaticText at the position
	 * given by current_row() and \e p_column.
	 */
	void display_decimal
	(	jewel::Decimal const& p_decimal,
		int p_column
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

#endif  // GUARD_gridded_scrolled_panel_hpp
