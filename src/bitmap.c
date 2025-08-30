#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


void loadBMPHeaders (FILE *fp, BITMAPFILEHEADER *FileHeader, BITMAPINFOHEADER *InfoHeader)
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


void leituraHeader(FILE *F, BITMAPFILEHEADER *H){
     fread(&H->Type,sizeof (unsigned short int),1,F);
     fread(&H->Size,sizeof (unsigned int),1,F);
     fread(&H->Reserved1,sizeof (unsigned short int),1,F);
     fread(&H->Reserved2,sizeof (unsigned short int),1,F);
     fread(&H->OffBits,sizeof (unsigned int),1,F);
}

void leituraInfoHeader(FILE *F, BITMAPINFOHEADER *INFO_H){
     fread(&INFO_H->Size,sizeof (unsigned int),1,F);
     fread(&INFO_H->Width,sizeof (int),1,F);
     fread(&INFO_H->Height,sizeof (int),1,F);
     fread(&INFO_H->Planes,sizeof (unsigned short int),1,F);
     fread(&INFO_H->BitCount,sizeof (unsigned short int),1,F);
     fread(&INFO_H->Compression,sizeof (unsigned int),1,F);
     fread(&INFO_H->SizeImage,sizeof (unsigned int),1,F);
     fread(&INFO_H->XResolution,sizeof (int),1,F);
     fread(&INFO_H->YResolution,sizeof (int),1,F);
     fread(&INFO_H->NColours,sizeof (unsigned int),1,F);
     fread(&INFO_H->ImportantColours,sizeof (unsigned int),1,F);        
}

void printHeaders (BITMAPFILEHEADER *FileHeader,  BITMAPINFOHEADER *InfoHeader)
{
     /*system("cls");*/
     printf("*************** File Header ***************\n\n");
     
     printf("Magic number for file: %x\n", FileHeader->Type);   
     printf("File's size: %d\n",FileHeader->Size);           
     printf("Offset to bitmap data: %d\n", FileHeader->OffBits);
     
     printf("\n\n");
     printf("*************** Info Header ***************\n\n");
     printf("Info header's size: %d\n", InfoHeader->Size);
     printf("Width: %d\n", InfoHeader->Width);          
     printf("Height: %d\n",InfoHeader->Height);
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
