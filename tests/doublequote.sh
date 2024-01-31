key=value
if false; then
    echo "if $key";
elif false; then
    echo "elif $key";
elif true; then
    echo "elif2   $key  ";
    echo elif2   $key  ;
    echo 'elif2   \$key  ';
    echo elif2   $key  ;
    echo elif2   \$key  ;
fi


