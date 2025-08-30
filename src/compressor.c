#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jpeg.h"
#include "bitmap.h"
#include "math.h"

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

Pixel_YCbCr **convertYCbCr(Pixel **Image, BITMAPINFOHEADER *InfoHeader)
{
    Pixel_YCbCr **imgYCbCr = (Pixel_YCbCr **)malloc(InfoHeader->Height * sizeof(Pixel_YCbCr *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
        imgYCbCr[i] = (Pixel_YCbCr *)malloc(InfoHeader->Width * sizeof(Pixel_YCbCr));

    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            imgYCbCr[i][j].Y = (unsigned char)(0.299 * Image[i][j].R + 0.587 * Image[i][j].G + 0.114 * Image[i][j].B);
            imgYCbCr[i][j].Cb = (unsigned char)(128 - 0.168736 * Image[i][j].R - 0.331264 * Image[i][j].G + 0.5 * Image[i][j].B);
            imgYCbCr[i][j].Cr = (unsigned char)(128 + 0.5 * Image[i][j].R - 0.418688 * Image[i][j].G - 0.081312 * Image[i][j].B);
        }
    }
    return imgYCbCr;
}

Pixel **convertBMP(unsigned char **Y, Chromancy **chromancy, BITMAPINFOHEADER *InfoHeader)
{
    Pixel **Image_upscaled = (Pixel **)malloc(InfoHeader->Height * sizeof(Pixel *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        Image_upscaled[i] = (Pixel *)malloc(InfoHeader->Width * sizeof(Pixel));
    }
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
       {
            // Converte para double ANTES de fazer as contas
            double y_val = Y[i][j];
            double cb_val = chromancy[i / 2][j / 2].Cb;
            double cr_val = chromancy[i / 2][j / 2].Cr;

            // Calcula os valores temporários em ponto flutuante
            double r_temp = y_val + 1.402 * (cr_val - 128);
            double g_temp = y_val - 0.344136 * (cb_val - 128) - 0.714136 * (cr_val - 128);
            double b_temp = y_val + 1.772 * (cb_val - 128);

            // "Clamping": Garante que o valor final esteja entre 0 e 255
            // Em C++, use std::max/min. Em C, você pode usar ifs.
            Image_upscaled[i][j].R = (unsigned char)fmax(0.0, fmin(255.0, r_temp));
            Image_upscaled[i][j].G = (unsigned char)fmax(0.0, fmin(255.0, g_temp));
            Image_upscaled[i][j].B = (unsigned char)fmax(0.0, fmin(255.0, b_temp));
        }
    }
    return Image_upscaled;
}

Chromancy **compressCbCr(Pixel_YCbCr **imgYCbCr, BITMAPINFOHEADER *InfoHeader)
{

    Chromancy **chromancy = (Chromancy **)malloc(InfoHeader->Height * sizeof(Chromancy *)); //
    for (int i = 0; i < InfoHeader->Height / 2; i++)
    {
        chromancy[i] = (Chromancy *)malloc(InfoHeader->Width / 2 * sizeof(Chromancy));
        chromancy[i] = (Chromancy *)malloc(InfoHeader->Width / 2 * sizeof(Chromancy));
    }

    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            if (i % 2 == 0 && j % 2 == 0)
            {
                chromancy[i / 2][j / 2].Cb = (imgYCbCr[i][j].Cb + imgYCbCr[i + 1][j].Cb + imgYCbCr[i][j + 1].Cb + imgYCbCr[i + 1][j + 1].Cb) / 4;
                chromancy[i / 2][j / 2].Cr = (imgYCbCr[i][j].Cr + imgYCbCr[i + 1][j].Cr + imgYCbCr[i][j + 1].Cr + imgYCbCr[i + 1][j + 1].Cr) / 4;
            }
        }
    }
    return chromancy;
}

unsigned char **allocate_memory(BITMAPINFOHEADER InfoHeader)
{
    unsigned char **Y = (unsigned char **)malloc(InfoHeader.Height * sizeof(unsigned char *));
    for (int i = 0; i < InfoHeader.Width; i++)
        Y[i] = (unsigned char *)malloc(InfoHeader.Width * sizeof(unsigned char));
    return Y;
}

int main(int argc, char *argv[])
{
    FILE *input;

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
    size_t size_source_name = strlen("./bmp_source/") + strlen(argv[1]) + 1;
    size_t size_result_name = strlen("./images/") + strlen(argv[2]) + 1;

    char *source_name = (char *)malloc(size_source_name * sizeof(char));
    char *result_name = (char *)malloc(size_result_name * sizeof(char));
    source_name[0] = '\0';
    result_name[0] = '\0';
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
    Pixel **Image = loadBMPImage(input, &InfoHeader);

    Pixel_YCbCr **imgYCbCr = convertYCbCr(Image, &InfoHeader);
    Chromancy **compressed_chromancy = compressCbCr(imgYCbCr, &InfoHeader);
    unsigned char **Y = allocate_memory(InfoHeader);

    for (int i = 0; i < InfoHeader.Height; i++)
        for (int j = 0; j < InfoHeader.Width; j++)
            Y[i][j] = imgYCbCr[i][j].Y;

    Pixel **converted_Image = convertBMP(Y, compressed_chromancy, &InfoHeader);

    exportImage(result_name, &FileHeader, &InfoHeader, converted_Image);

    return 0;
}