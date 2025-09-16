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

void fillBlocks(double ***blocks, int num_blocks, double **Y, BITMAPINFOHEADER InfoHeader)
{
    for (int k = 0; k < num_blocks; k++)
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                blocks[k][i][j] = Y[i][j];
            }
        }
    }
}

void applyDCT(double ***blocks, int num_blocks, int side)
{
    // init temp matrix
    double temp[side][side] ;
    for (int i = 0; i < side; i++)
        for (int j = 0; j < side; j++)
            temp[i][j] = 0;

    for (int k = 0; k < num_blocks; k++)
    {
        for (int i = 0; i < side; i++)
        {
            for (int j = 0; j < side; j++)
            {
                for (int u = 0; u < side; u++)
                {

                    temp[i][j] += blocks[k][i][u] * dct_matrix_transposed[j][u];
                }
            }
        }
        for (int i = 0; i < side; i++)
            for (int j = 0; j < side; j++)
                blocks[k][i][j]=temp[i][j] ;
        
    }
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
        char command[256];
        snprintf(command, sizeof(command), "ls -1A %s", "./bmp_source");

        printf("--- Images to process: %s ---\n", command);

        // Execute the command using system()
        int result = system(command);

        // Check if the command executed successfully
        if (result == -1) {
            perror("system");
            return 1;
        }

        printf("\n After using, save the results in ./images/\n");

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
    // preencher os blocks com os valores de Y
    fillBlocks(blocks, num_blocks, Y, InfoHeader);


int SIZE = 8;
double A[SIZE][SIZE] = {
    { 1,  2,  3,  4,  5,  6,  7,  8},
    { 9, 10, 11, 12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21, 22, 23, 24},
    {25, 26, 27, 28, 29, 30, 31, 32},
    {33, 34, 35, 36, 37, 38, 39, 40},
    {41, 42, 43, 44, 45, 46, 47, 48},
    {49, 50, 51, 52, 53, 54, 55, 56},
    {57, 58, 59, 60, 61, 62, 63, 64}
};

// Matriz B (8x8) preenchida com valores baseados nos índices (i+j)
double B[SIZE][SIZE] = {
    {0, 1, 2, 3, 4, 5, 6, 7},
    {1, 2, 3, 4, 5, 6, 7, 8},
    {2, 3, 4, 5, 6, 7, 8, 9},
    {3, 4, 5, 6, 7, 8, 9, 10},
    {4, 5, 6, 7, 8, 9, 10, 11},
    {5, 6, 7, 8, 9, 10, 11, 12},
    {6, 7, 8, 9, 10, 11, 12, 13},
    {7, 8, 9, 10, 11, 12, 13, 14}
};
    
    double C[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            C[i][j] = 0; // Inicializa o elemento
            for (int k = 0; k < SIZE; k++) {
                // C[i][j] += A[i][k] * dct_matrix[k][j];
                C[i][j] += A[i][k] * dct_matrix_transposed[j][k];
            }
        }
    }
   
       

    double **A_2 = (double **)malloc(InfoHeader.Height * sizeof(double *));
    for (int i = 0; i < InfoHeader.Width; i++){
        A_2[i] = (double *)malloc(InfoHeader.Width * sizeof(double));
    }
     for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A_2[i][j] = A[i][j]; // Inicializa o elemento
        }
    }
    double ***blocks_test = allocate_blocks(SIZE);
    fillBlocks(blocks_test, SIZE, A_2, InfoHeader);

    // --- Impressão da Matriz Resultante C ---
    printf("Matriz Resultante C = A * B: ANTES\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%8.2f", C[i][j]);
        }
        printf("\n");
    }
    
    printf("Matriz Resultante C = A * B: DEPOIS\n");
    applyDCT(blocks_test, SIZE,SIZE);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%8.2f", blocks_test[0][i][j]);
        }
        printf("\n");
    }
    // applyQuantization(blocks, num_blocks);

    free_blocks(blocks, num_blocks);
    double result[SIZE][SIZE] = {
        {  9.943,  -8.801,   3.757,  -2.939,   1.741,  -1.267,   0.647,  -0.249},
        { 31.087, -14.569,   8.341,  -4.531,   4.213,  -1.515,   2.079,   0.383},
        { 52.231, -20.337,  12.925,  -6.123,   6.685,  -1.763,   3.511,   1.015},
        { 73.375, -26.105,  17.509,  -7.715,   9.157,  -2.011,   4.943,   1.647},
        { 94.519, -31.873,  22.093,  -9.307,  11.629,  -2.259,   6.375,   2.279},
        {115.663, -37.641,  26.677, -10.899,  14.101,  -2.507,   7.807,   2.911},
        {136.807, -43.409,  31.261, -12.491,  16.573,  -2.755,   9.239,   3.543},
        {157.951, -49.177,  35.845, -14.083,  19.045,  -3.003,  10.671,   4.175}
    };
    // Pixel **converted_Image = convertBMP(Y, compressed_chromancy, &InfoHeader);

    // exportImage(result_name, &FileHeader, &InfoHeader, converted_Image);

    // fazer todos os fress
    return 0;
}