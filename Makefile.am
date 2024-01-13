bin_PROGRAMS = 42sh
42sh_SOURCES = src/42sh.c

42sh_CFLAGS = -Wall -pedantic -std=c99 -Werror -Wextra -Wvla
42sh_CPPFLAGS = -I$(top_srcdir)/src

42sh_LDADD = \
	$(top_build)/src/parser/libparser.a \
	$(top_build)/src/ast/libast.a \
	$(top_build)/src/exec/libexec.a \
	$(top_build)/src/builtin/libbuilt.a \
	$(top_build)/src/lexer/liblexer.a \
	$(top_build)/src/io_backend/libio.a

SUBDIRS = src/parser src/ast src/io_backend src/lexer src/builtin src/exec
