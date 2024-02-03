lib_LIBRARIES = libbuilt.a

libbuilt_a_SOURCES = \
	builtin.c \
	builtin_echo.c \
	builtin_export.c \
	builtin.h \
	tf.c

libbuilt_a_CPPFLAGS = -I$(top_srcdir)/src

libbuilt_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g

noinst_LIBRARIES = libbuilt.a
