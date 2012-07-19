cmake_minimum_required (VERSION 2.6)
project (main)
include_directories (/home/matthew/lib)
link_directories (/home/matthew/lib/jewel)
add_executable (main
				database.cpp
                main.cpp
				session.cpp)
target_link_libraries (main
					   boost_filesystem
					   boost_system
                       unittest++
					   jewel
					   sqlite3)




