#include <string.h>

// tools.c

int endsWith(const char* str, const char* endStr) {
    if (!str || !endStr) {
        return 0;  // 如果任一字符串为空，返回0
    }
    size_t strLen = strlen(str);
    size_t endStrLen = strlen(endStr);
    // 如果结尾字符串的长度大于要检查的字符串，那么它肯定不可能以结尾字符串结束
    if (endStrLen > strLen) {
        return 0;
    }
    // 比较结尾部分
    return (strcmp(str + strLen - endStrLen, endStr) == 0);
}
