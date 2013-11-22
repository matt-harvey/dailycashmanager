cmake_minimum_required (VERSION 2.6)

set (application_name "Phatbooks")
set (executable_stem_name "phatbooks")
set (file_extension ".phat")
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
add_definitions (
	-DPHATBOOKS_APPLICATION_NAME="${application_name}"
	-DPHATBOOKS_FILE_EXTENSION="${file_extension}"
	-DPHATBOOKS_VERSION_MAJOR=${version_major}
	-DPHATBOOKS_VERSION_MINOR=${version_minor}
	-DPHATBOOKS_VERSION_PATCH=${version_patch}
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

if (WIN32)
	
	# Note -mwindows prevents console output (so we don't want it during
	# development) but it also stops a console window from popping up when
	# a GUI app is opened by double-clicking its icon.
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")

	include_directories (include
	                     C:/Dev-Cpp
						 C:/Dev-Cpp/include/common
						 C:/include)
	link_directories(C:/Dev-Cpp/lib/wx/gcc_lib
					 C:/Dev-Cpp/lib
	                 C:/Dev-Cpp/MinGW/lib
					 C:/lib)
	add_definitions(-D__WXMSW__
	                -D__GNUWIN32
					-D__UNICODE
					-D_UNICODE
					-DUNICODE)
	set (windows_resources windows_resources.rc)
	set (libraries
		 jewel
		 UnitTest++
		 sqloxx
	     boost_date_time-mgw46-mt-1_53
		 boost_filesystem-mgw46-mt-1_53
		 boost_locale-mgw46-mt-1_53
		 boost_regex-mgw46-mt-1_53
		 boost_system-mgw46-mt-1_53
	     wxmsw29u
	     wxmsw29u_gl
	     wxscintilla
		 wxtiff
         wxjpeg
		 wxpng
		 wxzlib
		 wxregexu
		 wxexpat
		 winmm
		 shell32
		 comctl32
		 rpcrt4
		 wsock32
		 odbc32
		 opengl32)
else ()
	include_directories(include
						SYSTEM /usr/local/include/wx-2.9
						SYSTEM /usr/local/lib/wx/include/gtk2-unicode-static-2.9)
	link_directories(/usr/local/lib)
	set (libraries
		 jewel 
		 boost_date_time
		 boost_filesystem
		 boost_system
		 boost_regex
		 # boost_locale-mt
		 c
		 dl
		 m
		 z
		 UnitTest++
		 sqloxx
		 wx_gtk2u_xrc-2.9
		 wx_gtk2u_qa-2.9
		 wx_baseu_net-2.9
		 wx_gtk2u_html-2.9
		 wx_gtk2u_adv-2.9
		 wx_gtk2u_core-2.9
		 wx_baseu_xml-2.9
		 wx_baseu-2.9
		 gtk-x11-2.0
		 gdk-x11-2.0
		 atk-1.0
		 gio-2.0
		 pangoft2-1.0
		 pangocairo-1.0
		 gdk_pixbuf-2.0
		 cairo
		 pango-1.0
		 freetype 
		 fontconfig 
		 gobject-2.0
		 gthread-2.0
		 rt
		 glib-2.0
		 X11
		 Xxf86vm
		 wxregexu-2.9
		 png
		 pthread
		 # wxexpat-2.9
		 # wxtiff-2.9
		 # wxjpeg-2.9
		)
	set(windows_resources)  # Left empty

	add_definitions(#-D_FILE_OFFSET_BITS=64
	                #-D_LARGE_FILES
					-D__WXGTK__)
endif ()

set (common_sources
	#non-gui stuff...
	 src/account.cpp
	 src/account_table_iterator.cpp
	 src/account_type.cpp
	 src/app.cpp
	 src/augmented_account.cpp
	 src/balance_cache.cpp
	 src/make_default_accounts.cpp
	 src/amalgamated_budget.cpp
	 src/backup.cpp
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
	 src/phatbooks_database_connection.cpp
	 src/repeater.cpp
	 src/transaction_type.cpp
	 #gui stuff...
	 src/account_ctrl.cpp
	 src/account_dialog.cpp
	 src/account_list_ctrl.cpp
	 src/account_type_ctrl.cpp
	 src/balance_sheet_report.cpp
	 src/bs_account_entry_list_ctrl.cpp
	 src/budget_panel.cpp
	 src/date_ctrl.cpp
	 src/date_validator.cpp
	 src/decimal_text_ctrl.cpp
	 src/decimal_validator.cpp
	 src/draft_journal_list_ctrl.cpp
	 src/draft_journal_naming_dialog.cpp
	 src/entry_group_ctrl.cpp
	 src/entry_list_ctrl.cpp
	 src/entry_list_panel.cpp
	 src/error_reporter.cpp
	src/envelope_transfer_dialog.cpp
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
	 src/top_panel.cpp
	 src/transaction_ctrl.cpp
	 src/transaction_type_ctrl.cpp
	 src/welcome_dialog.cpp
	 src/window_utilities.cpp)

set (test_sources
	 tests/account_tests.cpp
	 tests/date_parser_tests.cpp
	 tests/date_tests.cpp
	 tests/draft_journal_tests.cpp
	 tests/filename_validation_tests.cpp
	 tests/finformat_tests.cpp
	 tests/frequency_tests.cpp
	 tests/interval_type_tests.cpp
	 tests/ordinary_journal_tests.cpp
	 tests/phatbooks_tests_common.cpp
	 tests/repeater_tests.cpp
	 tests/test.cpp
	 tests/transaction_type_tests.cpp)

add_library(phatbooks_common ${common_sources})

add_executable(${executable_stem_name} src/main.cpp ${windows_resources})
target_link_libraries (${executable_stem_name} phatbooks_common ${libraries})

add_executable (test ${test_sources})
target_link_libraries (test phatbooks_common ${libraries})

if (WIN32)

	# On Windows we use CPack to create a graphical installer
	# Do "make package" (or "mingw32-make package") to create the installer.
	set (application_name "Phatbooks")
	set (CPACK_NSIS_EXECUTABLES_DIRECTORY ".")  # Required to ensure that application properly launches after installing if that option is selected.
	set (CPACK_GENERATOR NSIS)
	set (CPACK_NSIS_PACKAGE_NAME "${application_name}")
	set (CPACK_PACKAGE_VENDOR "${application_name}")
	set (CPACK_PACKAGE_FILE_NAME "${application_name} setup")
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "${application_name}")

	# NOTE These version numbers must be kept in sync with those appearing in
	# "app.cpp" (App::version() function).
	set (CPACK_PACKAGE_VERSION_MAJOR "1")
	set (CPACK_PACKAGE_VERSION_MINOR "0")
	set (CPACK_PACKAGE_VERSION_PATCH "0")
	set (CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

	set (CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon.ico")
	set (CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon.ico")
	set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon.ico")
	set (CPACK_NSIS_INSTALLED_ICON_NAME "${executable_stem_name}.exe")
	set (CPACK_NSIS_MENU_LINKS "${executable_stem_name}.exe" "${application_name}")  # Put a link to the application executable in the Shortcuts menu.
	set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}\\\\LICENSE")
	set (CPACK_NSIS_MUI_FINISHPAGE_RUN "${executable_stem_name}.exe")  # Give user option to launch application immediately after installing.

	# Create an association on the user's machine between the application
	# file extension and the application
	set (file_description_word "${application_name}File")
	set (
		CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			WriteRegStr HKCR '${file_extension}' '' '${file_description_word}' 
			WriteRegStr HKCR '${file_description_word}' '' '${application_name} File' 
			WriteRegStr HKCR '${file_description_word}\\\\shell' '' 'open' 
			WriteRegStr HKCR '${file_description_word}\\\\DefaultIcon' '' '$INSTDIR\\\\${executable_stem_name}.exe, 0' 
			WriteRegStr HKCR '${file_description_word}\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\${executable_stem_name}.exe \\\"%1\\\"' 
			WriteRegStr HKCR '${file_description_word}\\\\shell\\\\edit' '' 'Edit ${application_name} File' 
			WriteRegStr HKCR '${file_description_word}\\\\shell\\\\edit\\\\command' '' '$INSTDIR\\\\${executable_stem_name}.exe \\\"%1\\\"' 
			System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)' 
		"
	)
	set (
		CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS " 
			DeleteRegKey HKCR '${file_extension}' 
			DeleteRegKey HKCR '${file_description_word}' 
		"
	)

	install(TARGETS ${executable_stem_name} RUNTIME DESTINATION .)
	include(CPack)
else ()
	install (
		TARGETS
			${executable_stem_name}
		RUNTIME DESTINATION
			"${CMAKE_INSTALL_PREFIX}/bin"
	)
endif ()

					


