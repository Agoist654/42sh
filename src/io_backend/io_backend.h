#ifndef IO_BACKEND_H
#define IO_BACKEND_H

void io_backend(int argc, char *argv[]);
char io_peek(void);
char io_pop(void);
int io_close(void);

#endif /* !IO_BACKEND_H */
