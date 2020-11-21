#include "line_buffer.h"

static bool line_buffer__has_newline(line_buffer_s *buffer);

/**
 * Initialize *line_buffer_s with the user provided buffer space and size
 * Use should initialize the buffer with whatever memory they need
 * @code
 *  char memory[256];
 *  line_buffer_s line_buffer = { };
 *  line_buffer__init(&line_buffer, memory, sizeof(memory));
 * @endcode
 */
void line_buffer__init(line_buffer_s *buffer, void *memory, size_t size) {
  buffer->max_size = size;
  buffer->memory = memory;

  buffer->write_index = 0;
  buffer->read_index = 0;
  buffer->capacity = 0;
}

// Adds a byte to the buffer, and returns true if the buffer had enough space to add the byte
bool line_buffer__add_byte(line_buffer_s *buffer, char byte) {
  if (buffer->write_index == buffer->read_index && buffer->capacity != 0) {
    return false;
  }

  ((char *)buffer->memory)[buffer->write_index] = byte;

  // Increment
  buffer->write_index = (buffer->write_index + 1) % buffer->max_size;
  buffer->capacity++; // will eventually equal to buffer->max_size;

  return true;
}

/**
 * If the line buffer has a complete line, it will remove that contents and save it to "char * line"
 * Note that the buffer may have multiple lines already in the buffer, so it will require multiple
 * calls to this function to empty out those lines
 *
 * The one corner case is that if the buffer is FULL, and there is no '\n' character, then you should
 * empty out the line to the user buffer even though there is no newline character
 *
 * @param line_max_size This is the max size of 'char * line' memory pointer
 *
 * if \n NOT received and memory is NOT full (return false)
 *
 */
bool line_buffer__remove_line(line_buffer_s *buffer, char *line, size_t line_max_size) {

  memset(line, 0, line_max_size); // O(n)

  if (buffer->capacity == 0 || (line_buffer__has_newline(buffer) == false && buffer->capacity != buffer->max_size)) {
    return false;
  }

  int l_counter = 0;
  while (1) {
    // Add memory here
    char c = ((char *)buffer->memory)[buffer->read_index];
    if (c != '\n') {
      line[l_counter] = c;
    }

    // State variables
    buffer->capacity--;
    buffer->read_index = (buffer->read_index + 1) % buffer->max_size;
    l_counter++;

    // NOTE: Break when
    // newline
    // write index and read index are same and no data present
    // trying to fill data into a tiny input_buffer
    if (c == '\n' || (buffer->read_index == buffer->write_index && buffer->capacity == 0) ||
        l_counter > line_max_size - 1) {
      break;
    }
  }

  // Make sure the last character is 0
  line[line_max_size - 1] = 0;
  return true;
}

/***
 *
 * STATIC FUNCTIONS
 *
 */
static bool line_buffer__has_newline(line_buffer_s *buffer) {
  bool has_newline = false;

  int read_index = buffer->read_index;
  while (read_index != buffer->write_index) {
    char c = ((char *)buffer->memory)[read_index];
    if (c == '\n') {
      has_newline = true;
      break;
    }

    read_index = (1 + read_index) % buffer->max_size;
  }

  return has_newline;
}
