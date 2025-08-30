/*
Programa que exemplifica como realizar a leitura de uma imagem RGB padrão, sem
compressão, com 8 bits por canal (24 bits de profundidade de pixel).

O cabeçalho de um BMP padrão possui 54 bytes, e logo após começam os dados,
armazenados na ordem B, G, R; sendo um byte (unsigned char) para cada valor.

O progrma cria uma matriz para armazenar os canais RGB lidos do arquivo BMP,
onde cada célula da matriz representa um pixel com seus valores RGB associados.

Com os canais RGB separados, é possível aplicar diversos processamentos à
imagem. Neste simples exemplo troca-se os canais de cor R e B, "invertendo"
essas cores em relação a uma imagem de entrada. A imagem de saída terá o header
da original copiado (para "gerar" um BMP) e os cores vermelho e azul trocadas.

Autor: Rudinei Goularte (2021).
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bitmap.h"

typedef struct
{
        unsigned char R;
        unsigned char G;
        unsigned char B;      
} Pixel;

void loadBMPImage(FILE *input, BITMAPINFOHEADER InfoHeader, Pixel *Image);
       
int main(int argc, char *argv[])
{
    FILE *input, *output;
    int i;
    unsigned char *v;
    
    FILE *fp;
    BITMAPFILEHEADER FileHeader;       /* File header */
    BITMAPINFOHEADER InfoHeader;       /* Info header */
    Pixel *Image;
           
    if(!(input = fopen("colors.bmp", "rb"))){
            printf("Error: could not open input file." );
            exit(1);
    }
    
    loadBMPHeaders (input, &FileHeader, &InfoHeader);
    
    /*Image = Bmp pixels quantity*/
    Image = (Pixel *) malloc((InfoHeader.Width * InfoHeader.Height) * sizeof(Pixel));
    
    loadBMPImage(input, InfoHeader, Image);
        
  if(!(output = fopen("out.bmp", "wb"))){
            printf("Error: could not open output file." );
            exit(1);
    }


/*====================================Brincadeira com RGB========================*/
   unsigned char aux_;
   for (i=0;i< (InfoHeader.Width * InfoHeader.Height); i++){
       aux_ = Image[i].R;
       Image[i].R = Image[i].B;
       Image[i].B = aux_;
       }
                    
   fseek(input, 0, SEEK_SET);
        
    for(i=0; i<54; i++)
             fputc(fgetc(input), output);

       
    for (i=0; i < (InfoHeader.Height * InfoHeader.Width); i++)
    {
       fputc(Image[i].B, output);
       fputc(Image[i].G, output);
       fputc(Image[i].R, output);
       
    }
   fclose(input);
   fclose (output);                               
   exit(0);

   return (0);
}



void loadBMPImage(FILE *input, BITMAPINFOHEADER InfoHeader, Pixel *Image){
   int i, j, tam;
   
   tam = InfoHeader.Height * InfoHeader.Width;
   fseek(input, 54, SEEK_SET); //skipping the header (54 bytes)
   
   for (i=0; i < tam; i++){
      
       Image[i].B = fgetc(input);
       Image[i].G = fgetc(input);
       Image[i].R = fgetc(input);
   }

}


