#!/bin/sh
a=0;b=0;
until [ $a -gt 5 ];
do until [ $b -gt $a ];
do b=$((b+1));echo $b;done;
a=$((a+1));
done
