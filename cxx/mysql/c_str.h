#ifndef _C_STR_H
#define _C_STR_H

char* ltrim(char* src)
{
    char* res = src;
    char* dst = src;
    while (*dst == ' ' || *dst == '\t')
        dst ++;
    while (*src++ = *dst++);
    return res;
}

#endif//_C_STR_H
