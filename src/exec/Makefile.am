lib_LIBRARIES = libexec.a

libexec_a_SOURCES =  \
	exec1.c \
	exec2.c \
	exec.h \
	redirection.c \
	redirection.h \
	dlist.c \
	dlist.h \
	pipeline.c \
	pipeline.h \
	hash.c \
	hash_map.c \
	hash_map.h

libexec_a_CPPFLAGS = -I$(top_srcdir)/src

libexec_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libexec.a
