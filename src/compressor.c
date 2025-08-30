#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jpeg.h"
#include "bitmap.h"

typedef struct
{
    unsigned char Y;
    unsigned char Cb;
    unsigned char Cr;
} Pixel_YCbCr;

Pixel_YCbCr **convertYCbCr(Pixel **Image, BITMAPINFOHEADER *InfoHeader, Pixel_YCbCr **imgYCbCr)
{
    imgYCbCr = (Pixel_YCbCr **)malloc(InfoHeader->Height * sizeof(Pixel_YCbCr *)); //
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

Pixel **convertBMP(Pixel_YCbCr **imgYCbCr, BITMAPINFOHEADER *InfoHeader, Pixel **Image)
{

    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            Image[i][j].R = (unsigned char) (imgYCbCr[i][j].Y + 1.402   * (imgYCbCr[i][j].Cr - 128));
            Image[i][j].G = (unsigned char) (imgYCbCr[i][j].Y - 0.344136 * (imgYCbCr[i][j].Cb - 128) - 0.714136 * (imgYCbCr[i][j].Cr - 128));
            Image[i][j].B = (unsigned char) (imgYCbCr[i][j].Y + 1.772   * (imgYCbCr[i][j].Cb - 128));
        }
    }
    return Image;
}

Pixel_YCbCr **compressYCbCr(Pixel_YCbCr **imgYCbCr,Pixel_YCbCr **aux_matrix ,BITMAPINFOHEADER *InfoHeader, int final_amount)
{

    aux_matrix = (Pixel_YCbCr **)malloc(InfoHeader->Height * sizeof(Pixel_YCbCr *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
        aux_matrix[i] = (Pixel_YCbCr *)malloc(InfoHeader->Width * sizeof(Pixel_YCbCr));
    printf("-> Compressing image with factor %d%%\n", final_amount);
    unsigned char Cr_pixel, Cb_pixel;
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        for (int j = 0; j < InfoHeader->Width; j++)
        {
            if (i == InfoHeader->Height) // last column
            {
                if (j == InfoHeader->Width) // Last pixel
                {
                    aux_matrix[i][j].Cb = imgYCbCr[i][j].Cb;
                    aux_matrix[i][j].Cr = imgYCbCr[i][j].Cr;
                }
                else // last column but not last pixel
                {
                    aux_matrix[i][j].Cb = (int)(imgYCbCr[i][j].Cb +
                                                imgYCbCr[i][j + 1].Cb) /
                                          2;
                    aux_matrix[i][j].Cr = (int)(imgYCbCr[i][j].Cr +
                                                imgYCbCr[i][j + 1].Cr) /
                                          2;
                }
            }

            else // last line
            {
                if (j == InfoHeader->Width) // last line but not last pixel
                {
                    aux_matrix[i][j].Cb = (int)(imgYCbCr[i][j].Cb + imgYCbCr[i + 1][j].Cb) / 2;
                    aux_matrix[i][j].Cr = (int)(imgYCbCr[i][j].Cr + imgYCbCr[i + 1][j].Cr) / 2;
                }
                else // normal pixel
                {
                    aux_matrix[i][j].Cb = (int)(imgYCbCr[i][j].Cb + imgYCbCr[i + 1][j].Cb + imgYCbCr[i][j + 1].Cb + imgYCbCr[i + 1][j + 1].Cb) / 4;

                    aux_matrix[i][j].Cr = (int)(imgYCbCr[i][j].Cr + imgYCbCr[i + 1][j].Cr + imgYCbCr[i][j + 1].Cr + imgYCbCr[i + 1][j + 1].Cr) / 4;
                }
            }
            aux_matrix[i][j].Y = imgYCbCr[i][j].Y;
        }
    }
    return aux_matrix;
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

    Pixel_YCbCr **imgYCbCr,**compressed_imgYCbCr;
    imgYCbCr = convertYCbCr(Image, &InfoHeader, imgYCbCr);
    compressed_imgYCbCr = compressYCbCr(imgYCbCr,compressed_imgYCbCr, &InfoHeader, 75);
    // Pixel ** compressedBMP;
    // compressedBMP = convertBMP(compressed_imgYCbCr, &InfoHeader, Image);
    
    // exportImage(result_name, &FileHeader, &InfoHeader, Image);
    return 0;
}