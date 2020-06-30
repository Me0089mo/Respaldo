#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159273

int main(int argc, char *argv[]){
  FILE *entrada, *salida;
  short *bitpers;
  int *chunkSize;
  unsigned char *data;
  char *dataS;

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
      fwrite(bitpers, sizeof(short), 1, salida);
      for(int i=0; i<4; i++)
        fputc(fgetc(entrada), salida);
      chunkSize = (int*)malloc(sizeof(int));
      fread(chunkSize, sizeof(int), 1, entrada);
      int dataChunk = *chunkSize;
      fwrite(chunkSize, sizeof(int), 1, salida);
      if(bitPer == 8){
        data = (unsigned char*)malloc(sizeof(char)*dataChunk);
        fread(data, sizeof(char), dataChunk, entrada);
      }
      else{
        dataS = (char*)malloc(sizeof(char)*dataChunk);
        fread(dataS, sizeof(char), dataChunk, entrada);
      }

      double *newData;
      double *muestras;
      double h[100];
      double conv, maximum = 0, absolute = 0;
      const double t = 1/44100.0;
      const double RC = 1/(4000.0*PI);
      for(int i=0; i<100; i++)
        h[i] = exp(-(t*i)/RC);

      if(bitPer == 8){
        newData = (double*)malloc(sizeof(double)*dataChunk);
        muestras = (double*)malloc(sizeof(double)*dataChunk);
        for(int n=0; n<dataChunk; n++){
          conv = 0;
          muestras[n] = (data[n]-128)/128.0;
          for(int k=0; k<100; k++){
            if(k<=n)
              conv += muestras[n-k]*h[k];
          }
          absolute = fabs(conv);
          if(absolute > maximum)
            maximum = absolute;
          newData[n] = conv;
        }
      }

      else{
        newData = (double*)malloc(sizeof(double)*(dataChunk/2));
        muestras = (double*)malloc(sizeof(double)*(dataChunk/2));
        for(int n=0, b=0; n<dataChunk/2; n++, b=b+2){
          conv = 0;
          muestras[n] = ((unsigned char)dataS[b] | dataS[b+1]<<8) / 32767.0;
          for(int k=0; k<100; k++){
            if(k<=n)
              conv += muestras[n-k]*h[k];
          }
          absolute = fabs(conv);
          if(absolute > maximum)
            maximum = absolute;
          newData[n] = conv;
        }
      }

      if(bitPer == 8){
        for(int i=0; i<dataChunk; i++)
          fputc((unsigned char)(newData[i]*128/maximum)+128, salida);
      }
      else{
        for(int i=0; i<dataChunk/2; i++){
          unsigned int newD = newData[i]*32767.0/maximum;
          unsigned char less = (unsigned char)newD;
          char more = (char)(newD>>8);
          fputc(less, salida);
          fputc(more, salida);
        }
      }
      while(!feof(entrada))
        fputc(fgetc(entrada), salida);
      fclose(entrada);
      fclose(salida);
    }
  }
  return 0;
}
