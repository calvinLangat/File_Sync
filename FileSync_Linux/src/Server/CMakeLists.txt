add_executable(Server server.cpp server.h)

target_include_directories(Server PRIVATE ${CMAKE_SOURCE_DIR}/src/Utils)

# Link with Utils
target_link_libraries(Server PRIVATE Utils)

# Ensure debug symbols are enabled
target_compile_options(Server PRIVATE $<$<CONFIG:Debug>:-g>)

