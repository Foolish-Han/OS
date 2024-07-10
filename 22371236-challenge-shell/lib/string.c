#include <types.h>

void* memcpy(void* dst, const void* src, size_t n) {
    void* dstaddr = dst;
    void* max = dst + n;

    if (((u_long)src & 3) != ((u_long)dst & 3)) {
        while (dst < max) {
            *(char*)dst++ = *(char*)src++;
        }
        return dstaddr;
    }

    while (((u_long)dst & 3) && dst < max) {
        *(char*)dst++ = *(char*)src++;
    }

    // copy machine words while possible
    while (dst + 4 <= max) {
        *(uint32_t*)dst = *(uint32_t*)src;
        dst += 4;
        src += 4;
    }

    // finish the remaining 0-3 bytes
    while (dst < max) {
        *(char*)dst++ = *(char*)src++;
    }
    return dstaddr;
}

void* memset(void* dst, int c, size_t n) {
    void* dstaddr = dst;
    void* max = dst + n;
    u_char byte = c & 0xff;
    uint32_t word = byte | byte << 8 | byte << 16 | byte << 24;

    while (((u_long)dst & 3) && dst < max) {
        *(u_char*)dst++ = byte;
    }

    // fill machine words while possible
    while (dst + 4 <= max) {
        *(uint32_t*)dst = word;
        dst += 4;
    }

    // finish the remaining 0-3 bytes
    while (dst < max) {
        *(u_char*)dst++ = byte;
    }
    return dstaddr;
}

size_t strlen(const char* s) {
    int n;

    for (n = 0; *s; s++) {
        n++;
    }

    return n;
}

char* strcpy(char* dst, const char* src) {
    char* ret = dst;

    while ((*dst++ = *src++) != 0) {
    }

    return ret;
}

const char* strchr(const char* s, int c) {
    for (; *s; s++) {
        if (*s == c) {
            return s;
        }
    }
    return 0;
}

int strcmp(const char* p, const char* q) {
    while (*p && *p == *q) {
        p++, q++;
    }

    if ((u_int)*p < (u_int)*q) {
        return -1;
    }

    if ((u_int)*p > (u_int)*q) {
        return 1;
    }

    return 0;
}

//! challenge-shell start
int strcat(char* dst, const char* src) {
    char* ptr = dst;
    while (*ptr != '\0') {
        ptr++;
    }
    while (*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return 0;
}
//! challenge-shell end
int strncmp(const char* s1, const char* s2, size_t n) {
    while (n > 0 && *s1 != 0 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *s1 - *s2;
}
int isdigit(char c) {
    return c >= '0' && c <= '9';
}
