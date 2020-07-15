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
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)

if (CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIRS})
	add_definitions(-DHAVE_CURL)
endif()

# SUBDIRECTORIES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add_subdirectory(Twitter-API-C-Library/networklib)
add_subdirectory(Twitter-API-C-Library/twitterlib)
add_subdirectory(tgbot-cpp)



# CLANG TIDY
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add_custom_target(compile_commands ALL
	COMMAND [ -f ${CMAKE_SOURCE_DIR}/build/compile_commands.json ] &&
        mv ${CMAKE_SOURCE_DIR}/build/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json || :)



add_executable(TwitterTGBot main.cpp tg_bot.h tg_bot.cpp get_timeline.h get_timeline.cpp)
target_link_libraries(TwitterTGBot twitterlib)
target_link_libraries(TwitterTGBot ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(TwitterTGBot ${LIBBSONCXX_LIBRARIES})
target_compile_features(TwitterTGBot PUBLIC cxx_std_17)
target_link_libraries(TwitterTGBot TgBot ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})