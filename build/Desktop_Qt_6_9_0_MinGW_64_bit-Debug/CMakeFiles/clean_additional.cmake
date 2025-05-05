# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\legilimens_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\legilimens_autogen.dir\\ParseCache.txt"
  "legilimens_autogen"
  )
endif()
