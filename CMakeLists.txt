### 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###

# Preliminaries

cmake_minimum_required (VERSION 2.8)

# Set project and version

set (application_name "DailyCashManager")
set (executable_stem_name "dailycashmanager")
set (file_extension ".dcm")
project (${application_name})
set (version_major 1)
set (version_minor 0)
set (version_patch 0)

# Custom configuration options

option (
	ENABLE_LOGGING
	"Enable logging (ON/OFF)?"
	ON
)
option (
	ENABLE_ASSERTION_LOGGING
	"Enable logging of assertion failures (ON/OFF)?"
	ON
)
option (
	ENABLE_EXCEPTION_LOGGING
	"Enable logging of thrown exceptions (ON/OFF)?"
	ON
)

# Define this stuff here as used below
if (WIN32)
	set (user_guide_rel_dir "doc")
else ()
	set (user_guide_rel_dir "share/doc/${executable_stem_name}")
endif ()

# Definitions to be passed to the compiler

# NOTE Adding definitions via "add_definitions" can cause problems
# in recent versions of CMake, where these get passed to RC_FLAGS, if
# the definitions are invalid for the RC compiler.

add_definitions (
	-DDCM_APPLICATION_NAME="${application_name}"
	-DDCM_FILE_EXTENSION="${file_extension}"
	-DDCM_VERSION_MAJOR=${version_major}
	-DDCM_VERSION_MINOR=${version_minor}
	-DDCM_VERSION_PATCH=${version_patch}
	-DDCM_USER_GUIDE_DIR="${CMAKE_INSTALL_PREFIX}/${user_guide_rel_dir}"
)
if (ENABLE_LOGGING)
	add_definitions (-DJEWEL_ENABLE_LOGGING)
endif ()
if (ENABLE_ASSERTION_LOGGING)
	add_definitions (-DJEWEL_ENABLE_ASSERTION_LOGGING)
endif ()
if (ENABLE_EXCEPTION_LOGGING)
	add_definitions (-DJEWEL_ENABLE_EXCEPTION_LOGGING)
endif ()
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
endif ()

# Find dependencies

find_package (
	Boost 1.53.0
	COMPONENTS
		date_time
		filesystem
		locale
		regex
		system
	REQUIRED
)
find_library (SQLOXX_LIBRARY sqloxx REQUIRED)
find_library (JEWEL_LIBRARY jewel REQUIRED)
find_library (UNIT_TEST_LIBRARY UnitTest++ REQUIRED)
find_package (
	wxWidgets 2.9.3
	COMPONENTS
		aui
		html
		adv
		core
		xml
		base
)
find_package (Tclsh 8.4 REQUIRED)
if (WIN32)
	set (
		extra_libraries
		winmm
		shell32
		comctl32
		rpcrt4
		wsock32
		odbc32
		opengl32
	)
elseif (UNIX)
	set (
		extra_libraries
		pthread
		dl
	)
endif ()
if (SQLOXX_LIBRARY-NOTFOUND)
	message ("Could not find Sqloxx library.")
endif ()
if (JEWEL_LIBRARY-NOTFOUND)
	message ("Could not find Jewel library.")
endif ()
if (UNIT_TEST_LIBRARY-NOTFOUND)
	message ("Could not find UnitTest++ - cannot build tests.")
endif ()
if (NOT TCLSH_FOUND)
	message ("Could not find Tclsh - cannot do pre-build code generation step.")
endif ()

# Build instructions
if (wxWidgets_FOUND)
	include (${wxWidgets_USE_FILE})
else ()
	# Any special instructions required in order to link / include wxWidgets
	# properly, should be isolated in the file included here.
	include (wxWidgetsSpecial.cmake)
endif ()
include_directories (
	include
	${JEWEL_INCLUDES}
	${SQLOXX_INCLUDES}
	${Boost_INCLUDE_DIRS}
	SYSTEM ${wxWidgets_INCLUDE_DIRS}
	${UNIT_TEST_INCLUDES}
)
set (
	libraries
	${JEWEL_LIBRARY}
	${SQLOXX_LIBRARY}
	${Boost_LIBRARIES}
	${wxWidgets_LIBRARIES}
	${extra_libraries}
)
add_custom_target (
	prebuild
	ALL
	COMMAND ${TCL_TCLSH} "${PROJECT_SOURCE_DIR}/prebuild_driver.tcl"
)

# Building the dcm_common library, which contains code used by both
# the main executable, and the test suite.

if (WIN32)
	set (windows_resources windows_resources.rc)
	# Note -mwindows prevents console output (so we don't want it during
	# development) but it also stops a console window from popping up when
	# a GUI app is opened by double-clicking its icon.
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
else ()
	set (windows_resources)  # left empty
endif ()
set (
	common_sources
	#non-gui stuff...
	src/account.cpp
	src/account_table_iterator.cpp
	src/account_type.cpp
	src/app.cpp
	src/augmented_account.cpp
	src/backup.cpp
	src/balance_cache.cpp
	src/make_default_accounts.cpp
	src/amalgamated_budget.cpp
	src/budget_item.cpp
	src/commodity.cpp
	src/make_currencies.cpp
	src/date.cpp
	src/date_parser.cpp
	src/frequency.cpp
	src/interval_type.cpp
	src/entry.cpp
	src/entry_table_iterator.cpp
	src/filename_validation.cpp
	src/finformat.cpp
	src/draft_journal.cpp
	src/draft_journal_table_iterator.cpp
	src/journal.cpp
	src/ordinary_journal.cpp
	src/persistent_journal.cpp
	src/dcm_database_connection.cpp
	src/repeater.cpp
	src/transaction_type.cpp
	#gui stuff...
	src/about_dialog.cpp
	src/account_ctrl.cpp
	src/account_dialog.cpp
	src/account_list_ctrl.cpp
	src/account_type_ctrl.cpp
	src/balance_sheet_report.cpp
	src/bs_account_entry_list_ctrl.cpp
	src/budget_panel.cpp
	src/button.cpp
	src/combo_box.cpp
	src/date_ctrl.cpp
	src/date_validator.cpp
	src/decimal_text_ctrl.cpp
	src/decimal_validator.cpp
	src/draft_journal_list_ctrl.cpp
	src/draft_journal_naming_dialog.cpp
	src/entry_group_ctrl.cpp
	src/entry_list_ctrl.cpp
	src/entry_list_panel.cpp
	src/envelope_transfer_dialog.cpp
	src/error_reporter.cpp
	src/filtered_entry_list_ctrl.cpp
	src/frame.cpp
	src/frequency_ctrl.cpp
	src/gridded_scrolled_panel.cpp
	src/locale.cpp
	src/multi_account_panel.cpp
	src/persistent_object_event.cpp
	src/pl_account_entry_list_ctrl.cpp
	src/pl_report.cpp
	src/reconciliation_entry_list_ctrl.cpp
	src/report.cpp
	src/report_panel.cpp
	src/setup_wizard.cpp
	src/sizing.cpp
	src/string_set_validator.cpp
	src/summary_datum.cpp
	src/text_ctrl.cpp
	src/top_panel.cpp
	src/transaction_ctrl.cpp
	src/transaction_type_ctrl.cpp
	src/welcome_dialog.cpp
	src/window_utilities.cpp
)
add_library (dcm_common ${common_sources})
add_dependencies (dcm_common prebuild)

# User guide

add_custom_target (
	user_guide
	ALL
	COMMAND ${TCL_TCLSH} "${PROJECT_SOURCE_DIR}/build_user_guide.tcl"
)
	
# Building the tests

set (
	test_sources
	tests/account_tests.cpp
	tests/date_parser_tests.cpp
	tests/date_tests.cpp
	tests/draft_journal_tests.cpp
	tests/filename_validation_tests.cpp
	tests/finformat_tests.cpp
	tests/frequency_tests.cpp
	tests/interval_type_tests.cpp
	tests/ordinary_journal_tests.cpp
	tests/dcm_tests_common.cpp
	tests/repeater_tests.cpp
	tests/test.cpp
	tests/transaction_type_tests.cpp
)
add_executable (test_driver ${test_sources})
target_link_libraries (
	test_driver
	dcm_common
	${libraries}
	${UNIT_TEST_LIBRARY}
)
if (WIN32)
	set (test_execution_command "${PROJECT_SOURCE_DIR}\\test_driver.exe")
else ()
	set (test_execution_command "${PROJECT_SOURCE_DIR}/test_driver")
endif ()
add_custom_target (
	test
	ALL
	COMMAND "${test_execution_command}"
	DEPENDS test_driver
)

# Building the main executable

add_executable(${executable_stem_name} src/main.cpp ${windows_resources})
target_link_libraries (${executable_stem_name} dcm_common ${libraries})

# Building the .desktop file (NOT part of ALL)
set (desktop_filename "${executable_stem_name}.desktop")
if (UNIX)
	set (icon_filename "icon_100_100.png")
	set (icon_rel_dir "share/pixmaps/${executable_stem_name}")
	set (path_to_installed_icon "${CMAKE_INSTALL_PREFIX}/${icon_rel_dir}/${icon_filename}")
	add_custom_target (
		desktop_file
		COMMAND ${TCL_TCLSH} generate_desktop_file.tcl ${application_name} ${executable_stem_name} ${path_to_installed_icon} ${desktop_filename}
	)
endif ()

# Installation instructions

install (TARGETS ${executable_stem_name} RUNTIME DESTINATION "bin")
install (DIRECTORY "user_guide/_build/html" DESTINATION ${user_guide_rel_dir})
if (UNIX)
	install (FILES "images/${icon_filename}" DESTINATION ${icon_rel_dir})
endif ()

# Instructions to build package (source package on Unix-like system, or
# binary installation package on Windows)

set (CPACK_PACKAGE_VERSION_MAJOR "${version_major}")
set (CPACK_PACKAGE_VERSION_MINOR "${version_minor}")
set (CPACK_PACKAGE_VERSION_PATCH "${version_patch}")
set (
	CPACK_PACKAGE_VERSION
	"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
)

if (WIN32)

	# On Windows we use CPack to create a graphical installer
	# Do "make package" (or "mingw32-make package") to create the installer.
	set (CPACK_NSIS_EXECUTABLES_DIRECTORY ".")  # Required to ensure that application properly launches after installing if that option is selected.
	set (CPACK_GENERATOR NSIS)
	set (CPACK_NSIS_PACKAGE_NAME "${application_name}")
	set (CPACK_PACKAGE_VENDOR "${application_name}")
	set (CPACK_PACKAGE_FILE_NAME "${application_name} setup")
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "${application_name}")

	set (CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon_200_200.ico")
	set (CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon_200_200.ico")
	set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon_200_200.ico")
	set (CPACK_NSIS_INSTALLED_ICON_NAME "${executable_stem_name}.exe")
	set (CPACK_NSIS_MENU_LINKS "${executable_stem_name}.exe" "${application_name}")  # Put a link to the application executable in the Shortcuts menu.
	set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}\\\\LICENSE")
	set (CPACK_NSIS_MUI_FINISHPAGE_RUN "${executable_stem_name}.exe")  # Give user option to launch application immediately after installing.

	# Create an association on the user's machine between the application
	# file extension and the application
	set (file_description_word "${application_name}File")
	set (
		CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			WriteRegStr HKEY_CLASSES_ROOT '${file_extension}' '' '${file_description_word}' 
			WriteRegStr HKEY_CLASSES_ROOT '${file_description_word}' '' '${application_name} File' 
			WriteRegStr HKEY_CLASSES_ROOT '${file_description_word}\\\\shell' '' 'open' 
			WriteRegStr HKEY_CLASSES_ROOT '${file_description_word}\\\\DefaultIcon' '' '$INSTDIR\\\\${executable_stem_name}.exe, 0' 
			WriteRegStr HKEY_CLASSES_ROOT '${file_description_word}\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\${executable_stem_name}.exe \\\"%1\\\"' 
			WriteRegStr HKEY_CLASSES_ROOT '${file_description_word}\\\\shell\\\\edit' '' 'Edit ${application_name} File' 
			WriteRegStr HKEY_CLASSES_ROOT '${file_description_word}\\\\shell\\\\edit\\\\command' '' '$INSTDIR\\\\${executable_stem_name}.exe \\\"%1\\\"' 
			System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)' 
		"
	)
	# NOTE we here we also delete the application name registry key as this is
	# the one created by wxConfig.
	set (
		CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS " 
			DeleteRegKey HKEY_CLASSES_ROOT '${file_extension}' 
			DeleteRegKey HKEY_CLASSES_ROOT '${file_description_word}' 
			DeleteRegKey HKEY_CURRENT_USER 'Software\\\\${application_name}'
		"
	)
	include (CPack)

else ()

	set (package_name "${application_name}-${version_major}.${version_minor}.${version_patch}-working")
	set (tarball_name "${package_name}.tar.gz")
	set (
		packaged_items
		include
		images
		src
		tests
		build_user_guide.tcl
		prebuild_driver.tcl
		CMakeLists.txt
		Doxyfile
		LICENSE
		coding_style
		currencies.csv
		database_conventions
		windows_resources.rc
	)
	add_custom_target (
		package
		COMMAND
			mkdir ${package_name} &&
			mkdir ${package_name}/user_guide &&
			cp user_guide/*.rst ${package_name}/user_guide &&
			cp user_guide/conf.py ${package_name}/user_guide &&
			cp -r ${packaged_items} ${package_name} &&
			rm ${package_name}/include/make_currencies_inc.hpp &&
			rm ${package_name}/include/user_guide/LICENSE.rst &&
			tar -sczf ${tarball_name} ${package_name} &&
			rm -rf ${package_name}
		DEPENDS ${library_name} test user_guide
	)
	set_directory_properties (
		PROPERTIES
			ADDITIONAL_MAKE_CLEAN_FILES "${tarball_name}"
			ADDITIONAL_MAKE_CLEAN_FILES "${desktop_filename}"
	)

endif ()

# Building the documentation

add_custom_target (
	docs
	COMMAND doxygen Doxyfile
)
