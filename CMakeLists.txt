if(CONFIG_ZMK)
  target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/behavior_android_switcher.c)
endif()
