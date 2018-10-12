#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Read in the location of the pixel array, the image width, and the image 
 * height in the given bitmap file.
 */
void read_bitmap_metadata(FILE *image, int *pixel_array_offset, int *width, int *height) {
    int error = 0;
    fseek(image, 10, SEEK_SET);
    error = fread(pixel_array_offset, sizeof(int), 1, image);
    if (error != 1) 
    {
        fprintf(stderr, "Error: could not read the pixel_array_offset");
        exit(1);
    }

    fseek(image, 18, SEEK_SET);
    error = fread(width, sizeof(int), 1, image);
    if (error != 1)
    {
        fprintf(stderr, "Error: could not read the width");
        exit(1);
    }

    fseek(image, 22, SEEK_SET);
    error = fread(height, sizeof(int), 1, image);
    if (error != 1)
    {
        fprintf(stderr, "Error: could not read the height");
        exit(1);
    }
}

/*
 * Read in pixel array by following these instructions:
 *
 * 1. First, allocate space for m `struct pixel *` values, where m is the 
 *    height of the image.  Each pointer will eventually point to one row of 
 *    pixel data.
 * 2. For each pointer you just allocated, initialize it to point to 
 *    heap-allocated space for an entire row of pixel data.
 * 3. Use the given file and pixel_array_offset to initialize the actual 
 *    struct pixel values. Assume that `sizeof(struct pixel) == 3`, which is 
 *    consistent with the bitmap file format.
 *    NOTE: We've tested this assumption on the Teaching Lab machines, but 
 *    if you're trying to work on your own computer, we strongly recommend 
 *    checking this assumption!
 * 4. Return the address of the first `struct pixel *` you initialized.
 */
struct pixel **read_pixel_array(FILE *image, int pixel_array_offset, int width, int height) {
    struct pixel **result = malloc(sizeof(struct pixel *) * height);
    fseek(image, pixel_array_offset, SEEK_SET);
    int error = 0;
    int i = 0;
    int j = 0;
    for (i = 0; i < height; i++)
    {
        result[i] = malloc(sizeof(struct pixel *) * width);
        for (j = 0; j < width; j++)
        {
            error = fread(&result[i][j].blue, 1, 1, image);
            if (error != 1)
            {
                fprintf(stderr, "Error: could not read the blue");
            }
            
            error = fread(&result[i][j].green, 1, 1, image);
            if (error != 1)
            {
                fprintf(stderr, "Error: could not read the green");
            }
            
            error = fread(&result[i][j].red, 1, 1, image);
            if (error != 1)
            {
                fprintf(stderr, "Error: could not read the red");
            }
        }
    }
    return result;
}


/*
 * Print the blue, green, and red colour values of a pixel.
 * You don't need to change this function.
 */
void print_pixel(struct pixel p) {
    printf("(%u, %u, %u)\n", p.blue, p.green, p.red);
}
