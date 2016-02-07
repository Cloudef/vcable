CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(vcable VERSION 0.0.1 LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/CMake")

# CPack
set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pkg")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Virtual routing cable")
set(CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION ON)

# Includes
include(GetGitRevisionDescription)
include(GNUInstallDirs)
include(CPack)
set(VCABLE_PLUGINS_FULL_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/vcable" CACHE STRING "Systemwide plugins path (absolute)" FORCE)
set(VCABLE_PLUGINS_PATH "${CMAKE_INSTALL_LIBDIR}/vcable" CACHE STRING "Systemwide plugins path" FORCE)

# Compiler options
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
   include(GCCCompatibleCompilerOptions)
   add_compiler_warnings(-Wall -Wextra -Wno-variadic-macros -Wno-long-long -Wformat=2 -Winit-self -Wfloat-equal -Wcast-align -Wpointer-arith -Wmissing-prototypes -Wsuggest-attribute=pure -Wsuggest-attribute=const)
   create_custom_linker_flags(Upstream -O1 --sort-common --as-needed -z,relro -z,now)
   check_c_compiler_flag(-fstack-protector-strong ok)
   if (ok)
      create_custom_compiler_flags(Upstream -g -O2 -fstack-protector-strong --param ssp-buffer-size=4 -fstack-check -flto)
   else ()
      create_custom_compiler_flags(Upstream -g -O2 -fstack-protector-all --param ssp-buffer-size=4 -fstack-check -flto)
   endif ()
endif ()

# -std=c99 -fpic -fpie -D_GNU_SOURCE
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-D_GNU_SOURCE)

get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file(plugin.h.in include/vcable/plugin.h @ONLY)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include" src)
add_subdirectory(src)
add_subdirectory(hosts)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/vcable" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
   feature_summary(WHAT ALL)
endif ()
