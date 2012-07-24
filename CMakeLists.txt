cmake_minimum_required (VERSION 2.6)
project (main)
include_directories (/home/matthew/lib)
link_directories (/home/matthew/lib/jewel)
add_executable (main
				account.cpp
				sqloxx_exceptions.cpp
                main.cpp
				database_connection.cpp
				phatbooks_database_connection.cpp
				sql_statement.cpp)
target_link_libraries (main
					   boost_filesystem
					   boost_system
                       unittest++
					   jewel
					   sqlite3)




