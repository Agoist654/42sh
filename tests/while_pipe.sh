#! /bin/sh
a=0;
true | while [ $a -lt 3 ];
do echo $a;a=$((a+1));
done
