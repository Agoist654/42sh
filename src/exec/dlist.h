#ifndef DLIST_H
#define DLIST_H

#include <stddef.h>
struct dlist_item
{
    int save_fd;
    int io_number;
    struct dlist_item *next;
    struct dlist_item *prev;
};

struct dlist
{
    size_t size;
    struct dlist_item *head;
    struct dlist_item *tail;
};

struct dlist *dlist_init(void);
int dlist_push_back(struct dlist *list, int save_fd, int io_number);
int dlist_close_fd(struct dlist *dlist);

#endif // DLIST_H
