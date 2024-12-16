#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELDS 16
#define FIELD_SIZE 32

typedef struct {
    char fields[FIELD_SIZE];
    int indexno;
} StmtFields;

StmtFields stmtF[MAX_FIELDS]; // 声明结构体数组

// 函数用于解析SQL语句并填充StmtFields数组
int parse_sql(const char *stmtsql, StmtFields stmtF[]) {
    const char *insert_pos = strstr(stmtsql, "INSERT INTO");
    if (insert_pos == NULL) return 0; // 没有找到"INSERT INTO"

    const char *start = strstr(insert_pos, "(");
    const char *end = strstr(start, ")");
    if (start == NULL || end == NULL) return 0; // 没有找到"("或")"

    // 移过"("字符
    start++;
    int field_count = 0;
    while (start < end && *start != ',' && field_count < MAX_FIELDS) {
        const char *field_start = start;
        // 寻找字段名的结束位置（字段名之间用逗号分隔）
        while (*start != ',' && *start != ')') start++;
        size_t field_len = start - field_start;
        if (field_len > 0) {
            strncpy(stmtF[field_count].fields, field_start, field_len);
            stmtF[field_count].fields[field_len] = '\0'; // 确保NULL终止
            stmtF[field_count].indexno = field_count;
            field_count++;
        }
        if (*start == ',') start++; // 跳过逗号
    }
    return field_count; // 返回找到的字段数量
}

int main() {
    const char *stmtsql= "INSERT INTO emp(empno, ename, job, sal, deptno) "
                      "VALUES (:empno, :ename, :job, :sal, :deptno)";
    int stmtFieldsCNT = parse_sql(stmtsql, stmtF); // 解析SQL并获取字段数量

    printf("Parsed %d fields:\n", stmtFieldsCNT);
    for (int i = 0; i < stmtFieldsCNT; i++) {
        printf("Field %d: %s, Index No: %d\n", i, stmtF[i].fields, stmtF[i].indexno);
    }

    return 0;
}
