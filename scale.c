#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

int sf;  // the scale_factor.

void scale_filter(Bitmap *bmp) {
    // the arugment *bmp only has the header data.
    // we have to keep reading from stdin to complete the bitmap.
    // the offset of the pixel array is <header_size>.
    Pixel p;
    Pixel scaled_pixels[bmp->height * sf][bmp->width * sf];

    for (int row = 0; row < bmp->height; row++) {
        for (int col = 0; col < bmp->width; col++) {
            // process one pixel at a time.
            if (fread(&p, sizeof(Pixel), 1, stdin) != 1) {
                perror("fread");
                exit(1);
            }

            for (int i = 0; i < sf; i++) {
                for (int j = 0; j < sf; j++) {
                    scaled_pixels[row * sf + i][col * sf + j] = p;
                }
            }

        }
    }

    // write out the transformed pixels to stdout.
    Pixel q;
    for (int row_in = 0; row_in < bmp->height * sf; row_in++) {
        for (int col_in = 0; col_in < bmp->width * sf; col_in++) {
            q = scaled_pixels[row_in][col_in];
            fwrite(&q, sizeof(Pixel), 1, stdout);
        }
    }

    // update bmp->width and bmp->height here.
    bmp->height *= sf;
    bmp->width *= sf;
}

int main(int argc, char const *argv[]) {
    // the 1st command-line argument is sf.
    sf = strtol(argv[1], NULL, 10);
    if (sf < 0) {
        printf("the scale_factor must be an integer greater than 0.\n");
        exit(1);
    }
    run_filter(scale_filter, sf);
    return 0;
}
