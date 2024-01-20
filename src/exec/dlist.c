#include "dlist.h"

#include <stdlib.h>
#include <unistd.h>

struct dlist *dlist_init(void)
{
    struct dlist *dl = malloc(sizeof(struct dlist));
    if (dl == NULL)
        return NULL;
    dl->size = 0;
    dl->head = NULL;
    dl->tail = NULL;
    return dl;
}

int dlist_push_back(struct dlist *list, int save_fd, int io_number)
{
    if (!list)
        return 0;
    if (save_fd < 0 || io_number < 0)
        return 0;
    struct dlist_item *dli = malloc(sizeof(struct dlist_item));
    if (!dli)
        return 0;

    else if (!list->tail)
    {
        list->head = dli;
        list->tail = dli;
        dli->next = NULL;
        dli->prev = NULL;

        dli->save_fd = save_fd;
        dli->io_number = io_number;
        list->size++;
        return 1;
    }

    dli->prev = list->tail;
    dli->next = NULL;
    list->tail->next = dli;
    list->tail = dli;

    dli->save_fd = save_fd;
    dli->io_number = io_number;
    list->size++;
    return 1;
}

static void dlist_item_destroy(struct dlist_item *item)
{
    if (item != NULL)
    {
        dlist_item_destroy(item->next);
        free(item);
    }
    return;
}

void dlist_destroy(struct dlist *list)
{
    if (list != NULL)
    {
        dlist_item_destroy(list->head);
        free(list);
    }
    return;
}

int dlist_close_fd(struct dlist *dlist)
{
    if (!dlist->head)
        return 1;
    struct dlist_item *tmp = dlist->head;
    while (tmp->prev != NULL)
    {
        close(tmp->save_fd);
        close(tmp->io_number);
        tmp = tmp->prev;
    }
    return 1;
}
