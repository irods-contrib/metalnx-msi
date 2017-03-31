cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR) #CPACK_DEBIAN_<COMPONENT>_PACKAGE_NAME

find_package(IRODS 4.2.0 EXACT REQUIRED)

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

project(metalnx-msi-plugins C CXX)

set(MLX_MSI_VERSION_MAJOR "1")
set(MLX_MSI_VERSION_MINOR "2")
set(MLX_MSI_VERSION_PATCH "0")
set(MLX_MSI_VERSION "${MLX_MSI_VERSION_MAJOR}.${MLX_MSI_VERSION_MINOR}.${MLX_MSI_VERSION_PATCH}")
set(MLX_HEADERS_PATH "${PROJECT_SOURCE_DIR}/microservices/core/include")
set(MLX_CMAKE_HEADERS_PATH "${PROJECT_BINARY_DIR}/microservices/core/include")

configure_file (
  "${MLX_HEADERS_PATH}/metalnx_msi_version.h.in"
  "${MLX_CMAKE_HEADERS_PATH}/metalnx_msi_version.h"
)

include_directories(/usr/include/libxml2)
include_directories(${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1)
include_directories("${MLX_CMAKE_HEADERS_PATH}")

set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++ -Wl,-z,defs")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++ -Wl,-z,defs")
add_compile_options(-nostdinc++ -Wall -Wextra -Werror)
add_compile_options(-Wno-unused-parameter) # Until fix of irods_re_plugin.hpp
add_compile_options(-Wno-unneeded-internal-declaration) # Until fix of https://github.com/irods/irods/issues/3396
link_libraries(c++abi)
link_libraries(xml2)
link_libraries(exif)
link_libraries(boost_filesystem)
link_libraries(boost_system)
link_libraries(hts)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

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

set(CPACK_PACKAGE_NAME "metalnx-msi-plugins")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_VERSION ${MLX_MSI_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${MLX_MSI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MLX_MSI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${MLX_MSI_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "Dell EMC")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Metalnx Microservices")
set(CPACK_PACKAGE_VENDOR "Dell EMC")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}")
set(CPACK_PACKAGING_INSTALL_PREFIX "/")

set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://bintray.com/metalnx")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/postinst;")

set(CPACK_RPM_COMPONENT_INSTALL OFF)
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_SUMMARY "Metalnx MSI plugins")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Metalnx Microservices plugins for iRODS 4.2+")
set(CPACK_RPM_PACKAGE_LICENSE "Apache 2.0")
set(CPACK_RPM_PACKAGE_VENDOR "Dell Inc.")
set(CPACK_RPM_PACKAGE_URL "https://bintray.com/metalnx")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

set(CPACK_DEBIAN_PACKAGE_NAME "metalnx-msi-plugins")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libexif-dev, libxml++2.6-dev")

set(CPACK_RPM_PACKAGE_NAME "metalnx-msi-plugins")

include(CPack)
