#include <stdio.h>
#include <stdlib.h>
#include "image_utils.h"

// Luminância DC
HuffmanCode g_huff_dc_luma[12] = {
    {0x00, 2}, {0x02, 3}, {0x03, 3}, {0x04, 3}, {0x05, 3}, {0x06, 3}, {0x0E, 4}, {0x1E, 5}, {0x3E, 6}, {0x7E, 7}, {0xFE, 8}, {0x1FE, 9}};

// Crominância DC
HuffmanCode g_huff_dc_chroma[12] = {
    {0x00, 2}, {0x01, 2}, {0x02, 2}, {0x06, 3}, {0x0E, 4}, {0x1E, 5}, {0x3E, 6}, {0x7E, 7}, {0xFE, 8}, {0x1FE, 9}, {0x3FE, 10}, {0x7FE, 11}};

// Luminância AC
HuffmanCode g_huff_ac_luma[256] = {
    {0x0A, 4}, {0x00, 2}, {0x01, 2}, {0x04, 3}, {0x0B, 4}, {0x1A, 5}, {0x78, 7}, {0xF7C, 12}, {0x05, 3}, {0x1B, 5}, {0xF7D, 12}, {0xFFDCC, 20}, {0x1C, 5}, {0x38, 6}, {0x79, 7}, {0xF7E, 12}, {0xFFDCD, 20}, {0xFFDD0, 20}, {0x02, 3}, {0x0C, 4}, {0x39, 6}, {0xF7F, 12}, {0xFFDD1, 20}, {0xFFDD2, 20}, {0xFFDD3, 20}, {0xFFDD4, 20}, {0xFFDD5, 20}, {0xFFDD6, 20}, {0xFFDD7, 20}, {0xFFDD8, 20}, {0x1D, 5}, {0x58, 7}, {0xF80, 12}, {0xFFDD9, 20}, {0xFFDDA, 20}, {0xFFDDB, 20}, {0xFFDDC, 20}, {0xFFDDD, 20}, {0xFFDDE, 20}, {0xFFDDF, 20}, {0xFFDE0, 20}, {0xFFDE1, 20}, {0x06, 3}, {0x1E, 5}, {0x98, 8}, {0xF81, 12}, {0xFFDE2, 20}, {0xFFDE3, 20}, {0xFFDE4, 20}, {0xFFDE5, 20}, {0xFFDE6, 20}, {0xFFDE7, 20}, {0xFFDE8, 20}, {0xFFDE9, 20}, {0x0D, 4}, {0x3A, 6}, {0xF82, 12}, {0xFFDEA, 20}, {0xFFDEB, 20}, {0xFFDEC, 20}, {0xFFDED, 20}, {0xFFDEE, 20}, {0xFFDEF, 20}, {0xFFDF0, 20}, {0xFFDF1, 20}, {0xFFDF2, 20}, {0x1F, 5}, {0x59, 7}, {0xF83, 12}, {0xFFDF3, 20}, {0xFFDF4, 20}, {0xFFDF5, 20}, {0xFFDF6, 20}, {0xFFDF7, 20}, {0xFFDF8, 20}, {0xFFDF9, 20}, {0xFFDFA, 20}, {0xFFDFB, 20}, {0x0E, 4}, {0x7A, 7}, {0xF84, 12}, {0xFFDFC, 20}, {0xFFDFD, 20}, {0xFFDFE, 20}, {0xFFDFF, 20}, {0xFFE00, 20}, {0xFFE01, 20}, {0xFFE02, 20}, {0xFFE03, 20}, {0xFFE04, 20}, {0x3B, 6}, {0x99, 8}, {0xF85, 12}, {0xFFE05, 20}, {0xFFE06, 20}, {0xFFE07, 20}, {0xFFE08, 20}, {0xFFE09, 20}, {0xFFE0A, 20}, {0xFFE0B, 20}, {0xFFE0C, 20}, {0xFFE0D, 20}, {0x07, 4}, {0x5A, 7}, {0xF86, 12}, {0xFFE0E, 20}, {0xFFE0F, 20}, {0xFFE10, 20}, {0xFFE11, 20}, {0xFFE12, 20}, {0xFFE13, 20}, {0xFFE14, 20}, {0xFFE15, 20}, {0xFFE16, 20}, {0x0F, 4}, {0x7B, 7}, {0xF87, 12}, {0xFFE17, 20}, {0xFFE18, 20}, {0xFFE19, 20}, {0xFFE1A, 20}, {0xFFE1B, 20}, {0xFFE1C, 20}, {0xFFE1D, 20}, {0xFFE1E, 20}, {0xFFE1F, 20}, {0x3C, 6}, {0x9A, 8}, {0xF88, 12}, {0xFFE20, 20}, {0xFFE21, 20}, {0xFFE22, 20}, {0xFFE23, 20}, {0xFFE24, 20}, {0xFFE25, 20}, {0xFFE26, 20}, {0xFFE27, 20}, {0xFFE28, 20}, {0x5B, 7}, {0xF89, 12}, {0xFFE29, 20}, {0xFFE2A, 20}, {0xFFE2B, 20}, {0xFFE2C, 20}, {0xFFE2D, 20}, {0xFFE2E, 20}, {0xFFE2F, 20}, {0xFFE30, 20}, {0xFFE31, 20}, {0xFFE32, 20}, {0x03, 3}, {0x1A, 5}, {0x38, 6}, {0x59, 7}, {0xF7D, 12}, {0xF81, 12}, {0xF86, 12}, {0xFFDCE, 20}, {0x08, 4}, {0x79, 7}, {0xF84, 12}, {0xFFE02, 20}, {0xFFE15, 20}, {0xFFE22, 20}, {0xFFE29, 20}, {0xFFE33, 20}, {0xFFE34, 20}, {0xFFE35, 20}, {0xFFE36, 20}, {0xFFE37, 20}, {0x99, 8}, {0xF88, 12}, {0xFFE38, 20}, {0xFFE39, 20}, {0xFFE3A, 20}, {0xFFE3B, 20}, {0xFFE3C, 20}, {0xFFE3D, 20}, {0xFFE3E, 20}, {0xFFE3F, 20}, {0xFFE40, 20}, {0xFFE41, 20}, {0x09, 4}, {0x39, 6}, {0x58, 7}, {0x98, 8}, {0xF7C, 12}, {0xF7F, 12}, {0xF83, 12}, {0xF8A, 12}, {0xFFE42, 20}, {0x19, 5}, {0x78, 7}, {0xF82, 12}, {0xFFDFB, 20}, {0xFFE12, 20}, {0xFFE1F, 20}, {0xFFE28, 20}, {0xFFE43, 20}, {0xFFE44, 20}, {0xFFE45, 20}, {0xFFE46, 20}, {0xFFE47, 20}, {0xF89, 12}, {0xFFE48, 20}, {0xFFE49, 20}, {0xFFE4A, 20}, {0xFFE4B, 20}, {0xFFE4C, 20}, {0xFFE4D, 20}, {0xFFE4E, 20}, {0xFFE4F, 20}, {0xFFE50, 20}, {0xFFE51, 20}};

// Crominância AC
HuffmanCode g_huff_ac_chroma[256] = {
    {0xFE590, 19}, {0x00, 2}, {0x01, 2}, {0x02, 3}, {0x0A, 4}, {0x18, 5}, {0x38, 6}, {0x78, 7}, {0xF7C, 12}, {0x0B, 4}, {0x58, 7}, {0xF7D, 12}, {0xFFDCC, 20}, {0x03, 3}, {0x19, 5}, {0x98, 8}, {0xF7E, 12}, {0xFFDCD, 20}, {0x1A, 5}, {0xF7F, 12}, {0xFFDD0, 20}, {0xFFDD1, 20}, {0xFFDD2, 20}, {0xFFDD3, 20}, {0xFFDD4, 20}, {0xFFDD5, 20}, {0xFFDD6, 20}, {0xFFDD7, 20}, {0xFFDD8, 20}, {0xFFDD9, 20}, {0x04, 3}, {0x1B, 5}, {0xF80, 12}, {0xFFDDA, 20}, {0xFFDDB, 20}, {0xFFDDC, 20}, {0xFFDDD, 20}, {0xFFDDE, 20}, {0xFFDDF, 20}, {0xFFDE0, 20}, {0xFFDE1, 20}, {0xFFDE2, 20}, {0x0C, 4}, {0x39, 6}, {0xF81, 12}, {0xFFDE3, 20}, {0xFFDE4, 20}, {0xFFDE5, 20}, {0xFFDE6, 20}, {0xFFDE7, 20}, {0xFFDE8, 20}, {0xFFDE9, 20}, {0xFFDEA, 20}, {0xFFDEB, 20}, {0x1C, 5}, {0x59, 7}, {0xF82, 12}, {0xFFDEC, 20}, {0xFFDED, 20}, {0xFFDEE, 20}, {0xFFDEF, 20}, {0xFFDF0, 20}, {0xFFDF1, 20}, {0xFFDF2, 20}, {0xFFDF3, 20}, {0xFFDF4, 20}, {0x05, 3}, {0x1D, 5}, {0xF83, 12}, {0xFFDF5, 20}, {0xFFDF6, 20}, {0xFFDF7, 20}, {0xFFDF8, 20}, {0xFFDF9, 20}, {0xFFDFA, 20}, {0xFFDFB, 20}, {0xFFDFC, 20}, {0xFFDFD, 20}, {0x0D, 4}, {0x3A, 6}, {0xF84, 12}, {0xFFDFE, 20}, {0xFFDFF, 20}, {0xFFE00, 20}, {0xFFE01, 20}, {0xFFE02, 20}, {0xFFE03, 20}, {0xFFE04, 20}, {0xFFE05, 20}, {0xFFE06, 20}, {0x0E, 4}, {0x5A, 7}, {0xF85, 12}, {0xFFE07, 20}, {0xFFE08, 20}, {0xFFE09, 20}, {0xFFE0A, 20}, {0xFFE0B, 20}, {0xFFE0C, 20}, {0xFFE0D, 20}, {0xFFE0E, 20}, {0xFFE0F, 20}, {0x1E, 5}, {0x79, 7}, {0xF86, 12}, {0xFFE10, 20}, {0xFFE11, 20}, {0xFFE12, 20}, {0xFFE13, 20}, {0xFFE14, 20}, {0xFFE15, 20}, {0xFFE16, 20}, {0xFFE17, 20}, {0xFFE18, 20}, {0x0F, 4}, {0x3B, 6}, {0xF87, 12}, {0xFFE19, 20}, {0xFFE1A, 20}, {0xFFE1B, 20}, {0xFFE1C, 20}, {0xFFE1D, 20}, {0xFFE1E, 20}, {0xFFE1F, 20}, {0xFFE20, 20}, {0xFFE21, 20}, {0x1F, 5}, {0x7A, 7}, {0xF88, 12}, {0xFFE22, 20}, {0xFFE23, 20}, {0xFFE24, 20}, {0xFFE25, 20}, {0xFFE26, 20}, {0xFFE27, 20}, {0xFFE28, 20}, {0xFFE29, 20}, {0xFFE2A, 20}, {0x3C, 6}, {0x99, 8}, {0xF89, 12}, {0xFFE2B, 20}, {0xFFE2C, 20}, {0xFFE2D, 20}, {0xFFE2E, 20}, {0xFFE2F, 20}, {0xFFE30, 20}, {0xFFE31, 20}, {0xFFE32, 20}, {0xFFE33, 20}, {0x06, 4}, {0x1A, 5}, {0x58, 7}, {0xF7C, 12}, {0xF7F, 12}, {0xF82, 12}, {0xF85, 12}, {0xF88, 12}, {0xFFE34, 20}, {0x07, 4}, {0x1B, 5}, {0x78, 7}, {0xF7D, 12}, {0xF80, 12}, {0xF83, 12}, {0xF86, 12}, {0xF89, 12}, {0xFFE35, 20}, {0x38, 6}, {0x98, 8}, {0xF8A, 12}, {0xFFE36, 20}, {0xFFE37, 20}, {0xFFE38, 20}, {0xFFE39, 20}, {0xFFE3A, 20}, {0xFFE3B, 20}, {0xFFE3C, 20}, {0xFFE3D, 20}, {0xFFE3E, 20}, {0x08, 4}, {0x1C, 5}, {0xF8B, 12}, {0xFFE3F, 20}, {0xFFE40, 20}, {0xFFE41, 20}, {0xFFE42, 20}, {0xFFE43, 20}, {0xFFE44, 20}, {0xFFE45, 20}, {0xFFE46, 20}, {0xFFE47, 20}, {0x09, 4}, {0x3A, 6}, {0xF8C, 12}, {0xFFE48, 20}, {0xFFE49, 20}, {0xFFE4A, 20}, {0xFFE4B, 20}, {0xFFE4C, 20}, {0xFFE4D, 20}, {0xFFE4E, 20}, {0xFFE4F, 20}, {0xFFE50, 20}, {0x5A, 7}, {0xF8D, 12}, {0xFFE51, 20}, {0xFFE52, 20}, {0xFFE53, 20}, {0xFFE54, 20}, {0xFFE55, 20}, {0xFFE56, 20}, {0xFFE57, 20}, {0xFFE58, 20}, {0xFFE59, 20}, {0xFFE5A, 20}};


void loadBMPHeaders(FILE *fp, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader)
{
    leituraHeader(fp, FileHeader);
    leituraInfoHeader(fp, InfoHeader);
    if (InfoHeader->Compression != 0)
    {
        printf("This is a compressed BMP!!!");
        fclose(fp);
        return;
    }

    printHeaders(FileHeader, InfoHeader);
}

void leituraHeader(FILE *F, BITMAPFILEHEADER *H)
{
    fread(&H->Type, sizeof(unsigned short int), 1, F);
    fread(&H->Size, sizeof(unsigned int), 1, F);
    fread(&H->Reserved1, sizeof(unsigned short int), 1, F);
    fread(&H->Reserved2, sizeof(unsigned short int), 1, F);
    fread(&H->OffBits, sizeof(unsigned int), 1, F);
}

void leituraInfoHeader(FILE *F, BITMAPINFOHEADER *INFO_H)
{
    fread(&INFO_H->Size, sizeof(unsigned int), 1, F);
    fread(&INFO_H->Width, sizeof(int), 1, F);
    fread(&INFO_H->Height, sizeof(int), 1, F);
    fread(&INFO_H->Planes, sizeof(unsigned short int), 1, F);
    fread(&INFO_H->BitCount, sizeof(unsigned short int), 1, F);
    fread(&INFO_H->Compression, sizeof(unsigned int), 1, F);
    fread(&INFO_H->SizeImage, sizeof(unsigned int), 1, F);
    fread(&INFO_H->XResolution, sizeof(int), 1, F);
    fread(&INFO_H->YResolution, sizeof(int), 1, F);
    fread(&INFO_H->NColours, sizeof(unsigned int), 1, F);
    fread(&INFO_H->ImportantColours, sizeof(unsigned int), 1, F);
}

void printHeaders(BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader)
{
    /*system("cls");*/
    printf("*************** File Header ***************\n\n");

    printf("Magic number for file: %x\n", FileHeader->Type);
    printf("File's size: %d\n", FileHeader->Size);
    printf("Offset to bitmap data: %d\n", FileHeader->OffBits);

    printf("\n\n");
    printf("*************** Info Header ***************\n\n");
    printf("Info header's size: %d\n", InfoHeader->Size);
    printf("Width: %d\n", InfoHeader->Width);
    printf("Height: %d\n", InfoHeader->Height);
    printf("Color planes: %d\n", InfoHeader->Planes);
    printf("Bits per pixel: %d\n", InfoHeader->BitCount);
    printf("Compression type (0 = no compression): %d\n", InfoHeader->Compression);
    printf("Image's data size: %d\n", InfoHeader->SizeImage);
    printf("X Pixels per meter: %d\n", InfoHeader->XResolution);
    printf("Y Pixels per meter: %d\n", InfoHeader->YResolution);
    printf("Number of colors: %d\n", InfoHeader->NColours);
    printf("Numberof important colors: %d\n", InfoHeader->ImportantColours);

    system("pause");
}

Pixel **loadBMPImage(FILE *input, BITMAPINFOHEADER *InfoHeader)
{
    Pixel **Image = (Pixel **)malloc(InfoHeader->Height * sizeof(Pixel *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        Image[i] = (Pixel *)malloc(InfoHeader->Width * sizeof(Pixel));
    }

    fseek(input, 54, SEEK_SET); // skipping the header (54 bytes)
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            fread(&Image[i][j].R, sizeof(int), 1, input);
            fread(&Image[i][j].G, sizeof(int), 1, input);
            fread(&Image[i][j].B, sizeof(int), 1, input);
        }
    }
    return Image;
}

void exportImage(char *output_filename, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader, Pixel **Image)
{
    FILE *file_pointer;
    if (!(file_pointer = fopen(output_filename, "wb")))
    {
        printf("Error: could not open output file.");
        exit(1);
    }
    fwrite(&FileHeader->Type, sizeof(unsigned short), 1, file_pointer);
    fwrite(&FileHeader->Size, sizeof(unsigned int), 1, file_pointer);
    fwrite(&FileHeader->Reserved1, sizeof(unsigned short int), 1, file_pointer);
    fwrite(&FileHeader->Reserved2, sizeof(unsigned short int), 1, file_pointer);
    fwrite(&FileHeader->OffBits, sizeof(unsigned int), 1, file_pointer);

    fwrite(&InfoHeader->Size, sizeof(unsigned int), 1, file_pointer);
    fwrite(&InfoHeader->Width, sizeof(int), 1, file_pointer);
    fwrite(&InfoHeader->Height, sizeof(int), 1, file_pointer);
    fwrite(&InfoHeader->Planes, sizeof(unsigned short int), 1, file_pointer);
    fwrite(&InfoHeader->BitCount, sizeof(unsigned short int), 1, file_pointer);
    fwrite(&InfoHeader->Compression, sizeof(unsigned int), 1, file_pointer);
    fwrite(&InfoHeader->SizeImage, sizeof(unsigned int), 1, file_pointer);
    fwrite(&InfoHeader->XResolution, sizeof(int), 1, file_pointer);
    fwrite(&InfoHeader->YResolution, sizeof(int), 1, file_pointer);
    fwrite(&InfoHeader->NColours, sizeof(unsigned int), 1, file_pointer);
    fwrite(&InfoHeader->ImportantColours, sizeof(unsigned int), 1, file_pointer);

    printf("-> Exporting image to %s\n", output_filename);
    printf("-> Image size: %d x %d\n", InfoHeader->Width, InfoHeader->Height);
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            fwrite(&Image[i][j].R, sizeof(unsigned char), 1, file_pointer);
            fwrite(&Image[i][j].G, sizeof(unsigned char), 1, file_pointer);
            fwrite(&Image[i][j].B, sizeof(unsigned char), 1, file_pointer);
        }
    }

    fclose(file_pointer);
}

double ***allocate_blocks_Y(int num_blocks)
{
    // 1. Aloca a "lista" de ponteiros para as matrizes 8x8 (double**)
    double ***blocks = (double ***)malloc(num_blocks * sizeof(double **));
    if (blocks == NULL)
    {
        // Falha na alocação
        return NULL;
    }

    // 2. Para cada bloco na lista, aloca uma matriz 8x8
    for (int k = 0; k < num_blocks; k++)
    {
        // 2a. Aloca as 8 linhas (ponteiros para double*) para a matriz k
        blocks[k] = (double **)malloc(BLOCK_SIZE * sizeof(double *));
        if (blocks[k] == NULL)
        {
            // Falha na alocação, precisa liberar o que já foi alocado
            // (código de limpeza omitido por simplicidade, mas importante em produção)
            return NULL;
        }

        // 2b. Para cada linha da matriz k, aloca os 8 elementos (doubles)
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            blocks[k][i] = (double *)malloc(BLOCK_SIZE * sizeof(double));
            if (blocks[k][i] == NULL)
            {
                // Falha na alocação
                return NULL;
            }
        }
    }

    return blocks;
}

Chromancy ***allocate_blocks_chr(int num_blocks)
{
    // 1. Aloca a "lista" de ponteiros para as matrizes 8x8 (Chromancy**)
    Chromancy ***blocks = (Chromancy ***)malloc(num_blocks * sizeof(Chromancy **));
    if (blocks == NULL)
    {
        // Falha na alocação
        return NULL;
    }

    // 2. Para cada bloco na lista, aloca uma matriz 8x8
    for (int k = 0; k < num_blocks; k++)
    {
        // 2a. Aloca as 8 linhas (ponteiros para Chromancy*) para a matriz k
        blocks[k] = (Chromancy **)malloc(BLOCK_SIZE * sizeof(Chromancy *));
        if (blocks[k] == NULL)
        {
            // Falha na alocação, precisa liberar o que já foi alocado
            // (código de limpeza omitido por simplicidade, mas importante em produção)
            return NULL;
        }

        // 2b. Para cada linha da matriz k, aloca os 8 elementos (Chromancys)
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            blocks[k][i] = (Chromancy *)malloc(BLOCK_SIZE * sizeof(Chromancy));
            if (blocks[k][i] == NULL)
            {
                // Falha na alocação
                return NULL;
            }
        }
    }

    return blocks;
}

void free_blocks_Y(double ***blocks, int num_blocks)
{
    if (blocks == NULL)
        return;

    for (int k = 0; k < num_blocks; k++)
    {
        if (blocks[k] != NULL)
        {
            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                // Libera cada linha da matriz 8x8
                free(blocks[k][i]);
            }
            // Libera a matriz 8x8
            free(blocks[k]);
        }
    }
    // Libera a lista de matrizes
    free(blocks);
}

void free_blocks_chr(Chromancy ***blocks, int num_blocks)
{
    if (blocks == NULL)
        return;

    for (int k = 0; k < num_blocks; k++)
    {
        if (blocks[k] != NULL)
        {
            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                // Libera cada linha da matriz 8x8
                free(blocks[k][i]);
            }
            // Libera a matriz 8x8
            free(blocks[k]);
        }
    }
    // Libera a lista de matrizes
    free(blocks);
}



// --- BITSTREAM FUNCTIONS ---

void bitstream_init(Bitstream *stream, FILE *f) {
    stream->file = f;
    stream->buffer = 0;
    stream->bit_count = 0;
}

static void write_byte(Bitstream *stream) {
    fputc(stream->buffer, stream->file);
    // Regra de "Byte Stuffing" do JPEG: se escrevermos 0xFF, devemos inserir um byte 0x00
    if (stream->buffer == 0xFF) {
        fputc(0x00, stream->file);
    }
    stream->buffer = 0;
    stream->bit_count = 0;
}

void write_bits(Bitstream *stream, uint16_t code, int length) {
    for (int i = length - 1; i >= 0; i--) {
        // Pega o i-ésimo bit do código
        int bit = (code >> i) & 1;
        
        // Adiciona o bit ao buffer
        stream->buffer = (stream->buffer << 1) | bit;
        stream->bit_count++;
        
        if (stream->bit_count == 8) {
            write_byte(stream);
        }
    }
}

void bitstream_flush(Bitstream *stream) {
    if (stream->bit_count > 0) {
        // Preenche o resto do byte com '1's, como a especificação JPEG manda
        stream->buffer <<= (8 - stream->bit_count);
        stream->buffer |= (0xFF >> stream->bit_count);
        write_byte(stream);
    }
}

// --- LÓGICA DE CODIFICAÇÃO ---

// Converte um valor (DC diff ou AC coeff) em sua Categoria e Amplitude
int get_category_and_amplitude(int value, uint16_t *amplitude_bits) {
    if (value == 0) return 0;

    int temp = (value > 0) ? value : -value;
    int category = 0;
    
    // Encontra a categoria (equivalente a floor(log2(temp)) + 1)
    while (temp > 0) {
        temp >>= 1;
        category++;
    }

    // Calcula os bits da amplitude
    // Se o valor for negativo, os bits são o complemento do valor positivo
    if (value > 0) {
        *amplitude_bits = value;
    } else {
        *amplitude_bits = (1 << category) - 1 + value;
    }
    
    return category;
}

void encode_block(double block[BLOCK_SIZE][BLOCK_SIZE], int *prev_dc, HuffmanCode *dc_table, HuffmanCode *ac_table, Bitstream *stream) {
    
    int zigzag_coeffs[64];

    // 1. Aplicar o Zig-Zag Scan e arredondar
    for (int i = 0; i < 64; i++) {
        int row = g_zigzag_map[i] / BLOCK_SIZE;
        int col = g_zigzag_map[i] % BLOCK_SIZE;
        zigzag_coeffs[i] = round(block[row][col]);
    }

    // 2. Codificar o Coeficiente DC
    int dc_diff = zigzag_coeffs[0] - *prev_dc;
    *prev_dc = zigzag_coeffs[0];

    uint16_t dc_amplitude;
    int dc_category = get_category_and_amplitude(dc_diff, &dc_amplitude);
    
    write_bits(stream, dc_table[dc_category].code, dc_table[dc_category].length);
    write_bits(stream, dc_amplitude, dc_category);

    // 3. Codificar os 63 Coeficientes AC
    int zero_run_count = 0;
    for (int i = 1; i < 64; i++) {
        if (zigzag_coeffs[i] == 0) {
            zero_run_count++;
        } else {
            // Se tivermos mais de 15 zeros, escrevemos o código ZRL (15,0)
            while (zero_run_count >= 16) {
                write_bits(stream, ac_table[0xF0].code, ac_table[0xF0].length); // ZRL
                zero_run_count -= 16;
            }

            uint16_t ac_amplitude;
            int ac_category = get_category_and_amplitude(zigzag_coeffs[i], &ac_amplitude);
            
            // Combina RUN e SIZE em um byte
            uint8_t run_size_symbol = (zero_run_count << 4) | ac_category;

            write_bits(stream, ac_table[run_size_symbol].code, ac_table[run_size_symbol].length);
            write_bits(stream, ac_amplitude, ac_category);
            
            zero_run_count = 0; // Reseta a contagem de zeros
        }
    }

    // 4. Se o restante do bloco for de zeros, escreve o código EOB (0,0)
    if (zero_run_count > 0) {
        write_bits(stream, ac_table[0x00].code, ac_table[0x00].length); // EOB
    }
}