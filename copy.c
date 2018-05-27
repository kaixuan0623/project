#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time (because copy is a pixel-by-pixel transformation).
 *   2. Immediately write out each pixel.
 */
void copy_filter(Bitmap *bmp) {
    // the arugment *bmp only has the header data.
    // we have to keep reading from stdin to complete the bitmap.
    // the offset of the start of the pixel array is <header_size>.
    int pixel_array_size = bmp->width * bmp->height;  // how many pixels in total.
    Pixel pixel;
    for (int i = 0; i < pixel_array_size; i++) {
        // Since we have already done the read_header() before, the current offset is at <header_size>.
        fread(&pixel, sizeof(Pixel), 1, stdin);
        // write out the pixel immediately.
        fwrite(&pixel, sizeof(Pixel), 1, stdout);
    }
}

int main() {
    // Run the filter program with copy_filter to process the pixels.
    // You shouldn't need to change this implementation.
    run_filter(copy_filter, 1);
    return 0;
}
