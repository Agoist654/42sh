# define the sub directories
SUBDIRS = \
	ast \
	parser \
	io_backend \
	lexer \
	exec \
	builtin

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
	ast/libast.a \
	parser/libparser.a \
	io_backend/libio.a \
	builtin/libbuilt.a \
	exec/libexec.a \
	lexer/liblexer.a
