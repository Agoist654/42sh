key=value
if false; then
    echo "if $key";
elif false; then
    echo "elif $key";
else
    echo "elif2 $key";
    echo elif2   $key  ;
    echo 'elif2   \$key  ';
    echo elif2   $key  ;
    echo elif2   \$key  ;
fi
