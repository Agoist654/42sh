#! /bin/sh
a=0;b=1;
while [ $a -lt 4 ] && [ $b -lt 4 ];
do echo $a;a=$((a+2));echo $b;b=$((b+2));
done
