#!/bin/sh
a=0;
until [ $a -gt 5 ];
do if [ $a -lt 3 ];then echo $a;else echo value is too big;fi;
a=$((a+1));
done
