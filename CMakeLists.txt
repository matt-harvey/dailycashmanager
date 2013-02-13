cmake_minimum_required (VERSION 2.6)

project (phatbooks_project)

set (immediate_directories
     .
	 account
	 application
	 commodity
	 date
	 entry
	 finformat
	 graphical_session
	 journal
	 output_aux
	 phatbooks_database_connection
	 phatbooks_persistent_object
	 phatbooks_text_session
	 repeater
	 session)

if (WIN32)
	include_directories (${immediate_directories}
	                     C:/Dev-Cpp
						 C:/Dev-Cpp/include/common
						 C:/include)
	link_directories(C:/Dev-Cpp/lib
	                 C:/Dev-Cpp/MinGW/lib
					 C:/lib)
	set (libraries
	     boost_date_time-mgw46-mt-1_53
		 boost_filesystem-mgw46-mt-1_53
		 boost_locale-mgw46-mt-1_53
		 boost_regex-mgw46-mt-1_53
		 boost_system-mgw46-mt-1_53
		 jewel
		 UnitTest++
		 sqloxx)
		 # NEED to add wxWidgets libraries here
		 # NEED to add definitions required by wxWidgets here?
else ()
	include_directories(${immediate_directories}
	                    /usr/lib64/wx/include/gtk2-unicode-release-2.8
						/usr/include/wx-2.8
						/usr/lib/wx/include/gtk2-unicode-release-2.8
						/usr/include/wx-2.8)
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
		 pthread
		 UnitTest++
		 sqloxx
		 wx_gtk2u_richtext-2.8
		 wx_gtk2u_aui-2.8
		 wx_gtk2u_xrc-2.8
		 wx_gtk2u_qa-2.8
		 wx_gtk2u_core-2.8
		 wx_baseu_xml-2.8
		 wx_baseu_net-2.8
		 wx_baseu-2.8)
	add_definitions(-D_FILE_OFFSET_BITS=64
	                -D_LARGE_FILES
					-D__WXGTK__)
endif ()


set (main_files
	 account/account.cpp
	 account/account_impl.cpp
	 account/account_reader.cpp
	 account/account_type.cpp
	 account/balance_cache.cpp
	 application/application.cpp
	 commodity/commodity.cpp
	 commodity/commodity_impl.cpp
	 consolixx/get_input.cpp
	 consolixx/table.cpp
	 consolixx/text_session.cpp
	 date/date.cpp
	 entry/entry.cpp
	 entry/entry_impl.cpp
	 entry/entry_reader.cpp
	 finformat/finformat.cpp
	 journal/draft_journal.cpp
	 journal/draft_journal_impl.cpp
	 journal/draft_journal_reader.cpp
	 journal/journal.cpp
	 journal/proto_journal.cpp
	 journal/ordinary_journal.cpp
	 journal/ordinary_journal_impl.cpp
	 journal/ordinary_journal_reader.cpp
	 journal/persistent_journal.cpp
	 session/session.cpp
	 phatbooks_database_connection/phatbooks_database_connection.cpp
	 phatbooks_persistent_object/phatbooks_persistent_object.cpp
	 phatbooks_text_session/phatbooks_text_session.cpp
	 repeater/interval_type.cpp
	 repeater/repeater.cpp
	 repeater/repeater_impl.cpp
	 repeater/repeater_reader.cpp)

set (gui_files
	 graphical_session/graphical_session.cpp
	 graphical_session/wx/my_app.cpp
	 graphical_session/wx/my_frame.cpp)

set (test_files
	 ${main_files}
	 tests/test.cpp
	 tests/draft_journal_tests.cpp
	 tests/interval_type_tests.cpp
	 tests/ordinary_journal_tests.cpp
	 tests/phatbooks_tests_common.cpp
	 tests/repeater_tests.cpp)



add_executable (phatbooks main.cpp ${main_files} ${gui_files})
target_link_libraries (phatbooks ${libraries})

add_executable (phatbooks_test ${test_files})
target_link_libraries (phatbooks_test ${libraries})

if (WIN32)
	set (binary_installation_dir C:/bin)
else ()
	set (binary_installation_dir bin)
endif ()

install(TARGETS phatbooks DESTINATION ${binary_installation_dir})
					


