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
    for (int i = 0; i < InfoHeader.Height; i++)
        Y[i] = (double *)malloc(InfoHeader.Width * sizeof(double));
    return Y;
}

void fillBlocks(double ***blocks, int num_blocks, double **Y, BITMAPINFOHEADER InfoHeader)
{

    // it fill transposed to optimize cache miss
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

void applyDCT2(double ***blocks, int num_blocks)
{
    /*
    All the further operation is to provide the DCT transformation:

    B = M * A * M^-1
    but knowing that M is orthogonal, M^-1 = M^T

    B = M * A * M^T

    where:
    A is the input block (8x8)
    M is the DCT matrix (8x8)
    M^T is the transpose of M (8x8) = M^-1 because M is orthogonal

    To minimize the cache miss and optimize the matrix operation,
     we alwyas the operarions A * B with A * B^T
     and the result is the same but the operation
     is faster because minimize the cache miss when
     getting the elements of the matrix line by line



    */

    // init temp matrix
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++)
    {
        // multiply M_dct_t with blocks[k] and store in temp
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp[i][j] = 0; // init the element
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp[i][j] += M_dct[i][u] * blocks[k][u][j];
                }
            }
            // multiply M_dct_t with blocks[k] and store in temp

            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    temp2[i][j] = 0; // init the element
                    for (int u = 0; u < BLOCK_SIZE; u++)
                    {
                        temp2[i][j] += temp[i][u] * M_dct[j][u];
                    }
                }
            }
            for (int i = 0; i < BLOCK_SIZE; i++)
                for (int j = 0; j < BLOCK_SIZE; j++)
                    blocks[k][i][j] = temp2[i][j];
        }
    }
}

void applyDCT_inverse2(double ***blocks, int num_blocks)
{
    /*
    All the further operation is to provide the DCT inverse transformation:

    A = M^-1 * B * M
    but knowing that M is orthogonal, M^-1 = M^T

    A = M * B * M^T

    where:
    A is the output block of pixel values (8x8)
    B is the input block of coeficientes (8x8)
    M is the DCT matrix (8x8)
    M^T is the transpose of M (8x8) = M^-1 because M is orthogonal

    To minimize the cache miss and optimize the matrix operation,
     we alwyas the operarions A * B with A * B^T
     and the result is the same but the operation
     is faster because minimize the cache miss when
     getting the elements of the matrix line by line

    */

    // init temp matrix
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++)
    {
        // multiply M_dct_t with blocks[k] and store in temp
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp[i][j] = 0; // init the element
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp[i][j] += M_dct_t[i][u] * blocks[k][u][j];
                }
            }
            // multiply M_dct_t with blocks[k] and store in temp

            for (int i = 0; i < BLOCK_SIZE; i++)
            {   
                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    temp2[i][j] = 0; // init the element
                    for (int u = 0; u < BLOCK_SIZE; u++)
                    {
                        temp2[i][j] += temp[i][u] * M_dct_t[j][u];
                    }
                }
            }
            for (int i = 0; i < BLOCK_SIZE; i++)
                for (int j = 0; j < BLOCK_SIZE; j++)
                    blocks[k][i][j] = temp2[i][j];
        }
    }
}


void applyDCT(double ***blocks, int num_blocks) {
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++) {
        // temp = M * A
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                temp[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++) {
                    temp[i][j] += M_dct[i][u] * blocks[k][u][j];
                }
            }
        }
        // temp2 = temp * M^T
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                temp2[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++) {
                    temp2[i][j] += temp[i][u] * M_dct[j][u];  // M^T
                }
            }
        }
        // Salvar no bloco
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                blocks[k][i][j] = temp2[i][j];
    }
}

void applyDCT_inverse(double ***blocks, int num_blocks) {
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++) {
        // temp = M^T * B
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                temp[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++) {
                    temp[i][j] += M_dct_t[i][u] * blocks[k][u][j];
                }
            }
        }
        // temp2 = temp * M
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                temp2[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++) {
                    temp2[i][j] += temp[i][u] * M_dct[u][j];
                }
            }
        }
        // Salvar no bloco
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                blocks[k][i][j] = temp2[i][j];
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
        if (result == -1)
        {
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

    // Matriz estática 8x8 do tipo int
    double A[BLOCK_SIZE][BLOCK_SIZE] = {
        {52, 55, 61, 66, 70, 61, 64, 73},
        {63, 59, 66, 90, 109, 85, 69, 72},
        {62, 59, 68, 113, 144, 104, 66, 73},
        {63, 58, 71, 122, 154, 106, 70, 69},
        {67, 61, 68, 104, 126, 88, 68, 70},
        {79, 65, 60, 70, 77, 68, 58, 75},
        {85, 71, 64, 59, 55, 61, 65, 83},
        {87, 79, 69, 68, 65, 76, 78, 94}};

    double C[BLOCK_SIZE][BLOCK_SIZE];
  
 
    double **A_2 = (double **)malloc(InfoHeader.Height * sizeof(double *));
    for (int i = 0; i < InfoHeader.Width; i++)
    {
        A_2[i] = (double *)malloc(InfoHeader.Width * sizeof(double));
    }
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            A_2[i][j] = A[i][j]; // Inicializa o elemento
        }
    }
    double ***blocks_test = allocate_blocks(BLOCK_SIZE);
    fillBlocks(blocks_test, BLOCK_SIZE, A_2, InfoHeader);

    printf("Matriz Resultante C = A * B: ANTES\n");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            printf("%8.2f", blocks_test[0][i][j]);
        }
        printf("\n");
    }
    applyDCT(blocks_test, 8);

    printf("Matriz Resultante C = A * B: DEPOIS\n");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            printf("%8.2f", blocks_test[0][i][j]);
        }
        printf("\n");
    }
    applyDCT_inverse(blocks_test, 8);

    printf("DCT inversa\n");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            printf("%8.2f", blocks_test[0][i][j]);
        }
        printf("\n");
    }
    // applyQuantization(blocks, num_blocks);

    free_blocks(blocks, num_blocks);
    // Pixel **converted_Image = convertBMP(Y, compressed_chromancy, &InfoHeader);

    // exportImage(result_name, &FileHeader, &InfoHeader, converted_Image);

    // fazer todos os fress
    return 0;
}