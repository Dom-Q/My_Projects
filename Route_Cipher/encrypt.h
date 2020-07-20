#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//------------//
// Encryption //
//------------//

// Encrypts the file with the Route Cipher
// Returns 1 if file successfully encrypted
int cipher(char* filename);

// Encrypts with diagonal route and returns the number of shifts per character per diagonal row
int diagonal_route(char** matrix, int size, int width, int height);

// Encrypts message using inward spiral route
int inward_spiral_route(char** matrix, int size, int width, int height);

//------------//
// Decryption //
//------------//

// Decipher a file encrypted using this program's encrytpion method
// Returns 1 if file successfully decrypted
int decipher(int shifts, int width);

// Decrypt Spiral Route from (0,0)
void reverse_inward_spiral(char** matrix, int shifts, int width, int height);

// Decrypts text encrypted using diagonal route
void reverse_diagonal_route(char** matrix, int shifts, int width, int height);

//---------//
// Helpers //
//---------//

// Counts the number of characters (whitespace and non-whitespace) in the file
// Returns 1 or 0; if 0, file is too large
int count_chars(FILE** file_ptr, int* chars, int* spaces, int* tabs, int* newlines);

// Returns a random row that is a factor of the size of the matrix
int get_row(int size);

// Find the row boundary in matrix of any given index
int find_row_bound(int quadrant, int index, int width, int height);

// Find the column boundary for any index in matrix
int find_col_bound(int quadrant, int index, int width, int height);

// Determine if current row bisects matrix
int is_row_median(int height, int row);

// Determine if current column bisects matrix
int is_col_median(int width, int col);

// Determine if col median exists
int col_med_exists(int width);

// Determine if row median exists
int row_med_exists(int height);

//--------------------------------//
// Matrix Manipulation Algorithms //
//--------------------------------//

// Recursive algorithm for finding new index of a char in the original array following the spiral route inward from (0,0)
// Returns new index
int find_inward_spiral_index(int shifts, int width, int height, int index);

// Recursive algorithm for finding the new index for any given char in from the original matrix
// Returns the index 
int find_reverse_spiral_index(int shifts, int width, int height, int index);

// Finds the end of the spiral route from (0,0) of a given matrix
int find_spiral_end(int width, int height);

// Returns the quadrant that the index passed to "find_inward_spiral_index" can be found in the matrix
// Returns 0 if error
int find_quadrant(int index, int width, int height);

#endif