/*
Programa exemplo para gravar bits de um código binário em arquivo.

A representação de um bitstream binário ( sequência de zeros e uns) é feita em 
memória normalmente usando inteiros (int ou char/unsigned char), que possuem 8 
ou até 32 bits por símbolo. Seria um desperdício de espaço gravar o bitstream 
em disco como inteiros em vez de gravar como bits.

Este exemplo mostra como um bitstream pode ser gravado em disco como bits.

Autor: Rudinei Goularte (2019)
*/
#include <stdio.h>
int main(void){
    char bitstream[9] = {'0','1','1','0','0','1','1','1','\0'}; /* 103 em binário*/
    unsigned char buffer = 0; /* ocupa um byte de espaço de memória*/
    int i;
    
    FILE *out, *out2; /* Arquivos para gravar o bitstream*/
    
    for (i=0;i<8;i++) /* transfere o conteúdo do bitstream para um byte*/
       buffer = (buffer<<1) | (bitstream[i]=='1');
       
    printf("\n%d", buffer);
    
   
    out2 = fopen("out.txt","w"); /*grava o bitstream como caracter -> 64 bits*/
    for(i=0;i<8;i++)
       fputc(bitstream[i], out2);
    fclose(out2);

    out = fopen("out.bin","wb"); /* grava o bitstream como bits -> 8 bits*/
    fwrite(&buffer,sizeof(buffer), 1, out);
    fclose(out);
    
    out = fopen("out.bin","rb");
    fread(&buffer,sizeof(unsigned char),1,out);
    printf("\n -->%d", buffer);
    fclose(out);
        
    printf("/n");
}
