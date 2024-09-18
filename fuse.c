#include "fuse.h"
#include <stdio.h>  /* fopen/fclose/fread/fwrite */
#include <stdlib.h> /* malloc/free */
#include <string.h> /* strlen */

#define xDEBUG

#ifdef __cplusplus
extern "C" {
#endif

int fuse_file_append(const char *file_name, FILE *output_file) {
  FILE *input_file = NULL;
  int buffer_length = 8, position = 0;
  char *input_buffer = NULL;

  if (!output_file) {
    return E_BAD_DESTINATION;
  }

  /* Filename */
  fwrite(file_name, 1, 1 + strlen(file_name), output_file);

  input_file = fopen(file_name, "rb");
  if (!input_file) {
    return E_BAD_SOURCE;
  }

  input_buffer = calloc(buffer_length, 1);
  if (!input_buffer) {
    fclose(input_file);
    return E_NO_MEMORY;
  }

  while (!feof(input_file)) {
    position += fread(input_buffer + position, 1, (buffer_length - position),
                      input_file);

    if (position >= buffer_length) {
      buffer_length <<= 1;
      input_buffer = realloc(input_buffer, 1 * buffer_length);
      if (input_buffer == NULL) {
        fclose(input_file);
        return E_NO_MEMORY;
      }
    }
  }
  fclose(input_file);

  fwrite(&position, sizeof(position), 1, output_file);
  fwrite(input_buffer, 1, position, output_file);

  free(input_buffer);
  return 0;
}

int fuse(char const **filenames, int num_files, char const *output) {
  /* const int BUFFER_SIZE = 1 << 16; 65 Kb */
  FILE *output_file = fopen(output, "wb");
  int i = 0;
  if (!output_file) {
    return E_BAD_SOURCE;
  }

  for (; i < num_files; i++) {
    int err = fuse_file_append(filenames[i], output_file);
    if (err) {
      fclose(output_file);
      return err;
    }
  }
  fclose(output_file);

  return 0;
}

/* {FNAME}\0{LENGTH}{data...} */

int unfuse_file(FILE *fused_file) {
  char unfused_file_name[256] = {"\0"};
  FILE *unfused_file = NULL;
  void *buffer = NULL;
  int file_size = 0;
  int i = 0;

  if (fused_file == NULL) {
    return E_BAD_SOURCE;
  }

  for (i = 0; (size_t)i < sizeof(unfused_file_name); i++) {
    unfused_file_name[i] = getc(fused_file);

    if (unfused_file_name[i] == '\0') {
      break;
    }
  }

  /* read file size */
  if (fread(&file_size, sizeof(file_size), 1, fused_file) < 1) {
    return E_BAD_SOURCE;
  }

  /* open file */
  unfused_file = fopen(unfused_file_name, "wb");

  if (unfused_file == NULL) {
    return E_BAD_DESTINATION;
  }

  /* allocate buffer to store it */
  buffer = calloc(file_size, 1);
  if (buffer == NULL) {
    return E_NO_MEMORY;
  }

  /* read 'file_size' bytes */
  if (fread(buffer, 1, file_size, fused_file) < (size_t)file_size) {
    return E_BAD_SOURCE;
  }

  fwrite(buffer, 1, file_size, unfused_file);
  fclose(unfused_file);
  free(buffer);

  return 0;
}

int unfuse(char const *filename) {
  FILE *fused_file = fopen(filename, "rb");

  if (fused_file == NULL) {
    return E_BAD_SOURCE;
  }

  while (!feof(fused_file)) {
    const int err = unfuse_file(fused_file);
    if (err) {
      fclose(fused_file);
      return err;
    }
  }
  fclose(fused_file);

  return 0;
}

#ifdef __cplusplus
}
#endif
