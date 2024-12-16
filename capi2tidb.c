//#include <iostream>
#include <stdio.h>
#include <mysql.h>
#include <string.h>
//using namespace std;

int main()
{
    int i = 0;
    int ret = 0;

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[3];
    int deptno=1000;
    char dname[32]="dn";
    char loc[32]="loc1201";
    int rc;

    // 1. 初始化连接环境
    MYSQL* mysqlconn = mysql_init(NULL);
    if(mysqlconn == NULL)\
    {
        printf("mysql_init() error\n");
        return -1;
    }

    // 2. 连接数据库服务器
    mysqlconn = mysql_real_connect(mysqlconn, "10.2.103.28", "root", "", 
                               "testly", 4000, NULL, 0);
    if(mysqlconn == NULL)
    {
        printf("mysql_real_connect() error\n");
        return -1;
    }

    printf("mysql api使用的默认编码: %s\n", mysql_character_set_name(mysqlconn));

    // 设置编码为utf8
    mysql_set_character_set(mysqlconn, "utf8mb4");
    mysql_options(mysqlconn, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    printf("mysql api使用的修改之后的编码: %s\n", mysql_character_set_name(mysqlconn));

    printf("恭喜, 连接数据库服务器成功了...\n");

    // 3. 执行一个sql语句
    // 查询scott数据库下的dept部门表
    const char* sql = "select * from dept";
    // 执行这个sql语句
    ret = mysql_query(mysqlconn, sql);
    if(ret != 0)
    {
        printf("mysql_query() a失败了, 原因: %s\n", mysql_error(mysqlconn));
        return -1;
    }

    // 4. 取出结果集
    MYSQL_RES* res = mysql_store_result(mysqlconn);
    if(res == NULL)
    {
        printf("mysql_store_result() 失败了, 原因: %s\n", mysql_error(mysqlconn));
        return -1;
    }

    // 5. 得到结果集中的列数
    int num = mysql_num_fields(res);

    // 6. 得到所有列的名字, 并且输出
    MYSQL_FIELD * fields = mysql_fetch_fields(res);
    for(i=0; i<num; ++i)
    {
        printf("%s\t\t", fields[i].name);
    }
    printf("\n");

    // 7. 遍历结果集中所有的行
    MYSQL_ROW row;
    while( (row = mysql_fetch_row(res)) != NULL)
    {
        // 将当前行中的每一列信息读出
        for(i=0; i<num; ++i)
        {
            printf("%s\t\t", row[i]);
        }
        printf("\n");
    }

    // 8. 释放资源 - 结果集
    mysql_free_result(res);

    // 9. 写数据库
    // 以下三条是一个完整的操作, 对应的是一个事务
    // 设置事务为手动提交
    mysql_autocommit(mysqlconn, 0); 
    int ret1 = mysql_query(mysqlconn, "insert into dept values(61, '海军', '圣地玛丽乔亚')");
    int ret2 = mysql_query(mysqlconn, "insert into dept values(62, '七武海', '世界各地')");
    int ret3 = mysql_query(mysqlconn, "insert into dept values(63, '四皇', '新世界')");
    printf("ret1 = %d, ret2 = %d, ret3 = %d\n", ret1, ret2, ret3);

    if(ret1==0 && ret2==0 && ret3==0)
    {
        // 提交事务
        mysql_commit(mysqlconn);
    }
    else
    {
        mysql_rollback(mysqlconn);
    }

    //dml with bind
    // 创建预处理语句对象
    stmt = mysql_stmt_init(mysqlconn);
    if (stmt == NULL) {
        fprintf(stderr, "mysql_stmt_init(), out of memory\n");
        exit(1);
    }

    // 插入操作
    const char *insert_query = "INSERT INTO DEPT (deptno, dname, loc) VALUES (?, ?, ?)";
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query))) {
        fprintf(stderr, "mysql_stmt_prepare(), INSERT failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    // 绑定插入参数
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (int *)&deptno;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char *)dname;
    bind[1].buffer_length = strlen(dname);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char *)loc;
    bind[2].buffer_length = strlen(loc);

    ret1 = mysql_query(mysqlconn, "insert into dept values(61, '海军', '圣地玛丽乔亚')");
    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    // 插入数据
    //deptno = 1000;
    //strcpy(dname, "ACCOUNTING");
    //strcpy(loc, "NEW YORK");
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    // 查询操作
    const char *select_query = "SELECT dname, loc FROM DEPT WHERE deptno = ?";
    if (mysql_stmt_prepare(stmt, select_query, strlen(select_query))) {
        fprintf(stderr, "mysql_stmt_prepare(), SELECT failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    // 绑定查询参数
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&deptno;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char *)dname;
    bind[1].buffer_length = 32;
    //bind[1].charsetnr = mysql_get_charset_info(mysqlconn).number;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char *)loc;
    bind[2].buffer_length = 32;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    if (mysql_stmt_bind_result(stmt, bind + 1)) {
        fprintf(stderr, "mysql_stmt_bind_result() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    deptno = 1000;
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    while (mysql_stmt_fetch(stmt) == 0) {
        printf("dname: %s, loc: %s\n", dname, loc);
    }

    // 删除操作
    const char *delete_query = "DELETE FROM DEPT WHERE deptno = ?";
    if (mysql_stmt_prepare(stmt, delete_query, strlen(delete_query))) {
        fprintf(stderr, "mysql_stmt_prepare(), DELETE failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    // 绑定删除参数
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&deptno;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    deptno = 10;
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        exit(1);
    }


    {
        // 提交事务
        mysql_commit(mysqlconn);
    }


    // 释放数据库资源
    mysql_close(mysqlconn);
    
  return 0;
}
