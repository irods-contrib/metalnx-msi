cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR) #CPACK_DEBIAN_<COMPONENT>_PACKAGE_NAME

find_package(IRODS 4.2.0 EXACT REQUIRED)

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

project(metalnx-msi-plugins C CXX)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

include(${IRODS_TARGETS_PATH})
include_directories(/usr/include/libxml2)
include_directories(/usr/local/include)
#include_directories(/usr/local/lib)

set(IRODS_PLUGIN_VERSION_MAJOR "2")
set(IRODS_PLUGIN_VERSION_MINOR "0")
set(IRODS_PLUGIN_VERSION_PATCH "0")
set(IRODS_PLUGIN_VERSION "${IRODS_PLUGIN_VERSION_MAJOR}.${IRODS_PLUGIN_VERSION_MINOR}.${IRODS_PLUGIN_VERSION_PATCH}")

if (NOT CPACK_PACKAGING_INSTALL_PREFIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/" CACHE STRING "Package root path. \"/\" is correct for normal package builds.." FORCE)
  message(STATUS "Setting unspecified CPACK_PACKAGING_INSTALL_PREFIX to '${CPACK_PACKAGING_INSTALL_PREFIX}'. This is the correct setting for normal builds.")
endif()

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build {Debug, Release}." FORCE)
  message(STATUS "Setting unspecified CMAKE_BUILD_TYPE to '${CMAKE_BUILD_TYPE}'")
endif()

set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lboost_filesystem -lboost_system -lxml2 -lhts -lexif -stdlib=libc++ -Wl,-z,defs")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lboost_filesystem -lboost_system -lxml2 -lhts -lexif -stdlib=libc++ -Wl,-z,defs")
add_compile_options(-nostdinc++ -Wall -Wextra -Werror)
add_compile_options(-Wno-unused-parameter) # Until fix of irods_re_plugin.hpp
add_compile_options(-Wno-unneeded-internal-declaration) # Until fix of https://github.com/irods/irods/issues/3396
link_libraries(c++abi)
include_directories(${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1)
include_directories(/opt/irods-externals/boost)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

set(
  METALNX_MICROSERVICES
  msiget_illumina_meta
  msiobjget_microservices
  msiobjget_version
  msiobjjpeg_extract
  msiobjput_mdbam
  msiobjput_mdmanifest
  msiobjput_mdvcf
  msiobjput_populate
  )


foreach(IRODS_CURRENT_PLUGIN_TARGET ${METALNX_MICROSERVICES})
  add_library(
    ${IRODS_CURRENT_PLUGIN_TARGET}
    MODULE
    ${CMAKE_SOURCE_DIR}/microservices/${IRODS_CURRENT_PLUGIN_TARGET}/lib${IRODS_CURRENT_PLUGIN_TARGET}.cpp
    )
  target_include_directories(
    ${IRODS_CURRENT_PLUGIN_TARGET}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/microservices/core/include
    ${IRODS_INCLUDE_DIRS}
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    )
  target_link_libraries(
    ${IRODS_CURRENT_PLUGIN_TARGET}
    PRIVATE
    irods_server
    irods_common
    ${CURL_LIBRARIES}
    )
  target_compile_definitions(${IRODS_CURRENT_PLUGIN_TARGET} PRIVATE RODS_SERVER ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)
  set_property(TARGET ${IRODS_CURRENT_PLUGIN_TARGET} PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})
  install(
    TARGETS
    ${IRODS_CURRENT_PLUGIN_TARGET}
    LIBRARY
    DESTINATION ${IRODS_PLUGINS_DIRECTORY}/microservices
    )
endforeach()

#install(
#  FILES
#  ${CMAKE_SOURCE_DIR}/packaging/test_irods_microservice_plugins_curl.py
#  DESTINATION ${IRODS_HOME_DIRECTORY}/scripts/irods/test
#  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
#  )

#install(
#  FILES
#  ${CMAKE_SOURCE_DIR}/rules/curlGetObj.r
#  ${CMAKE_SOURCE_DIR}/rules/curlGetStr.r
#  ${CMAKE_SOURCE_DIR}/rules/curlPost.r
#  DESTINATION ${IRODS_HOME_DIRECTORY}/clients/icommands/test/rules
#  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
#  )


if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "ubuntu" OR IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "debian")
  if (NOT IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME)
    execute_process(
      COMMAND "lsb_release" "-s" "-c"
      RESULT_VARIABLE IRODS_PLUGIN_EXECUTE_PROCESS_RESULT_LINUX_DISTRIBUTION_VERSION_CODENAME
      OUTPUT_VARIABLE IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME
      )
    string(STRIP ${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME} IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME)
    if (NOT ${IRODS_PLUGIN_EXECUTE_PROCESS_RESULT_LINUX_DISTRIBUTION_VERSION_CODENAME} STREQUAL "0")
      message(FATAL_ERROR "Linux lsb_release shortname detection failed\n${IRODS_PLUGIN_EXECUTE_PROCESS_RESULT_LINUX_DISTRIBUTION_VERSION_CODENAME}")
    endif()
    set(IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME ${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME} CACHE STRING "Linux distribution version codename, e.g. {precise, wheezy, trusty, jessie, ...}." FORCE)
    message(STATUS "Setting unspecified IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME to '${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME}'")
  endif()
endif()

if (NOT CPACK_GENERATOR)
  set(CPACK_GENERATOR ${IRODS_CPACK_GENERATOR} CACHE STRING "CPack generator to use, e.g. {DEB, RPM, TGZ}." FORCE)
  message(STATUS "Setting unspecified CPACK_GENERATOR to ${CPACK_GENERATOR}. This is the correct setting for normal builds.")
endif()

if (CPACK_GENERATOR STREQUAL "RPM")
  set(IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX "-${IRODS_PLUGIN_VERSION}-1.${CMAKE_SYSTEM_PROCESSOR}")
elseif(CPACK_GENERATOR STREQUAL "DEB")
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR "amd64")
  endif()
  set(IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX "_${IRODS_PLUGIN_VERSION}~${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME}_${CMAKE_SYSTEM_PROCESSOR}")
else()
  set(IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX "-${IRODS_PLUGIN_VERSION}-${IRODS_LINUX_DISTRIBUTION_NAME}${IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_FILE_NAME "metalnx-msi-plugins${IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_VERSION ${IRODS_PLUGIN_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${IRODS_PLUGIN_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${IRODS_PLUGIN_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${IRODS_PLUGIN_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "Renaissance Computing Institute <info@irods.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The integrated Rule-Oriented Data System")
set(CPACK_PACKAGE_VENDOR "Renaissance Computing Institute <info@irods.org>")

set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://irods.org")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/postinst;")

set(CPACK_RPM_COMPONENT_INSTALL OFF)
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "BSD-3-Clause")
set(CPACK_RPM_PACKAGE_VENDOR "iRODS Consortium")
set(CPACK_RPM_PACKAGE_URL "https://irods.org")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/postinst")

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

set(CPACK_DEBIAN_PACKAGE_NAME "metalnx-msi-plugins")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime (= ${IRODS_CPACK_DEBIAN_PACKAGE_VERSION}), libc6, libcurl3")

set(CPACK_RPM_PACKAGE_NAME "metalnx-msi-plugins")
if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos" OR IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos linux")
  set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, curl-devel")
elseif (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "opensuse")
  set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, curl-devel")
endif()

include(CPack)
