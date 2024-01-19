#!/bin/sh
a=0
if true;then while [ $a -lt 5 ];
do echo $a;a=$((a+1));
done;fi
