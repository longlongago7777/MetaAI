#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main() {
    char orastmt[] = "INSERT INTO emp(empno, ename, job, sal, deptno) VALUES (:empno, :ename, :job, :sal, :deptno)";
    regex_t regex;
    regmatch_t matches[1];
    char result[1024]; // 确保这个数组足够大以存储最终的字符串
    char *pattern = "(:[a-zA-Z0-9_]+)"; // 匹配形如 :name 的命名占位符
    char *replacement = "?"; // 替换为匿名占位符 ?
    const char *cursor = orastmt; // 使用 const char* 作为游标
    int offset = 0; // 用于跟踪结果字符串中的偏移量

    // 编译正则表达式
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return 1;
    }

    // 查找并替换所有匹配项
    while (regexec(&regex, cursor, 1, matches, 0) == 0) {
        size_t match_start = matches[0].rm_so;
        size_t match_end = matches[0].rm_eo;

        // 将匹配前的字符串复制到结果中
        strncpy(result + offset, cursor, match_start);
        offset += match_start;

        // 添加替换字符串
        result[offset++] = '?';
        result[offset] = '\0'; // 确保字符串以空字符结尾

        // 更新游标位置
        cursor += match_end;
    }

    // 添加剩余的字符串
    strcpy(result + offset, cursor);

    // 输出替换后的SQL语句
    printf("Modified SQL: %s\n", result);

    // 释放正则表达式
    regfree(&regex);

    return 0;
}
