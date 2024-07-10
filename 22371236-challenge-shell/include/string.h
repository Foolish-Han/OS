#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void* memcpy(void* dst, const void* src, size_t n);
void* memset(void* dst, int c, size_t n);
size_t strlen(const char* s);
char* strcpy(char* dst, const char* src);
const char* strchr(const char* s, int c);
int strcmp(const char* p, const char* q);
//! challenge-shell start
int strcat(char* dst, const char* src);
int strncmp(const char* s1, const char* s2, size_t n);
int isdigit(char c);
//! challenge-shell end
#endif
