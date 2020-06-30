#!/bin/sh
#usage: wav2png.sh file.wav
#para sintetizar con sox: sox -t null /dev/null sine_500hz_3s.wav synth 0.01 sine 500.


BASE=${1%.wav}
PNG=$BASE.png
WAV=$BASE.wav
DAT=$BASE.dat

#echo $BASE,$PNG,$WAV,$DAT
sox $WAV $DAT
grep -v '^;' $DAT >$DAT.clean
FREQ=`head -1 $DAT|tr -d ';'`

#echo -e "set terminal png;set title '$FREQ';set output '$PNG'; plot '$DAT.clean'" |gnuplot
echo "set terminal png;set title '$FREQ';set output '$PNG'; plot '$DAT.clean' with lines" |gnuplot
