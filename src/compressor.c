#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "image_utils.h"

typedef struct
{
    char magic[4];
    uint32_t width;
    uint32_t height;
    uint32_t num_blocks_y;
    uint32_t num_blocks_chr;
    uint32_t len_y;
    uint32_t len_cb;
    uint32_t len_cr;
} HuffmanFileHeader;

int main(int argc, char *argv[])
{
    FILE *input;

    BITMAPFILEHEADER FileHeader; /* File header */
    BITMAPINFOHEADER InfoHeader; /* Info header */
    //  *************************************** INIT CODE ********************************************

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
   
    
    //  ******************************* DEFINE QUALITY ****************************************************
    // Get user-defined quality
    const int quality = atoi(argv[1]);
    if (quality != 75 && quality != 50 && quality != 25)
    {
        printf("--- Quality available: 75 50 25 (quality percentage) ---\n");
        exit(1);
    }
    //  ***********************************************************************************

    
    //  ************************************ PREPARE THE FILES ***********************************************
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
    //  ***********************************************************************************

    // Initialize canonical JPEG Huffman tables
    init_jpeg_huffman_tables();

    //  ************************************** HEADER ANALISIS *********************************************
    // Load BMP headers
    loadBMPHeaders(input, &FileHeader, &InfoHeader); // Load of the headers to the stack to fast access
    printHeaders(&FileHeader, &InfoHeader);

    if (InfoHeader.Height % BLOCK_SIZE != 0 || InfoHeader.Width % BLOCK_SIZE != 0)
    {
        printf("Error: Image dimensions must be multiples of %d.\n", BLOCK_SIZE);
        exit(1);
    }

    if (InfoHeader.Height % (BLOCK_SIZE * 2) != 0 || InfoHeader.Width % (BLOCK_SIZE * 2) != 0)
    {
        printf("Error: Image dimensions must be multiples of %d for 4:2:0 chroma subsampling.\n", BLOCK_SIZE * 2);
        exit(1);
    }

    const int num_blocks = (InfoHeader.Height / BLOCK_SIZE) * (InfoHeader.Width / BLOCK_SIZE);
     const int num_blocks_chr = (InfoHeader.Height / (BLOCK_SIZE * 2)) * (InfoHeader.Width / (BLOCK_SIZE * 2));
    //  ***********************************************************************************

    // Number of 8x8 blocks depending on image size
    printf("Number of 8x8 blocks of Y: %d\n", num_blocks);
    printf("Number of 8x8 blocks of Chr: %d\n", num_blocks_chr);

    //  ********************************** LOAD IMAGE *************************************************
    // Load the entire image into heap memory (supports arbitrary image sizes)
    Pixel **Image = loadBMPImage(input, &FileHeader, &InfoHeader);
    fclose(input);
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
    //  ***********************************************************************************

    // Image data is now expressed in YCbCr
    double ***blocks_y = allocate_blocks_Y(num_blocks);
    Chromancy ***blocks_chr = allocate_blocks_chr(num_blocks_chr);
    
    // Fill block buffers with luminance and chrominance values
    fillBlocks_Y(blocks_y, num_blocks, Y, InfoHeader);
    fillBlocks_chr(blocks_chr, num_blocks_chr, compressed_chromancy, InfoHeader);
    //  ***********************************************************************************
    
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
    
    //  ***********************************************************************************
    // Perform DCT on Y and chrominance blocks before Huffman coding
    printf("\n\n\n ---------------------     DCT     ---------------------:");
    applyDCT_Y(blocks_y, num_blocks);
    applyDCT_chr(blocks_chr, num_blocks_chr);
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
    //  ***********************************************************************************


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

    const int (*q_table_y)[BLOCK_SIZE] = NULL;
    const int (*q_table_chr)[BLOCK_SIZE] = NULL;


    //  ********************************** QUANTIZATION *********************************************
    // Quantization of Y and CbCr blocks
    switch (quality)
    {
    case 75:
        q_table_y = LUMINANCE_Q75;
        q_table_chr = CHROMINANCE_Q75;
        break;

    case 50:
        q_table_y = LUMINANCE_Q50;
        q_table_chr = CHROMINANCE_Q50;
        break;

    case 25:
        q_table_y = LUMINANCE_Q25;
        q_table_chr = CHROMINANCE_Q25;
        break;

    default:
        fprintf(stderr, "Unsupported quality level %d.\n", quality);
        exit(1);
    }
    
    applyQuantization_Y(blocks_y, num_blocks, q_table_y);
    applyQuantization_chr(blocks_chr, num_blocks_chr, q_table_chr);


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

 

    // 2. HUFFMAN ENCODING
    char base_name[256];
    size_t len = strlen(result_name);
    if (len > 4 && strcmp(result_name + len - 4, ".bmp") == 0)
    {
        snprintf(base_name, sizeof(base_name), "%.*s", (int)(len - 4), result_name);
    }
    else
    {
        snprintf(base_name, sizeof(base_name), "%s", result_name);
    }

    const size_t filename_capacity = 256;
    const char suffix_bin[] = ".bin";
    size_t base_len = strlen(base_name);
    if (base_len + strlen(suffix_bin) >= filename_capacity)
    {
        fprintf(stderr, "Output path '%s' is too long for Huffman bitstream.\n", base_name);
        return -1;
    }

    char filename_bin[256];
    strcpy(filename_bin, base_name);
    strcat(filename_bin, suffix_bin);

    FILE *fp_bin = fopen(filename_bin, "wb");
    if (!fp_bin)
    {
        perror("Erro ao abrir arquivo binário para escrita");
        return -1;
    }

    HuffmanFileHeader hf_header = {
        {'J', 'H', 'F', 'F'},
        (uint32_t)InfoHeader.Width,
        (uint32_t)InfoHeader.Height,
        (uint32_t)num_blocks,
        (uint32_t)num_blocks_chr,
        0, 0, 0};

    if (fwrite(&hf_header, sizeof(hf_header), 1, fp_bin) != 1)
    {
        perror("Erro ao escrever cabeçalho Huffman");
        fclose(fp_bin);
        return -1;
    }

    long segment_start = ftell(fp_bin);

    BitstreamWriter writer;
    bitstream_writer_init(&writer, fp_bin);

    int prev_dc_y = 0;
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
        encode_block(temp_block, &prev_dc_y, &g_luma_huff_tables, &writer);
    }
    bitstream_writer_flush(&writer);

    long after_y = ftell(fp_bin);
    hf_header.len_y = (uint32_t)(after_y - segment_start);

    bitstream_writer_init(&writer, fp_bin);

    long start_cb = ftell(fp_bin);
    int prev_dc_cb = 0;
    for (int k = 0; k < num_blocks_chr; k++)
    {
        int temp_block[BLOCK_SIZE][BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp_block[i][j] = (int)blocks_chr[k][i][j].Cb;
            }
        }
        encode_block(temp_block, &prev_dc_cb, &g_chroma_huff_tables, &writer);
    }
    bitstream_writer_flush(&writer);

    long after_cb = ftell(fp_bin);
    hf_header.len_cb = (uint32_t)(after_cb - start_cb);

    bitstream_writer_init(&writer, fp_bin);

    long start_cr = ftell(fp_bin);
    int prev_dc_cr = 0;
    for (int k = 0; k < num_blocks_chr; k++)
    {
        int temp_block[BLOCK_SIZE][BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                temp_block[i][j] = (int)blocks_chr[k][i][j].Cr;
            }
        }
        encode_block(temp_block, &prev_dc_cr, &g_chroma_huff_tables, &writer);
    }
    bitstream_writer_flush(&writer);

    long after_cr = ftell(fp_bin);
    hf_header.len_cr = (uint32_t)(after_cr - start_cr);

    if (fseek(fp_bin, 0, SEEK_SET) != 0 || fwrite(&hf_header, sizeof(hf_header), 1, fp_bin) != 1)
    {
        perror("Erro ao atualizar cabeçalho Huffman");
        fclose(fp_bin);
        return -1;
    }

    fclose(fp_bin);

    printf("\nBitstream combinado salvo em '%s'. DC finais -> Y:%d Cb:%d Cr:%d\n", filename_bin, prev_dc_y, prev_dc_cb, prev_dc_cr);

    // 3. HUFFMAN DECODING
    FILE *fp_bin_in = fopen(filename_bin, "rb");
    if (!fp_bin_in)
    {
        perror("Erro ao abrir arquivo binário para leitura - decodificação de huffman");
        return -1;
    }

    HuffmanFileHeader header_in;
    if (fread(&header_in, sizeof(header_in), 1, fp_bin_in) != 1)
    {
        perror("Erro ao ler cabeçalho Huffman");
        fclose(fp_bin_in);
        return -1;
    }

    if (memcmp(header_in.magic, "JHFF", 4) != 0)
    {
        fprintf(stderr, "Arquivo Huffman inválido: magic incorreto.\n");
        fclose(fp_bin_in);
        return -1;
    }

    if (header_in.width != (uint32_t)InfoHeader.Width || header_in.height != (uint32_t)InfoHeader.Height)
    {
        fprintf(stderr, "Aviso: dimensões do cabeçalho Huffman (%u x %u) diferem da imagem (%d x %d).\n",
                header_in.width, header_in.height, InfoHeader.Width, InfoHeader.Height);
    }

    if (header_in.num_blocks_y != (uint32_t)num_blocks || header_in.num_blocks_chr != (uint32_t)num_blocks_chr)
    {
        fprintf(stderr, "Aviso: cabeçalho Huffman contém contagens diferentes (Y=%u, Chr=%u).\n",
                header_in.num_blocks_y, header_in.num_blocks_chr);
    }

    long offset_y = ftell(fp_bin_in);
    long offset_cb = offset_y + header_in.len_y;
    long offset_cr = offset_cb + header_in.len_cb;

    BitstreamReader reader;

    if (fseek(fp_bin_in, offset_y, SEEK_SET) != 0)
    {
        perror("Erro ao posicionar para segmento Y");
        fclose(fp_bin_in);
        return -1;
    }
    bitstream_reader_init(&reader, fp_bin_in);

    int prev_dc_dec_y = 0;
    for (int k = 0; k < num_blocks; k++)
    {
        int decoded_block[BLOCK_SIZE][BLOCK_SIZE];
        decode_block(decoded_block, &prev_dc_dec_y, &g_luma_huff_tables, &reader);
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks_y[k][i][j] = (double)decoded_block[i][j];
            }
        }
    }

    if (fseek(fp_bin_in, offset_cb, SEEK_SET) != 0)
    {
        perror("Erro ao posicionar para segmento Cb");
        fclose(fp_bin_in);
        return -1;
    }
    bitstream_reader_init(&reader, fp_bin_in);

    int prev_dc_dec_cb = 0;
    for (int k = 0; k < num_blocks_chr; k++)
    {
        int decoded_block[BLOCK_SIZE][BLOCK_SIZE];
        decode_block(decoded_block, &prev_dc_dec_cb, &g_chroma_huff_tables, &reader);
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks_chr[k][i][j].Cb = (double)decoded_block[i][j];
            }
        }
    }

    if (fseek(fp_bin_in, offset_cr, SEEK_SET) != 0)
    {
        perror("Erro ao posicionar para segmento Cr");
        fclose(fp_bin_in);
        return -1;
    }
    bitstream_reader_init(&reader, fp_bin_in);

    int prev_dc_dec_cr = 0;
    for (int k = 0; k < num_blocks_chr; k++)
    {
        int decoded_block[BLOCK_SIZE][BLOCK_SIZE];
        decode_block(decoded_block, &prev_dc_dec_cr, &g_chroma_huff_tables, &reader);
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks_chr[k][i][j].Cr = (double)decoded_block[i][j];
            }
        }
    }

    fclose(fp_bin_in);

    // 4. Dequantization and IDCT
	applyDequantization_Y(blocks_y, num_blocks, q_table_y);
	applyDequantization_chr(blocks_chr, num_blocks_chr, q_table_chr);
    applyDCT_inverse_Y(blocks_y, num_blocks);
	applyDCT_inverse_chr(blocks_chr, num_blocks_chr);

	// 5. Merge blocks and rebuild RGB image
    unsigned char **Y_rec = allocate_y_u8(InfoHeader);
    mergeBlocks_Y(blocks_y, num_blocks, Y_rec, InfoHeader);
	mergeBlocks_chr(blocks_chr, num_blocks_chr, compressed_chromancy, InfoHeader);
    Pixel **reconstructed = convertBMP(Y_rec, compressed_chromancy, &InfoHeader);

    // 6. Exportar BMP
    exportImage(result_name, &FileHeader, &InfoHeader, reconstructed);

    free_blocks_Y(blocks_y, num_blocks);
    free_blocks_chr(blocks_chr, num_blocks_chr);

    return 0;
}
