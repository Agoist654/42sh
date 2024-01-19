# define the sub directories
SUBDIRS = \
	ast \
	parser \
	io_backend \
	lexer \
	exec \
	expansion \
	builtin

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g

42sh_LDADD =  \
	parser/libparser.a \
	ast/libast.a \
	exec/libexec.a \
	lexer/liblexer.a \
	io_backend/libio.a \
	expansion/libexp.a \
	builtin/libbuilt.a
