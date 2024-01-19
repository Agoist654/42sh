#!/bin/sh
a=0;b=0;
while [ $a -lt 5 ];
do while [ $b -lt $a ];
do b=$((b+1));echo $b;done;
a=$((a+1));
done
