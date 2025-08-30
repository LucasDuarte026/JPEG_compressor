#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jpeg.h"
#include "bitmap.h"

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} Pixel;

Pixel **loadBMPImage(FILE *input, BITMAPINFOHEADER InfoHeader, Pixel **Image)
{
    int i, j, tam;

    Image = (Pixel **)malloc(InfoHeader.Height * sizeof(Pixel *)); //
    for (int i = 0; i < InfoHeader.Height; i++)
        Image[i] = (Pixel *)malloc(InfoHeader.Width * sizeof(Pixel));

    tam = InfoHeader.Height * InfoHeader.Width;

    fseek(input, 54, SEEK_SET); // skipping the header (54 bytes)
    for (i = 0; i < InfoHeader.Height; i++)
    {
        for (j = 0; j < InfoHeader.Width; j++)
        {
            fread(&Image[i][j].R, sizeof(unsigned char), 1, input);
            fread(&Image[i][j].G, sizeof(unsigned char), 1, input);
            fread(&Image[i][j].B, sizeof(unsigned char), 1, input);
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

int main(int argc, char *argv[])
{
    FILE *input, *output;
    int i;
    unsigned char *v;

    BITMAPFILEHEADER FileHeader; /* File header */
    BITMAPINFOHEADER InfoHeader; /* Info header */

    printf("--------- --------- --------- --------- --------- --------- --------- --------- \n");
    printf("--------- ------- >   Conversor de imagens BMP para JPEG!   < ------- --------- \n");
    printf("--------- --------- --------- --------- --------- --------- --------- --------- \n");
    printf("Author: Lucas sales Duarte\n\n");

    // test if the user has provided the input and output file names
    if (argc < 3)
    {
        printf("Commands Error:\n");
        printf("Usage: %s <source file> <result file>\n", argv[0]);
        exit(1);
    }

    char *source_name = (char *)malloc(strlen(argv[1]) + 13 * sizeof(char));
    char *result_name = (char *)malloc(strlen(argv[1]) + 9 * sizeof(char));

    strcat(source_name, "./bmp_source/");
    strcat(source_name, argv[1]);

    strcat(result_name, "./images/");
    strcat(result_name, argv[2]);

    printf("-> Image\n\t- Source file: %s\n\n", source_name);
    if (!(input = fopen(source_name, "rb")))
    {
        printf("Error: could not open input file.");
        exit(1);
    }
    printf("File successfully opened!\n");

    loadBMPHeaders(input, &FileHeader, &InfoHeader); // Load of the headers to the stack to fast access
    printHeaders(&FileHeader, &InfoHeader);

    // load da imagem para a heap para memória variávels (imagem de tamanho genérico)
    Pixel **Image;
    Image = loadBMPImage(input, InfoHeader, Image);

    printf("-> Image size: %d x %d\n", InfoHeader.Width, InfoHeader.Height);
    
    exportImage(result_name, &FileHeader, &InfoHeader, Image);
    return 0;
}