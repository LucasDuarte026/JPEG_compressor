#ifndef _IMAGE_UTILS_H_
#define _IMAGE_UTILS_H_
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define BLOCK_SIZE 8

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

 

//==================================================================================
// HUFFMAN STRUCTURES
//==================================================================================

typedef struct
{
    uint32_t code;  // Huffman bit pattern
    int      length; // Number of bits in the code
} HuffmanCode;

typedef struct
{
    // Canonical Annex K JPEG tables
    HuffmanCode* dc_table;
    HuffmanCode* ac_table;
} HuffmanTable;

// Structure that manages bit-level writing to a file
typedef struct
{
    FILE* file;
    uint8_t buffer;     // Temporary 8-bit buffer
    int     bit_count;  // Number of bits currently stored in the buffer
} BitstreamWriter;

// Structure that manages bit-level reading from a file
typedef struct
{
    FILE* file;
    uint8_t buffer;          // Temporary 8-bit buffer
    int     bit_position;    // Position of the next bit to read (from 7 down to -1)
} BitstreamReader;


//==================================================================================
// CONSTANTS AND GLOBAL TABLES
//==================================================================================

// --- Quantization Matrices ---
extern const int LUMINANCE_Q50[8][8];
extern const int CHROMINANCE_Q50[8][8];
extern const int LUMINANCE_Q75[8][8];
extern const int CHROMINANCE_Q75[8][8];
extern const int LUMINANCE_Q25[8][8];
extern const int CHROMINANCE_Q25[8][8];

// --- DCT Matrices ---
extern const double M_dct[8][8];
extern const double M_dct_t[8][8];

// --- Zig-Zag Map ---
extern const int g_zigzag_map[64];

// --- Huffman Tables (defined in image_utils.c) ---
extern HuffmanTable g_luma_huff_tables;
extern HuffmanTable g_chroma_huff_tables;



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

void leituraInfoHeader(FILE *F, BITMAPINFOHEADER *INFO_H);
void leituraHeader(FILE *F, BITMAPFILEHEADER *H);

// --- BMP Handling Functions ---
void loadBMPHeaders(FILE *fp, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);
void printHeaders(BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);
void exportImage(char *output_filename, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader, Pixel **Image);
Pixel **loadBMPImage(FILE *input, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);
Pixel **convertBMP(unsigned char **Y, Chromancy **chromancy, BITMAPINFOHEADER *InfoHeader);

// --- Memory Allocation Functions ---
double **allocate_memory(BITMAPINFOHEADER InfoHeader);
double ***allocate_blocks_Y(int num_blocks);
Chromancy ***allocate_blocks_chr(int num_blocks_chr);
void        free_blocks_Y(double ***blocks, int num_blocks);
void        free_blocks_chr(Chromancy ***blocks, int num_blocks);
void        freeImage(double ***matrix, int height, int width);
unsigned char **allocate_y_u8(BITMAPINFOHEADER InfoHeader);
void mergeBlocks_Y(double ***blocks, int num_blocks, unsigned char **Y_out, BITMAPINFOHEADER InfoHeader);
void mergeBlocks_chr(Chromancy ***blocks, int num_blocks_chr, Chromancy **chromancy_out, BITMAPINFOHEADER InfoHeader);


// --- Block Manipulation Functions ---
void fillBlocks_Y(double ***blocks, int num_blocks, double **Y, BITMAPINFOHEADER InfoHeader);
void fillBlocks_chr(Chromancy ***blocks, int num_blocks_chr, Chromancy **compressed_chromancy, BITMAPINFOHEADER InfoHeader);

// --- Huffman Functions ---
void bitstream_writer_init(BitstreamWriter* writer, FILE* f);
void bitstream_writer_flush(BitstreamWriter* writer);
void encode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int* prev_dc, HuffmanTable* huff_tables, BitstreamWriter* writer);

void bitstream_reader_init(BitstreamReader* reader, FILE* f);
void decode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int* prev_dc, HuffmanTable* huff_tables, BitstreamReader* reader);

// Initializes global JPEG Huffman tables (Annex K canonical tables)
void init_jpeg_huffman_tables(void);


// Ycbcr conversion and chroma compression
Pixel_YCbCr_d **convertYCbCr(Pixel **Image, BITMAPINFOHEADER *InfoHeader);
Chromancy **compressCbCr(Pixel_YCbCr_d **imgYCbCr, BITMAPINFOHEADER *InfoHeader);

// DCT and Quantization
void applyDCT_Y(double ***blocks, int num_blocks);
void applyDCT_inverse_Y(double ***blocks, int num_blocks);
void applyQuantization_Y(double ***blocks, int num_blocks, const int q_table[8][8]);
void applyDCT_chr(Chromancy ***blocks, int num_blocks);
void applyDCT_inverse_chr(Chromancy ***blocks, int num_blocks);
void applyQuantization_chr(Chromancy ***blocks, int num_blocks, const int q_table[8][8]);
void applyDequantization_Y(double ***blocks, int num_blocks, const int q_table[8][8]);
void applyDequantization_chr(Chromancy ***blocks, int num_blocks, const int q_table[8][8]);


#endif
