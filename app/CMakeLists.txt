set(SRV_SRC_FILES src/srv.cpp)

add_executable(srv ${SRV_SRC_FILES})

target_link_libraries(srv myaio -lm)
