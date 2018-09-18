add_compile_options(
    "-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
      "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
      )
    #add_library(fastfillarray fastfillarray.cpp navarroarray.cpp)
