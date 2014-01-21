Legal
=====

Copyright 2013 Matthew Harvey

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Overview
========

DailyCashManager is graphical desktop application written in standard C++, using
C++11 features. It provides a means for individuals and households to
keep a record of their financial transactions, and to manage their
spending using the envelope budgeting system.

Dependencies
============

DailyCashManager is written in standard C++, and utilizes some C++11 features.
To build and install the application, you will need:

- A reasonably conformant C++ compiler and standard library implementation
  (DailyCashManager has been successfully built with GCC 4.7.2 and with
  later versions; it has not been tested with other compilers)

- A Tcl interpreter (known to work with version 8.6)

- CMake (version 2.8 or later)

- The Boost C++ libraries (version 1.53.0 or later)

- The Jewel C++ libraries (version 1.0.0 or later)

- The Sqloxx C++ libraries (version 1.0.0 or later)

- wxWidgets (version 2.9.3 or later)

- UnitTest++ (known to work with version 1.4)

If you want to build the API documentation, you will need:

- Doxygen (known to work with version 1.8.3.1)

If you want to build the user guide web contents, you will need:

- Sphinx (documentation generator - known to work with version 1.1.3)
- sphinx-better-theme (known to work with version 0.1.4)

To build a binary installer for Windows, you will need:

- Nullsoft Scriptable Install System (NSIS) (known to work with version 2.46)

At the time of writing, these dependencies can be obtained from the following
locations:

:Tcl:                       http://tcl.tk
:CMake:                     http://www.cmake.org
:Boost:	                    http://www.boost.org
:Jewel:                     https://github.com/skybaboon/jewel
:Sqloxx:                    https://github.com/skybaboon/sqloxx
:UnitTest++:	            http://unittest-cpp.sourceforge.net
:Doxygen:	                http://www.stack.nl/~dimitri/doxygen
:Sphinx:                    http://sphinx-doc.org
:sphinx-better-theme:       https://pypi.python.org/pypi/sphinx-better-theme
:NSIS:                      http://nsis.sourceforge.net

Initial build configuration
===========================

Open a command line and ``cd`` to the project root.

On Unix-like systems, enter::

	cmake -i

(On Windows, you will need to use the ``-G`` option to choose a Makefile
generator, or else run ``cmake-gui.exe .``. See CMake documentation for further
details.)

You will be prompted with ``Would you like to see the advanced options? [No]:``.
Enter ``n``. Then follow the prompts.

(If you run into trouble with the build or have atypical requirements, you can
return to this step and answer ``y`` to configure more detailed build options.)

If in doubt about a particular option, it is generally best simply to hit enter
and keep the default setting for the option.

Note the options ``ENABLE_ASSERTION_LOGGING``, ``ENABLE_EXCEPTION_LOGGING`` and
``ENABLE_LOGGING``. These determine whether the
``JEWEL_ENABLE_ASSERTION_LOGGING``, ``JEWEL_ENABLE_EXCEPTION_LOGGING`` and
``JEWEL_ENABLE_LOGGING`` macros will be defined. If in doubt, it is recommended
to leave these logging options ON. (For more information on the significance of
these macros, see the documentation for the jewel::Log class, in the Jewel
library.)

Basic installation
==================

On Unix-like systems
--------------------

On Unix-like systems, you must currently build and install from source.
Unpackage the source tarball, ``cd`` into the root of the project source
directory, then enter::
	
	make install

You may need to run this as root, i.e.::

	sudo make install

This will cause the application and tests to be built (if not built already),
and will cause the tests to be run, with the results output to the console.

If and only if all the tests succeed, installation of the application
will then proceed. For your information, a list of the installed files will be
saved in the file "install_manifest.txt", in the project root.

If any tests fail, you are encouraged to send the developer your test output,
along with the file "DailyCashManager_test.log" (which should appear in the
project root), and the details of your system and build environment. (See
Contact_ below for contact details.)

On Windows
----------

Installing from binary installer
................................

This is by far the easiest way to install DailyCashManager on Windows.

A binary installer can be obtained from
https://sourceforge.net/projects/dailycashmanager/files.

Having downloaded the binary installer, simply double-click on the installer
icon, and follow the prompts.

Building the binary installer
.............................

If you want to build and install from source on Windows, you should build the
binary installer first, then install the application using the binary
installer. To build the binary installer, open a command prompt, and ``cd`` to
the root of the unpackaged source directory. Then run ``cmake -G ...`` to tell
CMake which generator to use (with the ``...`` replaced with an appropriate
string specifying the generator - see CMake documentation for details).

The binary installer will be built via CMake using the NSIS CMake module. By
default, installers built via this module will *not* overwrite existing files
with the same name. This is likely to cause unexpected behaviour when using
the installer to upgrade from one version of the application to another. To
avoid this confusion, and create an installer that will always overwrite
older versions, you will need to manually change the file "NSIS.template.in",
which should be located in the directory in which CMake is installed on your
system, under the subdirectory
"share\\cmake-[major-version].[minor-version]\\Modules". In "NSIS.template.in",
just above the section entitled "General", add a line reading::

    SetOverwrite on

Save the amended "NSIS.template.in". (Note this will, of course, affect all
NSIS installers built using this CMake module, not just for DailyCashManager.)
Now, return to the unpackaged DailyCashManager source directory, and on the
command line, and enter::

    make package

This will cause the application and tests to be built, and will cause the
tests to be run, with the results output to the console. Only if all the tests
succeed, a binary installer will be created, titled "DailyCashManager-vX.Y.Z-
setup.exe" (or similar, with "X", "Y" and "Z" being replaced by major, minor
and patch version numbers, respectively). Execute this file, and follow the
prompts to install the application.

On some configurations on Windows, it has been found that CMake is unable to
locate wxWidgets during the build process, even if is installed on the system.
To get around this, the CMake build script ("CMakeLists.txt") provides for the
possibility of invoking an additional, custom build script, titled
"wxWidgetsSpecial.cmake". If CMake cannot find wxWidgets in the normal way, then
it looks for a file by this name, and invokes it if found. If you are
familiar with the CMake scripting language, and you run into this problem, this
gives you the opportunity to manually tell CMake which libraries etc. to link
to on your system, by putting appropriate additional CMake commands into a file
you create with this name; on most systems, however, you should not have to do
this.

To generate the documentation
=============================

If you have Doxygen installed and want to generate the API documentation, then
enter the following at the project root::

	make docs

HTML documentation will then be generated in the project root directory,
under "html", and can be browsed by opening the following file in your
web browser::

	[project root]/html/index.html

Almost all of the Doxygen markup is contained in the
C++ headers; so an alternative source of information on the DailyCashManager
API, is simply to examine the headers directly.

Other build targets
===================

To clean build
--------------

Go to the project root and enter::
	
	make clean

This will clean all build targets from the project root, including
the source tarball (see below) if present, but NOT including the HTML
documentation or the User Guide. This is due to a quirk of CMake. To remove the
HTML documentation, simply manually delete the "html" directory from the
project root. To remove the generated User Guide, manually delete the
"user_guide/_build" directory.

Note this will *not* cause the application to be uninstalled from the host
system.

To build without installing or testing
--------------------------------------

At the project root, enter::

	make dailycashmanager


To build and run the test suite without installing
--------------------------------------------------

At the project root, enter::

	make test

After the test driver executable is built, the tests will automatically be run
and the results displayed.

If any tests fail, you are strongly encouraged to send the developer
your test output, along with the file "DailyCashManager_test.log" (which should
appear in the project root), and the details of your system and build
environment. (See below for contact details.)

To build a package for distribution
-----------------------------------

If you are running a Unix-like system, and have the ``tar`` command available,
you can build a tarball of the project sources by entering the following
at the project root::
	
	make package

The tarball will appear in the project root directory.

If you are running Windows, the same command (``make package``) will cause a
binary installer to be created; again, this will appear in the project root
directory.

As a safety measure, running ``make package`` always causes the tests to be
built and run, prior to the package being built. The package will not be built
unless all the tests pass.

To build a .desktop file
------------------------

On Linux systems, you may want to create a .desktop file for use with
``xdg-desktop-icon`` and related utilities. This can be done by entering::

    make desktop_file

The file "dailycashmanager.desktop" should then appear in the project root.

To build the user guide
-----------------------

To build the user guide, enter::

    make user_guide

The user guide web contents will be output to "user_guide/_build/html".

To build multiple targets in one go
-----------------------------------

To build the application, build the tests and run the tests with one command, go
to the project root, and enter::

	make

Note this will NOT install the application, will NOT generate the documentation,
will NOT generate a .desktop file, will NOT generate the user guide, and will
NOT build a source tarball.

Uninstalling
============

On Unix-like systems
--------------------

There is currently no ``make uninstall`` target; however, manual uninstallation
is easy enough.

In the project directory from which you ran ``make install``, there should be a
plain text file entitled "install_manifest.txt". To uninstall DailyCashManager,
manually remove each of the files listed in this manifest. In addition, there
is likely to be a configuration file, ".DailyCashManager", in your home folder;
if present, remove it also (``rm ~/.DailyCashManager``).

If you want to be very thorough, you may also want to remove the log file,
which, if present, should be located at "/tmp/DailyCashManager.log".

On Windows
----------

Run the uninstaller, which should be located in the same folder as the
"dailycashmanager.exe". (Unless you specified otherwise when installing the
application, this will usually have defaulted to either "C:\\Program Files
(x86)\\DailyCashManager" or "C:\\Program Files\\DailyCashManager). The uninstaller
can usually also be accessed via the Start menu, under "All
Programs\\DailyCashManager\\Uninstall".

Contact
=======

dcm@matthewharvey.net
