#include <stdio.h>
#include <stdlib.h>
#include "image_utils.h"

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

double ***allocate_blocks(int num_blocks)
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

void free_blocks(double ***blocks, int num_blocks)
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

// void fillblocks(double ***blocks, double **Y, BITMAPINFOHEADER *InfoHeader){

// }
