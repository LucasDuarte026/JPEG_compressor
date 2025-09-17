#ifndef _IMAGE_UTILS_H_
#define _IMAGE_UTILS_H_
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define BLOCK_SIZE 8

// --- Qualidade 75% (Alta Qualidade, Baixa Compressão) ---

// Matriz de Luminância (Y) - Qualidade 75%
const int LUMINANCE_Q75[8][8] = {
    {8, 6, 5, 8, 12, 20, 26, 31},
    {6, 6, 7, 10, 13, 29, 30, 28},
    {7, 7, 8, 12, 20, 29, 35, 28},
    {7, 9, 11, 15, 26, 44, 40, 31},
    {9, 11, 19, 28, 34, 55, 52, 39},
    {12, 18, 28, 32, 41, 52, 57, 46},
    {25, 32, 39, 44, 52, 61, 60, 51},
    {36, 46, 48, 49, 56, 50, 52, 50}};

// Matriz de Crominância (Cb/Cr) - Qualidade 75%
const int CHROMINANCE_Q75[8][8] = {
    {9, 9, 12, 24, 50, 50, 50, 50},
    {9, 11, 13, 33, 50, 50, 50, 50},
    {12, 13, 28, 50, 50, 50, 50, 50},
    {24, 33, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50}};

// --- Qualidade 50% (Padrão/Base) ---

// Matriz de Luminância (Y) - Qualidade 50%
const int LUMINANCE_Q50[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}};

// Matriz de Crominância (Cb/Cr) - Qualidade 50%
const int CHROMINANCE_Q50[8][8] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}};

// --- Qualidade 25% (Baixa Qualidade, Alta Compressão) ---

// Matriz de Luminância (Y) - Qualidade 25%
const int LUMINANCE_Q25[8][8] = {
    {32, 22, 20, 32, 48, 80, 102, 122},
    {24, 24, 28, 38, 52, 116, 120, 110},
    {28, 26, 32, 48, 80, 114, 138, 112},
    {28, 34, 44, 58, 102, 174, 160, 124},
    {36, 44, 74, 112, 136, 218, 206, 154},
    {48, 70, 110, 128, 162, 208, 226, 184},
    {98, 128, 156, 174, 206, 242, 240, 202},
    {144, 184, 190, 196, 224, 200, 206, 198}};

// Matriz de Crominância (Cb/Cr) - Qualidade 25%
const int CHROMINANCE_Q25[8][8] = {
    {34, 36, 48, 94, 198, 198, 198, 198},
    {36, 42, 52, 132, 198, 198, 198, 198},
    {48, 52, 112, 198, 198, 198, 198, 198},
    {94, 132, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198}};
// matrix for DCT in 8x8 blocks
const double M_dct[8][8] = {
    {0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536},
    {0.4904, 0.4157, 0.2778, 0.0975, -0.0975, -0.2778, -0.4157, -0.4904},
    {0.4619, 0.1913, -0.1913, -0.4619, -0.4619, -0.1913, 0.1913, 0.4619},
    {0.4157, -0.0975, -0.4904, -0.2778, 0.2778, 0.4904, 0.0975, -0.4157},
    {0.3536, -0.3536, -0.3536, 0.3536, 0.3536, -0.3536, -0.3536, 0.3536},
    {0.2778, -0.4904, 0.0975, 0.4157, -0.4157, -0.0975, 0.4904, -0.2778},
    {0.1913, -0.4619, 0.4619, -0.1913, -0.1913, 0.4619, -0.4619, 0.1913},
    {0.0975, -0.2778, 0.4157, -0.4904, 0.4904, -0.4157, 0.2778, -0.0975}};

// Transpose of M_dct that is equal to the inverse of M_dct because M_dct is orthogonal
const double M_dct_t[8][8] = {
    {0.3536, 0.4904, 0.4619, 0.4157, 0.3536, 0.2778, 0.1913, 0.0975},
    {0.3536, 0.4157, 0.1913, -0.0975, -0.3536, -0.4904, -0.4619, -0.2778},
    {0.3536, 0.2778, -0.1913, -0.4904, -0.3536, 0.0975, 0.4619, 0.4157},
    {0.3536, 0.0975, -0.4619, -0.2778, 0.3536, 0.4157, -0.1913, -0.4904},
    {0.3536, -0.0975, -0.4619, 0.2778, 0.3536, -0.4157, -0.1913, 0.4904},
    {0.3536, -0.2778, -0.1913, 0.4904, -0.3536, -0.0975, 0.4619, -0.4157},
    {0.3536, -0.4157, 0.1913, 0.0975, -0.3536, 0.4904, -0.4619, 0.2778},
    {0.3536, -0.4904, 0.4619, -0.4157, 0.3536, -0.2778, 0.1913, -0.0975}};

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
    double Y;
    double Cb;
    double Cr;
} Pixel_YCbCr_d;
typedef struct
{
    double Cb;
    double Cr;
} Chromancy;

typedef struct
{
    uint32_t code; // O código de bits
    int length;    // O número de bits no código
} HuffmanCode;

// Estrutura para gerenciar a escrita bit a bit no arquivo
typedef struct
{
    FILE *file;
    uint8_t buffer;
    int bit_count;
} Bitstream;

// --- MAPA ZIG-ZAG PADRÃO ---
const int g_zigzag_map[64] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63};

// --- TABELAS HUFFMAN PADRÃO JPEG (Anexo K) ---

extern HuffmanCode g_huff_dc_luma[12];
extern HuffmanCode g_huff_dc_chroma[12];
extern HuffmanCode g_huff_ac_luma[256];
extern HuffmanCode g_huff_ac_chroma[256];


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
Pixel **loadBMPImage(FILE *input, BITMAPINFOHEADER *InfoHeader);
void freeImage(double ***matrix, int height, int width);

void free_blocks_Y(double ***blocks, int num_blocks);
void free_blocks_chr(Chromancy ***blocks, int num_blocks);

double ***allocate_blocks_Y(int num_blocks);
Chromancy ***allocate_blocks_chr(int num_blocks_chr);
void bitstream_init(Bitstream *stream, FILE *f) ;
static void write_byte(Bitstream *stream) ;
void write_bits(Bitstream *stream, uint16_t code, int length) ;
void bitstream_flush(Bitstream *stream) ;
void encode_block(double block[BLOCK_SIZE][BLOCK_SIZE], int *prev_dc, HuffmanCode *dc_table, HuffmanCode *ac_table, Bitstream *stream);


#endif
