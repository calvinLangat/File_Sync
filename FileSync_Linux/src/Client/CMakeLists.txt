add_executable(Client client.cpp client.h)
target_link_libraries(Client PRIVATE $<TARGET_OBJECTS:Utils>)
