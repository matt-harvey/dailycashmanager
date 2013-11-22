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

// TODO HIGH PRIORITY Under KDE (at least on Mageia) the way I have set
// up the widths of wxComboBox and wxTextCtrl and wxButton (in various
// controls in which these feature), where they are
// supposed to be the same height, they actually turn out to be slightly
// different heights. However even if I manually set them all to the same
// hard-coded height number, they still seem to come out different heights
// on KDE. It doesn't make a lot of sense.

// TODO MEDIUM PRIORITY Tooltips aren't showing on Windows.

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

// TODO HIGH PRIORITY Make the GUI display acceptably on smaller screen
// i.e. laptop.

// TODO MEDIUM PRIORITY Startup time under Windows is really slow, even when
// compiled in Release mode.

#include "app.hpp"
#include <wx/app.h>

wxIMPLEMENT_APP(phatbooks::App);
