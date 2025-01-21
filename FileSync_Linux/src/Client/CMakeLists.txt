add_executable(Client client.cpp client.h)

target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/src/Utils)

# Link with Utils
target_link_libraries(Client PRIVATE Utils)

# Ensure debug symbols are enabled
target_compile_options(Client PRIVATE $<$<CONFIG:Debug>:-g>)

