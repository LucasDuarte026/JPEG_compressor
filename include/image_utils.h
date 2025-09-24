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
// ESTRUTURAS PARA HUFFMAN
//==================================================================================

typedef struct
{
    uint32_t code; // O código de bits de Huffman
    int      length; // O número de bits no código
} HuffmanCode;

typedef struct
{
    // As tabelas padrão do Anexo K do JPEG
    HuffmanCode* dc_table;
    HuffmanCode* ac_table;
} HuffmanTable;

// Estrutura para gerenciar a escrita bit a bit em um arquivo
typedef struct
{
    FILE* file;
    uint8_t buffer;     // Buffer temporário de 8 bits
    int     bit_count;  // Quantos bits estão em uso no buffer
} BitstreamWriter;

// Estrutura para gerenciar a leitura bit a bit de um arquivo
typedef struct
{
    FILE* file;
    uint8_t buffer;          // Buffer temporário de 8 bits
    int     bit_position;    // Posição do próximo bit a ser lido (de 7 a -1)
} BitstreamReader;


//==================================================================================
// CONSTANTES E TABELAS GLOBAIS
//==================================================================================

// --- Matrizes de Quantização ---
extern const int LUMINANCE_Q50[8][8];
extern const int CHROMINANCE_Q50[8][8];

// --- Matrizes DCT ---
extern const double M_dct[8][8];
extern const double M_dct_t[8][8];

// --- Mapa Zig-Zag ---
extern const int g_zigzag_map[64];

// --- Tabelas de Huffman (definidas em image_utils.c) ---
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

// --- Funções de Manipulação de BMP ---
void loadBMPHeaders(FILE *fp, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);
void printHeaders(BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader);
void exportImage(char *output_filename, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader, Pixel **Image);
Pixel **loadBMPImage(FILE *input, BITMAPINFOHEADER *InfoHeader);

// --- Funções de Alocação de Memória ---
double **allocate_memory(BITMAPINFOHEADER InfoHeader);
double ***allocate_blocks_Y(int num_blocks);
Chromancy ***allocate_blocks_chr(int num_blocks_chr);
void        free_blocks_Y(double ***blocks, int num_blocks);
void        free_blocks_chr(Chromancy ***blocks, int num_blocks);
void        freeImage(double ***matrix, int height, int width);


// --- Funções de Manipulação de Blocos ---
void fillBlocks_Y(double ***blocks, int num_blocks, double **Y, BITMAPINFOHEADER InfoHeader);
void fillBlocks_chr(Chromancy ***blocks, int num_blocks_chr, Chromancy **compressed_chromancy, BITMAPINFOHEADER InfoHeader);

// --- Funções de Huffman ---
void bitstream_writer_init(BitstreamWriter* writer, FILE* f);
void bitstream_writer_flush(BitstreamWriter* writer);
void encode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int* prev_dc, HuffmanTable* huff_tables, BitstreamWriter* writer);

void bitstream_reader_init(BitstreamReader* reader, FILE* f);
void decode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int* prev_dc, HuffmanTable* huff_tables, BitstreamReader* reader);


// Ycbcr conversion and chroma compression
Pixel_YCbCr_d **convertYCbCr(Pixel **Image, BITMAPINFOHEADER *InfoHeader);
Chromancy **compressCbCr(Pixel_YCbCr_d **imgYCbCr, BITMAPINFOHEADER *InfoHeader);

// DCT and Quantization
void applyDCT_Y(double ***blocks, int num_blocks);
void applyQuantization_Y(double ***blocks, int num_blocks, const int q_table[8][8]);
void applyDCT_Y(double ***blocks, int num_blocks);
void applyQuantization_Y(double ***blocks, int num_blocks, const int q_table[8][8]);


#endif
