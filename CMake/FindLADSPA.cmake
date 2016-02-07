#.rst:
# FindLADSPA
# -------
#
# Find LADSPA library
#
# Try to find LADSPA library. The following values are defined
#
# ::
#
#   LADSPA_FOUND         - True if LADSPA is available
#   LADSPA_INCLUDE_DIRS  - Include directories for LADSPA
#   LADSPA_DEFINITIONS   - List of definitions for LADSPA
#
#=============================================================================
# Copyright (c) 2015 Jari Vetoniemi
#
# Distributed under the OSI-approved BSD License (the "License");
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

include(FeatureSummary)
set_package_properties(LADSPA PROPERTIES
   URL "https://web.archive.org/web/20150620022645/http://www.ladspa.org/"
   DESCRIPTION "Linux Audio Developer's Simple Plugin API")

find_package(PkgConfig)
pkg_check_modules(PC_LADSPA QUIET LADSPA)
find_path(LADSPA_INCLUDE_DIRS NAMES ladspa.h HINTS ${PC_LADSPA_INCLUDE_DIRS})
set(LADSPA_DEFINITIONS ${PC_LADSPA_CFLAGS_OTHER})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LADSPA DEFAULT_MSG LADSPA_INCLUDE_DIRS)
mark_as_advanced(LADSPA_INCLUDE_DIRS LADSPA_DEFINITIONS)
