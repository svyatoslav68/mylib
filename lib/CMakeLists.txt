cmake_minimum_required(VERSION 3.10)

project(my_library)
SET(SOURCE_DIR ../src)
SET(HEADERS_DIR ../include)
SET(TARGET_GET_STR getstr)
SET(TARGET_MYSQL_CONNECTION connection)

list(APPEND CMAKE_MODULE_PATH $ENV{HOME}/cmake_modules)

find_package(Boost REQUIRED COMPONENTS program_options date_time)
find_package(MariaDBClient REQUIRED)
if(MariaDBClient_FOUND)
	include_directories(${MariaDBClient_INCLUDE_DIR})
	message("MariaDBClient_INCLUDE_DIR = ${MariaDBClient_INCLUDE_DIR}")
	message("MariaDBClient_LIBRARY = ${MariaDBClient_LIBRARY}")
else()
	message("Не получится собрать приложение,\nне найден MariaDBClient")
	exit()
endif()

add_library(${TARGET_GET_STR} STATIC ${SOURCE_DIR}/str_from_file.cpp)
target_include_directories(${TARGET_GET_STR} PUBLIC ${HEADERS_DIR})
if(Boost_FOUND)
	message("Boost_LIBRARIES = " ${Boost_LIBRARIES})
	target_link_libraries(${TARGET_GET_STR} ${Boost_LIBRARIES})
else()
	message("Не получится собрать библиотеку\n, не найден Boost")
endif()


add_library(${TARGET_MYSQL_CONNECTION} STATIC ${SOURCE_DIR}/mysql_connect.c)
target_include_directories(${TARGET_MYSQL_CONNECTION} PUBLIC ${HEADERS_DIR})
target_link_libraries(${TARGET_MYSQL_CONNECTION} ${MariaDBClient_LIBRARY})

set_target_properties(
	${TARGET_GET_STR} PROPERTIES
	CXX_STANDARD 17	#Собирать в соответствии со стандартом c17
	)

set_target_properties(
	${TARGET_MYSQL_CONNECTION} PROPERTIES
	C_STANDARD	11
	)
