cmake_minimum_required (VERSION 2.6)
project (main)
include_directories (/home/matthew/lib
					 /home/matthew/Workbench/versioned/phatbooks
                     /home/matthew/Workbench/versioned/phatbooks/account
					 /home/matthew/Workbench/versioned/phatbooks/commodity
					 /home/matthew/Workbench/versioned/phatbooks/date
					 /home/matthew/Workbench/versioned/phatbooks/entry
					 /home/matthew/Workbench/versioned/phatbooks/import_from_nap
					 /home/matthew/Workbench/versioned/phatbooks/journal
					 /home/matthew/Workbench/versioned/phatbooks/phatbooks_database_connection
					 /home/matthew/Workbench/versioned/phatbooks/phatbooks_text_session
					 /home/matthew/Workbench/versioned/phatbooks/repeater)
link_directories (/home/matthew/lib/jewel)
add_executable (main
				account/account.cpp
				commodity/commodity.cpp
				consolixx/consolixx.cpp
				date/date.cpp
				entry/entry.cpp
				import_from_nap/import_from_nap.cpp
				journal/draft_journal.cpp
				journal/journal.cpp
				journal/ordinary_journal.cpp
                main.cpp
				sqloxx/shared_sql_statement.cpp
				sqloxx/database_connection.cpp
				sqloxx/sql_statement_manager.cpp
				phatbooks_database_connection/phatbooks_database_connection.cpp
				phatbooks_text_session/phatbooks_text_session.cpp
				repeater/repeater.cpp
				sqloxx/sql_statement.cpp)
target_link_libraries (main
					   boost_date_time
					   boost_filesystem
					   boost_regex
					   boost_system
                       unittest++
					   jewel
					   sqlite3)




