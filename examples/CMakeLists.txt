project(example)


add_executable(example simple.c)
target_link_libraries(example CArgParse)
