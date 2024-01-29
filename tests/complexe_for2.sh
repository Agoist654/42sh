key=value
a=1
b=2
c=3
d=4
ee=6
for var in $a $b $c $d;  do

    echo $key $var;
    for var2 in 1 2 3 4 5; do
        echo $var2;
    done

done
