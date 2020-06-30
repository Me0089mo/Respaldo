/*programa filtroRCN: recibe como argumento el numero de muestras a procesar. Las muestras se introducen byte a byte por la entrada estándar y la salida se obtiene byta a byte por la salida estándar*/

#include<math.h>
#include<stdio.h>
#include<stdlib.h>

void main(int argc, char *argv[])
{
#define PI 3.14159273
long n=0;
double fm=44100;
double f=1000;
long N;
char *fin;
if (argc>1)
N=strtol(argv[1],&fin,10);
double a[3]={0.875411,-1.74213, 0.875411};
double b[3]={1, -1.74213, 0.750821};
double x[3]={0,0,0};
double y[3]={0,0,0};
do{
x[1]=x[0];
y[1]=y[0];
x[0]=getchar()/256.0;
y[0]=(a[0]*x[0]+a[1]*x[1]-b[1]*y[1])/b[0];
putchar(y[0]*256);
n++;
}while(n<N);
}
