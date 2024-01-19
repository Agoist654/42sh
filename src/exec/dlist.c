#include "dlist.h"

#include <stdlib.h>
#include <unistd.h>

struct dlist *dlist_init(void)
{
    struct dlist *dl = malloc(sizeof(struct dlist));
    if (!dl)
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

static void dlist_remove_at(struct dlist *list, size_t index)
{
    if (index >= list->size || list->head == NULL)
        return;
    if (index == 0 && list->size == 1)
    {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->size--;
    }
    else if (index == list->size - 1)
    {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
        list->size--;
    }
    else if (index == 0)
    {
        list->head = list->head->next;
        free(list->head->prev);
        list->head->prev = NULL;
        list->size--;
    }
    else
    {
        struct dlist_item *tmp = list->head;
        for (size_t k = 0; k != index; k++)
        {
            tmp = tmp->next;
        }
        tmp->next->prev = tmp->prev;
        tmp->prev->next = tmp->next;
        free(tmp);
        list->size--;
    }
}

void dlist_destroy(struct dlist *list)
{
    if (!list || !list->head)
        return;
    struct dlist_item *tmp = list->head;
    struct dlist_item *ttmp = list->head;
    while (ttmp->next != NULL)
    {
        ttmp = tmp->next;
        dlist_remove_at(list, 0);
        tmp = ttmp;
    }
    dlist_remove_at(list, 0);
    free(list);
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
