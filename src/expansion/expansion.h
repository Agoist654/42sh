#ifndef EXPANSION_H
#define EXPANSION_H

struct str
{
    char *str;
    int current_pos;
    int size;
};

char *expansion(char *str);

#endif /* !EXPANSION_H */
