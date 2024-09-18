#include <cstdio>  /* printf/fopen/fclose/fread/fwrite */
#include <fstream>  /* ifstream */
#include <cstdlib> /* free/malloc */
#include <vector> /* vector */
#include "fuse.h"

#define BUFFER_SIZE 1<<16 /* 65K */

int  compare_files( char const * file1, char const * file2 ); /* like POSIX diff */

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
  std::ifstream F1( file1, std::ios::binary);
  if ( F1.fail() ) return 1;

  std::ifstream F2( file2, std::ios::binary);
  if ( F2.fail() ) return 2; // F1 will be closed automatically

  int size1 = 0, size2 = 0;
  F1.seekg(0, std::ios::end);
  size1 = F1.tellg();
  F1.seekg(0, std::ios::beg); /* go back to the beginning */

  F2.seekg(0, std::ios::end);
  size2 = F2.tellg();
  F2.seekg(0, std::ios::beg); /* go back to the beginning */

  if ( size1 != size2 ) {
    printf( "+++Different sizes: %d != %d\n", size1, size2);
    return 3;
  } 

  std::vector<char> buffer1( BUFFER_SIZE );
  std::vector<char> buffer2( BUFFER_SIZE );

  int size_rem = size1;
  while ( F1.good() && F2.good() ) {
    int to_read = (size_rem < BUFFER_SIZE ) ? size_rem+1 : BUFFER_SIZE;
    size_rem -= to_read;
    F1.read( &buffer1[0], to_read );
    F2.read( &buffer2[0], to_read );

    for ( int i =0; i<to_read; ++ i ) {
      if ( buffer1[i] != buffer2[i] ) {
        printf( "+++Files %s and %s DIFFERat byte %i %i - %i\n", file1, file2, i, buffer1[i], buffer2[i] );
        return 7;
      }
    }
  }


  return 0;
}
