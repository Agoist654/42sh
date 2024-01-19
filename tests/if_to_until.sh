#!/bin/sh
a=0;
if [ $a -lt 5 ];then until [ $a -gt 5 ];do echo $a;a=$((a+1));done;
else echo $a is bigger than 5;fi
