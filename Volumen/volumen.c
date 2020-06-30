/*
Programa: Escriba un programa que lea un archivo wav y genere otro archivo
wav cuyo volumen sea la mitad del primero. El programa recibe los nombres de
los archivos de entrada y salida de su linea de comando

Prueba: Generar un archivo .wav con Goldwave de 0.016s a 2000 muestras
por segundo y f(x) = ((N-n)/N-0.5)*2

*/

#include <stdio.h>
#include <stdlib.h>

typedef struct chunkInfo{
  unsigned char chunkId[4];
  int *chunkSize;
  short *format, *chanels;
  int *sampler, *byter;
  short *blockal, *bitpers;
}chunkInfo;

typedef struct chunkData{
   char chunkId[4];
  int *chunkSize;
  unsigned char *data;
}chunkData;

int main(int argc, char *argv[]) {
  FILE *entrada, *salida;
  char *stream;
  long file_len;
  //Variables for reading the file
  char RIFF[4], WAVE[4];
  char comp_wave[4];
  int chunk;
  chunkInfo info;
  chunkData data;
  char *footer;
  //Auxiliars
  char buffer, buffer2[2], buffer4[4];
  if(argc < 3)
    printf("No se ha indicado alguno de los archivos");
  else{
    entrada = fopen(argv[1], "rb");
    salida = fopen(argv[2], "wb");
    if(entrada != NULL){
      //Reading chunk info
      fread(RIFF, sizeof(char), 4, entrada);
      fread(buffer4, sizeof( char), 4, entrada);
      fread(WAVE, sizeof( char), 4, entrada);
      chunk = buffer4[0] | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
      //Reading subchunk1 info
      fread(info.chunkId, sizeof(char), 4, entrada);
      info.chunkSize= (int*)malloc(sizeof(int));
      fread(info.chunkSize, sizeof(int), 1, entrada);
      info.format = (short*)malloc(sizeof(short));
      fread(info.format, sizeof(short), 1, entrada);
      info.chanels = (short*)malloc(sizeof(short));
      fread(info.chanels, sizeof(short), 1, entrada);
      info.sampler = (int*)malloc(sizeof(int));
      fread(info.sampler, sizeof(int), 1, entrada);
      info.byter = (int*)malloc(sizeof(int));
      fread(info.byter, sizeof(int), 1, entrada);
      info.blockal = (short*)malloc(sizeof(short));
      fread(info.blockal, sizeof(short), 1, entrada);
      info.bitpers = (short*)malloc(sizeof(short));
      fread(info.bitpers, sizeof(short), 1, entrada);
      short bitPer = *info.bitpers&0XFF | (*info.bitpers>>8)&0xFF;
      int sampRate = *info.sampler&0xFF | (*info.sampler>>8)&0xFF | (*info.sampler>>16)&0xFF | (*info.sampler>>24)&0xFF;
      short blockAlign = *info.blockal&0XFF | (*info.blockal>>8)&0xFF;
      //Reading subchunk2 info
      fread(data.chunkId, sizeof( char), 4, entrada);
      data.chunkSize = (int*)malloc(sizeof(int));
      fread(data.chunkSize, sizeof(int), 1, entrada);
      int dataChunk = *data.chunkSize&0xFF | (*data.chunkSize>>8)&0xFF | (*data.chunkSize>>16)&0xFF | (*data.chunkSize>>24)&0xFF;
      data.data = (char*)malloc(sizeof(char)*dataChunk);
      fread(data.data, sizeof(char), dataChunk, entrada);

      if(bitPer == 8){
        for(int i=0 ; i<dataChunk; i++){
          char aux = data.data[i]-128;
          aux = aux>>1;
          data.data[i] = aux+128);
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

      //Creating the reduced file
      //Writing chunk
      char buff4[4], buff2[2], aux[4];
      fwrite(RIFF, sizeof(char), 4, salida);
      buff4[0]=chunk&0xFF;buff4[1]=(chunk>>8)&0xFF;buff4[2]=(chunk>>16)&0xFF;buff4[3]=(chunk>>24)&0xFF;
      fwrite(buff4, sizeof(int), 1, salida);
      fwrite(WAVE, sizeof(char), 4, salida);
      //Writing subchunk 1
      fwrite(info.chunkId, sizeof(char), 4, salida);
      fwrite(info.chunkSize, sizeof(int), 1, salida);
      fwrite(info.format, sizeof(short), 1, salida);
      fwrite(info.chanels, sizeof(short), 1, salida);
      fwrite(info.sampler, sizeof(int), 1, salida);
      fwrite(info.byter, sizeof(int), 1, salida);
      fwrite(info.blockal, sizeof(short), 1, salida);
      fwrite(info.bitpers, sizeof(short), 1, salida);
      //Writing subchunk 2
      fwrite(data.chunkId, sizeof(int), 1, salida);
      fwrite(data.chunkSize, sizeof(int), 1, salida);
      fwrite(data.data, sizeof(char), dataChunk, salida);
      //Writing the footer
      while(!feof(entrada))
        fputc(fgetc(entrada), salida);
      //fwrite(footer, sizeof(footer_len), 1, salida);
      fclose(entrada);
      fclose(salida);
    }
    else{
      printf("Error al abrir archivo");
    }
  }
}
