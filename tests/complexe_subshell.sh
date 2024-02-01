key1=1
key2=2

key3=3
key4=4

myf1()
{
    if [ $1 -eq $key1 ]; then
        echo good 1 key1 : $key1
    elif [ $1 -eq $key2 ]; then
        echo  good 1 key2 : $key2
    elif [ $1 -eq $key3 ]; then
        echo  good 1 key3 : $key3
    elif [ $1 -eq $key1 ]; then
        echo  good 1 key4 : $key4
    elif [ $1 -eq $key4 ]; then
        echo  good 1 key4 : $key4
    else
        echo else
    fi
    for var in $key2 3 4;do
            echo \g\o\od \\\\2 : $var
    done
    if 'true'; then
        echo t'ru'\e
    fi
}


for k in 1 2 3;do
    myf1 $key1 $key2 $key3
done
myf1 $key1
myf1 $key2 $key3

echo my sub shell \

echo \\'single'\\
echo \\"doulbe"\\


for i in 1 2 $key3; do
    (i=7;c=8;echo $i;
i=$c;echo $c $i)
echo $i
done

a=z
(
echo first subsheel start====;
a=b;
echo $a;echo $a

(a=c;echo $a
echo secondsubshell start========

echo secondsubshell end========
)
echo $a
echo first subsheel end====;
)
echo $a

