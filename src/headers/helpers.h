//
// Created by Harrison on 9/8/2016.
//

#ifndef CS430_PROJ1_IMAGES_HELPERS_H
#define CS430_PROJ1_IMAGES_HELPERS_H

#include <stdio.h>
#include "image.h"

/**
 * Peeks the next character in the provided stream
 * @param fp - The file stream to peek into
 * @return - The character that was read
 */
int peek(FILE* fp);

/**
 * Scans over whitespace and comments in the provided file stream
 * @param fp - The file being read
 * @return the total number of characters skipped
 */
int skip_ignored_characters(FILE* fp);

/**
 * Scans over whitespace in the provided file stream
 * @param fp - The file being read
 * @return the total number of characters skipped
 */
int skip_whitespace(FILE* fp);

/**
 * Converts a width, x, and y coordinate to an index in a 1D array
 * @param width - the width of the grid
 * @param x - the x coordinate
 * @param y - the y coordinate
 * @return - the index in the array
 */
long wxy_to_index(int width, int x, int y);

/**
 * Converts an index (of 1D array) and width (of 2D grid) into x and y coordinates
 * @param index - The index in the 1D array
 * @param width - The width of the 2D grid
 * @param x - The pointer to where the x coordinate should be stored
 * @param y - The pointer to where the y coordinate should be stored
 */
void index_to_xwy(long index, int width, int* x, int* y);

/**
 * Constrains a value between a minimum and maximum value.
 * @param min The minimum value to return
 * @param max The maximum value to return
 * @param val The value to clamp
 * @return The clamped value
 */
double clamp(double min, double max, double val);

#endif //CS430_PROJ1_IMAGES_HELPERS_H
