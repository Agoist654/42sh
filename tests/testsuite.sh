#! /bin/sh

REF_OUT="../tests/ref.out"
REF_ERR="../tests/ref_err.out"

TES_OUT="../tests/test.out"
TES_ERR="../tests/test_err.out"

TES_RES="../tests/test_res.out"
REF_RES="../tests/ref_res.out"

#DIFF="tests/diff.out"

print(){

    echo "================================================\n"
#    echo "input: >./42sh "
#    for ARG in $*; do
#        echo $ARG
#    done
#    echo "<"


    touch "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"

    bash --posix $@ 2> "$REF_ERR" >"$REF_OUT"
    echo $? > "$REF_RES"

    ../src/42sh $@ 2> "$TES_ERR" > "$TES_OUT"
    echo $? > "$TES_RES"

    echo "$TEST_NAME": \>"$@"\<
    diff -u  "$TES_OUT" "$REF_OUT"
#    diff -u  "$TES_ERR" "$REF_ERR"
    diff -u  "$TES_RES" "$REF_RES"
#    rm "$REF_OUT" "$TES_OUT" "$REF_ERR" "$TES_ERR"
}


#BASIC
echo  " "
echo  " "
echo  "TESTS "

TEST_NAME="simple echo -c"
print -c "echo a b c"

TEST_NAME="simple echo option n"
print -c "echo -n a b c"

TEST_NAME="simple echo option e"
print -c "echo -e a b c\n"

TEST_NAME="simple echo option e and E"
print -c "echo -eEEeeeE a b c\n"

TEST_NAME="string echo"
print simple_echo.sh

TEST_NAME="stdin echo"
cat simple_echo.sh | print

TEST_NAME="double echo"
print double_echo.sh

TEST_NAME="double echo without ;"
print double_in_single__echo.sh

TEST_NAME="simple if true"
print simple_if_true.sh

TEST_NAME="simple if false"
print simple_if_false.sh

TEST_NAME="if true to false"
print if_true_to_false.sh

TEST_NAME="if true then if echo"
print if_in_if.sh

TEST_NAME="if true then if false"
print if_true_if_false.sh

TEST_NAME="double if false"
print dif_false.sh

TEST_NAME="if if true"
print if_if_true.sh

TEST_NAME="if if false"
print if_if_false.sh

TEST_NAME="if echo if"
print if_echo.sh

TEST_NAME="if to another if"
print if_another_if.sh

TEST_NAME="if execute file"
print ./simple_echo.sh

TEST_NAME="simple while"
print simple_while.sh

TEST_NAME="double while"
print double_while.sh

TEST_NAME="while with if"
print while_to_if.sh

TEST_NAME="if with while"
print if_to_while.sh

TEST_NAME="simple until"
print simple_until.sh

TEST_NAME="double until"
print double_until.sh

TEST_NAME="until with if"
print until_to_if.sh

TEST_NAME="if with until"
print if_to_until.sh
