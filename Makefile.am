bin_PROGRAMS = 42sh
42sh_SOURCES = src/42sh.c

42sh_CFLAGS = -Wall -pedantic -std=c99 -Werror -Wextra -Wvla
42sh_CPPFLAGS = -I$(top_srcdir)/src

42sh_LDADD = \
	$(top_build)/src/parser/libparser.a \
	$(top_build)/src/ast/libast.a \
	$(top_build)/src/lexer/liblexer.a \
	$(top_build)/src/IO_backend/libio.a

SUBDIRS = src/parser src/ast src/IO_backend src/lexer
