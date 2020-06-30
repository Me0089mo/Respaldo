#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define pi 3.14159

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

void main(int argc, char *argv[]){
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
      fread(buffer4, sizeof(char), 4, entrada);
      fread(WAVE, sizeof(char), 4, entrada);
      //Reading subchunk1
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
      //Reading subchunk2
      fread(data.chunkId, sizeof( char), 4, entrada);
      data.chunkSize = (int*)malloc(sizeof(int));
      fread(data.chunkSize, sizeof(int), 1, entrada);
      int dataChunk = *data.chunkSize;
      data.data = (char*)malloc(sizeof(char)*dataChunk);
      fread(data.data, sizeof(char), dataChunk, entrada);

      short bitPer = *info.bitpers;
      unsigned char transformData[dataChunk*2];
      //Discrete Fourier Transform
      if(bitPer == 8){
        double X_real[dataChunk];
        double X_imag[dataChunk];
        int cont_data = 0;
        for(int i=0; i<dataChunk; i++){
          X_real[i] = 0;
          X_imag[i] = 0;
          for(int j=0; j<dataChunk; j++){
            double angle = (2*pi*i*j)/dataChunk;
            X_real[i] += (data.data[j]-128)*cos(angle);
            X_imag[i] += (data.data[j]-128)*(0-sin(angle));
          }
        }
        for(int i=0, j=0; i<dataChunk*2; i++){
          if(i<dataChunk)
            transformData[i] = (char)(X_real[i]+128/dataChunk);
          else{
            transformData[i] = (char)(X_imag[j]+128/dataChunk);
            j++;
          }
        }
      }
      else{

      }

      //Writing output file
      short numChannels = 2;
      int sampRate = *info.sampler;
      short blockAlign = numChannels*(bitPer/8);
      int byter = numChannels*sampRate*(bitPer/8);

      int subSize1 = (int)(*info.chunkSize);
      int subSize2 = dataChunk*numChannels;
      int chunkSize = subSize1+subSize2+20;
      //Writing info chunk
      fwrite(RIFF, sizeof(char), 4, salida);
      buffer4[0]=chunkSize&0xFF;buffer4[1]=(chunkSize>>8)&0xFF;buffer4[2]=(chunkSize>>16)&0xFF;buffer4[3]=(chunkSize>>24)&0xFF;
      //int *chkSize = chunkSize;
      fwrite(chkSize, sizeof(int), 1, salida);
      fwrite(WAVE, sizeof(char), 4, salida);

      //Writing subchunk 1
      //Number of channels
      info.chanels[0] = numChannels&0xFF;
      info.chanels[1] = (numChannels>>8)&0xFF;
      //Byte rate
      info.byter[0] = byter&0xFF;
      info.byter[1] = (byter>>8)&0xFF;
      info.byter[2] = (byter>>16)&0xFF;
      info.byter[3] = (byter>>24)&0xFF;
      //Block align
      info.blockal[0] = blockAlign&0xFF;
      info.blockal[1] = (blockAlign>>8)&0xFF;
      fwrite(info.chunkId, sizeof(char), 4, salida);
      fwrite(info.chunkSize, sizeof(int), 1, salida);
      fwrite(info.format, sizeof(short), 1, salida);
      fwrite(info.chanels, sizeof(short), 1, salida);
      fwrite(info.sampler, sizeof(int), 1, salida);
      fwrite(info.byter, sizeof(int), 1, salida);
      fwrite(info.blockal, sizeof(short), 1, salida);
      fwrite(info.bitpers, sizeof(short), 1, salida);

      //Writing subchunk 2
      //Chunk size
      /*data.chunkSize[0] = subSize2&0xFF;
      data.chunkSize[1] = (subSize2>>8)&0xFF;
      data.chunkSize[2] = (subSize2>>16)&0xFF;
      data.chunkSize[3] = (subSize2>>24)&0xFF;*/
      *data.chunkSize = subSize2;
      printf("%d", *data.chunkSize);
      fwrite(data.chunkId, sizeof(int), 1, salida);
      fwrite(data.chunkSize, sizeof(int), 1, salida);
      for(int i=0; i<dataChunk*2; i++)
        fputc(transformData[i], salida);
      //fwrite(transformData, sizeof(char), dataChunk*2, salida);

      //Writing the footer
      while(!feof(entrada))
        fputc(fgetc(entrada), salida);
      //fwrite(footer, sizeof(footer_len), 1, salida);
      fclose(entrada);
      fclose(salida);
    }
    else
      printf("Error al abrir archivo");
  }
}
