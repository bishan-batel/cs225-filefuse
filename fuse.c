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
  fwrite(file_name, sizeof(char), 1 + strlen(file_name), output_file);

  input_file = fopen(file_name, "rb");
  if (!input_file) {
    return E_BAD_SOURCE;
  }

  input_buffer = calloc(buffer_length, sizeof(char));
  if (!input_buffer) {
    fclose(input_file);
    return E_NO_MEMORY;
  }

  while (!feof(input_file)) {
    position += fread(input_buffer + position, sizeof(char),
                      (buffer_length - position), input_file);

    /* so pissed rn, yesterday wasnt working cause i think runing xxd fucked up
     one of the files and it was comparing my output with a fucked up
     testX_expected_output */

    /* position += fread(input_buffer + position, sizeof(char), */
    /*                   (buffer_length - position), input_file); */

    if (position >= buffer_length) {
      buffer_length <<= 1;
      input_buffer = realloc(input_buffer, sizeof(char) * buffer_length);
      if (input_buffer == NULL) {
        fclose(input_file);
        return E_NO_MEMORY;
      }
    }
  }
  fclose(input_file);

  fwrite(&position, sizeof(position), 1, output_file);
  fwrite(input_buffer, sizeof(char), position, output_file);

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
  char unfused_file_name[256] = "\0";
  FILE *unfused_file = NULL;
  char *buffer = NULL;
  int file_size = 0;
  int i = 0;

  if (fused_file == NULL) {
    return E_BAD_SOURCE;
  }

  while ((unfused_file_name[i++] = getc(fused_file)) != '\0')
    ;

  /* for () { */
  /*   unfused_file_name[i] = fgetc(fused_file); */
  /**/
  /*   if (unfused_file_name[i] == '\0') { */
  /*     break; */
  /*   } */
  /* } */

  if (fread(&file_size, sizeof(file_size), 1, fused_file) < sizeof(file_size)) {
    return E_BAD_DESTINATION;
  }

  unfused_file = fopen(unfused_file_name, "wb");

  if (unfused_file == NULL) {
    return E_BAD_DESTINATION;
  }

  buffer = calloc(1, file_size);
  if (buffer == NULL) {
    return E_NO_MEMORY;
  }

  if (fread(&buffer, sizeof(char), file_size, fused_file) < (size_t)file_size) {
    return E_BAD_SOURCE;
  }

  fwrite(&buffer, sizeof(char), file_size, unfused_file);
  free(buffer);
  fclose(unfused_file);

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
