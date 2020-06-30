#include <stdio.h>
#include <stdlib.h>

typedef struct chunkData{
  char chunkId[4];
  int *chunkSize;
  unsigned char *data;
}chunkData;

int main(int argc, char *argv[]) {
  FILE *entrada, *salida;
  short *bitpers;
  chunkData data;
  if(argc < 3)
    printf("No se ha indicado alguno de los archivos");
  else{
    entrada = fopen(argv[1], "rb");
    salida = fopen(argv[2], "wb");
    if(entrada != NULL){
      for(int i=0; i<34; i++)
        fputc(fgetc(entrada), salida);
      bitpers = (short*)malloc(sizeof(short));
      fread(bitpers, sizeof(short), 1, entrada);
      short bitPer = *bitpers;
      fread(data.chunkId, sizeof(int), 1, entrada);
      data.chunkSize = (int*)malloc(sizeof(int));
      fread(data.chunkSize, sizeof(int), 1, entrada);
      int dataChunk = *data.chunkSize;
      data.data = (unsigned char*)malloc(sizeof(char)*dataChunk);
      fread(data.data, sizeof(char), dataChunk, entrada);

      if(bitPer == 8){
        for(int i=0 ; i<dataChunk; i++){
          char aux = data.data[i]-128;
          aux = aux>>1;
          data.data[i] = aux+128;
        }
      }
      else{
        char acarreo;
        char negativo;
        for(int i=0 ; i<dataChunk; i+=2){
          acarreo = data.data[i+1]&0x01;
          acarreo = acarreo<<7;
          negativo = data.data[i+1]&0x80;
          data.data[i+1] = data.data[i+1]>>1;
          data.data[i+1] = data.data[i+1]|negativo;
          data.data[i] = data.data[i]>>1;
          data.data[i] = data.data[i]|acarreo;
        }
      }

      fwrite(bitpers, sizeof(short), 1, salida);
      fwrite(data.chunkId, sizeof(int), 1, salida);
      fwrite(data.chunkSize, sizeof(int), 1, salida);
      fwrite(data.data, sizeof(char), dataChunk, salida);
      while(!feof(entrada))
        fputc(fgetc(entrada), salida);
      fclose(entrada);
      fclose(salida);
    }
    else{
      printf("Error al abrir archivo");
    }
  }
}
