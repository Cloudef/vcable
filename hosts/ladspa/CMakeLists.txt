find_package(LADSPA)
set_package_properties(LADSPA PROPERTIES TYPE REQUIRED PURPOSE "Enables LADSPA host")

if (LADSPA_FOUND)
   add_library(vcable-ladspa MODULE ladspa.c)
   target_link_libraries(vcable-ladspa PRIVATE vcable)
   add_host(vcable-ladspa "${CMAKE_INSTALL_LIBDIR}/ladspa/")
endif ()
