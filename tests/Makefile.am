check_PROGRAMS = criterion

criterion_SOURCES = test_builtin.c
criterion_CPPFLAGS = -I$(top_builddir)/src
criterion_CFLAGS = -g

criterion_LDADD = \
	-lcriterion \
	$(top_builddir)/src/builtin/libbuilt.a

check-local: criterion
	./criterion
	./testsuite.sh $(top_builddir)/42sh
