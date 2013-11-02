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

// TODO HIGH PRIORITY Under KDE (at least on Mageia) the way I have set
// up the widths of wxComboBox and wxTextCtrl and wxButton (in various
// controls in which these feature), where they are
// supposed to be the same height, they actually turn out to be slightly
// different heights. However even if I manually set them all to the same
// hard-coded height number, they still seem to come out different heights
// on KDE. It doesn't make a lot of sense.

// TODO HIGH PRIORITY Go through all the classes inheriting from
// sqloxx::PersistentObject<...> and ensure the do_save(...) functions in each
// are atomic with respect to the in-memory objects (noting that Sqloxx
// already ensures atomicity with respect to the database), and that they
// conform to the restrictions detailed in the sqloxx::PersistentObject<...>
// API documentation. (Note I have already done this for \e do_load(...)
// functions.)

// TODO MEDIUM PRIORITY Tooltips aren't showing on Windows.

// TODO HIGH PRIORITY Make the installer create an association on the user's
// system between the Phatbooks file extension and the Phatbooks application.
// See CMake book, page. 162.

/// TODO MEDIUM PRIORITY The database file should perhaps have a checksum to
// guard against its contents changing other than via the application.

// TODO HIGH PRIORITY Facilitate automatic checking for updates from user's
// machine, or at least provide an easy process for installing updates
// via NSIS. It appears that the default configuration of CPack/NSIS is
// such that updates will not overwrite existing files. Some manual NSIS
// scripting may be required to enable this. Also take into account that
// the user may have to restart their computer in the event that they have
// files open while the installer (or "updater") is running (although I
// \e think that the default configuration under CPack does this
// automatically).

// TODO HIGH PRIORITY Create the licence. Needs to go both in LICENSE.txt and
// into the NSIS installer (can configure CMakeLists.txt to get the licence text
// from LICENSE.txt). Take care when creating the license, especially re.
// the third party libraries used in the project.

// TODO HIGH PRIORITY Create a decent icon for the application. We want this
// in both .ico form (for Windows executable icon) and .xpm
// form (for icon for Frame). Note, when I exported my
// "token icon" using GIMP to .ico format, and later used this
// to create a double-clickable icon in Windows, only the text
// on the icon appeared, and the background image became
// transparent for some reason. Furthermore, when set as the
// large in-windows icon in the CPack/NSIS installer, the icon
// wasn't showing at all. Once decent icon is created, also make sure it is
// pulled into the wxBitmap in SetupWizard.

// TODO HIGH PRIORITY Create a better name for the application.

// TODO HIGH PRIORITY Set the version number in a single location and find a
// way to ensure this is reflected consistently everywhere it appears
// (website, installer, licence text etc.).

// TODO HIGH PRIORITY We need a proper solution to the potential for integer
// overflow. Mostly we use jewel::Decimal arithmetic - which will throw if
// unsafe - but we're not actually handling these exceptions for the user. The
// program would just crash. In particular, it may be possible for the user
// to save a BudgetItem which both causes the program to crash, and makes it
// so that they can never open their file again, since the act of updating
// the AmalgamatedBudget causes overflow. (I have already prevented this
// from occurring in the case of saving a PersistentJournal that would cause
// overflow.) The user should be prompted to roll over to a new file, before
// this situation can arise, both in the case of BudgetItem and in the case
// of PersistentJournal. (Also, what about Account opening balance changes
// that might cause overflow? But these are always via the posting of a
// PersistentJournal, so...)

// TODO HIGH PRIORITY Make the GUI display acceptably on smaller screen
// i.e. laptop.

// TODO HIGH PRIORITY Make it so that if the program crashes, a dialog is
// displayed which
// preferably allows the user to email the log file and an optional
// user-supplied message to "help desk" with a single mouse click from
// the window itself; or, if not, at least tells the user
// where the log file is and gets recommends they email a particular
// email address with the log file attached. An alternative to email
// is just to use HTTP to send the logfile directly to a server (after
// getting the user's permission). Could use the server to format an
// email. Use third party library, wxCrashReport?

// TODO MEDIUM PRIORITY Startup time under Windows is really slow, even when
// compiled in Release mode.


#include "app.hpp"
#include <wx/app.h>

wxIMPLEMENT_APP(phatbooks::App);
