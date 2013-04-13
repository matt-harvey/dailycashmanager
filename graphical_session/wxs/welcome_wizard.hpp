#ifndef GUARD_welcome_wizard_hpp
#define GUARD_welcome_wizard_hpp

#include <wx/wizard.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

// Forward declaration
class Frame;


/**
 * Represents the wizard that is presented to GUI users when they first
 * start up the application.
 */
class WelcomeWizard:
	public wxWizard
{
public:
	
	WelcomeWizard(Frame* parent);

private:
	
	class Page: public wxWizardPageSimple
	{
	};


};  // WelcomeWizard

}  // namespace gui
}  // namesapce phatbooks


#endif  // GUARD_welcome_wizard_hpp
