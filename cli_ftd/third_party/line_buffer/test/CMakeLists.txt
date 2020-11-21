add_executable(
  test_line_buffer test_line_buffer.c
  ../line_buffer.c
)
target_include_directories(test_line_buffer PRIVATE
  ../
)
target_link_libraries(
  test_line_buffer PRIVATE
  unity
  fff 
  test_includes
)

add_test(NAME test_line_buffer COMMAND test_line_buffer)
