#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"


/*
 * Read in bitmap header data from stdin, and return a pointer to
 * a new Bitmap struct containing the important metadata for the image file.
 *
 */
Bitmap *read_header() {
    // Move the offsets to 2 from the beginning.
    unsigned char a1;
    if (fread(&a1, 1, 1, stdin) != 1) {
        perror("fread");
        exit(1);
    }

    unsigned char a2;
    if (fread(&a2, 1, 1, stdin) != 1) {
        perror("fread");
        exit(1);
    }

    int file_size;
    if (fread(&file_size, sizeof(int), 1, stdin) != 1) {
        perror("fread");
        exit(1);
    }

    // jump the gap.
    unsigned char b1;
    fread(&b1, 1, 1, stdin);
    unsigned char b2;
    fread(&b2, 1, 1, stdin);
    unsigned char b3;
    fread(&b3, 1, 1, stdin);
    unsigned char b4;
    fread(&b4, 1, 1, stdin);

    int header_size;
    if (fread(&header_size, sizeof(int), 1, stdin) != 1) {
        perror("fread");
        exit(1);
    }

    // now we have the size of the header.Use this to allocate space for header.
    // used for header in Bitmap struct.
    unsigned char *header_data = malloc(header_size);

    memcpy(header_data, &a1, 1);
    memcpy(header_data + 1, &a2, 1);
    memcpy(header_data + 2, &file_size, 4);
    memcpy(header_data + 6, &b1, 1);
    memcpy(header_data + 7, &b2, 1);
    memcpy(header_data + 8, &b3, 1);
    memcpy(header_data + 9, &b4, 1);
    memcpy(header_data + 10, &header_size, 4);

    unsigned char c1;
    fread(&c1, 1, 1, stdin);
    unsigned char c2;
    fread(&c2, 1, 1, stdin);
    unsigned char c3;
    fread(&c3, 1, 1, stdin);
    unsigned char c4;
    fread(&c4, 1, 1, stdin);
    memcpy(header_data + 14, &c1, 1);
    memcpy(header_data + 15, &c2, 1);
    memcpy(header_data + 16, &c3, 1);
    memcpy(header_data + 17, &c4, 1);

    int width;
    if (fread(&width,  sizeof(int),  1, stdin) != 1) {
        perror("fread");
        exit(1);
    }
    memcpy(header_data + 18, &width, 4);

    int height;
    if (fread(&height, sizeof(int), 1, stdin) != 1) {
        perror("fread");
        exit(1);
    }
    memcpy(header_data + 22, &height, 4);

    // read the rest of the header.
    unsigned char u;
    for (int i = 26; i < header_size; i++) {
        fread(&u, 1, 1, stdin);
        memcpy(header_data + i, &u, 1);
    }

    Bitmap *bitmap = malloc(sizeof(Bitmap));  // pointer to Bitmap (struct).

    bitmap->headerSize = header_size;
    bitmap->header = header_data;
    bitmap->width = width;
    bitmap->height = height;

    return bitmap;
}

/*
 * Write out bitmap metadata to stdout.
 * You may add extra fprintf calls to *stderr* here for debugging purposes.
 */
void write_header(const Bitmap *bmp) {
    fwrite(bmp->header, bmp->headerSize, 1, stdout);
}

/*
 * Free the given Bitmap struct.
 */
void free_bitmap(Bitmap *bmp) {
    free(bmp->header);
    free(bmp);
}

/*
 * Update the bitmap header to record a resizing of the image.
 *
 * TODO: complete this function when working on the "scale" filter.
 *
 * Notes:
 *   1. As with read_header, use memcpy and the provided macros in bitmap.h.
 *
 *   2. bmp->header *must* be updated, as this is what's written out
 *      in write_header.
 *
 */
void scale(Bitmap *bmp, int scale_factor) {
    // for now, we already have the header_data.

    int new_width = bmp->width * scale_factor;
    int new_height = bmp->height * scale_factor;
    int new_size = new_width * new_height * sizeof(Pixel) + bmp->headerSize;

    memcpy(bmp->header + BMP_FILE_SIZE_OFFSET, &new_size, 4);

    memcpy(bmp->header + BMP_WIDTH_OFFSET, &new_width, 4);

    memcpy(bmp->header + BMP_HEIGHT_OFFSET, &new_height, 4);

}


/*
 * The "main" function.
 *
 * Run a given filter function, and apply a scale factor if necessary.
 * You don't need to modify this function to make it work with any of
 * the filters for this assignment.
 */
void run_filter(void (*filter)(Bitmap *), int scale_factor) {
    // Create a new Bitmap with the same header as the input Bitmap from stdin.
    Bitmap *bmp = read_header();

    if (scale_factor > 1) {
        scale(bmp, scale_factor);  // update the header_data.
    }

    write_header(bmp);

    // Note: here is where we call the filter function.
    // We construct the rest of the bitmap here.
    filter(bmp);

    free_bitmap(bmp);
}


/******************************************************************************
 * The gaussian blur and edge detection filters.
 *****************************************************************************/
const int gaussian_kernel[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}
};

const int kernel_dx[3][3] = {
    {1, 0, -1},
    {2, 0, -2},
    {1, 0, -1}
};

const int kernel_dy[3][3] = {
    {1, 2, 1},
    {0, 0, 0},
    {-1, -2, -1}
};

const int gaussian_normalizing_factor = 16;


Pixel apply_gaussian_kernel(Pixel *row0, Pixel *row1, Pixel *row2) {
    int b = 0, g = 0, r = 0;
    Pixel *rows[3] = {row0, row1, row2};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b += rows[i][j].blue * gaussian_kernel[i][j];
            g += rows[i][j].green * gaussian_kernel[i][j];
            r += rows[i][j].red * gaussian_kernel[i][j];
        }
    }

    b /= gaussian_normalizing_factor;
    g /= gaussian_normalizing_factor;
    r /= gaussian_normalizing_factor;

    Pixel new = {
        .blue = b,
        .green = g,
        .red = r
    };

    return new;
}


Pixel apply_edge_detection_kernel(Pixel *row0, Pixel *row1, Pixel *row2) {
    int b_dx = 0, b_dy = 0;
    int g_dx = 0, g_dy = 0;
    int r_dx = 0, r_dy = 0;
    Pixel *rows[3] = {row0, row1, row2};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b_dx += rows[i][j].blue * kernel_dx[i][j];
            b_dy += rows[i][j].blue * kernel_dy[i][j];
            g_dx += rows[i][j].green * kernel_dx[i][j];
            g_dy += rows[i][j].green * kernel_dy[i][j];
            r_dx += rows[i][j].red * kernel_dx[i][j];
            r_dy += rows[i][j].red * kernel_dy[i][j];
        }
    }
    int b = floor(sqrt(square(b_dx) + square(b_dy)));
    int g = floor(sqrt(square(g_dx) + square(g_dy)));
    int r = floor(sqrt(square(r_dx) + square(r_dy)));

    int edge_val = max(r, max(g, b));
    Pixel new = {
        .blue = edge_val,
        .green = edge_val,
        .red = edge_val
    };

    return new;
}
