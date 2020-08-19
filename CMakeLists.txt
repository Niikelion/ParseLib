cmake_minimum_required(VERSION 3.9.2)
project(parselib VERSION 1.0.0 DESCRIPTION "Parse library")

set(XML_SOURCES
	src/xml/xml.cpp
	include/parselib/XML/xml.hpp
	include/parselib/core/core.hpp
)

set(JSON_SOURCES
	src/json/json.cpp
	include/parselib/JSON/json.hpp
	include/parselib/core/core.hpp
)

add_library(xml ${XML_SOURCES})

target_include_directories(xml INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(xml PRIVATE
	include/parselib/core
	include/parselib/XML)

add_library(parselib::xml ALIAS xml)

add_library(json ${JSON_SOURCES})

target_include_directories(json INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(json PRIVATE
	include/parselib/core
	include/parselib/JSON)

add_library(parselib::json ALIAS json)

install(TARGETS xml json
	EXPORT parselibTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    parselibConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

install(EXPORT parselibTargets
        FILE parselibTargets.cmake
        NAMESPACE parselib::
        DESTINATION lib/cmake/parselib
         )
		 
configure_file(parselibConfig.cmake.in parselibConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/parselibConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/parselibConfigVersion.cmake"
        DESTINATION lib/cmake/parselib
        )
		
export(TARGETS xml json NAMESPACE parselib:: FILE parselibTargets.cmake)