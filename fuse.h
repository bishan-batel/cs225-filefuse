#ifndef FUSE_H
#define FUSE_H

#include <stddef.h> /* size_t definition */

#ifdef __cplusplus
extern "C" {
#endif

enum 
{E_BAD_SOURCE=1, E_BAD_DESTINATION, E_NO_MEMORY };

int unfuse( char const * filename );
int fuse( char const ** filenames, int num_files, char const * output);

#ifdef __cplusplus
}
#endif

#endif 

/* 
+-------------+--------+-----------+-------------+--------+-----------+....
|filename1    |filesize|data ......|filename1    |filesize|data ......|....
|\0 terminated| int    |           |\0 terminated| int    |           |....
+-------------+--------+-----------+-------------+--------+-----------+....
*/
