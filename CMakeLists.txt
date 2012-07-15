cmake_minimum_required (VERSION 2.6)
project (main)
include_directories (/home/matthew/lib)
link_directories (/home/matthew/lib/jewel)
add_executable (main
                main.cpp
				create_database.cpp)
target_link_libraries (main unittest++ jewel sqlite3)




