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

// TODO MEDIUM PRIORITY Do proper "make package" instructions using CPack to
// make RPM and .deb packages.

// TODO MEDIUM PRIORITY Within "make install" and/or "make package" for
// Linux, include installation of icon and association of file extension with
// application so that .dcm files can be opened by double-clicking and so that
// application icon appears along with other applications' icons as usual for
// Gnome, KDE etc..

// TODO MEDIUM PRIORITY Under KDE (at least on Mageia) the way I have set
// up the widths of ComboBox and TextCtrl and wxButton (in various
// controls in which these feature), where they are
// supposed to be the same height, they actually turn out to be slightly
// different heights. However even if I manually set them all to the same
// hard-coded height number, they still seem to come out different heights
// on KDE. It doesn't make a lot of sense.

// TODO HIGH PRIORITY Figure out why icon isn't appearing in body of NSIS
// installer.

// TODO MEDIUM PRIORITY Tooltips aren't showing on Windows.

/// TODO MEDIUM PRIORITY The database file should perhaps have a checksum to
// guard against its contents changing other than via the application.

// TODO LOW PRIORITY Facilitate automatic checking for updates from user's
// machine.

// TODO HIGH PRIORITY Make the GUI display acceptably on smaller screen
// i.e. laptop.

// TODO MEDIUM PRIORITY Give user the option to export to CSV.

// TODO LOW PRIORITY Allow export/import to/from .qif (?) format.

// TODO MEDIUM PRIORITY Allow window borders to be dragged around, especially
// for DraftJournalListCtrl. This make it easier for users on laptops.

// TODO MEDIUM PRIORITY When the user creates a new file, ask if they want to
// create a shortcut to the file on their Desktop (assuming they didn't
// actually create the file in their desktop).

#include "app.hpp"
#include <wx/app.h>

wxIMPLEMENT_APP(dcm::App);
