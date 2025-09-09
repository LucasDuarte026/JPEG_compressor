#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_utils.h"
#include <math.h>

Pixel_YCbCr_d **convertYCbCr(Pixel_d **Image, BITMAPINFOHEADER *InfoHeader)
{
    Pixel_YCbCr_d **imgYCbCr = (Pixel_YCbCr_d **)malloc(InfoHeader->Height * sizeof(Pixel_YCbCr_d *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
        imgYCbCr[i] = (Pixel_YCbCr_d *)malloc(InfoHeader->Width * sizeof(Pixel_YCbCr_d));

    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            imgYCbCr[i][j].Y = (double)(0.299 * Image[i][j].R + 0.587 * Image[i][j].G + 0.114 * Image[i][j].B);
            imgYCbCr[i][j].Cb = (double)(128 - 0.168736 * Image[i][j].R - 0.331264 * Image[i][j].G + 0.5 * Image[i][j].B);
            imgYCbCr[i][j].Cr = (double)(128 + 0.5 * Image[i][j].R - 0.418688 * Image[i][j].G - 0.081312 * Image[i][j].B);
        }
    }
    return imgYCbCr;
}

Pixel_d **convertBMP(unsigned char **Y, Chromancy_d **chromancy, BITMAPINFOHEADER *InfoHeader)
{
    Pixel_d **Image_upscaled = (Pixel_d **)malloc(InfoHeader->Height * sizeof(Pixel_d *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        Image_upscaled[i] = (Pixel_d *)malloc(InfoHeader->Width * sizeof(Pixel_d));
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

Chromancy_d **compressCbCr(Pixel_YCbCr_d **imgYCbCr, BITMAPINFOHEADER *InfoHeader)
{

    Chromancy_d **chromancy = (Chromancy_d **)malloc(InfoHeader->Height * sizeof(Chromancy_d *)); //
    for (int i = 0; i < InfoHeader->Height / 2; i++)
        chromancy[i] = (Chromancy_d *)malloc(InfoHeader->Width / 2 * sizeof(Chromancy_d));

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

double **allocate_memory(BITMAPINFOHEADER InfoHeader)
{
    double **Y = (double **)malloc(InfoHeader.Height * sizeof(double *));
    for (int i = 0; i < InfoHeader.Width; i++)
        Y[i] = (double *)malloc(InfoHeader.Width * sizeof(double));
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
    size_t size_source_name = strlen("./") + strlen(argv[1]) + 1;
    size_t size_result_name = strlen("./") + strlen(argv[2]) + 1;

    char *source_name = (char *)malloc(size_source_name * sizeof(char));
    char *result_name = (char *)malloc(size_result_name * sizeof(char));
    source_name[0] = '\0';
    result_name[0] = '\0';
    strcat(source_name, "./");
    strcat(source_name, argv[1]);
    strcat(result_name, "./");
    strcat(result_name, argv[2]);

    printf("-> Image\n\t- Source file: %s\n\n", source_name);
    if (!(input = fopen(source_name, "rb")))
    {

        printf("Error: could not open input file.\n");
        exit(1);
    }
    printf("File successfully opened!\n");

    loadBMPHeaders(input, &FileHeader, &InfoHeader); // Load of the headers to the stack to fast access
    printHeaders(&FileHeader, &InfoHeader);

    if (InfoHeader.Height % 8 != 0 || InfoHeader.Width % 8 != 0)
    {
        printf("Error: Image dimensions must be multiples of 8.\n");
        exit(1);
    }

    int num_blocks = (InfoHeader.Height / 8) * (InfoHeader.Width / 8);
    printf("Number of 8x8 blocks: %d\n", num_blocks);

    // load da imagem para a heap para memória variávels (imagem de tamanho genérico)
    Pixel_d **Image = loadBMPImage(input, &InfoHeader);

    Pixel_YCbCr_d **imgYCbCr = convertYCbCr(Image, &InfoHeader);
    Chromancy_d **compressed_chromancy = compressCbCr(imgYCbCr, &InfoHeader);
    double **Y = allocate_memory(InfoHeader);

    for (int i = 0; i < InfoHeader.Height; i++)
    {
        for (int j = 0; j < InfoHeader.Width; j++)
        {
            Y[i][j] = imgYCbCr[i][j].Y;
        }
    }

    double ***blocks = allocate_blocks(num_blocks);
    // fillblocks(blocks, Y, InfoHeader );
    // preencher os blocks com os valores de Y
    free_blocks(blocks, num_blocks);

    // Pixel **converted_Image = convertBMP(Y, compressed_chromancy, &InfoHeader);

    // exportImage(result_name, &FileHeader, &InfoHeader, converted_Image);

    // fazer todos os fress
    return 0;
}