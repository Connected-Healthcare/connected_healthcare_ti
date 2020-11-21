#include "unity.h"

// Include the source we wish to test
#include "line_buffer.h"

static line_buffer_s line_buffer;
static char memory[8];

// This method re-initializes the line_buffer for the rest of the tests
void setUp(void) { line_buffer__init(&line_buffer, memory, sizeof(memory)); }

void tearDown(void) {}

void test_line_buffer__nominal_case(void) {
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "abc");
}

void test_line_buffer__slash_r_slash_n_case(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, '\r');
  line_buffer__add_byte(&line_buffer, '\n');

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab\r");
}

// Line buffer should be able to add multiple lines and we should be able to remove them one at a time
void test_line_buffer__multiple_lines(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, '\n');
  line_buffer__add_byte(&line_buffer, 'c');
  line_buffer__add_byte(&line_buffer, 'd');
  line_buffer__add_byte(&line_buffer, '\n');

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab");

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "cd");
}

void test_line_buffer__overflow_case(void) {
  // Add chars until full capacity
  for (size_t i = 0; i < sizeof(memory); i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }

  // Buffer should be full now
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, 'b'));

  // Retreive truncated output (without the newline char)
  // Do not modify this test; instead, change your API to make this test pass
  // Note that line buffer was full with "abcdefgh" but we should only
  // retreive "abcdefg" because we need to write NULL char to line[8]
  char line[8] = {0};
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "abcdefg");
}

/**
 * ADDED CASES
 */

void test_remove_line_without_newline(void) {
  for (size_t i = 0; i < sizeof(memory) - 2; i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }

  static char line[8] = {0};
  TEST_ASSERT_FALSE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "");
}

void test_add_byte_overflow(void) {
  for (size_t i = 0; i < sizeof(memory); i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, 'z'));
}

void test_add_remove_line_after_rollover(void) {
  // Add 1st
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  // Add 2nd
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  char data[8] = {0};

  // Read 1st (create a rollover hole)
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, data, sizeof(data)));
  TEST_ASSERT_EQUAL_STRING(data, "a");

  // Add 3 (Add to rollover hole)
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '1'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '2'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  // IMP:Cannot add more since write == read and capacity != 0
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, '4'));

  // Read 2nd
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, data, sizeof(data)));
  TEST_ASSERT_EQUAL_STRING(data, "bcde");

  // IMP: Read 3rd (read overflow case)
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, data, sizeof(data)));
  TEST_ASSERT_EQUAL_STRING(data, "12");

  // False Read
  TEST_ASSERT_FALSE(line_buffer__remove_line(&line_buffer, data, sizeof(data)));
  TEST_ASSERT_EQUAL_STRING(data, "");

  // Write again after empty (not starting a 0)
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '1'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '2'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, data, sizeof(data)));
  TEST_ASSERT_EQUAL_STRING(data, "12");
}

void test_add_byte_two_newlines(void) {
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '1'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '2'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '3'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  char line[8] = {0};
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "123");

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "");
}

void test_add_byte_rollover_two_newline(void) {
  char line[8] = {0};

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '1'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '2'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '3'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "123");

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '4'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '5'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '6'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "456");

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "");
}

void test_remove_line_empty() {
  char line[8] = {0};
  TEST_ASSERT_FALSE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "");
}

void test_remove_line_small_input_buffer() {
  char input_buffer[4] = {0};

  for (size_t i = 0; i < sizeof(memory) - 1; i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  // Buffer should be full now
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, 'z'));

  // Read data when the input_buffer is small
  // NOTE, d gets read and is set to 0
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, input_buffer, sizeof(input_buffer)));
  TEST_ASSERT_EQUAL_STRING(input_buffer, "abc");

  // Read the remaining data
  // h is read and set to 0
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, input_buffer, sizeof(input_buffer)));
  TEST_ASSERT_EQUAL_STRING(input_buffer, "efg");
}

void test_remove_line_small_input_buffer_opposite() {
  char input_buffer[4] = {0};

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'f'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'g'));

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, input_buffer, sizeof(input_buffer)));
  TEST_ASSERT_EQUAL_STRING(input_buffer, "abc");

  // should be false since capacity is not full and we have not received new line
  TEST_ASSERT_FALSE(line_buffer__remove_line(&line_buffer, input_buffer, sizeof(input_buffer)));
  TEST_ASSERT_EQUAL_STRING(input_buffer, "");

  // make the case pass by adding \n
  // we get defg inside input buffer of LENGTH 4
  // we get def\0 (after making the last character 0)
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, input_buffer, sizeof(input_buffer)));
  TEST_ASSERT_EQUAL_STRING(input_buffer, "def");
}
