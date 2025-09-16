#ifndef _IMAGE_UTILS_H_
#define _IMAGE_UTILS_H_
#include <stdio.h>
#define BF_TYPE 0x4D42 /* "MB" */

const int BLOCK_SIZE = 8; /* Size of the blocks 8x8 to all the */

// DCT matrix (Pre-calculated for performance)
const double dct_matrix[8][8] = {
    {0.354, 0.354, 0.354, 0.354, 0.354, 0.354, 0.354, 0.354},
    {0.490, 0.416, 0.278, 0.098, -0.098, -0.278, -0.416, -0.490},
    {0.462, 0.191, -0.191, -0.462, -0.462, -0.191, 0.191, 0.462},
    {0.416, -0.098, -0.490, -0.278, 0.278, 0.490, 0.098, -0.416},
    {0.354, -0.354, -0.354, 0.354, 0.354, -0.354, -0.354, 0.354},
    {0.278, -0.490, 0.098, 0.416, -0.416, -0.098, 0.490, -0.278},
    {0.191, -0.462, 0.462, -0.191, -0.191, 0.462, -0.462, 0.191},
    {0.098, -0.278, 0.416, -0.490, 0.490, -0.416, 0.278, -0.098}
};
const double dct_matrix_transposed[8][8] = {
    {0.354, 0.490, 0.462, 0.416, 0.354, 0.278, 0.191, 0.098},
    {0.354, 0.416, 0.191, -0.098, -0.354, -0.490, -0.462, -0.278},
    {0.354, 0.278, -0.191, -0.490, -0.354, 0.098, 0.462, 0.416},
    {0.354, 0.098, -0.462, -0.278, 0.354, 0.416, -0.191, -0.490},
    {0.354, -0.098, -0.462, 0.278, 0.354, -0.416, -0.191, 0.490},
    {0.354, -0.278, -0.191, 0.490, -0.354, -0.098, 0.462, -0.416},
    {0.354, -0.416, 0.191, 0.098, -0.354, 0.490, -0.462, 0.278},
    {0.354, -0.490, 0.462, -0.416, 0.354, -0.278, 0.191, -0.098}
};

typedef struct /**** BMP file header structure ****/
{
    unsigned short Type;      /* Magic number for file */
    unsigned int Size;        /* Size of file */
    unsigned short Reserved1; /* Reserved */
    unsigned short Reserved2; /* ... */
    unsigned int OffBits;     /* Offset to bitmap data */
} BITMAPFILEHEADER;

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} Pixel;

typedef struct
{
    unsigned char Y;
    unsigned char Cb;
    unsigned char Cr;
} Pixel_YCbCr;
typedef struct
{
    unsigned char Cb;
    unsigned char Cr;
} Chromancy;

typedef struct
{
    double R;
    double G;
    double B;
} Pixel_d;

typedef struct
{
    double Y;
    double Cb;
    double Cr;
} Pixel_YCbCr_d;
typedef struct
{
    double Cb;
    double Cr;
} Chromancy_d;

typedef struct /**** BMP file info structure ****/
{
    unsigned int Size;             /* Size of info header */
    int Width;                     /* Width of image */
    int Height;                    /* Height of image */
    unsigned short Planes;         /* Number of color planes */
    unsigned short BitCount;       /* Number of bits per pixel */
    unsigned int Compression;      /* Type of compression to use */
    unsigned int SizeImage;        /* Size of image data */
    int XResolution;               /* X pixels per meter */
    int YResolution;               /* Y pixels per meter */
    unsigned int NColours;         /* Number of colors used */
    unsigned int ImportantColours; /* Number of important colors */
} BITMAPINFOHEADER;

void loadBMPHeaders(FILE *fp, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);

void printHeaders(BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);
void leituraInfoHeader(FILE *F, BITMAPINFOHEADER *INFO_H);
void leituraHeader(FILE *F, BITMAPFILEHEADER *H);
void exportImage(char *output_filename, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader, Pixel **Image);
Pixel_d **loadBMPImage(FILE *input, BITMAPINFOHEADER *InfoHeader);
void freeImage(double ***matrix, int height, int width);


void free_blocks(double ***blocks, int num_blocks);
double ***allocate_blocks(int num_blocks);
#endif
