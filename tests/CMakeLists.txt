project(CArgParseTest_)

#单命令的单一选项多值测试
add_executable(${PROJECT_NAME}single_arg test_single_arg.c)
target_link_libraries(${PROJECT_NAME}single_arg CArgParse)
add_test(${PROJECT_NAME}single_arg ${PROJECT_NAME}single_arg install -p testpackge1 testpackge2 testpackge3)

# 单命令的多选项测试
add_executable(${PROJECT_NAME}mult_arg test_mult_arg.c)
target_link_libraries(${PROJECT_NAME}mult_arg CArgParse)
add_test(${PROJECT_NAME}mult_arg ${PROJECT_NAME}mult_arg install -p testpackge1 testpackge2 testpackge3 -i www.test.com -f file1.txt file2.txt)

# 全局选项测试
add_executable(${PROJECT_NAME}global_arg test_global_arg.c)
target_link_libraries(${PROJECT_NAME}global_arg CArgParse)
add_test(${PROJECT_NAME}global_arg ${PROJECT_NAME}global_arg -v install -p testpackge1 testpackge2 testpackge3 -q)

# 子命令测试
add_executable(${PROJECT_NAME}subcommand test_subcommand.c)
target_link_libraries(${PROJECT_NAME}subcommand CArgParse)
add_test(${PROJECT_NAME}subcommand ${PROJECT_NAME}subcommand -v install tools -t tool1 tool2)

# 未知命令测试
add_executable(${PROJECT_NAME}unknow_command test_unknow_command.c)
target_link_libraries(${PROJECT_NAME}unknow_command CArgParse)
add_test(${PROJECT_NAME}unknow_command ${PROJECT_NAME}unknow_command unknow)
set_tests_properties(${PROJECT_NAME}unknow_command PROPERTIES WILL_FAIL TRUE)
set_tests_properties(${PROJECT_NAME}unknow_command PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR: Last command is not found")

# 未知命令选项测试,预期打印该命令的帮助信息
add_executable(${PROJECT_NAME}unknow_command_arg test_unknow_command.c)
target_link_libraries(${PROJECT_NAME}unknow_command_arg CArgParse)
add_test(${PROJECT_NAME}unknow_command_arg ${PROJECT_NAME}unknow_command_arg install --unknow)
set_tests_properties(${PROJECT_NAME}unknow_command_arg PROPERTIES WILL_FAIL TRUE)

# 未知命令选项测试,预期打印该命令的帮助信息
add_executable(${PROJECT_NAME}val test_val.c)
target_link_libraries(${PROJECT_NAME}val CArgParse)
add_test(${PROJECT_NAME}val ${PROJECT_NAME}val file1.txt file2.txt file3.txt -v -q)