#! /bin/sh

REF_OUT="tests/ref.out"
REF_ERR="tests/ref_err.out"

TES_OUT="tests/test.out"
TES_ERR="tests/test_err.out"

TES_RES="tests/test_res.out"
REF_RES="tests/ref_res.out"

#DIFF="tests/diff.out"

print(){

    echo "as file================================================\n"

rm "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"
    touch "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"

    bash --posix tests/$@ 2> "$REF_ERR" >"$REF_OUT"
    echo $? > "$REF_RES"

    ./src/42sh tests/$@ 2> "$TES_ERR" > "$TES_OUT"
    echo $? > "$TES_RES"

    echo "$TEST_NAME": \>"$@"\<
    diff -u  "$TES_OUT" "$REF_OUT"
    diff -u  "$TES_ERR" "$REF_ERR"
    diff -u  "$TES_RES" "$REF_RES"
}

printin(){

    echo "as stdin================================================\n"

rm "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"
    touch "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"

    bash --posix < tests/$@ 2> "$REF_ERR" >"$REF_OUT"
    echo $? > "$REF_RES"

    ./src/42sh < tests/$@ 2> "$TES_ERR" > "$TES_OUT"
    echo $? > "$TES_RES"

    echo "$TEST_NAME": \>"$@"\<
    diff -u  "$TES_OUT" "$REF_OUT"
    diff -u  "$TES_ERR" "$REF_ERR"
    diff -u  "$TES_RES" "$REF_RES"
}


printnoerr(){

    echo "as stdin================================================\n"

rm "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"
    touch "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"

    bash --posix tests/$@ 2> "$REF_ERR" >"$REF_OUT"
    echo $? > "$REF_RES"

    ./src/42sh tests/$@ 2> "$TES_ERR" > "$TES_OUT"
    echo $? > "$TES_RES"

    echo "$TEST_NAME": \>"$@"\<
    diff -u  "$TES_OUT" "$REF_OUT"
    #diff -u  "$TES_ERR" "$REF_ERR"
    diff -u  "$TES_RES" "$REF_RES"
}



printc(){

    echo "as string================================================\n"

rm "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"
    touch "$REF_ERR" "$REF_OUT" "$TES_ERR"  "$TES_OUT" "$TES_RES" "$REF_RES"

    bash --posix $@ 2> "$REF_ERR" >"$REF_OUT"
    echo $? > "$REF_RES"

    ./src/42sh $@ 2> "$TES_ERR" > "$TES_OUT"
    echo $? > "$TES_RES"

    echo "$TEST_NAME": \>"$@"\<
    diff -u  "$TES_OUT" "$REF_OUT"
    diff -u  "$TES_ERR" "$REF_ERR"
    diff -u  "$TES_RES" "$REF_RES"
}



#BASIC
echo  " "
echo  " "
echo  "TESTS "

TEST_NAME="simple echo -c"
printc -c "echo a b c"

TEST_NAME="simple echo option n"
printc -c "echo -n a b c"

TEST_NAME="simple echo option e"
printc -c "echo -e a b c\n"

TEST_NAME="simple echo option e and E"
printc -c "echo -eEEeeeE a b c\n"

TEST_NAME="string echo"
printin simple_echo.sh

TEST_NAME="double echo"
printin double_echo.sh

TEST_NAME="double echo without ;"
print double_in_single_echo.sh

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
printin if_another_if.sh

TEST_NAME="if execute file"
print ./simple_echo.sh

TEST_NAME="simple_while1"
print simple_while1

TEST_NAME="simple_while3"
print simple_while3

TEST_NAME="hard_while_2"
print hard_while_2

TEST_NAME="hard_while_while1"
print hard_while_while1

#TEST_NAME="double while"
#print double_while.sh

#TEST_NAME="while with if"
#print while_to_if.sh

#TEST_NAME="if with while"
#print if_to_while.sh

TEST_NAME="simple_until1"
print simple_until1

TEST_NAME="simple_until3"
print simple_until3

TEST_NAME="hard_until1"
print hard_until1

TEST_NAME="hard_until2"
print hard_until2

TEST_NAME="two_loops"
print two_loops
#TEST_NAME="double until"
#print double_until.sh

#TEST_NAME="until with if"
#print until_to_if.sh

#TEST_NAME="if with until"
#print if_to_until.sh

#TEST_NAME="if !true"
#print if_not_true.sh

#TEST_NAME="if !false"
#print if_not_false.sh

#TEST_NAME="while !true"
#print while_not_true.sh

#TEST_NAME="while !false"
#print while_not_false.sh

#TEST_NAME="until !true"
#print until_not_true.sh

#TEST_NAME="until !false"
#print until_not_false.sh

TEST_NAME="negation_while_until"
print negation_while_until

TEST_NAME="simple pipe"
print simple_pipe.sh

TEST_NAME="if pipe"
print if_pipe.sh

#TEST_NAME="while pipe"
#print while_pipe.sh

#TEST_NAME="until pipe"
#print until_pipe.sh

TEST_NAME="simple and"
print simple_and.sh

TEST_NAME="simple or"
print simple_or.sh

TEST_NAME="if and"
print if_and.sh

#TEST_NAME="while and"
#print while_and.sh

#TEST_NAME="until and"
#print until_and.sh

TEST_NAME="if or"
print if_or.sh

#TEST_NAME="until or"
#print while_or.sh

#TEST_NAME="while or"
#print until_or.sh

TEST_NAME="simple_var"
print simple_var.sh

TEST_NAME="get_pwd"
printc -c 'echo $PWD'

TEST_NAME="a"
printin a

TEST_NAME="badexec1"
printnoerr badexec1

TEST_NAME="badexec2"
printnoerr badexec2

TEST_NAME="badexec3"
printnoerr badexec3

TEST_NAME="badexec4"
printnoerr badexec4

TEST_NAME="badexec5"
printnoerr badexec5

TEST_NAME="badexec6"
printnoerr badexec6

TEST_NAME="4"
print 4

TEST_NAME="badl"
print badl

TEST_NAME="complexe_for"
print complexe_for.sh

TEST_NAME="complexe_for2"
print complexe_for2.sh

TEST_NAME="complexe_for3"
printnoerr complexe_for3.sh

TEST_NAME="simple_for"
printin for.sh

TEST_NAME="l"
print l

TEST_NAME="p"
print p

TEST_NAME="quote"
print quote

TEST_NAME="quote2"
print quote2

TEST_NAME="quote3"
print quote3

TEST_NAME="quote4"
print quote4

TEST_NAME="badtriplepipe"
printnoerr badtriplepipe

TEST_NAME="var_for_var"
print var_for_var

TEST_NAME="nested_for"
print nested_for

TEST_NAME="doublefor"
print doublefor

TEST_NAME="big_arg"
print big_arg

#TEST_NAME="if_redir"
#print if_redir

TEST_NAME="bizare_for.sh"
printnoerr bizare_for.sh

TEST_NAME="a_n"
print a_n

TEST_NAME="doublequote.sh"
print doublequote.sh

TEST_NAME="doublequote2.sh"
print doublequote2.sh

TEST_NAME="var_for_var"
print var_for_var

TEST_NAME="echo_e_backslash1"
print echo_e_backslash1

TEST_NAME="echo_e_backslash2"
print echo_e_backslash2

TEST_NAME="echo_e_backslash3"
print echo_e_backslash3

TEST_NAME="echo_many_e_E2"
print echo_many_e_E2

TEST_NAME="echo_mix_options1"
print echo_mix_options1

TEST_NAME="echo_mix_options2"
print echo_mix_options2

TEST_NAME="echo_eEe"
print echo_eEe

TEST_NAME="function_echo.sh"
print function_echo.sh

TEST_NAME="in"
print in

TEST_NAME="tst"
print tst

TEST_NAME="var_simple_bracket"
print var_simple_bracket

TEST_NAME="var_simple_command"
print var_simple_command

TEST_NAME="sq_echo_mix1"
print sq_echo_mix1

TEST_NAME="sq_echo_mix2"
print sq_echo_mix2

TEST_NAME="quote_command"
print quote_command

TEST_NAME="simple_subshell.sh"
print simple_subshell.sh

TEST_NAME="simple_subshell2.sh"
print simple_subshell2.sh

TEST_NAME="subshell.sh"
print subshell.sh

TEST_NAME="subshell2.sh"
print subshell2.sh

TEST_NAME="quote_var1"
print quote_var1

TEST_NAME="simple_continue.sh"
print simple_continue.sh

TEST_NAME="if_continue.sh"
print if_continue.sh

TEST_NAME="pipe.sh"
print pipe.sh

TEST_NAME="quoted_builtin"
print quoted_builtin

#have prob, it execve the builtin true and not our true_f
TEST_NAME="a"
print a

#TEST_NAME="complexe_subshell.sh"
#print complexe_subshell.sh

TEST_NAME="simple_continue1.sh"
print simple_continue1.sh

TEST_NAME="until_break"
print until_break

TEST_NAME="while_break_cond"
print while_break_cond

TEST_NAME="simple_redir.sh"
print simple_redir.sh

TEST_NAME="exit_without_arg"
print exit_without_arg

TEST_NAME="exit_forloop"
print exit_forloop

TEST_NAME="realloc"
print realloc

TEST_NAME="realloc_add_redir"
print realloc_add_redir

TEST_NAME="simple_break"
print simple_break

TEST_NAME="simple_export_unset"
print simple_export_unset

TEST_NAME="unset_vf"
printnoerr unset_vf

TEST_NAME="unset_f"
printnoerr unset_f

TEST_NAME="unset_v"
print unset_v

TEST_NAME="parse_prefix"
print parse_prefix

TEST_NAME="add_redir_cmd"
print add_redir_cmd

TEST_NAME="list_rmv"
printnoerr list_rmv

TEST_NAME="list_rmv_var"
printnoerr list_rmv_var

TEST_NAME="list_var_rm"
printnoerr list_var_rm

#TEST_NAME="bad_unterminated_quote"
#print bad_unterminated_quote

#TEST_NAME="bad_weak2"
#print bad_weak2

#TEST_NAME="bad_weak3"
#print bad_weak3
