cmake_minimum_required (VERSION 2.6)
project (main)
include_directories (/home/matthew/lib)
link_directories (/home/matthew/lib/jewel)
add_executable (main 
                main.cpp
		account.cpp
		application.cpp
		database.cpp
		identifier.cpp
		stored_object.cpp)
target_link_libraries (main unittest++)
target_link_libraries (main jewel)




