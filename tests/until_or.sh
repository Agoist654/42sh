#! /bin/sh
a=0;b=1;
until [ $a -gt 4 ] || [ $b -gt 4 ];
do echo $a;a=$((a+2));echo $b;b=$((b+2));
done
