#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_utils.h"

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
    printf("-> Image\n\t- result file: %s\n\n", result_name);
    if (!(input = fopen(source_name, "rb")))
    {

        printf("Error: could not open input file.\n");
        exit(1);
    }
    printf("File successfully opened!\n");
    // Initialize canonical JPEG Huffman tables
    init_jpeg_huffman_tables();

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
    Pixel **Image = loadBMPImage(input, &FileHeader, &InfoHeader);
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

    // preencher os blocks com os valores de Y e crominância
    fillBlocks_Y(blocks_y, num_blocks, Y, InfoHeader);
    fillBlocks_chr(blocks_chr, num_blocks_chr, compressed_chromancy, InfoHeader);

    printf("\n\n\n --------------------- ANTES DCT ---------------------:");
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

    // Perform DCT on Y and (optionally) on chroma blocks before Huffman
    applyDCT_Y(blocks_y, num_blocks);
    // applyDCT_chr(blocks_chr, num_blocks_chr);

    printf("\n\n\n ---------------------     DCT    ---------------------:");
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

    // Example: we could inverse after decoding
    // applyDCT_inverse_chr(blocks_chr, num_blocks_chr);

    printf("\n\n\n --------------------- DCT INVERSA ---------------------:");
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

    /*
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
    */

    printf("\n\n\n --------------------- DCT quantizado em %d ---------------------:", quality);
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

 

    // 2. CODIFICAÇÃO HUFFMAN
    char filename[256];
    size_t len = strlen(result_name);
    if (len > 4) {
        strncpy(filename, result_name, len - 4);
        filename[len - 4] = '\0';
    } else {
        strcpy(filename, result_name);
    }
    strcat(filename, ".bin");

    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("Erro ao abrir arquivo para escrita");
        return -1;
    }

    BitstreamWriter writer;
    bitstream_writer_init(&writer, fp);

    int prev_dc = 0; // Para o primeiro bloco, o DC anterior é 0
    for (int k = 0; k < num_blocks; k++)
    {
        int temp_block[BLOCK_SIZE][BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp_block[i][j] = (int)blocks_y[k][i][j];
            }
        }
        encode_block(temp_block, &prev_dc, &g_luma_huff_tables, &writer);
    }

    bitstream_writer_flush(&writer);
    fclose(fp);

    printf("\nBloco codificado e salvo em '%s'.\n", filename);
    printf("Valor DC final: %d\n", prev_dc);

    // 3. DECODIFICAÇÃO HUFFMAN
    fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("Erro ao abrir arquivo para leitura - decodificação de huffman");
        return -1;
    }

    BitstreamReader reader;
    bitstream_reader_init(&reader, fp);

    int prev_dc_dec = 0; // Reseta o DC para a decodificação
    for (int k = 0; k < num_blocks; k++)
    {
        int decoded_block[BLOCK_SIZE][BLOCK_SIZE];
        decode_block(decoded_block, &prev_dc_dec, &g_luma_huff_tables, &reader);
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                blocks_y[k][i][j] = (double)decoded_block[i][j];
    }
    fclose(fp);

    // 4. IDCT
    applyDCT_inverse_Y(blocks_y, num_blocks);

    // 5. Merge de blocos Y e reconstrução RGB
    unsigned char **Y_rec = allocate_y_u8(InfoHeader);
    mergeBlocks_Y(blocks_y, num_blocks, Y_rec, InfoHeader);
    Pixel **reconstructed = convertBMP(Y_rec, compressed_chromancy, &InfoHeader);

    // 6. Exportar BMP
    exportImage(result_name, &FileHeader, &InfoHeader, reconstructed);

    free_blocks_Y(blocks_y, num_blocks);
    free_blocks_chr(blocks_chr, num_blocks_chr);

    return 0;
}