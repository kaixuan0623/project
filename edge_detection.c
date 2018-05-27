#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time.
 *   2. transform the pixel.
 *
 * Note that this function should allocate space only for a single Pixel;
 */
void edge_detection_filter(Bitmap *bmp) {
    // Try to read ,store and manipulate the first three row first.
    int width = bmp->width;
    int height = bmp->height;

    Pixel pixel_matrix[3][width];
    Pixel p;
    Pixel q;

    for (int row = 0; row < height; row++) {
        if (row == 0) { // transforming the 1st row, reads, stores the first three rows.

            for (int row_in = 0; row_in < 3; row_in++) {
                for (int col_in = 0; col_in < width; col_in++) {
                    fread(&p, sizeof(Pixel), 1, stdin);
                    pixel_matrix[row_in][col_in] = p;
                }
            }  // finish reading.

            for (int col_pro = 0; col_pro < width; col_pro++) {  // transforming pixel
                if (col_pro == 0) { // the 1st col.
                    q = apply_edge_detection_kernel(pixel_matrix[0], pixel_matrix[1], pixel_matrix[2]);
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else if (col_pro == width - 1) { // the last col.
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (width - 3), pixel_matrix[1] + (width - 3), pixel_matrix[2] + (width - 3));
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else {
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (col_pro - 1), pixel_matrix[1] + (col_pro - 1), pixel_matrix[2] + (col_pro - 1));
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                }
            }

        } else if (row == 1) {  // transforming the 2nd row, don't need to read in again.

            for (int col_pro = 0; col_pro < width; col_pro++) {  // transforming pixel
                if (col_pro == 0) { // the 1st col.
                    q = apply_edge_detection_kernel(pixel_matrix[0], pixel_matrix[1], pixel_matrix[2]);
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else if (col_pro == width - 1) { // the last col.
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (width - 3), pixel_matrix[1] + (width - 3), pixel_matrix[2] + (width - 3));
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else {
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (col_pro - 1), pixel_matrix[1] + (col_pro - 1), pixel_matrix[2] + (col_pro - 1));
                    fwrite(&q, sizeof(Pixel), 1, stdout);
                }
            }
        } else if (row == height - 1) {  // the last row. Don't read.

            for (int col_pro = 0; col_pro < width; col_pro++) {  // transforming pixel
                if (col_pro == 0) { // the 1st col.
                    q = apply_edge_detection_kernel(pixel_matrix[0], pixel_matrix[1], pixel_matrix[2]);  // Assuming the pixel_matrix already stored the info.
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else if (col_pro == width - 1) { // the last col.
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (width - 3), pixel_matrix[1] + (width - 3), pixel_matrix[2] + (width - 3));
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else {
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (col_pro - 1), pixel_matrix[1] + (col_pro - 1), pixel_matrix[2] + (col_pro - 1));
                    fwrite(&q, sizeof(Pixel), 1, stdout);
                }
            }

        } else {
            // General cases. Should be careful on the reading!
            for (int j = 0; j < width; j++) {
                pixel_matrix[0][j] = pixel_matrix[1][j];
            }

            for (int k = 0; k < width; k++) {
                pixel_matrix[1][k] = pixel_matrix[2][k];
            }

            for (int col_in = 0; col_in < width; col_in++) { // read 1 more row.
                fread(&p, sizeof(Pixel), 1, stdin);
                pixel_matrix[2][col_in] = p;
            }
            // finish reading.

            for (int col_pro = 0; col_pro < width; col_pro++) {  // transforming pixel
                if (col_pro == 0) { // the 1st col.
                    q = apply_edge_detection_kernel(pixel_matrix[0], pixel_matrix[1], pixel_matrix[2]);
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else if (col_pro == width - 1) { // the last col.
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (width - 3), pixel_matrix[1] + (width - 3), pixel_matrix[2] + (width - 3));
                    fwrite(&q, sizeof(Pixel), 1, stdout);

                } else {
                    q = apply_edge_detection_kernel(pixel_matrix[0] + (col_pro - 1), pixel_matrix[1] + (col_pro - 1), pixel_matrix[2] + (col_pro - 1));
                    fwrite(&q, sizeof(Pixel), 1, stdout);
                }
            }
        }
    }
}

int main() {
    run_filter(edge_detection_filter, 1);
    return 0;
}
