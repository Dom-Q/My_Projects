#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Encrypts the file with the Route Cipher
int cipher(char* filename, int array_size);

// Counts the number of characters (whitespace and non-whitespace) in the file
// Returns 1 or 0; if 0, file is too large
int count_chars(FILE** file_ptr, int* chars, int* spaces, int* tabs, int* newlines);

// Returns a random row that is a factor of the size of the matrix
int get_row(int size);

// Encrypts with diagonal route and returns the number of shifts per character per diagonal row
int diagonal_route(char** matrix, int size, int width, int height);

// Encrypts message using inward spiral route
int inward_spiral_route(char** matrix, int size, int width, int height);

#endif