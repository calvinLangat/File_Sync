add_executable(Server server.cpp server.h)
target_link_libraries(Server PRIVATE $<TARGET_OBJECTS:Utils>)
