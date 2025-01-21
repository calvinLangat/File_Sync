add_library(Utils STATIC utils.cpp utils.h)

# Ensure debug symbols are enabled
target_compile_options(Utils PRIVATE $<$<CONFIG:Debug>:-g>)
