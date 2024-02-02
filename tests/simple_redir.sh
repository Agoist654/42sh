echo tot > tot.txt
echo tot >> tot.txt
cat < tot.txt
cat 0< tot.txt
rm tot.txt
echo tot >> tot.txt
echo tot 2>> tot.txt
echo tot 1> tot.txt

rm tot.txt

echo tot > tot.txt
echo tot >&1 tot.txt
echo tot >&2 tot.txt
echo tot 1>&2 tot.txt
echo tot 1>| tot.txt
echo tot >| tot.txt

cat <&1 tot.txt
cat 0<&1 tot.txt

