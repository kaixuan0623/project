#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time (because copy is a pixel-by-pixel transformation).
 *   2. transform the pixel to grey scale.
 *
 * Note that this function should allocate space only for a single Pixel;
 */
void greyscale_filter(Bitmap *bmp) {
    // the arugment *bmp only has the header data.
    // we have to keep reading from stdin to complete the bitmap.
    // the offset of the start of the pixel array is <header_size>.
    int pixel_array_size = bmp->width * bmp->height;   // how many pixels in total.
    Pixel pixel;
    Pixel q;
    for (int i = 0; i < pixel_array_size; i++) {
        // Since we have already done the read_header() before, the current offset is at <header_size>.
        fread(&pixel, sizeof(Pixel), 1, stdin);
        // write out the pixel immediately.
        unsigned char grey = (pixel.blue + pixel.green + pixel.red) / 3;
        q.blue = grey;
        q.green = grey;
        q.red = grey;
        fwrite(&q, sizeof(Pixel), 1, stdout);
    }
}

int main() {
    run_filter(greyscale_filter, 1);
    return 0;
}
