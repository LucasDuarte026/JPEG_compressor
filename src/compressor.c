#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_utils.h"

Pixel_YCbCr_d **convertYCbCr(Pixel **Image, BITMAPINFOHEADER *InfoHeader)
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

Chromancy **compressCbCr(Pixel_YCbCr_d **imgYCbCr, BITMAPINFOHEADER *InfoHeader)
{

    Chromancy **chromancy = (Chromancy **)malloc(InfoHeader->Height * 0.5 * sizeof(Chromancy *)); //
    for (int i = 0; i < InfoHeader->Height / 2; i++)
        chromancy[i] = (Chromancy *)malloc(InfoHeader->Width * 0.5 * sizeof(Chromancy));

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

void fillBlocks_Y(double ***blocks, int num_blocks, double **Y, BITMAPINFOHEADER InfoHeader)
{
    int num_blocks_per_row = InfoHeader.Width / BLOCK_SIZE;

    for (int k = 0; k < num_blocks; k++)
    {
        // Calcula a linha e coluna inicial do bloco 'k' na imagem grande
        int start_row = (k / num_blocks_per_row) * BLOCK_SIZE;
        int start_col = (k % num_blocks_per_row) * BLOCK_SIZE;

        // Copia o bloco 8x8 da imagem Y para blocks[k]
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                // A fonte dos dados vem da posição calculada em Y
                blocks[k][i][j] = Y[start_row + i][start_col + j];
            }
        }
    }
}

void fillBlocks_chr(Chromancy ***blocks, int num_blocks, Chromancy **Chromancy, BITMAPINFOHEADER InfoHeader)
{
    int num_blocks_per_row = InfoHeader.Width / BLOCK_SIZE;

    for (int k = 0; k < num_blocks; k++)
    {
        // Calcula a linha e coluna inicial do bloco 'k' na imagem grande
        int start_row = (k / num_blocks_per_row) * BLOCK_SIZE;
        int start_col = (k % num_blocks_per_row) * BLOCK_SIZE;

        // Copia o bloco 8x8 da imagem Y para blocks[k]
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                // A fonte dos dados vem da posição calculada em Y
                blocks[k][i][j].Cb = Chromancy[start_row + i][start_col + j].Cb;
                blocks[k][i][j].Cr = Chromancy[start_row + i][start_col + j].Cr;
            }
        }
    }
}

void applyDCT_Y(double ***blocks, int num_blocks)
{
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++)
    {
        // temp = M * A
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp[i][j] += M_dct[i][u] * (blocks[k][u][j] - 128);
                }
            }
        }
        // temp2 = temp * M^T
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp2[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp2[i][j] += temp[i][u] * M_dct[j][u]; // M^T
                }
            }
        }
        // Salvar no bloco
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                blocks[k][i][j] = temp2[i][j];
    }
}

void applyDCT_chr(Chromancy ***blocks, int num_blocks)
{
    Chromancy temp[BLOCK_SIZE][BLOCK_SIZE];
    Chromancy temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++)
    {
        // temp = M * A
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp[i][j].Cb = 0;
                temp[i][j].Cr = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp[i][j].Cb += M_dct[i][u] * (blocks[k][u][j].Cb - 128);
                    temp[i][j].Cr += M_dct[i][u] * (blocks[k][u][j].Cr - 128);
                }
            }
        }
        // temp2 = temp * M^T
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp2[i][j].Cb = 0;
                temp2[i][j].Cr = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp2[i][j].Cb += temp[i][u].Cb * M_dct[j][u]; // M^T
                    temp2[i][j].Cr += temp[i][u].Cr * M_dct[j][u]; // M^T
                }
            }
        }
        // Salvar no bloco
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                blocks[k][i][j] = temp2[i][j];
    }
}

void applyDCT_inverse_Y(double ***blocks, int num_blocks)
{
    double temp[BLOCK_SIZE][BLOCK_SIZE];
    double temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++)
    {
        // temp = M^T * B
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp[i][j] += M_dct_t[i][u] * blocks[k][u][j];
                }
            }
        }
        // temp2 = temp * M
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp2[i][j] = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp2[i][j] += temp[i][u] * M_dct[u][j];
                }
            }
        }
        // Salvar no bloco
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                blocks[k][i][j] = (temp2[i][j] + 128);
    }
}

void applyDCT_inverse_chr(Chromancy ***blocks, int num_blocks)
{
    Chromancy temp[BLOCK_SIZE][BLOCK_SIZE];
    Chromancy temp2[BLOCK_SIZE][BLOCK_SIZE];

    for (int k = 0; k < num_blocks; k++)
    {
        // temp = M^T * B
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp[i][j].Cb = 0;
                temp[i][j].Cr = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp[i][j].Cb += M_dct_t[i][u] * blocks[k][u][j].Cb;
                    temp[i][j].Cr += M_dct_t[i][u] * blocks[k][u][j].Cr;
                }
            }
        }
        // temp2 = temp * M
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp2[i][j].Cb = 0;
                temp2[i][j].Cr = 0;
                for (int u = 0; u < BLOCK_SIZE; u++)
                {
                    temp2[i][j].Cb += temp[i][u].Cb * M_dct[u][j];
                    temp2[i][j].Cr += temp[i][u].Cr * M_dct[u][j];
                }
            }
        }
        // Salvar no bloco
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks[k][i][j].Cb = (temp2[i][j].Cb + 128);
                blocks[k][i][j].Cr = (temp2[i][j].Cr + 128);
            }
    }
}

void applyQuantization_Y(double ***blocks, int num_blocks, const int div_mat[8][8])
{
    for (int k = 0; k < num_blocks; k++)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks[k][i][j] = blocks[k][i][j] / div_mat[i][j];
            }
        }
    }
}

void applyQuantization_chr(Chromancy ***blocks, int num_blocks, const int div_mat[8][8])
{
    for (int k = 0; k < num_blocks; k++)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks[k][i][j].Cb = blocks[k][i][j].Cb / div_mat[i][j];
                blocks[k][i][j].Cr = blocks[k][i][j].Cr / div_mat[i][j];
            }
        }
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
    if (argc < 4)
    {
        printf("Commands Error:\n");
        printf("Usage: %s <quality> <source file> <result file>\n", argv[0]);
        char command[256];
        snprintf(command, sizeof(command), "ls -1A %s", "./bmp_source");
        printf("--- Quality available: 75 50 25 (quality percentage) ---\n");

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
    const int quality = atoi(argv[1]);

    if (quality != 75 && quality != 50 && quality != 25)
    {
        printf("--- Quality available: 75 50 25 (quality percentage) ---\n");
        exit(1);
    }

    size_t size_source_name = strlen("./") + strlen(argv[2]) + 1;
    size_t size_result_name = strlen("./") + strlen(argv[3]) + 1;

    char *source_name = (char *)malloc(size_source_name * sizeof(char));
    char *result_name = (char *)malloc(size_result_name * sizeof(char));
    source_name[0] = '\0';
    result_name[0] = '\0';
    strcat(source_name, "./");
    strcat(source_name, argv[2]);
    strcat(result_name, "./");
    strcat(result_name, argv[3]);

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

    const int num_blocks = (InfoHeader.Height / 8) * (InfoHeader.Width / 8);
    const int num_blocks_chr = num_blocks / 4;
    printf("Number of 8x8 blocks of Y: %d\n", num_blocks);
    printf("Number of 8x8 blocks of Chr: %d\n", num_blocks);

    // load da imagem para a heap para memória variávels (imagem de tamanho genérico)
    Pixel **Image = loadBMPImage(input, &InfoHeader);
    Pixel_YCbCr_d **imgYCbCr = convertYCbCr(Image, &InfoHeader);
    Chromancy **compressed_chromancy = compressCbCr(imgYCbCr, &InfoHeader);
    double **Y = allocate_memory(InfoHeader);
    for (int i = 0; i < InfoHeader.Height; i++)
    {
        for (int j = 0; j < InfoHeader.Width; j++)
        {
            Y[i][j] = imgYCbCr[i][j].Y;
        }
    }

    double ***blocks_y = allocate_blocks_Y(num_blocks);
    Chromancy ***blocks_chr = allocate_blocks_chr(num_blocks_chr);

    // preencher os blocks_y com os valores de Y
    fillBlocks_Y(blocks_y, num_blocks, Y, InfoHeader);
    fillBlocks_chr(blocks_chr, num_blocks_chr, compressed_chromancy, InfoHeader);

    /* printf("\n\n\n --------------------- ANTES DCT ---------------------:");
        for (int k = 0; k < 2; k++)
        {
             printf("\n\nblock %d\n", k);

            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    printf("|%4.0f %4.0f %4.0f|", blocks_y[k][i][j], blocks_chr[k][i][j].Cb, blocks_chr[k][i][j].Cr);
                }
                printf("\n");
            }
        }
    */
    applyDCT_Y(blocks_y, num_blocks);
    applyDCT_chr(blocks_chr, num_blocks_chr);

    /* printf("\n\n\n ---------------------     DCT    ---------------------:");
        for (int k = 0; k < 2; k++)
        {
             printf("\n\nblock %d\n", k);

            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    printf("|%4.0f %4.0f %4.0f|", blocks_y[k][i][j], blocks_chr[k][i][j].Cb, blocks_chr[k][i][j].Cr);
                }
                printf("\n");
            }
        }
    */
    applyDCT_inverse_Y(blocks_y, num_blocks);
    applyDCT_inverse_chr(blocks_chr, num_blocks_chr);

    /* printf("\n\n\n --------------------- DCT INVERSA ---------------------:");
        for (int k = 0; k < 2; k++)
        {
             printf("\n\nblock %d\n", k);

            for (int i = 0; i < BLOCK_SIZE; i++)
            {
                for (int j = 0; j < BLOCK_SIZE; j++)
                {
                    printf("|%4.0f %4.0f %4.0f|", blocks_y[k][i][j], blocks_chr[k][i][j].Cb, blocks_chr[k][i][j].Cr);
                }
                printf("\n");
            }
        }
    */

    switch (quality)
    {
    case 75:
        applyQuantization_Y(blocks_y, num_blocks, LUMINANCE_Q75);
        applyQuantization_chr(blocks_chr, num_blocks_chr, CHROMINANCE_Q75);
        break;

    case 50:
        applyQuantization_Y(blocks_y, num_blocks, LUMINANCE_Q50);
        applyQuantization_chr(blocks_chr, num_blocks_chr, CHROMINANCE_Q50);
        break;

    case 25:
        applyQuantization_Y(blocks_y, num_blocks, LUMINANCE_Q25);
        applyQuantization_chr(blocks_chr, num_blocks_chr, CHROMINANCE_Q25);
        break;

    default:
        break;
    }

    /* printf("\n\n\n --------------------- DCT quantizado em %d ---------------------:", quality);
    for (int k = 0; k < 6; k++)
    {

        printf("\n\nblock %d\n", k);

        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                printf("|%4.0f %4.0f %4.0f|", blocks_y[k][i][j], blocks_chr[k][i][j].Cb, blocks_chr[k][i][j].Cr);
            }
            printf("\n");
        }
    }
    */

    FILE *output_file = fopen("output.bin", "wb");
    if (!output_file)
    {
        perror("It wasn't able to open the file!");
        return 1;
    }
    Bitstream stream;
    bitstream_init(&stream, output_file);

    int prev_dc_y = 0;
    int prev_dc_cb = 0;
    int prev_dc_cr = 0;
    for (int k = 0; k < num_blocks; k++)
    {
        /* code */
    }

  
    printf("Codificando blocos Y...\n");
    for (int k = 0; k < num_blocks; k++) {
        // 1. Crie um bloco temporário com o tipo correto (8x8 contíguo)
        double temp_block[BLOCK_SIZE][BLOCK_SIZE];

        // 2. Copie os dados do seu bloco double** para o bloco temporário
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                temp_block[i][j] = blocks_y[k][i][j];
            }
        }

        // 3. Chame a função com o bloco temporário, que agora tem o tipo certo
        encode_block(temp_block, &prev_dc_y, g_huff_dc_luma, g_huff_ac_luma, &stream);
    }
    // printf("Codificando bloco Cb...\n");
    // encode_block(block_cb, &prev_dc_cb, g_huff_dc_chroma, g_huff_ac_chroma, &stream);

    // printf("Codificando bloco Cr...\n");
    // encode_block(block_cr, &prev_dc_cr, g_huff_dc_chroma, g_huff_ac_chroma, &stream);

    bitstream_flush(&stream);
    fclose(output_file);

    printf("Arquivo binário 'output.bin' gerado com sucesso!\n");

    // Pixel **converted_Image = convertBMP(Y, compressed_chromancy, &InfoHeader);

    // exportImage(result_name, &FileHeader, &InfoHeader, converted_Image);
    free_blocks_Y(blocks_y, num_blocks);
    free_blocks_chr(blocks_chr, num_blocks_chr);

    // fazer todos os fress
    return 0;
}