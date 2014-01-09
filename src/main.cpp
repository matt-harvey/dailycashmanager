/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// TODO MEDIUM PRIORITY There is some flicker still when refreshing Report
// tabs on Windows.

// TODO MEDIUM PRIORITY Under KDE (at least on Mageia) the way I have set
// up the widths of ComboBox and TextCtrl and wxButton (in various
// controls in which these feature), where they are
// supposed to be the same height, they actually turn out to be slightly
// different heights. However even if I manually set them all to the same
// hard-coded height number, they still seem to come out different heights
// on KDE. It doesn't make a lot of sense.

// TODO MEDIUM PRIORITY Tooltips aren't showing on Windows.

/// TODO MEDIUM PRIORITY The database file should perhaps have a checksum to
// guard against its contents changing other than via the application.

// TODO HIGH PRIORITY Facilitate automatic checking for updates from user's
// machine, or else provide an easy way for users to sign up to a mailing
// list that keeps them informed about updates. Also note we had to manually
// add "SetOverwrite on" to the CMake NSIS template file, to ensure the
// generated installer will overwrite existing files when doing updates. This
// is not ideal.

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

// TODO HIGH PRIORITY Make the GUI display acceptably on smaller screen
// i.e. laptop.

// TODO MEDIUM PRIORITY Give user the option to export to CSV.

// TODO LOW PRIORITY Allow export/import to/from .qif (?) format.

// TODO MEDIUM PRIORITY Allow window borders to be dragged around, especially
// for DraftJournalListCtrl. This make it easier for users on laptops.

// TODO HIGH PRIORITY Publish user guide.

// TODO HIGH PRIORITY Incorporate instructions for making user guide into
// README.rst. Note user guide COULD be installed onto user's system
// as just a bunch of .html files, which can still be opened by the default
// browser (instead of having to go to website). Do we want this?

// TODO MEDIUM PRIORITY When the user creates a new file, ask if they want to
// create a shortcut to the file on their Desktop (assuming they didn't
// actually create the file in their desktop).

#include "app.hpp"
#include <wx/app.h>

wxIMPLEMENT_APP(dcm::App);
