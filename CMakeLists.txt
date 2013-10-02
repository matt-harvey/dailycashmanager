cmake_minimum_required (VERSION 2.6)

project (phatbooks)

set (immediate_directories
     .
	 account
	 application
	 budget
	 commodity
	 currency
	 date
	 entry 
	 filename_validation
	 finformat
	 graphical_session
	 images
	 journal
	 output_aux
	 phatbooks_database_connection
	 phatbooks_persistent_object
	 string_conv
	 string_flags
	 repeater
	 session
	 transaction_side
	 transaction_type
	 visibility)

# add_definitions(-DPHATBOOKS_DISALLOW_DASH_FOR_ZERO)
add_definitions(-DJEWEL_ENABLE_LOGGING
			    -DJEWEL_ENABLE_ASSERTION_LOGGING
				-DJEWEL_ENABLE_EXCEPTION_LOGGING)

if (WIN32)
	include_directories (${immediate_directories}
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
	     boost_date_time-mgw46-mt-1_53
		 boost_filesystem-mgw46-mt-1_53
		 boost_locale-mgw46-mt-1_53
		 boost_regex-mgw46-mt-1_53
		 boost_system-mgw46-mt-1_53
		 jewel
		 UnitTest++
		 sqloxx
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
	include_directories(${immediate_directories}
						SYSTEM /usr/local/include/wx-2.9
						SYSTEM /usr/local/lib/wx/include/gtk2-unicode-static-2.9)
	link_directories(/usr/local/lib)
	set (libraries
		 boost_date_time
		 boost_filesystem
		 boost_locale-mt
		 c
		 dl
		 boost_regex
		 boost_system
		 jewel 
		 m
		 z
		 pthread
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
		 png
		 wxregexu-2.9
		 wxexpat-2.9
		 wxtiff-2.9
		 wxjpeg-2.9)
	set(windows_resources)  # Left empty

	add_definitions(#-D_FILE_OFFSET_BITS=64
	                #-D_LARGE_FILES
					-D__WXGTK__)
endif ()

set (main_files
	 account/account.cpp
	 account/account_handle.cpp
	 account/account_table_iterator.cpp
	 account/account_type.cpp
	 account/balance_cache.cpp
	 account/make_default_accounts.cpp
	 application/application.cpp
	 budget/amalgamated_budget.cpp
	 budget/budget_item.cpp
	 budget/budget_item_handle.cpp
	 commodity/commodity.cpp
	 currency/make_currencies.cpp
	 date/date.cpp
	 date/date_parser.cpp
	 date/frequency.cpp
	 date/interval_type.cpp
	 entry/entry.cpp
	 entry/entry_table_iterator.cpp
	 filename_validation/filename_validation.cpp
	 finformat/finformat.cpp
	 journal/draft_journal.cpp
	 journal/draft_journal_table_iterator.cpp
	 journal/journal.cpp
	 journal/ordinary_journal.cpp
	 journal/ordinary_journal_handle.cpp
	 journal/persistent_journal.cpp
	 session/session.cpp
	 phatbooks_database_connection/phatbooks_database_connection.cpp
	 repeater/repeater.cpp
	 transaction_type/transaction_type.cpp)

set (gui_files
	 graphical_session/graphical_session.cpp
	 graphical_session/wxs/account_ctrl.cpp
	 graphical_session/wxs/account_dialog.cpp
	 graphical_session/wxs/account_list_ctrl.cpp
	 graphical_session/wxs/account_type_ctrl.cpp
	 graphical_session/wxs/app.cpp
	 graphical_session/wxs/balance_sheet_report.cpp
	 graphical_session/wxs/bs_account_entry_list_ctrl.cpp
	 graphical_session/wxs/budget_panel.cpp
	 graphical_session/wxs/date_ctrl.cpp
	 graphical_session/wxs/date_validator.cpp
	 graphical_session/wxs/decimal_text_ctrl.cpp
	 graphical_session/wxs/decimal_validator.cpp
	 graphical_session/wxs/draft_journal_list_ctrl.cpp
	 graphical_session/wxs/draft_journal_naming_dialog.cpp
	 graphical_session/wxs/entry_group_ctrl.cpp
	 graphical_session/wxs/entry_list_ctrl.cpp
	 graphical_session/wxs/entry_list_panel.cpp
	 graphical_session/wxs/filtered_entry_list_ctrl.cpp
	 graphical_session/wxs/frame.cpp
	 graphical_session/wxs/frequency_ctrl.cpp
	 graphical_session/wxs/gridded_scrolled_panel.cpp
	 graphical_session/wxs/locale.cpp
	 graphical_session/wxs/multi_account_panel.cpp
	 graphical_session/wxs/persistent_object_event.cpp
	 graphical_session/wxs/pl_account_entry_list_ctrl.cpp
	 graphical_session/wxs/pl_report.cpp
	 graphical_session/wxs/reconciliation_entry_list_ctrl.cpp
	 graphical_session/wxs/report.cpp
	 graphical_session/wxs/report_panel.cpp
	 graphical_session/wxs/setup_wizard.cpp
	 graphical_session/wxs/sizing.cpp
	 graphical_session/wxs/string_set_validator.cpp
	 graphical_session/wxs/summary_datum.cpp
	 graphical_session/wxs/top_panel.cpp
	 graphical_session/wxs/transaction_ctrl.cpp
	 graphical_session/wxs/transaction_type_ctrl.cpp
	 graphical_session/wxs/welcome_dialog.cpp
	 graphical_session/wxs/window_utilities.cpp)

set (test_files
	 ${main_files}
	 tests/test.cpp
	 tests/date_parser_tests.cpp
	 tests/date_tests.cpp
	 tests/draft_journal_tests.cpp
	 tests/filename_validation_tests.cpp
	 tests/frequency_tests.cpp
	 tests/interval_type_tests.cpp
	 tests/ordinary_journal_tests.cpp
	 tests/phatbooks_tests_common.cpp
	 tests/repeater_tests.cpp
	 tests/transaction_type_tests.cpp)

add_executable(
	phatbooks
	main.cpp
	${main_files}
	${gui_files}
	${windows_resources}
)
target_link_libraries (phatbooks ${libraries})

add_executable (phatbooks_test ${test_files})
target_link_libraries (phatbooks_test ${libraries})

if (WIN32)
	
	# Note -mwindows prevents console output (so we don't want it during
	# development) but it also stops a console window from popping up when
	# a GUI app is opened by double-clicking its icon. So in due course we
	# will want to reinstate this flag. But best done in CMakeCache.txt, otherwise
	# things seem to get weird.
	# set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -mwindows")

	# On Windows we use CPack to create a graphical installer
	# Do "make package" (or "mingw32-make package") to create the installer.
	set (application_name "Phatbooks")
	set (CPACK_NSIS_EXECUTABLES_DIRECTORY ".")  # Required to ensure that application properly launches after installing if that option is selected.
	set (CPACK_GENERATOR NSIS)
	set (CPACK_NSIS_PACKAGE_NAME "${application_name}")
	set (CPACK_PACKAGE_VENDOR "${application_name}")
	set (CPACK_PACKAGE_FILE_NAME "${application_name} setup")
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "${application_name}")
	set (CPACK_PACKAGE_VERSION "1.0.0")
	set (CPACK_PACKAGE_VERSION_MAJOR "1")
	set (CPACK_PACKAGE_VERSION_MINOR "0")
	set (CPACK_PACKAGE_VERSION_PATCH "0")
	set (CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon.ico")
	set (CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon.ico")
	set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\images\\\\icon.ico")
	set (CPACK_NSIS_INSTALLED_ICON_NAME "phatbooks.exe")
	set (CPACK_NSIS_MENU_LINKS "phatbooks.exe" "${application_name}")  # Put a link to the application executable in the Shortcuts menu.
	set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}\\\\licence.txt")
	set (CPACK_NSIS_MUI_FINISHPAGE_RUN "phatbooks.exe")  # Give user option to launch application immediately after installing.
	install(TARGETS phatbooks RUNTIME DESTINATION .)
	include(CPack)
else ()
	install(TARGETS phatbooks RUNTIME DESTINATION /usr/local/bin)
endif ()

					


