#include <stdio.h>  /* printf/fopen/fclose/fread/fwrite */
#include <stdlib.h> /* free/malloc */
#include <string.h> /* strlen */
#include "fuse.h"

#define BUFFER_SIZE 1<<16 /* 65K */

int  compare_files( char const * file1, char const * file2 ); /* like POSIX diff */
void test0(void);
void test1(void);
void test2(void);
void test3(void);
void test4(void);

void test0(void)
{
  char const * combined           = "test0_student_output";
  char const * expected_combined  = "test0_expected_output"; 
  char const * pp[] = { "test0_in1", "test0_in2", "test0_in3", "test0_in4" };
  fuse( pp, sizeof(pp)/sizeof(*pp), combined );
  if ( compare_files( combined, expected_combined ) == 0 ) {
    printf( "+++Looks good\n" );
  }
}

void test1(void)
{
  char const * combined           = "test1_student_output";
  char const * expected_combined  = "test1_expected_output"; 
  char const * pp[] = { "test1_in1", "test1_in2" };
  fuse( pp, sizeof(pp)/sizeof(*pp), combined );
  if ( compare_files( combined, expected_combined ) == 0 ) {
    printf( "+++Looks good\n" );
  }
}

void test2(void)
{
  char const * combined           = "test2_student_output";
  char const * expected_combined  = "test2_expected_output"; 
  char const * pp[] = { "test2_in1" };
  fuse( pp, sizeof(pp)/sizeof(*pp), combined );
  if ( compare_files( combined, expected_combined ) == 0 ) {
    printf( "+++Looks good\n" );
  }
}

void test3(void)
{
  char const * combined = "test3_input"; /* contains test3_student_out1, test3_student_out2, test3_student_out3 */
  unfuse( combined );
  if( 
  compare_files( "test3_student_out1", "test3_expected_out1" ) +
  compare_files( "test3_student_out2", "test3_expected_out2" ) +
  compare_files( "test3_student_out3", "test3_expected_out3" ) == 0 ) {
    printf( "+++Looks good\n" );
  }

}

void test4(void)
{
  char const * combined = "test4_input"; /* contains 5 (larger) files test4_student_out1 etc */
  unfuse( combined );
  if ( 
  compare_files( "test4_student_out1", "test4_expected_out1" ) +
  compare_files( "test4_student_out2", "test4_expected_out2" ) +
  compare_files( "test4_student_out3", "test4_expected_out3" ) +
  compare_files( "test4_student_out4", "test4_expected_out4" ) +
  compare_files( "test4_student_out5", "test4_expected_out5" ) == 0 ) {
    printf( "+++Looks good\n" );
  }
}

void (*pTests[])(void) = { 
  test0, test1, test2, test3, test4

};

int main(int argc, char *argv[] ) {
  if (argc >1) {
    int test = 0;
    sscanf(argv[1],"%i",&test);
    pTests[test]();
  }
  return 0;
}


int compare_files( char const * file1, char const * file2 )
{
  FILE *F1, *F2;
  char * buffer1, *buffer2;
  int size1 = 0, size2 = 0;
  int ret_code = 0;

  F1 = fopen( file1, "rb" ); 
  if ( !F1 ) {
    printf( "+++Cannot open: %s\n", file1);
    ret_code = 1;
    goto END5;
  } 
  F2 = fopen( file2, "rb" );
  if ( !F2 ) {
    printf( "+++Cannot open: %s\n", file2);
    ret_code = 2;
    goto END4;
  } 

  fseek( F1, 0L, SEEK_END );
  size1 = ftell( F1 );
  fseek( F1, 0L, SEEK_SET ); /* go back to the beginning */

  fseek( F2, 0L, SEEK_END );
  size2 = ftell( F2 );
  fseek( F2, 0L, SEEK_SET ); /* go back to the beginning */

  if ( size1 != size2 ) {
    printf( "+++Different sizes: %d != %d\n", size1, size2);
    ret_code = 3;
    goto END3;
  } 
  buffer1 = (char*)malloc( BUFFER_SIZE );
  if ( !buffer1 ) {
    ret_code = 4;
    goto END3;
  }
  buffer2 = (char*)malloc( BUFFER_SIZE );
  if ( !buffer2 ) {
    ret_code = 5;
    goto END2;
  } else { 
    int pos=0;
    int bytes_read1 = 0;
    int bytes_read2 = 0;
    int size = size1;  /* sizes are equal by now */

    while ( pos < size ) {
      int i;
      bytes_read1 = fread( buffer1, 1, BUFFER_SIZE, F1 );
      bytes_read2 = fread( buffer2, 1, BUFFER_SIZE, F2 );
      if ( bytes_read1 != bytes_read2 ) {
        printf( "+++Something very bad happened - both files have the same size, but we could not read the same number of bytes\n" );
        ret_code = 6;
        goto END1;
      }
      /* compare bytes */
      for ( i=0; i<bytes_read1; ++i ) {
        if ( buffer1[i] != buffer2[i] ) {
          printf( "+++Files %s and %s DIFFER at byte %d\n", file1, file2, pos+i );
          ret_code = 7;
          goto END1;
        }
      }
      pos += bytes_read1;
    }
  }
END1: free( buffer2 );
END2: free( buffer1 );
END3: fclose( F2 );
END4: fclose( F1 );
END5: /* nothing to clean up */
      return ret_code;
}
