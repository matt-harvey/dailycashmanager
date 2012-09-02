cmake_minimum_required (VERSION 2.6)
project (main)
include_directories (/home/matthew/lib)
link_directories (/home/matthew/lib/jewel)
add_executable (main
				account.cpp
				account_storage_manager.cpp
				commodity.cpp
				commodity_storage_manager.cpp
				consolixx.cpp
				date.cpp
				entry.cpp
				journal.cpp
				journal_storage_manager.cpp
                main.cpp
				database_connection.cpp
				phatbooks_database_connection.cpp
				phatbooks_text_session.cpp
				repeater.cpp
				sql_statement.cpp)
target_link_libraries (main
					   boost_filesystem
					   boost_regex
					   boost_system
                       unittest++
					   jewel
					   sqlite3)




