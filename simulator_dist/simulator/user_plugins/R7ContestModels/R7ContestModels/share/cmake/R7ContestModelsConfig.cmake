#Copyright (c) 2021-2025 Air Systems Research Center, Acquisition, Technology & Logistics Agency(ATLA)
#----------------------------------------------------------------
# xxxConfig.cmake template for ASRCAISim1's user_plugin
#----------------------------------------------------------------

# ==============================================
# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.19)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
get_filename_component(_PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(_PACKAGE_NAME "${_PACKAGE_PREFIX_DIR}" NAME_WE)
message("_PACKAGE_PREFIX_DIR: " "${_PACKAGE_PREFIX_DIR}")
message("Package Name: " ${_PACKAGE_NAME})
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget ${_PACKAGE_NAME})
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)
unset(_PACKAGE_NAME)
# ==============================================


get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(PACKAGE_NAME "${PACKAGE_PREFIX_DIR}" NAME_WE)
message(STATUS "Package Name:${PACKAGE_NAME}")
if(EXISTS "${PACKAGE_PREFIX_DIR}/BuildIdentifier.txt")
  file(
    READ
    "${PACKAGE_PREFIX_DIR}/BuildIdentifier.txt"
    "${PACKAGE_NAME}BUILD_IDENTIFIER"
  )
endif()
set("${PACKAGE_NAME}_INCLUDE_DIR" "${PACKAGE_PREFIX_DIR}/include")
set("${PACKAGE_NAME}_LIBRARY_DIR" "${PACKAGE_PREFIX_DIR}")
find_library("${PACKAGE_NAME}_LIBRARY"
  NAMES
    ${PACKAGE_NAME}
  PATHS
    "${PACKAGE_PREFIX_DIR}"
  NO_DEFAULT_PATH
)
if(NOT "${${PACKAGE_NAME}BUILD_IDENTIFIER}" STREQUAL "")
  set("${PACKAGE_NAME}_INCLUDE_DIR_B" "${PACKAGE_PREFIX_DIR}/include/${${PACKAGE_NAME}BUILD_IDENTIFIER}")
  find_library("${PACKAGE_NAME}_LIBRARY_B"
    NAMES
      "${PACKAGE_NAME}${${PACKAGE_NAME}BUILD_IDENTIFIER}"
    PATHS
      "${PACKAGE_PREFIX_DIR}"
    NO_DEFAULT_PATH
  )
  set("${PACKAGE_NAME}_LIBRARY_DIR_B" "${PACKAGE_PREFIX_DIR}")
else()
  set("${PACKAGE_NAME}_INCLUDE_DIR_B" "${${PACKAGE_NAME}_INCLUDE_DIR}")
  set("${PACKAGE_NAME}_LIBRARY_B" "${${PACKAGE_NAME}_LIBRARY}")
  set("${PACKAGE_NAME}_LIBRARY_DIR_B" "${PACKAGE_PREFIX_DIR}")
endif()
message(STATUS "LIBRARY:${${PACKAGE_NAME}_LIBRARY}")
message(STATUS "LIBRARY_B:${${PACKAGE_NAME}_LIBRARY_B}")

if(EXISTS ${PACKAGE_PREFIX_DIR}/thirdParty/include)
  set("${PACKAGE_NAME}_INCLUDE_DIRS" "${${PACKAGE_NAME}_INCLUDE_DIR}" "${PACKAGE_PREFIX_DIR}/thirdParty/include")
  set("${PACKAGE_NAME}_INCLUDE_DIRS_B" "${${PACKAGE_NAME}_INCLUDE_DIR_B}" "${PACKAGE_PREFIX_DIR}/thirdParty/include")
else()
  set("${PACKAGE_NAME}_INCLUDE_DIRS" "${${PACKAGE_NAME}_INCLUDE_DIR}")
  set("${PACKAGE_NAME}_INCLUDE_DIRS_B" "${${PACKAGE_NAME}_INCLUDE_DIR_B}")
endif()
if(EXISTS ${PACKAGE_PREFIX_DIR}/thirdParty/lib)
  set("${PACKAGE_NAME}_LIBRARY_DIRS" "${${PACKAGE_NAME}_LIBRARY_DIR}" "${PACKAGE_PREFIX_DIR}/thirdParty/lib")
  set("${PACKAGE_NAME}_LIBRARY_DIRS_B" "${${PACKAGE_NAME}_LIBRARY_DIR_B}" "${PACKAGE_PREFIX_DIR}/thirdParty/lib")
else()
  set("${PACKAGE_NAME}_LIBRARY_DIRS" "${${PACKAGE_NAME}_LIBRARY_DIR}")
  set("${PACKAGE_NAME}_LIBRARY_DIRS_B" "${${PACKAGE_NAME}_LIBRARY_DIR_B}")
endif()

add_library("${PACKAGE_NAME}::All" SHARED IMPORTED)
set_target_properties("${PACKAGE_NAME}::All" PROPERTIES
  IMPORTED_LOCATION "${${PACKAGE_NAME}_LIBRARY}"
  INTERFACE_INCLUDE_DIRECTORIES "${${PACKAGE_NAME}_INCLUDE_DIRS}"
)
if(WIN32)
  set_target_properties("${PACKAGE_NAME}::All" PROPERTIES
    IMPORTED_IMPLIB "${${PACKAGE_NAME}_LIBRARY}"
  )
endif()

set("${PACKAGE_NAME}_LIBRARIES" "${${PACKAGE_NAME}_LIBRARY}")
set("${PACKAGE_NAME}_LIBRARIES_B" "${${PACKAGE_NAME}_LIBRARY_B}")

add_library("${PACKAGE_NAME}::All_B" SHARED IMPORTED)
set_target_properties("${PACKAGE_NAME}::All_B" PROPERTIES
  IMPORTED_LOCATION "${${PACKAGE_NAME}_LIBRARY_B}"
  INTERFACE_INCLUDE_DIRECTORIES "${${PACKAGE_NAME}_INCLUDE_DIRS_B}"
)
if(WIN32)
  set_target_properties("${PACKAGE_NAME}::All_B" PROPERTIES
    IMPORTED_IMPLIB "${${PACKAGE_NAME}_LIBRARY_B}"
  )
endif()

#add third party dependency cmake paths
if(EXISTS "${PACKAGE_PREFIX_DIR}/thirdParty/thirdPartyConfigPath.cmake")
  include("${PACKAGE_PREFIX_DIR}/thirdParty/thirdPartyConfigPath.cmake")
endif()

# ==============================================
# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
