#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include "builtin/builtin.h"

Test(echo, normal) {
    cr_redirect_stdout();
    char *tab[3] = { "echo", "normal", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("normal\n");
}

Test(echo, n) {
    cr_redirect_stdout();
    char *tab[4] = { "echo", "-n", "normal", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("normal");
}

Test(echo, eE) {
    cr_redirect_stdout();
    char *tab[4] = { "echo", "-eE", "normal\n", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("normal\\n\n");
}

Test(echo, na) {
    cr_redirect_stdout();
    char *tab[4] = { "echo", "-na", "normal", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("-na normal");
}

Test(echo, e) {
    cr_redirect_stdout();
    char *tab[4] = { "echo", "-e", "normal\n", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("normal\n\n");
}

Test(echo, eeEeEe) {
    cr_redirect_stdout();
    char *tab[4] = { "echo", "-eeEeEe", "normal\n", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("normal\n\n");
}

Test(echo, eEne) {
    cr_redirect_stdout();
    char *tab[5] = { "echo", "-eE", "-ne", "normal\n", NULL };
    echo(tab);
    fflush(NULL);
    cr_expect_stdout_eq_str("normal\n");
}
