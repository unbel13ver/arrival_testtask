add_library(myaio src/lib.c src/utils.c inc/lib.h inc/utils.h)

set_target_properties(myaio PROPERTIES PUBLIC_HEADER inc/lib.h)

target_link_libraries(myaio -lpthread)
