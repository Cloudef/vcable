set(hosts ladspa)

macro(add_host)
   set_target_properties(${ARGV0} PROPERTIES PREFIX "")
   install(TARGETS ${ARGV0} DESTINATION "${ARGV1}")
endmacro()

foreach (host ${hosts})
   add_subdirectory(${host})
endforeach ()
