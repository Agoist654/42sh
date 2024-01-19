#!/bin/sh
a=0;
until [ $a -gt 5 ];
do echo $a;a=$((a+1));
done
