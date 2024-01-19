#! /bin/sh
a=0;b=1;
while [ $a -lt 3 ] || [ $b -lt 3 ];
do echo $a;a=$((a+2));echo $b;b=$((b+2));
done
