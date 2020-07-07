cmake_minimum_required(VERSION 3.16)
project(TwitterTGBot)

set(CMAKE_CXX_STANDARD 17)




# BUILD OPTIONS
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
add_compile_options(-Wall)
enable_testing()
include(GNUInstallDirs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/libmongoc-1.0")
include_directories("/usr/local/include/libbson-1.0")
include_directories("/usr/local/lib")



# EXTERNAL LIBRARIES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
find_package(Boost 1.58.0 REQUIRED COMPONENTS system iostreams)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_property(TARGET boost PROPERTY
    INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})


# SUBDIRECTORIES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add_subdirectory(networklib)
add_subdirectory(twitterlib)


# CLANG TIDY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add_custom_target(compile_commands ALL
	COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] &&
        mv ${CMAKE_SOURCE_DIR}/build/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json || :)



add_executable(TwitterTGBot main.cpp)
target_link_libraries(TwitterTGBot twitterlib)
target_link_libraries(TwitterTGBot ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(TwitterTGBot ${LIBBSONCXX_LIBRARIES})
target_compile_features(TwitterTGBot PUBLIC cxx_std_17)
