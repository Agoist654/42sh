#!/bin/sh
a=0
while [ $a -lt 5 ];
do if true;then echo $a;else echo false;fi;a=$((a+1));
done
