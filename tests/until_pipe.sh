#! /bin/sh
a=0;
true | until [ $a -gt 3 ];
do echo $a;a=$((a+1));
done
