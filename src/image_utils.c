#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==================================================================================
// GLOBAL TABLE DEFINITIONS
//==================================================================================

// --- Quality 75% (High Quality, Low Compression) ---

// Luminance matrix (Y) - Quality 75%
const int LUMINANCE_Q75[8][8] = {
    {8, 6, 5, 8, 12, 20, 26, 31},
    {6, 6, 7, 10, 13, 29, 30, 28},
    {7, 7, 8, 12, 20, 29, 35, 28},
    {7, 9, 11, 15, 26, 44, 40, 31},
    {9, 11, 19, 28, 34, 55, 52, 39},
    {12, 18, 28, 32, 41, 52, 57, 46},
    {25, 32, 39, 44, 52, 61, 60, 51},
    {36, 46, 48, 49, 56, 50, 52, 50}};

// Chrominance matrix (Cb/Cr) - Quality 75%
const int CHROMINANCE_Q75[8][8] = {
    {9, 9, 12, 24, 50, 50, 50, 50},
    {9, 11, 13, 33, 50, 50, 50, 50},
    {12, 13, 28, 50, 50, 50, 50, 50},
    {24, 33, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {50, 50, 50, 50, 50, 50, 50, 50}};

// --- Quality 50% (Baseline) ---

// Luminance matrix (Y) - Quality 50%
const int LUMINANCE_Q50[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}};

// Chrominance matrix (Cb/Cr) - Quality 50%
const int CHROMINANCE_Q50[8][8] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}};

// --- Quality 25% (Low Quality, High Compression) ---

// Luminance matrix (Y) - Quality 25%
const int LUMINANCE_Q25[8][8] = {
    {32, 22, 20, 32, 48, 80, 102, 122},
    {24, 24, 28, 38, 52, 116, 120, 110},
    {28, 26, 32, 48, 80, 114, 138, 112},
    {28, 34, 44, 58, 102, 174, 160, 124},
    {36, 44, 74, 112, 136, 218, 206, 154},
    {48, 70, 110, 128, 162, 208, 226, 184},
    {98, 128, 156, 174, 206, 242, 240, 202},
    {144, 184, 190, 196, 224, 200, 206, 198}};

// Chrominance matrix (Cb/Cr) - Quality 25%
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
// Luminance DC
HuffmanCode g_huff_dc_luma[12] = {
    {0x00, 2}, {0x02, 3}, {0x03, 3}, {0x04, 3}, {0x05, 3}, {0x06, 3}, {0x0E, 4}, {0x1E, 5}, {0x3E, 6}, {0x7E, 7}, {0xFE, 8}, {0x1FE, 9}};

    // Chrominance DC
    HuffmanCode g_huff_dc_chroma[12] = {
        {0x00, 2}, {0x01, 2}, {0x02, 2}, {0x06, 3}, {0x0E, 4}, {0x1E, 5}, {0x3E, 6}, {0x7E, 7}, {0xFE, 8}, {0x1FE, 9}, {0x3FE, 10}, {0x7FE, 11}};

        // Luminance AC
        HuffmanCode g_huff_ac_luma[256] = {
    {0x0A, 4}, {0x00, 2}, {0x01, 2}, {0x04, 3}, {0x0B, 4}, {0x1A, 5}, {0x78, 7}, {0xF7C, 12}, {0x05, 3}, {0x1B, 5}, {0xF7D, 12}, {0xFFDCC, 20}, {0x1C, 5}, {0x38, 6}, {0x79, 7}, {0xF7E, 12}, {0xFFDCD, 20}, {0xFFDD0, 20}, {0x02, 3}, {0x0C, 4}, {0x39, 6}, {0xF7F, 12}, {0xFFDD1, 20}, {0xFFDD2, 20}, {0xFFDD3, 20}, {0xFFDD4, 20}, {0xFFDD5, 20}, {0xFFDD6, 20}, {0xFFDD7, 20}, {0xFFDD8, 20}, {0x1D, 5}, {0x58, 7}, {0xF80, 12}, {0xFFDD9, 20}, {0xFFDDA, 20}, {0xFFDDB, 20}, {0xFFDDC, 20}, {0xFFDDD, 20}, {0xFFDDE, 20}, {0xFFDDF, 20}, {0xFFDE0, 20}, {0xFFDE1, 20}, {0x06, 3}, {0x1E, 5}, {0x98, 8}, {0xF81, 12}, {0xFFDE2, 20}, {0xFFDE3, 20}, {0xFFDE4, 20}, {0xFFDE5, 20}, {0xFFDE6, 20}, {0xFFDE7, 20}, {0xFFDE8, 20}, {0xFFDE9, 20}, {0x0D, 4}, {0x3A, 6}, {0xF82, 12}, {0xFFDEA, 20}, {0xFFDEB, 20}, {0xFFDEC, 20}, {0xFFDED, 20}, {0xFFDEE, 20}, {0xFFDEF, 20}, {0xFFDF0, 20}, {0xFFDF1, 20}, {0xFFDF2, 20}, {0x1F, 5}, {0x59, 7}, {0xF83, 12}, {0xFFDF3, 20}, {0xFFDF4, 20}, {0xFFDF5, 20}, {0xFFDF6, 20}, {0xFFDF7, 20}, {0xFFDF8, 20}, {0xFFDF9, 20}, {0xFFDFA, 20}, {0xFFDFB, 20}, {0x0E, 4}, {0x7A, 7}, {0xF84, 12}, {0xFFDFC, 20}, {0xFFDFD, 20}, {0xFFDFE, 20}, {0xFFDFF, 20}, {0xFFE00, 20}, {0xFFE01, 20}, {0xFFE02, 20}, {0xFFE03, 20}, {0xFFE04, 20}, {0x3B, 6}, {0x99, 8}, {0xF85, 12}, {0xFFE05, 20}, {0xFFE06, 20}, {0xFFE07, 20}, {0xFFE08, 20}, {0xFFE09, 20}, {0xFFE0A, 20}, {0xFFE0B, 20}, {0xFFE0C, 20}, {0xFFE0D, 20}, {0x07, 4}, {0x5A, 7}, {0xF86, 12}, {0xFFE0E, 20}, {0xFFE0F, 20}, {0xFFE10, 20}, {0xFFE11, 20}, {0xFFE12, 20}, {0xFFE13, 20}, {0xFFE14, 20}, {0xFFE15, 20}, {0xFFE16, 20}, {0x0F, 4}, {0x7B, 7}, {0xF87, 12}, {0xFFE17, 20}, {0xFFE18, 20}, {0xFFE19, 20}, {0xFFE1A, 20}, {0xFFE1B, 20}, {0xFFE1C, 20}, {0xFFE1D, 20}, {0xFFE1E, 20}, {0xFFE1F, 20}, {0x3C, 6}, {0x9A, 8}, {0xF88, 12}, {0xFFE20, 20}, {0xFFE21, 20}, {0xFFE22, 20}, {0xFFE23, 20}, {0xFFE24, 20}, {0xFFE25, 20}, {0xFFE26, 20}, {0xFFE27, 20}, {0xFFE28, 20}, {0x5B, 7}, {0xF89, 12}, {0xFFE29, 20}, {0xFFE2A, 20}, {0xFFE2B, 20}, {0xFFE2C, 20}, {0xFFE2D, 20}, {0xFFE2E, 20}, {0xFFE2F, 20}, {0xFFE30, 20}, {0xFFE31, 20}, {0xFFE32, 20}, {0x03, 3}, {0x1A, 5}, {0x38, 6}, {0x59, 7}, {0xF7D, 12}, {0xF81, 12}, {0xF86, 12}, {0xFFDCE, 20}, {0x08, 4}, {0x79, 7}, {0xF84, 12}, {0xFFE02, 20}, {0xFFE15, 20}, {0xFFE22, 20}, {0xFFE29, 20}, {0xFFE33, 20}, {0xFFE34, 20}, {0xFFE35, 20}, {0xFFE36, 20}, {0xFFE37, 20}, {0x99, 8}, {0xF88, 12}, {0xFFE38, 20}, {0xFFE39, 20}, {0xFFE3A, 20}, {0xFFE3B, 20}, {0xFFE3C, 20}, {0xFFE3D, 20}, {0xFFE3E, 20}, {0xFFE3F, 20}, {0xFFE40, 20}, {0xFFE41, 20}, {0x09, 4}, {0x39, 6}, {0x58, 7}, {0x98, 8}, {0xF7C, 12}, {0xF7F, 12}, {0xF83, 12}, {0xF8A, 12}, {0xFFE42, 20}, {0x19, 5}, {0x78, 7}, {0xF82, 12}, {0xFFDFB, 20}, {0xFFE12, 20}, {0xFFE1F, 20}, {0xFFE28, 20}, {0xFFE43, 20}, {0xFFE44, 20}, {0xFFE45, 20}, {0xFFE46, 20}, {0xFFE47, 20}, {0xF89, 12}, {0xFFE48, 20}, {0xFFE49, 20}, {0xFFE4A, 20}, {0xFFE4B, 20}, {0xFFE4C, 20}, {0xFFE4D, 20}, {0xFFE4E, 20}, {0xFFE4F, 20}, {0xFFE50, 20}, {0xFFE51, 20}};

    // Chrominance AC
    HuffmanCode g_huff_ac_chroma[256] = {
        {0xFE590, 19}, {0x00, 2}, {0x01, 2}, {0x02, 3}, {0x0A, 4}, {0x18, 5}, {0x38, 6}, {0x78, 7}, {0xF7C, 12}, {0x0B, 4}, {0x58, 7}, {0xF7D, 12}, {0xFFDCC, 20}, {0x03, 3}, {0x19, 5}, {0x98, 8}, {0xF7E, 12}, {0xFFDCD, 20}, {0x1A, 5}, {0xF7F, 12}, {0xFFDD0, 20}, {0xFFDD1, 20}, {0xFFDD2, 20}, {0xFFDD3, 20}, {0xFFDD4, 20}, {0xFFDD5, 20}, {0xFFDD6, 20}, {0xFFDD7, 20}, {0xFFDD8, 20}, {0xFFDD9, 20}, {0x04, 3}, {0x1B, 5}, {0xF80, 12}, {0xFFDDA, 20}, {0xFFDDB, 20}, {0xFFDDC, 20}, {0xFFDDD, 20}, {0xFFDDE, 20}, {0xFFDDF, 20}, {0xFFDE0, 20}, {0xFFDE1, 20}, {0xFFDE2, 20}, {0x0C, 4}, {0x39, 6}, {0xF81, 12}, {0xFFDE3, 20}, {0xFFDE4, 20}, {0xFFDE5, 20}, {0xFFDE6, 20}, {0xFFDE7, 20}, {0xFFDE8, 20}, {0xFFDE9, 20}, {0xFFDEA, 20}, {0xFFDEB, 20}, {0x1C, 5}, {0x59, 7}, {0xF82, 12}, {0xFFDEC, 20}, {0xFFDED, 20}, {0xFFDEE, 20}, {0xFFDEF, 20}, {0xFFDF0, 20}, {0xFFDF1, 20}, {0xFFDF2, 20}, {0xFFDF3, 20}, {0xFFDF4, 20}, {0x05, 3}, {0x1D, 5}, {0xF83, 12}, {0xFFDF5, 20}, {0xFFDF6, 20}, {0xFFDF7, 20}, {0xFFDF8, 20}, {0xFFDF9, 20}, {0xFFDFA, 20}, {0xFFDFB, 20}, {0xFFDFC, 20}, {0xFFDFD, 20}, {0x0D, 4}, {0x3A, 6}, {0xF84, 12}, {0xFFDFE, 20}, {0xFFDFF, 20}, {0xFFE00, 20}, {0xFFE01, 20}, {0xFFE02, 20}, {0xFFE03, 20}, {0xFFE04, 20}, {0xFFE05, 20}, {0xFFE06, 20}, {0x0E, 4}, {0x5A, 7}, {0xF85, 12}, {0xFFE07, 20}, {0xFFE08, 20}, {0xFFE09, 20}, {0xFFE0A, 20}, {0xFFE0B, 20}, {0xFFE0C, 20}, {0xFFE0D, 20}, {0xFFE0E, 20}, {0xFFE0F, 20}, {0x1E, 5}, {0x79, 7}, {0xF86, 12}, {0xFFE10, 20}, {0xFFE11, 20}, {0xFFE12, 20}, {0xFFE13, 20}, {0xFFE14, 20}, {0xFFE15, 20}, {0xFFE16, 20}, {0xFFE17, 20}, {0xFFE18, 20}, {0x0F, 4}, {0x3B, 6}, {0xF87, 12}, {0xFFE19, 20}, {0xFFE1A, 20}, {0xFFE1B, 20}, {0xFFE1C, 20}, {0xFFE1D, 20}, {0xFFE1E, 20}, {0xFFE1F, 20}, {0xFFE20, 20}, {0xFFE21, 20}, {0x1F, 5}, {0x7A, 7}, {0xF88, 12}, {0xFFE22, 20}, {0xFFE23, 20}, {0xFFE24, 20}, {0xFFE25, 20}, {0xFFE26, 20}, {0xFFE27, 20}, {0xFFE28, 20}, {0xFFE29, 20}, {0xFFE2A, 20}, {0x3C, 6}, {0x99, 8}, {0xF89, 12}, {0xFFE2B, 20}, {0xFFE2C, 20}, {0xFFE2D, 20}, {0xFFE2E, 20}, {0xFFE2F, 20}, {0xFFE30, 20}, {0xFFE31, 20}, {0xFFE32, 20}, {0xFFE33, 20}, {0x06, 4}, {0x1A, 5}, {0x58, 7}, {0xF7C, 12}, {0xF7F, 12}, {0xF82, 12}, {0xF85, 12}, {0xF88, 12}, {0xFFE34, 20}, {0x07, 4}, {0x1B, 5}, {0x78, 7}, {0xF7D, 12}, {0xF80, 12}, {0xF83, 12}, {0xF86, 12}, {0xF89, 12}, {0xFFE35, 20}, {0x38, 6}, {0x98, 8}, {0xF8A, 12}, {0xFFE36, 20}, {0xFFE37, 20}, {0xFFE38, 20}, {0xFFE39, 20}, {0xFFE3A, 20}, {0xFFE3B, 20}, {0xFFE3C, 20}, {0xFFE3D, 20}, {0xFFE3E, 20}, {0x08, 4}, {0x1C, 5}, {0xF8B, 12}, {0xFFE3F, 20}, {0xFFE40, 20}, {0xFFE41, 20}, {0xFFE42, 20}, {0xFFE43, 20}, {0xFFE44, 20}, {0xFFE45, 20}, {0xFFE46, 20}, {0xFFE47, 20}, {0x09, 4}, {0x3A, 6}, {0xF8C, 12}, {0xFFE48, 20}, {0xFFE49, 20}, {0xFFE4A, 20}, {0xFFE4B, 20}, {0xFFE4C, 20}, {0xFFE4D, 20}, {0xFFE4E, 20}, {0xFFE4F, 20}, {0xFFE50, 20}, {0x5A, 7}, {0xF8D, 12}, {0xFFE51, 20}, {0xFFE52, 20}, {0xFFE53, 20}, {0xFFE54, 20}, {0xFFE55, 20}, {0xFFE56, 20}, {0xFFE57, 20}, {0xFFE58, 20}, {0xFFE59, 20}, {0xFFE5A, 20}};
*/

// --- STANDARD HUFFMAN TABLES (JPEG Annex K) ---

// Luminance DC table (12 symbols for categories 0-11)
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

// Chrominance DC table (12 symbols for categories 0-11)
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

// Luminance AC table (256 symbols for (RUN, SIZE))
HuffmanCode g_huff_ac_luma_table[256] = {
    /* Symbols from 0x00 to 0xFF. The index equals (RUN << 4) | SIZE. */
    /* 0x00 (EOB) */ {0x0A, 4}, /* 0x01 */ {0x00, 2}, /* 0x02 */ {0x01, 2}, /* 0x03 */ {0x04, 3},
    /* 0x04 */ {0x0B, 4}, /* 0x05 */ {0x1A, 5}, /* 0x06 */ {0x78, 7}, /* 0x07 */ {0xF7C, 12},
    /* 0x08 */ {0, 0}, /* 0x09 */ {0, 0}, /* 0x0A */ {0, 0}, /* 0x0B */ {0, 0},
    /* 0x0C */ {0, 0}, /* 0x0D */ {0, 0}, /* 0x0E */ {0, 0}, /* 0x0F */ {0, 0},
    /* 0x10 */ {0, 0}, /* 0x11 */ {0x05, 3}, /* 0x12 */ {0x1B, 5}, /* 0x13 */ {0x38, 6},
    /* 0x14 */ {0x58, 7}, /* 0x15 */ {0x98, 8}, /* 0x16 */ {0xF7D, 12}, /* ... zeroed values omitted for brevity */
    [0xF0] = {0xFFE33, 20}                                              // ZRL (Zero-Run-Length) - illustrative value
};

// Chrominance AC table (256 symbols for (RUN, SIZE))
HuffmanCode g_huff_ac_chroma_table[256] = {
    /* 0x00 (EOB) */ {0x0A, 4}, /* 0x01 */ {0x00, 2}, /* 0x02 */ {0x01, 2}, /* 0x03 */ {0x02, 3},
    /* 0x04 */ {0x0A, 4}, /* 0x05 */ {0x18, 5}, /* 0x06 */ {0x38, 6}, /* 0x07 */ {0x78, 7},
    /* ... zeroed values omitted for brevity */
    [0xF0] = {0xFFE48, 20} // ZRL (Zero-Run-Length)
};
HuffmanTable g_luma_huff_tables = {g_huff_dc_luma_table, g_huff_ac_luma_table};
HuffmanTable g_chroma_huff_tables = {g_huff_dc_chroma_table, g_huff_ac_chroma_table};

// --- Canonical JPEG tables (Annex K) and builder ---
static const uint8_t STD_LUMA_DC_BITS[16]   = {0x00,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00};
static const uint8_t STD_LUMA_DC_VALS[12]   = {0,1,2,3,4,5,6,7,8,9,10,11};
static const uint8_t STD_CHROMA_DC_BITS[16] = {0x00,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00};
static const uint8_t STD_CHROMA_DC_VALS[12] = {0,1,2,3,4,5,6,7,8,9,10,11};

static const uint8_t STD_LUMA_AC_BITS[16]   = {0x00,0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7D};
static const uint8_t STD_LUMA_AC_VALS[162]  = {
    0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,
    0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,
    0x24,0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
    0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
    0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,
    0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,
    0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,
    0xF9,0xFA
};

static const uint8_t STD_CHROMA_AC_BITS[16] = {0x00,0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,0x01,0x02,0x77};
static const uint8_t STD_CHROMA_AC_VALS[162] = {
    0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,
    0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,0x23,0x33,0x52,0xF0,
    0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,0x1A,0x26,
    0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,
    0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,
    0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,
    0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,
    0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,
    0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,
    0xF9,0xFA
};

static void build_canonical_table(const uint8_t bits[16], const uint8_t *huffval, int num_vals, HuffmanCode out_table[], int out_table_size)
{
    for (int i = 0; i < out_table_size; i++)
    {
        out_table[i].code = 0;
        out_table[i].length = 0;
    }
    unsigned int code = 0;
    int k = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < bits[i]; j++)
        {
            if (k >= num_vals) return;
            uint8_t symbol = huffval[k++];
            out_table[symbol].code = code;
            out_table[symbol].length = i + 1;
            code++;
        }
        code <<= 1;
    }
}

void init_jpeg_huffman_tables(void)
{
    build_canonical_table(STD_LUMA_DC_BITS,   STD_LUMA_DC_VALS,   12,  g_huff_dc_luma_table,   12);
    build_canonical_table(STD_CHROMA_DC_BITS, STD_CHROMA_DC_VALS, 12,  g_huff_dc_chroma_table, 12);
    build_canonical_table(STD_LUMA_AC_BITS,   STD_LUMA_AC_VALS,   162, g_huff_ac_luma_table,   256);
    build_canonical_table(STD_CHROMA_AC_BITS, STD_CHROMA_AC_VALS, 162, g_huff_ac_chroma_table, 256);
}
//==================================================================================
// HUFFMAN LOGIC - ENCODING
//==================================================================================

/**
 * @brief Initializes the bitstream writer structure.
 */
void bitstream_writer_init(BitstreamWriter *writer, FILE *f)
{
    writer->file = f;
    writer->buffer = 0;
    writer->bit_count = 0;
}

/**
 * @brief Writes a full byte to the file, applying byte stuffing when required.
 */
static void write_byte(BitstreamWriter *writer)
{
    fputc(writer->buffer, writer->file);
    // JPEG byte-stuffing rule: when the byte is 0xFF, insert an extra 0x00.
    if (writer->buffer == 0xFF)
    {
        fputc(0x00, writer->file);
    }
    writer->buffer = 0;
    writer->bit_count = 0;
}

/**
 * @brief Appends a sequence of bits (a code) to the buffer.
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
 * @brief Flushes any remaining bits in the buffer to the file (padding).
 */
void bitstream_writer_flush(BitstreamWriter *writer)
{
    if (writer->bit_count > 0)
    {
        // Complete the byte with ones, as required by the JPEG padding rule.
        writer->buffer <<= (8 - writer->bit_count);
        writer->buffer |= (0xFF >> writer->bit_count);
        write_byte(writer);
    }
}

/**
 * @brief Converts a coefficient value into its Category and Amplitude representation.
 * This is a crucial step defined by tables F.1 and F.2 of the JPEG specification.
 * @param value Coefficient value (e.g., -5, 27).
 * @param amplitude_bits Output pointer that receives the amplitude bits.
 * @return Coefficient category (bit-length of the amplitude).
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

    // Determine the category (floor(log2(abs(value))) + 1)
    while (temp > 0)
    {
        temp >>= 1;
        category++;
    }

    // Positive values use the straightforward binary representation.
    // Negative values are encoded in ones' complement form.
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
 * @brief Encodes one 8x8 block of quantized coefficients.
 * @param block 8x8 integer block (after DCT and quantization).
 * @param prev_dc Pointer to the previous block DC value (for differential coding).
 * @param huff_tables Huffman tables (DC and AC) to use.
 * @param writer Bitstream writer for the output file.
 */
void encode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int *prev_dc, HuffmanTable *huff_tables, BitstreamWriter *writer)
{
    int zigzag_coeffs[64];

    // 1. Apply the zig-zag scan
    for (int i = 0; i < 64; i++)
    {
        zigzag_coeffs[i] = block[g_zigzag_map[i] / BLOCK_SIZE][g_zigzag_map[i] % BLOCK_SIZE];
    }

    // 2. Encode the DC coefficient
    int dc_diff = zigzag_coeffs[0] - *prev_dc;
    *prev_dc = zigzag_coeffs[0];

    uint32_t dc_amplitude;
    int dc_category = get_category_and_amplitude(dc_diff, &dc_amplitude);

    write_bits(writer, huff_tables->dc_table[dc_category].code, huff_tables->dc_table[dc_category].length);
    if (dc_category > 0)
    {
        write_bits(writer, dc_amplitude, dc_category);
    }

    // 3. Encode the 63 AC coefficients
    int zero_run_count = 0;
    for (int i = 1; i < 64; i++)
    {
        if (zigzag_coeffs[i] == 0)
        {
            zero_run_count++;
        }
        else
        {
            // Emit ZRL if more than 15 zeros were accumulated
            while (zero_run_count >= 16)
            {
                write_bits(writer, huff_tables->ac_table[0xF0].code, huff_tables->ac_table[0xF0].length); // ZRL (15,0)
                zero_run_count -= 16;
            }

            uint32_t ac_amplitude;
            int ac_category = get_category_and_amplitude(zigzag_coeffs[i], &ac_amplitude);

            // Symbol combines zero run length (RUN) and coefficient category (SIZE)
            uint8_t run_size_symbol = (zero_run_count << 4) | ac_category;

            write_bits(writer, huff_tables->ac_table[run_size_symbol].code, huff_tables->ac_table[run_size_symbol].length);
            write_bits(writer, ac_amplitude, ac_category);

            zero_run_count = 0; // Reset zero counter
        }
    }

    // Emit EOB if remaining coefficients are zeros
    if (zero_run_count > 0)
    {
        write_bits(writer, huff_tables->ac_table[0x00].code, huff_tables->ac_table[0x00].length); // EOB (0,0)
    }
}

//==================================================================================
// HUFFMAN LOGIC - DECODING
//==================================================================================

/**
 * @brief Initializes the bitstream reader structure.
 */
void bitstream_reader_init(BitstreamReader *reader, FILE *f)
{
    reader->file = f;
    reader->buffer = 0;
    reader->bit_position = -1; // Force the first byte read
}

/**
 * @brief Reads a single bit from the file.
 */
static int read_bit(BitstreamReader *reader)
{
    if (reader->bit_position < 0)
    {
        int byte = fgetc(reader->file);
        if (byte == EOF)
            return -1;

        // Handle byte stuffing: if 0xFF is followed by 0x00, discard the 0x00
        if (byte == 0xFF)
        {
            int next_byte = fgetc(reader->file);
            if (next_byte == 0x00)
            {
                // Stuffed byte: the actual payload byte is 0xFF
            }
            else if (next_byte == EOF)
            {
                return -1;
            }
            else
            {
                // Encountered a JPEG marker; push byte back to the stream
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
 * @brief Searches the bitstream for a Huffman code present in the table.
 */
static int find_huffman_symbol(BitstreamReader *reader, HuffmanCode table[], int table_size)
{
    uint32_t current_code = 0;
    int length = 0;

    while (length < 16)
    { // JPEG baseline limits Huffman codes to 16 bits
        int bit = read_bit(reader);
        if (bit == -1)
            return -1; // Fim do arquivo

        current_code = (current_code << 1) | bit;
        length++;

        // Check whether the current code exists in the table
        for (int i = 0; i < table_size; i++)
        {
            if (table[i].length == length && table[i].code == current_code)
            {
                return i; // Return the symbol (table index), not the code itself
            }
        }
    }
    return -1; // Code not found
}

/**
 * @brief Reads the amplitude bits after a Huffman symbol has been decoded.
 */
static int read_amplitude(BitstreamReader *reader, int category)
{
    int amplitude = 0;
    for (int i = 0; i < category; i++)
    {
        int bit = read_bit(reader);
        if (bit == -1)
            return -1; // Error / EOF
        amplitude = (amplitude << 1) | bit;
    }
    return amplitude;
}

/**
 * @brief Rebuilds the original coefficient value from its category and amplitude.
 */
static int reconstruct_value(int category, int amplitude)
{
    if (category == 0)
        return 0;

    int half_range = 1 << (category - 1);

    if (amplitude >= half_range)
    {
        return amplitude; // Positive value
    }
    else
    {
        return amplitude - ((1 << category) - 1); // Negative value
    }
}

/**
 * @brief Decodes an 8x8 block of coefficients from the bitstream.
 */
void decode_block(int block[BLOCK_SIZE][BLOCK_SIZE], int *prev_dc, HuffmanTable *huff_tables, BitstreamReader *reader)
{
    int zigzag_coeffs[64] = {0}; // Initialize with zeros

    // 1. Decode the DC coefficient
    int dc_category = find_huffman_symbol(reader, huff_tables->dc_table, 12);
    if (dc_category >= 0 && dc_category < 12)
    {
        int dc_amplitude = read_amplitude(reader, dc_category);
        int dc_diff = reconstruct_value(dc_category, dc_amplitude);
        zigzag_coeffs[0] = dc_diff + *prev_dc;
        *prev_dc = zigzag_coeffs[0];
    }

    // 2. Decode the 63 AC coefficients
    for (int i = 1; i < 64;)
    {
        int ac_symbol = find_huffman_symbol(reader, huff_tables->ac_table, 256);
        if (ac_symbol == -1)
            break; // Error or end of stream

        if (ac_symbol == 0x00)
        {          // EOB
            break; // Remaining coefficients are zero
        }

        if (ac_symbol == 0xF0)
        {            // ZRL
            i += 16; // Skip 16 zeros
            continue;
        }

        int run = (ac_symbol >> 4) & 0x0F;
        int size = ac_symbol & 0x0F;

        i += run; // Skip `run` zeros

        if (i < 64 && size > 0)
        {
            int ac_amplitude = read_amplitude(reader, size);
            zigzag_coeffs[i] = reconstruct_value(size, ac_amplitude);
        }
        i++;
    }

    // 3. Apply the inverse zig-zag scan
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

Pixel **loadBMPImage(FILE *input, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader)
{
    Pixel **Image = (Pixel **)malloc(InfoHeader->Height * sizeof(Pixel *)); //
    for (int i = 0; i < InfoHeader->Height; i++)
    {
        Image[i] = (Pixel *)malloc(InfoHeader->Width * sizeof(Pixel));
    }

    // Seek to pixel data using header offset
    fseek(input, FileHeader->OffBits, SEEK_SET);

    int width = InfoHeader->Width;
    int height = InfoHeader->Height;
    int bits_per_pixel = InfoHeader->BitCount;
    int bytes_per_pixel = bits_per_pixel / 8;
    int row_stride_file = ((bits_per_pixel * width + 31) / 32) * 4;

    // Read bottom-up if height > 0 (standard BMP). Map to top-down in Image array.
    for (int y = 0; y < height; y++)
    {
        int dst_row = (height > 0) ? (height - 1 - y) : y;
        long row_start = (long)FileHeader->OffBits + (long)y * row_stride_file;
        fseek(input, row_start, SEEK_SET);

        for (int x = 0; x < width; x++)
        {
            unsigned char b = 0, g = 0, r = 0, a = 0;
            if (bytes_per_pixel == 3)
            {
                fread(&b, 1, 1, input);
                fread(&g, 1, 1, input);
                fread(&r, 1, 1, input);
            }
            else if (bytes_per_pixel == 4)
            {
                fread(&b, 1, 1, input);
                fread(&g, 1, 1, input);
                fread(&r, 1, 1, input);
                fread(&a, 1, 1, input); // ignore alpha
            }
            else if (bytes_per_pixel == 1)
            {
                // Grayscale indexed not supported; read as gray
                fread(&b, 1, 1, input);
                g = b; r = b;
            }
            else
            {
                // Unsupported bpp, attempt to skip
                fread(&b, 1, 1, input);
                fread(&g, 1, 1, input);
                fread(&r, 1, 1, input);
            }
            Image[dst_row][x].R = r;
            Image[dst_row][x].G = g;
            Image[dst_row][x].B = b;
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

    int width = InfoHeader->Width;
    int height = InfoHeader->Height;
    int bits_per_pixel = InfoHeader->BitCount;
    if (bits_per_pixel != 24 && bits_per_pixel != 32)
    {
        bits_per_pixel = 24; // fallback to 24bpp
        InfoHeader->BitCount = 24;
    }
    int bytes_per_pixel = bits_per_pixel / 8;
    int row_stride_file = ((bits_per_pixel * width + 31) / 32) * 4;

    // Update header sizes
    InfoHeader->SizeImage = row_stride_file * height;
    FileHeader->Size = FileHeader->OffBits + InfoHeader->SizeImage;

    // Write headers
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

    // Write pixels bottom-up in BGR(A) with proper stride/padding
    unsigned char pad[4] = {0, 0, 0, 0};
    for (int y = 0; y < height; y++)
    {
        int src_row = (height > 0) ? (height - 1 - y) : y;
        int written = 0;
        for (int x = 0; x < width; x++)
        {
            unsigned char r = Image[src_row][x].R;
            unsigned char g = Image[src_row][x].G;
            unsigned char b = Image[src_row][x].B;
            fwrite(&b, 1, 1, file_pointer);
            fwrite(&g, 1, 1, file_pointer);
            fwrite(&r, 1, 1, file_pointer);
            written += 3;
            if (bytes_per_pixel == 4)
            {
                unsigned char a = 0x00;
                fwrite(&a, 1, 1, file_pointer);
                written += 1;
            }
        }
        // pad to row_stride_file
        int pad_bytes = row_stride_file - written;
        if (pad_bytes > 0)
        {
            fwrite(pad, 1, pad_bytes, file_pointer);
        }
    }

    fclose(file_pointer);
}

unsigned char **allocate_y_u8(BITMAPINFOHEADER InfoHeader)
{
    unsigned char **Y = (unsigned char **)malloc(InfoHeader.Height * sizeof(unsigned char *));
    for (int i = 0; i < InfoHeader.Height; i++)
        Y[i] = (unsigned char *)malloc(InfoHeader.Width * sizeof(unsigned char));
    return Y;
}

double ***allocate_blocks_Y(int num_blocks)
{
    // 1. Allocate the list of pointers to 8x8 matrices (double**)
    double ***blocks = (double ***)malloc(num_blocks * sizeof(double **));
    if (blocks == NULL)
    {
        // Allocation failed
        return NULL;
    }

    // 2. For each block in the list, allocate an 8x8 matrix
    for (int k = 0; k < num_blocks; k++)
    {
        // 2a. Allocate the eight row pointers (double*) for matrix k
        blocks[k] = (double **)malloc(BLOCK_SIZE * sizeof(double *));
        if (blocks[k] == NULL)
        {
            // Allocation failed; previously allocated memory should be released
            // (cleanup code omitted here for brevity)
            return NULL;
        }

        // 2b. Allocate the eight elements (doubles) for each row
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            blocks[k][i] = (double *)malloc(BLOCK_SIZE * sizeof(double));
            if (blocks[k][i] == NULL)
            {
                // Allocation failed
                return NULL;
            }
        }
    }

    return blocks;
}

Chromancy ***allocate_blocks_chr(int num_blocks)
{
    // 1. Allocate the list of pointers to 8x8 matrices (Chromancy**)
    Chromancy ***blocks = (Chromancy ***)malloc(num_blocks * sizeof(Chromancy **));
    if (blocks == NULL)
    {
        // Allocation failed
        return NULL;
    }

    // 2. For each block in the list, allocate an 8x8 matrix
    for (int k = 0; k < num_blocks; k++)
    {
        // 2a. Allocate the eight row pointers (Chromancy*) for matrix k
        blocks[k] = (Chromancy **)malloc(BLOCK_SIZE * sizeof(Chromancy *));
        if (blocks[k] == NULL)
        {
            // Allocation failed; previously allocated memory should be released
            // (cleanup code omitted here for brevity)
            return NULL;
        }

        // 2b. Allocate the eight Chromancy elements for each row
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            blocks[k][i] = (Chromancy *)malloc(BLOCK_SIZE * sizeof(Chromancy));
            if (blocks[k][i] == NULL)
            {
                // Allocation failed
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
            // Free each row of the 8x8 matrix
                free(blocks[k][i]);
            }
            // Free the 8x8 matrix
            free(blocks[k]);
        }
    }
    // Free the list of matrices
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
                // Free each row of the 8x8 matrix
                free(blocks[k][i]);
            }
            // Free the 8x8 matrix
            free(blocks[k]);
        }
    }
    // Free the list of matrices
    free(blocks);
}

//==================================================================================
// DCT and IDCT / YCbCr and RGB / quantization and dequantization 
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
            // Convert to double before performing the calculations
            double y_val = Y[i][j];
            double cb_val = chromancy[i / 2][j / 2].Cb;
            double cr_val = chromancy[i / 2][j / 2].Cr;

            // Calculate temporary float values
            double r_temp = y_val + 1.402 * (cr_val - 128);
            double g_temp = y_val - 0.344136 * (cb_val - 128) - 0.714136 * (cr_val - 128);
            double b_temp = y_val + 1.772 * (cb_val - 128);

            // Clamp final values to the [0, 255] range using conditional checks
            Image_upscaled[i][j].R = (unsigned char)fmax(0.0, fmin(255.0, r_temp));
            Image_upscaled[i][j].G = (unsigned char)fmax(0.0, fmin(255.0, g_temp));
            Image_upscaled[i][j].B = (unsigned char)fmax(0.0, fmin(255.0, b_temp));
        }
    }
    return Image_upscaled;
}

Chromancy **compressCbCr(Pixel_YCbCr_d **imgYCbCr, BITMAPINFOHEADER *InfoHeader)
{

    int chr_height = InfoHeader->Height / 2;
    int chr_width = InfoHeader->Width / 2;

    Chromancy **chromancy = (Chromancy **)malloc(chr_height * sizeof(Chromancy *));
    for (int i = 0; i < chr_height; i++)
    {
        chromancy[i] = (Chromancy *)malloc(chr_width * sizeof(Chromancy));
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
        // Compute the starting row and column of block k within the full image
        int start_row = (k / num_blocks_per_row) * BLOCK_SIZE;
        int start_col = (k % num_blocks_per_row) * BLOCK_SIZE;

        // Copy the 8x8 block from the Y image into blocks[k]
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                // Source data comes from the computed position in the luminance plane
                blocks[k][i][j] = Y[start_row + i][start_col + j];
            }
        }
    }
}

void fillBlocks_chr(Chromancy ***blocks, int num_blocks, Chromancy **chromancy_plane, BITMAPINFOHEADER InfoHeader)
{
    int chr_width = InfoHeader.Width / 2;
    int chr_height = InfoHeader.Height / 2;
    int num_blocks_per_row = chr_width / BLOCK_SIZE;

    for (int k = 0; k < num_blocks; k++)
    {
        // Compute the starting row and column of block k within the subsampled chroma image
        int start_row = (k / num_blocks_per_row) * BLOCK_SIZE;
        int start_col = (k % num_blocks_per_row) * BLOCK_SIZE;

        // Copy the 8x8 block from the chroma plane into blocks[k]
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            int row = start_row + i;
            if (row >= chr_height)
                continue;

            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                int col = start_col + j;
                if (col >= chr_width)
                    continue;

                // Source data comes from the computed position in the chroma plane
                blocks[k][i][j].Cb = chromancy_plane[row][col].Cb;
                blocks[k][i][j].Cr = chromancy_plane[row][col].Cr;
            }
        }
    }
}

void mergeBlocks_Y(double ***blocks, int num_blocks, unsigned char **Y_out, BITMAPINFOHEADER InfoHeader)
{
    int num_blocks_per_row = InfoHeader.Width / BLOCK_SIZE;

    for (int k = 0; k < num_blocks; k++)
    {
        int start_row = (k / num_blocks_per_row) * BLOCK_SIZE;
        int start_col = (k % num_blocks_per_row) * BLOCK_SIZE;

        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                double v = blocks[k][i][j];
                if (v < 0) 
                    v = 0; 
                if (v > 255) 
                    v = 255;
                Y_out[start_row + i][start_col + j] = (unsigned char)(v + 0.5);
            }
        }
    }
}

void mergeBlocks_chr(Chromancy ***blocks, int num_blocks, Chromancy **chromancy_out, BITMAPINFOHEADER InfoHeader)
{
    int chr_width = InfoHeader.Width / 2;
    int chr_height = InfoHeader.Height / 2;
    int num_blocks_per_row = chr_width / BLOCK_SIZE;

    for (int k = 0; k < num_blocks; k++)
    {
        int start_row = (k / num_blocks_per_row) * BLOCK_SIZE;
        int start_col = (k % num_blocks_per_row) * BLOCK_SIZE;

        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            int row = start_row + i;
            if (row >= chr_height)
                continue;

            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                int col = start_col + j;
                if (col >= chr_width)
                    continue;

                double cb = blocks[k][i][j].Cb;
                double cr = blocks[k][i][j].Cr;

                if (cb < 0.0)
                    cb = 0.0;
                else if (cb > 255.0)
                    cb = 255.0;

                if (cr < 0.0)
                    cr = 0.0;
                else if (cr > 255.0)
                    cr = 255.0;

                chromancy_out[row][col].Cb = cb;
                chromancy_out[row][col].Cr = cr;
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
                blocks[k][i][j] = round(blocks[k][i][j] / div_mat[i][j]);
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
                blocks[k][i][j].Cb = round(blocks[k][i][j].Cb / div_mat[i][j]);
                blocks[k][i][j].Cr = round(blocks[k][i][j].Cr / div_mat[i][j]);
            }
        }
    }
}

void applyDequantization_Y(double ***blocks, int num_blocks, const int mul_mat[8][8])
{
    for (int k = 0; k < num_blocks; k++)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks[k][i][j] = blocks[k][i][j] * mul_mat[i][j];
            }
        }
    }
}

void applyDequantization_chr(Chromancy ***blocks, int num_blocks, const int mul_mat[8][8])
{
    for (int k = 0; k < num_blocks; k++)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                blocks[k][i][j].Cb = blocks[k][i][j].Cb * mul_mat[i][j];
                blocks[k][i][j].Cr = blocks[k][i][j].Cr * mul_mat[i][j];
            }
        }
    }
}
