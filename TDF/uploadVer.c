#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define pi 3.14159265

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
  char *dataS;
}chunkData;

int main(int argc, char *argv[]){
  FILE *entrada, *salida;
  char WAVE[4];
  chunkInfo info;
  chunkData data;

  if(argc < 3)
    printf("No se ha indicado alguno de los archivos");
  else{
    entrada = fopen(argv[1], "rb");
    salida = fopen(argv[2], "wb");
    if(entrada != NULL){
      for(int i=0; i<4; i++)
        fputc(fgetc(entrada), salida);
      for(int i=0; i<4; i++)
        fgetc(entrada);
      fread(WAVE, sizeof(char), 4, entrada);

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
      short bitPer = *info.bitpers;

      fread(data.chunkId, sizeof( char), 4, entrada);
      data.chunkSize = (int*)malloc(sizeof(int));
      fread(data.chunkSize, sizeof(int), 1, entrada);
      int dataChunk = *data.chunkSize;
      if(bitPer == 8){
        data.data = (unsigned char*)malloc(sizeof(char)*dataChunk);
        fread(data.data, sizeof(char), dataChunk, entrada);
      }
      else{
        data.dataS = (char*)malloc(sizeof(char)*dataChunk);
        fread(data.dataS, sizeof(char), dataChunk, entrada);
      }

      int dChunk = dataChunk/2;
      double *X_real;
      double *X_imag;
      if(bitPer == 8){
        X_real = (double*)malloc(sizeof(double)*dataChunk);
        X_imag = (double*)malloc(sizeof(double)*dataChunk);
        double muestras[dataChunk];
        for(int i=0; i<dataChunk; i++)
          muestras[i] = (data.data[i]-128)/128.0;
        for(int i=0; i<dataChunk; i++){
          X_real[i] = 0;
          X_imag[i] = 0;
          for(int j=0; j<dataChunk; j++){
            double angle = (2*pi*i*j)/dataChunk;
            X_real[i] += muestras[j]*cos(angle);
            X_imag[i] += muestras[j]*(-sin(angle));
          }
        }
      }
      else{
        X_real = (double*)malloc(sizeof(double)*dChunk);
        X_imag = (double*)malloc(sizeof(double)*dChunk);
        double muestras[dChunk];
        for(int n=0, b=0; n<dChunk; n++, b+=2)
          muestras[n] = ((unsigned char)data.dataS[b] | data.dataS[b+1]<<8)/32767.0;
        for(int i=0; i<dChunk; i++){
          X_real[i] = 0;
          X_imag[i] = 0;
          for(int j=0; j<dChunk; j++){
            double angle = (2*pi*i*j)/dChunk;
            X_real[i] += muestras[j]*cos(angle);
            X_imag[i] += muestras[j]*(-sin(angle));
          }
        }
      }

      short numChannels = 2;
      int sampRate = *info.sampler;
      short blockAlign = numChannels*(bitPer/8);
      int byter = numChannels*sampRate*(bitPer/8);

      int subSize1 = (int)(*info.chunkSize);
      int subSize2 = dataChunk*numChannels;
      int chunkSize = subSize1+subSize2+20;

      info.chanels = &numChannels;
      info.byter = &byter;
      info.blockal = &blockAlign;

      int *chkS = &chunkSize;
      fwrite(chkS, sizeof(int), 1, salida);
      fwrite(WAVE, sizeof(char), 4, salida);
      fwrite(info.chunkId, sizeof(char), 4, salida);
      fwrite(info.chunkSize, sizeof(int), 1, salida);
      fwrite(info.format, sizeof(short), 1, salida);
      fwrite(info.chanels, sizeof(short), 1, salida);
      fwrite(info.sampler, sizeof(int), 1, salida);
      fwrite(info.byter, sizeof(int), 1, salida);
      fwrite(info.blockal, sizeof(short), 1, salida);
      fwrite(info.bitpers, sizeof(short), 1, salida);

      *data.chunkSize = subSize2;
      fwrite(data.chunkId, sizeof(int), 1, salida);
      fwrite(data.chunkSize, sizeof(int), 1, salida);
      if(bitPer == 8){
        for(int i=0; i<dataChunk; i++){
          fputc((unsigned char)(X_real[i]*128/dataChunk)+128, salida);
          fputc((unsigned char)(X_imag[i]*128/dataChunk)+128, salida);
        }
      }
      else{
        for(int i=0; i<dataChunk/2; i++){
          unsigned int aux = (unsigned int)(X_real[i]*32767.0/dChunk);
          unsigned char less = (unsigned char)aux;
          char more = (char)(aux>>8);
          fputc(less, salida);
          fputc(more, salida);
          aux = (unsigned int)(X_imag[i]*32767.0/dChunk);
          less = (unsigned char)aux;
          more = (char)(aux>>8);
          fputc(less, salida);
          fputc(more, salida);
        }
      }

      while(!feof(entrada))
        fputc(fgetc(entrada), salida);
      fclose(entrada);
      fclose(salida);
    }
    else
      printf("Error al abrir archivo");
  }
  return 0;
}
