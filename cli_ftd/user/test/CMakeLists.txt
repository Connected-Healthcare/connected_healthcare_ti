# Test Runner for the User Module
add_executable(user_test user_test.c
    ../user.c
)
target_link_libraries(user_test PRIVATE unity fff test_includes)
add_test(NAME user_test COMMAND user_test)
