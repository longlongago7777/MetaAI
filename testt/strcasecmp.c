#include <stdio.h>
#include <ctype.h>

// 实现一个忽略大小写的strstr函数
char* stristr(const char *haystack, const char *needle) {
    if (!*needle) {
        return (char*)haystack; // 如果needle为空字符串，返回haystack的起始地址
    }
    
    const char *h = haystack;
    const char *n = needle;
    
    while (*h) {
        // 找到可能的匹配起始位置
        const char *h_temp = h;
        const char *n_temp = n;
        
        // 比较haystack和needle，忽略大小写
        while (*h_temp && *n_temp && tolower((unsigned char)*h_temp) == tolower((unsigned char)*n_temp)) {
            h_temp++;
            n_temp++;
        }
        
        // 如果needle已经完全匹配，返回匹配的起始位置
        if (!*n_temp) {
            return (char*)h;
        }
        
        // 移动到haystack的下一个字符，继续搜索
        h++;
    }
    
    return NULL; // 未找到匹配项
}

int main() {
    const char *sql = "INSERT INTO table VALUES (:value1, :value2)";
    const char *test_str1 = "values";
    const char *test_str2 = "valueS";
    const char *test_str3 = "VALUES";

    // 使用stristr测试
    char *result1 = stristr(sql, test_str1);
    char *result2 = stristr(sql, test_str2);
    char *result3 = stristr(sql, test_str3);

    if (result1) {
        printf("'%s' found in '%s' at position %ld\n", test_str1, sql, result1 - sql);
    } else {
        printf("'%s' not found in '%s'\n", test_str1, sql);
    }

    if (result2) {
        printf("'%s' found in '%s' at position %ld\n", test_str2, sql, result2 - sql);
    } else {
        printf("'%s' not found in '%s'\n", test_str2, sql);
    }

    if (result3) {
        printf("'%s' found in '%s' at position %ld\n", test_str3, sql, result3 - sql);
    } else {
        printf("'%s' not found in '%s'\n", test_str3, sql);
    }

    return 0;
}
