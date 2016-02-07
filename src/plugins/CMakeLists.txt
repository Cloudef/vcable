set(plugins jack)

macro(add_plugins)
   foreach (plugin ${ARGN})
      set_target_properties(${plugin} PROPERTIES PREFIX "")
   endforeach ()
   install(TARGETS ${ARGV} DESTINATION ${VCABLE_PLUGINS_PATH})
endmacro()

foreach (plugin ${plugins})
   add_subdirectory(${plugin})
endforeach ()
