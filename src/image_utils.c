#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==================================================================================
// DEFINIÇÃO DAS TABELAS GLOBAIS
//==================================================================================

// --- Qualidade 75% (Alta Qualidade, Baixa Compressão) ---

// Matriz de Luminância (Y) - Qualidade 75%
const int LUMINANCE_Q75[8][8] = {
    {8, 6, 5, 8, 12, 20, 26, 31},
    {6, 6, 7, 10, 13, 29, 30, 28},
    {7, 7, 8, 12, 20, 29, 35, 28},
    {7, 9, 11, 15, 26, 44, 40, 31},
    {9, 11, 19, 28, 34, 55, 52, 39},
    {12, 18, 28, 32, 41, 52, 57, 46},
    {25, 32, 39, 44, 52, 61, 60, 51},
    {36, 46, 48, 49, 56, 50, 52, 50}};

// Matriz de Crominância (Cb/Cr) - Qualidade 75%
const int CHROMINANCE_Q75[8][8] = {
    {9, 9, 12, 24, 50, 50, 50, 50},
    {9, 11, 13, 33, 50, 50, 50, 50},
    {12, 13, 28, 50, 50, 50, 50, 50},
    {24, 33, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50}};

// --- Qualidade 50% (Padrão/Base) ---

// Matriz de Luminância (Y) - Qualidade 50%
const int LUMINANCE_Q50[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}};

// Matriz de Crominância (Cb/Cr) - Qualidade 50%
const int CHROMINANCE_Q50[8][8] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}};

// --- Qualidade 25% (Baixa Qualidade, Alta Compressão) ---

// Matriz de Luminância (Y) - Qualidade 25%
const int LUMINANCE_Q25[8][8] = {
    {32, 22, 20, 32, 48, 80, 102, 122},
    {24, 24, 28, 38, 52, 116, 120, 110},
    {28, 26, 32, 48, 80, 114, 138, 112},
    {28, 34, 44, 58, 102, 174, 160, 124},
    {36, 44, 74, 112, 136, 218, 206, 154},
    {48, 70, 110, 128, 162, 208, 226, 184},
    {98, 128, 156, 174, 206, 242, 240, 202},
    {144, 184, 190, 196, 224, 200, 206, 198}};

// Matriz de Crominância (Cb/Cr) - Qualidade 25%
const int CHROMINANCE_Q25[8][8] = {
    {34, 36, 48, 94, 198, 198, 198, 198},
    {36, 42, 52, 132, 198, 198, 198, 198},
    {48, 52, 112, 198, 198, 198, 198, 198},
    {94, 132, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198},
    {198, 198, 198, 198, 198, 198, 198, 198}};
// matrix for DCT in 8x8 blocks
const double M_dct[8][8] = {
    {0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536},
    {0.4904, 0.4157, 0.2778, 0.0975, -0.0975, -0.2778, -0.4157, -0.4904},
    {0.4619, 0.1913, -0.1913, -0.4619, -0.4619, -0.1913, 0.1913, 0.4619},
    {0.4157, -0.0975, -0.4904, -0.2778, 0.2778, 0.4904, 0.0975, -0.4157},
    {0.3536, -0.3536, -0.3536, 0.3536, 0.3536, -0.3536, -0.3536, 0.3536},
    {0.2778, -0.4904, 0.0975, 0.4157, -0.4157, -0.0975, 0.4904, -0.2778},
    {0.1913, -0.4619, 0.4619, -0.1913, -0.1913, 0.4619, -0.4619, 0.1913},
    {0.0975, -0.2778, 0.4157, -0.4904, 0.4904, -0.4157, 0.2778, -0.0975}};

// Transpose of M_dct that is equal to the inverse of M_dct because M_dct is orthogonal
const double M_dct_t[8][8] = {
    {0.3536, 0.4904, 0.4619, 0.4157, 0.3536, 0.2778, 0.1913, 0.0975},
    {0.3536, 0.4157, 0.1913, -0.0975, -0.3536, -0.4904, -0.4619, -0.2778},
    {0.3536, 0.2778, -0.1913, -0.4904, -0.3536, 0.0975, 0.4619, 0.4157},
    {0.3536, 0.0975, -0.4619, -0.2778, 0.3536, 0.4157, -0.1913, -0.4904},
    {0.3536, -0.0975, -0.4619, 0.2778, 0.3536, -0.4157, -0.1913, 0.4904},
    {0.3536, -0.2778, -0.1913, 0.4904, -0.3536, -0.0975, 0.4619, -0.4157},
    {0.3536, -0.4157, 0.1913, 0.0975, -0.3536, 0.4904, -0.4619, 0.2778},
    {0.3536, -0.4904, 0.4619, -0.4157, 0.3536, -0.2778, 0.1913, -0.0975}};


const int g_zigzag_map[64] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};

/*
// Luminância DC
HuffmanCode g_huff_dc_luma[12] = {
    {0x00, 2}, {0x02, 3}, {0x03, 3}, {0x04, 3}, {0x05, 3}, {0x06, 3}, {0x0E, 4}, {0x1E, 5}, {0x3E, 6}, {0x7E, 7}, {0xFE, 8}, {0x1FE, 9}};

    // Crominância DC
    HuffmanCode g_huff_dc_chroma[12] = {
        {0x00, 2}, {0x01, 2}, {0x02, 2}, {0x06, 3}, {0x0E, 4}, {0x1E, 5}, {0x3E, 6}, {0x7E, 7}, {0xFE, 8}, {0x1FE, 9}, {0x3FE, 10}, {0x7FE, 11}};

        // Luminância AC
        HuffmanCode g_huff_ac_luma[256] = {
    {0x0A, 4}, {0x00, 2}, {0x01, 2}, {0x04, 3}, {0x0B, 4}, {0x1A, 5}, {0x78, 7}, {0xF7C, 12}, {0x05, 3}, {0x1B, 5}, {0xF7D, 12}, {0xFFDCC, 20}, {0x1C, 5}, {0x38, 6}, {0x79, 7}, {0xF7E, 12}, {0xFFDCD, 20}, {0xFFDD0, 20}, {0x02, 3}, {0x0C, 4}, {0x39, 6}, {0xF7F, 12}, {0xFFDD1, 20}, {0xFFDD2, 20}, {0xFFDD3, 20}, {0xFFDD4, 20}, {0xFFDD5, 20}, {0xFFDD6, 20}, {0xFFDD7, 20}, {0xFFDD8, 20}, {0x1D, 5}, {0x58, 7}, {0xF80, 12}, {0xFFDD9, 20}, {0xFFDDA, 20}, {0xFFDDB, 20}, {0xFFDDC, 20}, {0xFFDDD, 20}, {0xFFDDE, 20}, {0xFFDDF, 20}, {0xFFDE0, 20}, {0xFFDE1, 20}, {0x06, 3}, {0x1E, 5}, {0x98, 8}, {0xF81, 12}, {0xFFDE2, 20}, {0xFFDE3, 20}, {0xFFDE4, 20}, {0xFFDE5, 20}, {0xFFDE6, 20}, {0xFFDE7, 20}, {0xFFDE8, 20}, {0xFFDE9, 20}, {0x0D, 4}, {0x3A, 6}, {0xF82, 12}, {0xFFDEA, 20}, {0xFFDEB, 20}, {0xFFDEC, 20}, {0xFFDED, 20}, {0xFFDEE, 20}, {0xFFDEF, 20}, {0xFFDF0, 20}, {0xFFDF1, 20}, {0xFFDF2, 20}, {0x1F, 5}, {0x59, 7}, {0xF83, 12}, {0xFFDF3, 20}, {0xFFDF4, 20}, {0xFFDF5, 20}, {0xFFDF6, 20}, {0xFFDF7, 20}, {0xFFDF8, 20}, {0xFFDF9, 20}, {0xFFDFA, 20}, {0xFFDFB, 20}, {0x0E, 4}, {0x7A, 7}, {0xF84, 12}, {0xFFDFC, 20}, {0xFFDFD, 20}, {0xFFDFE, 20}, {0xFFDFF, 20}, {0xFFE00, 20}, {0xFFE01, 20}, {0xFFE02, 20}, {0xFFE03, 20}, {0xFFE04, 20}, {0x3B, 6}, {0x99, 8}, {0xF85, 12}, {0xFFE05, 20}, {0xFFE06, 20}, {0xFFE07, 20}, {0xFFE08, 20}, {0xFFE09, 20}, {0xFFE0A, 20}, {0xFFE0B, 20}, {0xFFE0C, 20}, {0xFFE0D, 20}, {0x07, 4}, {0x5A, 7}, {0xF86, 12}, {0xFFE0E, 20}, {0xFFE0F, 20}, {0xFFE10, 20}, {0xFFE11, 20}, {0xFFE12, 20}, {0xFFE13, 20}, {0xFFE14, 20}, {0xFFE15, 20}, {0xFFE16, 20}, {0x0F, 4}, {0x7B, 7}, {0xF87, 12}, {0xFFE17, 20}, {0xFFE18, 20}, {0xFFE19, 20}, {0xFFE1A, 20}, {0xFFE1B, 20}, {0xFFE1C, 20}, {0xFFE1D, 20}, {0xFFE1E, 20}, {0xFFE1F, 20}, {0x3C, 6}, {0x9A, 8}, {0xF88, 12}, {0xFFE20, 20}, {0xFFE21, 20}, {0xFFE22, 20}, {0xFFE23, 20}, {0xFFE24, 20}, {0xFFE25, 20}, {0xFFE26, 20}, {0xFFE27, 20}, {0xFFE28, 20}, {0x5B, 7}, {0xF89, 12}, {0xFFE29, 20}, {0xFFE2A, 20}, {0xFFE2B, 20}, {0xFFE2C, 20}, {0xFFE2D, 20}, {0xFFE2E, 20}, {0xFFE2F, 20}, {0xFFE30, 20}, {0xFFE31, 20}, {0xFFE32, 20}, {0x03, 3}, {0x1A, 5}, {0x38, 6}, {0x59, 7}, {0xF7D, 12}, {0xF81, 12}, {0xF86, 12}, {0xFFDCE, 20}, {0x08, 4}, {0x79, 7}, {0xF84, 12}, {0xFFE02, 20}, {0xFFE15, 20}, {0xFFE22, 20}, {0xFFE29, 20}, {0xFFE33, 20}, {0xFFE34, 20}, {0xFFE35, 20}, {0xFFE36, 20}, {0xFFE37, 20}, {0x99, 8}, {0xF88, 12}, {0xFFE38, 20}, {0xFFE39, 20}, {0xFFE3A, 20}, {0xFFE3B, 20}, {0xFFE3C, 20}, {0xFFE3D, 20}, {0xFFE3E, 20}, {0xFFE3F, 20}, {0xFFE40, 20}, {0xFFE41, 20}, {0x09, 4}, {0x39, 6}, {0x58, 7}, {0x98, 8}, {0xF7C, 12}, {0xF7F, 12}, {0xF83, 12}, {0xF8A, 12}, {0xFFE42, 20}, {0x19, 5}, {0x78, 7}, {0xF82, 12}, {0xFFDFB, 20}, {0xFFE12, 20}, {0xFFE1F, 20}, {0xFFE28, 20}, {0xFFE43, 20}, {0xFFE44, 20}, {0xFFE45, 20}, {0xFFE46, 20}, {0xFFE47, 20}, {0xF89, 12}, {0xFFE48, 20}, {0xFFE49, 20}, {0xFFE4A, 20}, {0xFFE4B, 20}, {0xFFE4C, 20}, {0xFFE4D, 20}, {0xFFE4E, 20}, {0xFFE4F, 20}, {0xFFE50, 20}, {0xFFE51, 20}};

    // Crominância AC
    HuffmanCode g_huff_ac_chroma[256] = {
        {0xFE590, 19}, {0x00, 2}, {0x01, 2}, {0x02, 3}, {0x0A, 4}, {0x18, 5}, {0x38, 6}, {0x78, 7}, {0xF7C, 12}, {0x0B, 4}, {0x58, 7}, {0xF7D, 12}, {0xFFDCC, 20}, {0x03, 3}, {0x19, 5}, {0x98, 8}, {0xF7E, 12}, {0xFFDCD, 20}, {0x1A, 5}, {0xF7F, 12}, {0xFFDD0, 20}, {0xFFDD1, 20}, {0xFFDD2, 20}, {0xFFDD3, 20}, {0xFFDD4, 20}, {0xFFDD5, 20}, {0xFFDD6, 20}, {0xFFDD7, 20}, {0xFFDD8, 20}, {0xFFDD9, 20}, {0x04, 3}, {0x1B, 5}, {0xF80, 12}, {0xFFDDA, 20}, {0xFFDDB, 20}, {0xFFDDC, 20}, {0xFFDDD, 20}, {0xFFDDE, 20}, {0xFFDDF, 20}, {0xFFDE0, 20}, {0xFFDE1, 20}, {0xFFDE2, 20}, {0x0C, 4}, {0x39, 6}, {0xF81, 12}, {0xFFDE3, 20}, {0xFFDE4, 20}, {0xFFDE5, 20}, {0xFFDE6, 20}, {0xFFDE7, 20}, {0xFFDE8, 20}, {0xFFDE9, 20}, {0xFFDEA, 20}, {0xFFDEB, 20}, {0x1C, 5}, {0x59, 7}, {0xF82, 12}, {0xFFDEC, 20}, {0xFFDED, 20}, {0xFFDEE, 20}, {0xFFDEF, 20}, {0xFFDF0, 20}, {0xFFDF1, 20}, {0xFFDF2, 20}, {0xFFDF3, 20}, {0xFFDF4, 20}, {0x05, 3}, {0x1D, 5}, {0xF83, 12}, {0xFFDF5, 20}, {0xFFDF6, 20}, {0xFFDF7, 20}, {0xFFDF8, 20}, {0xFFDF9, 20}, {0xFFDFA, 20}, {0xFFDFB, 20}, {0xFFDFC, 20}, {0xFFDFD, 20}, {0x0D, 4}, {0x3A, 6}, {0xF84, 12}, {0xFFDFE, 20}, {0xFFDFF, 20}, {0xFFE00, 20}, {0xFFE01, 20}, {0xFFE02, 20}, {0xFFE03, 20}, {0xFFE04, 20}, {0xFFE05, 20}, {0xFFE06, 20}, {0x0E, 4}, {0x5A, 7}, {0xF85, 12}, {0xFFE07, 20}, {0xFFE08, 20}, {0xFFE09, 20}, {0xFFE0A, 20}, {0xFFE0B, 20}, {0xFFE0C, 20}, {0xFFE0D, 20}, {0xFFE0E, 20}, {0xFFE0F, 20}, {0x1E, 5}, {0x79, 7}, {0xF86, 12}, {0xFFE10, 20}, {0xFFE11, 20}, {0xFFE12, 20}, {0xFFE13, 20}, {0xFFE14, 20}, {0xFFE15, 20}, {0xFFE16, 20}, {0xFFE17, 20}, {0xFFE18, 20}, {0x0F, 4}, {0x3B, 6}, {0xF87, 12}, {0xFFE19, 20}, {0xFFE1A, 20}, {0xFFE1B, 20}, {0xFFE1C, 20}, {0xFFE1D, 20}, {0xFFE1E, 20}, {0xFFE1F, 20}, {0xFFE20, 20}, {0xFFE21, 20}, {0x1F, 5}, {0x7A, 7}, {0xF88, 12}, {0xFFE22, 20}, {0xFFE23, 20}, {0xFFE24, 20}, {0xFFE25, 20}, {0xFFE26, 20}, {0xFFE27, 20}, {0xFFE28, 20}, {0xFFE29, 20}, {0xFFE2A, 20}, {0x3C, 6}, {0x99, 8}, {0xF89, 12}, {0xFFE2B, 20}, {0xFFE2C, 20}, {0xFFE2D, 20}, {0xFFE2E, 20}, {0xFFE2F, 20}, {0xFFE30, 20}, {0xFFE31, 20}, {0xFFE32, 20}, {0xFFE33, 20}, {0x06, 4}, {0x1A, 5}, {0x58, 7}, {0xF7C, 12}, {0xF7F, 12}, {0xF82, 12}, {0xF85, 12}, {0xF88, 12}, {0xFFE34, 20}, {0x07, 4}, {0x1B, 5}, {0x78, 7}, {0xF7D, 12}, {0xF80, 12}, {0xF83, 12}, {0xF86, 12}, {0xF89, 12}, {0xFFE35, 20}, {0x38, 6}, {0x98, 8}, {0xF8A, 12}, {0xFFE36, 20}, {0xFFE37, 20}, {0xFFE38, 20}, {0xFFE39, 20}, {0xFFE3A, 20}, {0xFFE3B, 20}, {0xFFE3C, 20}, {0xFFE3D, 20}, {0xFFE3E, 20}, {0x08, 4}, {0x1C, 5}, {0xF8B, 12}, {0xFFE3F, 20}, {0xFFE40, 20}, {0xFFE41, 20}, {0xFFE42, 20}, {0xFFE43, 20}, {0xFFE44, 20}, {0xFFE45, 20}, {0xFFE46, 20}, {0xFFE47, 20}, {0x09, 4}, {0x3A, 6}, {0xF8C, 12}, {0xFFE48, 20}, {0xFFE49, 20}, {0xFFE4A, 20}, {0xFFE4B, 20}, {0xFFE4C, 20}, {0xFFE4D, 20}, {0xFFE4E, 20}, {0xFFE4F, 20}, {0xFFE50, 20}, {0x5A, 7}, {0xF8D, 12}, {0xFFE51, 20}, {0xFFE52, 20}, {0xFFE53, 20}, {0xFFE54, 20}, {0xFFE55, 20}, {0xFFE56, 20}, {0xFFE57, 20}, {0xFFE58, 20}, {0xFFE59, 20}, {0xFFE5A, 20}};
*/

// --- TABELAS HUFFMAN PADRÃO (Anexo K da especificação JPEG) ---

// Tabela DC de Luminância (12 símbolos para as categorias 0-11)
HuffmanCode g_huff_dc_luma_table[12] = {
    {0x00, 2},
    {0x02, 3},
    {0x03, 3},
    {0x04, 3},
    {0x05, 3},
    {0x06, 3},
    {0x0E, 4},
    {0x1E, 5},
    {0x3E, 6},
    {0x7E, 7},
    {0xFE, 8},
    {0x1FE, 9}};

// Tabela DC de Crominância (12 símbolos para as categorias 0-11)
HuffmanCode g_huff_dc_chroma_table[12] = {
    {0x00, 2},
    {0x01, 2},
    {0x02, 2},
    {0x06, 3},
    {0x0E, 4},
    {0x1E, 5},
    {0x3E, 6},
    {0x7E, 7},
    {0xFE, 8},
    {0x1FE, 9},
    {0x3FE, 10},
    {0x7FE, 11}};

// Tabela AC de Luminância (256 símbolos para (RUN, SIZE))
HuffmanCode g_huff_ac_luma_table[256] = {
    /* Símbolos de 0x00 a 0xFF. O índice é o símbolo (RUN << 4) | SIZE. */
    /* 0x00 (EOB) */ {0x0A, 4}, /* 0x01 */ {0x00, 2}, /* 0x02 */ {0x01, 2}, /* 0x03 */ {0x04, 3},
    /* 0x04 */ {0x0B, 4}, /* 0x05 */ {0x1A, 5}, /* 0x06 */ {0x78, 7}, /* 0x07 */ {0xF7C, 12},
    /* 0x08 */ {0, 0}, /* 0x09 */ {0, 0}, /* 0x0A */ {0, 0}, /* 0x0B */ {0, 0},
    /* 0x0C */ {0, 0}, /* 0x0D */ {0, 0}, /* 0x0E */ {0, 0}, /* 0x0F */ {0, 0},
    /* 0x10 */ {0, 0}, /* 0x11 */ {0x05, 3}, /* 0x12 */ {0x1B, 5}, /* 0x13 */ {0x38, 6},
    /* 0x14 */ {0x58, 7}, /* 0x15 */ {0x98, 8}, /* 0x16 */ {0xF7D, 12}, /* ... etc (valores zerados foram omitidos) */
    [0xF0] = {0xFFE33, 20}                                              // ZRL (Zero-Run-Length) - Exemplo, valor pode variar.
};

// Tabela AC de Crominância (256 símbolos para (RUN, SIZE))
HuffmanCode g_huff_ac_chroma_table[256] = {
    /* 0x00 (EOB) */ {0x0A, 4}, /* 0x01 */ {0x00, 2}, /* 0x02 */ {0x01, 2}, /* 0x03 */ {0x02, 3},
    /* 0x04 */ {0x0A, 4}, /* 0x05 */ {0x18, 5}, /* 0x06 */ {0x38, 6}, /* 0x07 */ {0x78, 7},
    /* ... etc (valores zerados foram omitidos) */
    [0xF0] = {0xFFE48, 20} // ZRL (Zero-Run-Length)
};
HuffmanTable g_luma_huff_tables = {g_huff_dc_luma_table, g_huff_ac_luma_table};
HuffmanTable g_chroma_huff_tables = {g_huff_dc_chroma_table, g_huff_ac_chroma_table};

//==================================================================================
// LÓGICA DE HUFFMAN - ESCRITA (ENCODING)
//==================================================================================

/**
 * @brief Inicializa a estrutura de escrita de bitstream.
 */
void bitstream_writer_init(BitstreamWriter *writer, FILE *f)
{
    writer->file = f;
    writer->buffer = 0;
    writer->bit_count = 0;
}

/**
 * @brief Escreve um byte completo para o arquivo, aplicando byte-stuffing se necessário.
 */
static void write_byte(BitstreamWriter *writer)
{
    fputc(writer->buffer, writer->file);
    // Regra de "Byte Stuffing" do JPEG: se o byte for 0xFF, um 0x00 deve ser inserido.
    if (writer->buffer == 0xFF)
    {
        fputc(0x00, writer->file);
    }
    writer->buffer = 0;
    writer->bit_count = 0;
}

/**
 * @brief Adiciona uma sequência de bits (um código) ao buffer.
 */
static void write_bits(BitstreamWriter *writer, uint32_t code, int length)
{
    for (int i = length - 1; i >= 0; i--)
    {
        int bit = (code >> i) & 1;
        writer->buffer = (writer->buffer << 1) | bit;
        writer->bit_count++;
        if (writer->bit_count == 8)
        {
            write_byte(writer);
        }
    }
}

/**
 * @brief Garante que qualquer bit remanescente no buffer seja escrito no arquivo (padding).
 */
void bitstream_writer_flush(BitstreamWriter *writer)
{
    if (writer->bit_count > 0)
    {
        // Completa o byte com 1s, conforme especificação JPEG para padding.
        writer->buffer <<= (8 - writer->bit_count);
        writer->buffer |= (0xFF >> writer->bit_count);
        write_byte(writer);
    }
}

/**
 * @brief Converte um valor de coeficiente em sua Categoria e Amplitude.
 * Esta é uma etapa CRUCIAL e segue as tabelas F.1 e F.2 da especificação JPEG.
 * @param value O valor do coeficiente (ex: -5, 27).
 * @param amplitude_bits Ponteiro para armazenar os bits de amplitude calculados.
 * @return A categoria do coeficiente (tamanho em bits da amplitude).
 */
static int get_category_and_amplitude(int value, uint32_t *amplitude_bits)
{
    if (value == 0)
    {
        *amplitude_bits = 0;
        return 0;
    }

    int temp = (value > 0) ? value : -value;
    int category = 0;

    // Encontra a categoria (equivalente a floor(log2(abs(value))) + 1)
    while (temp > 0)
    {
        temp >>= 1;
        category++;
    }

    // Para valores positivos, a amplitude são os próprios bits do valor.
    // Para negativos, é o valor em complemento de 1.
    if (value > 0)
    {
        *amplitude_bits = (uint32_t)value;
    }
    else
    {
        *amplitude_bits = (uint32_t)((1 << category) - 1 + value);
    }
    return category;
}

/**
 * @brief Codifica um bloco 8x8 de coeficientes quantizados.
 * @param block Bloco 8x8 de inteiros (após DCT e quantização).
 * @param prev_dc Ponteiro para o valor DC do bloco anterior (codificação diferencial).
 * @param huff_tables As tabelas de Huffman (DC e AC) a serem usadas.
 * @param writer O escritor de bitstream para o arquivo de saída.
 */
void encode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int *prev_dc, HuffmanTable *huff_tables, BitstreamWriter *writer)
{
    int zigzag_coeffs[64];

    // 1. Aplicar o Zig-Zag Scan
    for (int i = 0; i < 64; i++)
    {
        zigzag_coeffs[i] = block[g_zigzag_map[i] / BLOCK_SIZE][g_zigzag_map[i] % BLOCK_SIZE];
    }

    // 2. Codificar o Coeficiente DC
    int dc_diff = zigzag_coeffs[0] - *prev_dc;
    *prev_dc = zigzag_coeffs[0];

    uint32_t dc_amplitude;
    int dc_category = get_category_and_amplitude(dc_diff, &dc_amplitude);

    write_bits(writer, huff_tables->dc_table[dc_category].code, huff_tables->dc_table[dc_category].length);
    if (dc_category > 0)
    {
        write_bits(writer, dc_amplitude, dc_category);
    }

    // 3. Codificar os 63 Coeficientes AC
    int zero_run_count = 0;
    for (int i = 1; i < 64; i++)
    {
        if (zigzag_coeffs[i] == 0)
        {
            zero_run_count++;
        }
        else
        {
            // Se acumulou mais de 15 zeros, escreve o código ZRL
            while (zero_run_count >= 16)
            {
                write_bits(writer, huff_tables->ac_table[0xF0].code, huff_tables->ac_table[0xF0].length); // ZRL (15,0)
                zero_run_count -= 16;
            }

            uint32_t ac_amplitude;
            int ac_category = get_category_and_amplitude(zigzag_coeffs[i], &ac_amplitude);

            // O símbolo é uma combinação do número de zeros (RUN) e da categoria (SIZE)
            uint8_t run_size_symbol = (zero_run_count << 4) | ac_category;

            write_bits(writer, huff_tables->ac_table[run_size_symbol].code, huff_tables->ac_table[run_size_symbol].length);
            write_bits(writer, ac_amplitude, ac_category);

            zero_run_count = 0; // Reseta a contagem de zeros
        }
    }

    // Se o restante do bloco for de zeros (zero_run_count > 0), escreve o código EOB
    if (zero_run_count > 0)
    {
        write_bits(writer, huff_tables->ac_table[0x00].code, huff_tables->ac_table[0x00].length); // EOB (0,0)
    }
}

//==================================================================================
// LÓGICA DE HUFFMAN - LEITURA (DECODING)
//==================================================================================

/**
 * @brief Inicializa a estrutura de leitura de bitstream.
 */
void bitstream_reader_init(BitstreamReader *reader, FILE *f)
{
    reader->file = f;
    reader->buffer = 0;
    reader->bit_position = -1; // Força a primeira leitura de um byte
}

/**
 * @brief Lê um único bit do arquivo.
 */
static int read_bit(BitstreamReader *reader)
{
    if (reader->bit_position < 0)
    {
        int byte = fgetc(reader->file);
        if (byte == EOF)
            return -1;

        // Trata o byte-stuffing: se for 0xFF seguido de 0x00, ignora o 0x00
        if (byte == 0xFF)
        {
            int next_byte = fgetc(reader->file);
            if (next_byte == 0x00)
            {
                // É um byte recheado, byte válido é 0xFF
            }
            else if (next_byte == EOF)
            {
                return -1;
            }
            else
            {
                // É um marcador JPEG, retorna o byte para o stream
                ungetc(next_byte, reader->file);
            }
        }
        reader->buffer = (uint8_t)byte;
        reader->bit_position = 7;
    }

    int bit = (reader->buffer >> reader->bit_position) & 1;
    reader->bit_position--;
    return bit;
}

/**
 * @brief Procura por um código de Huffman no bitstream que corresponda a um na tabela.
 */
static int find_huffman_symbol(BitstreamReader *reader, HuffmanCode table[], int table_size)
{
    uint32_t current_code = 0;
    int length = 0;

    while (length < 16)
    { // Limite de 16 bits para códigos Huffman no JPEG padrão
        int bit = read_bit(reader);
        if (bit == -1)
            return -1; // Fim do arquivo

        current_code = (current_code << 1) | bit;
        length++;

        // Procura na tabela por um código correspondente
        for (int i = 0; i < table_size; i++)
        {
            if (table[i].length == length && table[i].code == current_code)
            {
                return i; // Retorna o SÍMBOLO (índice), não o código
            }
        }
    }
    return -1; // Código não encontrado
}

/**
 * @brief Lê os bits de amplitude após um símbolo de Huffman ser encontrado.
 */
static int read_amplitude(BitstreamReader *reader, int category)
{
    int amplitude = 0;
    for (int i = 0; i < category; i++)
    {
        int bit = read_bit(reader);
        if (bit == -1)
            return -1; // Erro/EOF
        amplitude = (amplitude << 1) | bit;
    }
    return amplitude;
}

/**
 * @brief Reconstrói o valor original do coeficiente a partir de sua categoria e amplitude.
 */
static int reconstruct_value(int category, int amplitude)
{
    if (category == 0)
        return 0;

    int half_range = 1 << (category - 1);

    if (amplitude >= half_range)
    {
        return amplitude; // Positivo
    }
    else
    {
        return amplitude - ((1 << category) - 1); // Negativo
    }
}

/**
 * @brief Decodifica um bloco 8x8 de coeficientes do bitstream.
 */
void decode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int *prev_dc, HuffmanTable *huff_tables, BitstreamReader *reader)
{
    int zigzag_coeffs[64] = {0}; // Inicializa com zeros

    // 1. Decodificar o Coeficiente DC
    int dc_category = find_huffman_symbol(reader, huff_tables->dc_table, 12);
    if (dc_category >= 0 && dc_category < 12)
    {
        int dc_amplitude = read_amplitude(reader, dc_category);
        int dc_diff = reconstruct_value(dc_category, dc_amplitude);
        zigzag_coeffs[0] = dc_diff + *prev_dc;
        *prev_dc = zigzag_coeffs[0];
    }

    // 2. Decodificar os 63 Coeficientes AC
    for (int i = 1; i < 64;)
    {
        int ac_symbol = find_huffman_symbol(reader, huff_tables->ac_table, 256);
        if (ac_symbol == -1)
            break; // Erro ou fim do stream

        if (ac_symbol == 0x00)
        {          // EOB
            break; // O resto do bloco é zero
        }

        if (ac_symbol == 0xF0)
        {            // ZRL
            i += 16; // Pula 16 zeros
            continue;
        }

        int run = (ac_symbol >> 4) & 0x0F;
        int size = ac_symbol & 0x0F;

        i += run; // Pula 'run' zeros

        if (i < 64 && size > 0)
        {
            int ac_amplitude = read_amplitude(reader, size);
            zigzag_coeffs[i] = reconstruct_value(size, ac_amplitude);
        }
        i++;
    }

    // 3. Aplicar Zig-Zag Inverso
    for (int i = 0; i < 64; i++)
    {
        block[g_zigzag_map[i] / BLOCK_SIZE][g_zigzag_map[i] % BLOCK_SIZE] = zigzag_coeffs[i];
    }
}



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

double ***allocate_blocks_Y(int num_blocks)
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

Chromancy ***allocate_blocks_chr(int num_blocks)
{
    // 1. Aloca a "lista" de ponteiros para as matrizes 8x8 (Chromancy**)
    Chromancy ***blocks = (Chromancy ***)malloc(num_blocks * sizeof(Chromancy **));
    if (blocks == NULL)
    {
        // Falha na alocação
        return NULL;
    }

    // 2. Para cada bloco na lista, aloca uma matriz 8x8
    for (int k = 0; k < num_blocks; k++)
    {
        // 2a. Aloca as 8 linhas (ponteiros para Chromancy*) para a matriz k
        blocks[k] = (Chromancy **)malloc(BLOCK_SIZE * sizeof(Chromancy *));
        if (blocks[k] == NULL)
        {
            // Falha na alocação, precisa liberar o que já foi alocado
            // (código de limpeza omitido por simplicidade, mas importante em produção)
            return NULL;
        }

        // 2b. Para cada linha da matriz k, aloca os 8 elementos (Chromancys)
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            blocks[k][i] = (Chromancy *)malloc(BLOCK_SIZE * sizeof(Chromancy));
            if (blocks[k][i] == NULL)
            {
                // Falha na alocação
                return NULL;
            }
        }
    }

    return blocks;
}

void free_blocks_Y(double ***blocks, int num_blocks)
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

void free_blocks_chr(Chromancy ***blocks, int num_blocks)
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

//==================================================================================
// DCT E IDCT / YCbCr E RGB / quantization E dequantization 
//==================================================================================


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
