#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <ctype.h>
#include <regex.h>

#include <pthread.h>

//_thread_local int thread_local_var = 0; // 线程局部变量

#include "oci.h"

#include <mysql.h>
#include <errmsg.h>

#include "ociwrapper.h"



//oracle unrecongized macro
#define OCI_ATTR_SERVICE_NAME 10001
#define OCI_ATTR_INSTANCE 10002
#define OCI_ATTR_STMT 10003

//Global variables

    #define HOST_MAX_LENGTH 16
    #define PORT_MAX_LENGTH 6
    #define SERVICE_NAME_MAX_LENGTH 33
    char host[HOST_MAX_LENGTH] = {0};
    char port[PORT_MAX_LENGTH] = {0};
    char service_name[SERVICE_NAME_MAX_LENGTH] = {0};
    char user[SERVICE_NAME_MAX_LENGTH] = {0};
    char password[SERVICE_NAME_MAX_LENGTH] = {0};
    char dbname[SERVICE_NAME_MAX_LENGTH] = {0};

    __thread MYSQL *tidbconn ;
    __thread MYSQL_STMT *stmt;
    __thread MYSQL_BIND bind; //OCIDefineByPos
    __thread MYSQL_BIND bindByname[16]; //OCIBindByPos
    __thread int bindbnCnt =0 ; //OCIBindByPos
    __thread  int bindFNUM=0;
/*    MYSQL *tidbconn ;
    MYSQL_STMT *stmt;
    MYSQL_BIND bind; //OCIDefineByPos
    MYSQL_BIND bindByname[16]; //OCIBindByPos
    int bindbnCnt =0 ; //OCIBindByPos
*/


    //parse the  fields name and number to prepare for the bindbyname for mysql has no bindbyname, we need to implement it
    #define MAX_FIELDS 16
    #define FIELD_SIZE 32

    typedef struct {
        char fields[FIELD_SIZE];
        int indexno;
    } StmtFields;

    __thread StmtFields stmtF[MAX_FIELDS]; // 声明结构体数组

   int parse_sql(const char *orastmt );
    

sword   OCIInitialize   (ub4 mode, void  *ctxp,
                 void  *(*malocfp)(void  *ctxp, size_t size),
                 void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
                 void   (*mfreefp)(void  *ctxp, void  *memptr) )
{
	mysql_thread_init();
	return (sword)0;
}

sword   OCITerminate( ub4 mode)
{
	mysql_thread_end();
        return (sword)0;
}

sword   OCIEnvCreate (OCIEnv **envp, ub4 mode, void  *ctxp,
                 void  *(*malocfp)(void  *ctxp, size_t size),
                 void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
                 void   (*mfreefp)(void  *ctxp, void  *memptr),
                 size_t xtramem_sz, void  **usrmempp)
{
    sword errcode=0;
    tidbconn = mysql_init(NULL);
    if (tidbconn == NULL) {
        // 处理错误，例如返回 NULL 或抛出异常
        return errcode;
    }
    return errcode;
}

sword   OCIHandleAlloc(const void  *parenth, void  **hndlpp, const ub4 type,
                       const size_t xtramem_sz, void  **usrmempp)
{

	switch (type) {
        case OCI_HTYPE_ENV:
            printf("Handling environment handle\n");
            // 处理环境句柄的代码
            break;
        case OCI_HTYPE_ERROR:
            printf("Handling error handle\n");
            // 处理错误句柄的代码
            break;
        case OCI_HTYPE_SVCCTX:
            printf("Handling service context handle\n");
            // 处理服务上下文句柄的代码
            break;
        case OCI_HTYPE_STMT:
            printf("Handling statement handle\n");
	    if (xtramem_sz==0) break;
	    usrmempp=malloc(xtramem_sz);
	    if (*usrmempp == NULL) {
            // 处理内存分配失败
            fprintf(stderr, "Memory allocation failed\n");
            // 释放之前分配的句柄
            OCIHandleFree(*hndlpp, type);
	    return -1;
            }
            // 处理语句句柄的代码
            break;
        case OCI_HTYPE_DESCRIBE:
            printf("Handling describe handle\n");
            // 处理描述句柄的代码
            break;
        case OCI_HTYPE_SERVER:
            printf("Handling server handle\n");
            // 处理服务器上下文句柄的代码
            break;
        case OCI_HTYPE_SESSION:
            printf("Handling session handle\n");
            // 处理会话句柄的代码
            break;
        case OCI_HTYPE_TRANS:
            printf("Handling transaction handle\n");
            // 处理事务上下文句柄的代码
            break;
        default:
            printf("Unknown handle type\n");
            // 未知句柄类型的处理代码
            break;
    }	
	
        return (sword)0;
}

sword   OCIHandleFree(void  *hndlp, const ub4 type)
{
	//map --key and pointer value.
	//free
        return (sword)0;
}

sword   OCIDescriptorAlloc(const void  *parenth, void  **descpp,
                           const ub4 type, const size_t xtramem_sz,
                           void  **usrmempp)
{
        return (sword)0;
}

sword   OCIEnvInit (OCIEnv **envp, ub4 mode,
                    size_t xtramem_sz, void  **usrmempp)
{
        return (sword)0;
}

sword   OCIAttrSet (void  *trgthndlp, ub4 trghndltyp, void  *attributep,
                    ub4 size, ub4 attrtype, OCIError *errhp)
{

    switch (attrtype) {
        case OCI_ATTR_USERNAME:
	    // 确保 user 字符串有足够的空间来存储复制的数据
    	    if (user != NULL && attributep != NULL) {
            // 使用 memcpy 将 attributep 指向的数据复制到 user 字符串
            memcpy(user, attributep, size);
            // 确保字符串以空字符结尾
            user[size] = '\0';
          }
          break;
        case OCI_ATTR_PASSWORD:
	    // 确保 user 字符串有足够的空间来存储复制的数据
            if (user != NULL && attributep != NULL) {
            // 使用 memcpy 将 attributep 指向的数据复制到 user 字符串
            memcpy(password, attributep, size);
            // 确保字符串以空字符结尾
            password[size] = '\0';
            }
            break;
        case OCI_ATTR_SESSION:
            break;
        case OCI_ATTR_SERVER:
            break;
        case OCI_ATTR_SERVICE_NAME:
            break;
        case OCI_ATTR_INSTANCE:
            break;
        case OCI_ATTR_ENV:
            break;
        case OCI_ATTR_STMT:
            break;
        case OCI_ATTR_ROW_COUNT:
            break;
        case OCI_ATTR_PREFETCH_ROWS:
            break;
        default:
            fprintf(stderr, "Unsupported attribute type\n");
            break;
     }

     return (sword)0;
}

sword   OCIServerAttach  (OCIServer *srvhp, OCIError *errhp,
                          const OraText *dblink, sb4 dblink_len, ub4 mode)
{	

    //const char *dblink = "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=10.2.103.249)(PORT=1521))(CONNECT_DATA=(SERVICE_NAME=XE)))";
 //   char host[HOST_MAX_LENGTH] = {0};
 //   char port[PORT_MAX_LENGTH] = {0};
 //   char service_name[SERVICE_NAME_MAXLENGTH] = {0};
    char *ptr;

    // 解析 HOST
    ptr = strstr(dblink, "HOST=");
    if (ptr) {
        ptr += 5; // 跳过 "HOST=" 的长度
        strncpy(host, ptr, strchr(ptr, ')') - ptr);
        host[strcspn(host, ")")] = 0; // 去除尾部的 ')'
    }

    // 解析 PORT
    ptr = strstr(dblink, "PORT=");
    if (ptr) {
        ptr += 5; // 跳过 "PORT=" 的长度
        strncpy(port, ptr, strchr(ptr, ')') - ptr);
        port[strcspn(port, ")")] = 0; // 去除尾部的 ')'
    }

    // 解析 SERVICE_NAME
    ptr = strstr(dblink, "SERVICE_NAME=");
    if (ptr) {
        ptr += 13; // 跳过 "SERVICE_NAME=" 的长度
        strncpy(service_name, ptr, strchr(ptr, ')') - ptr);
        service_name[strcspn(service_name, ")")] = 0; // 去除尾部的 ')'
    }

    // 打印结果
    printf("Host: %s\n", host[0] ? host : "Not found");
    printf("Port: %s\n", port[0] ? port : "Not found");
    printf("Service Name: %s\n", service_name[0] ? service_name : "Not found");

	
    return (sword)0;
}

sword   OCIServerDetach  (OCIServer *srvhp, OCIError *errhp, ub4 mode)
{
        return (sword)0;
}

sword   OCISessionBegin  (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
                          ub4 credt, ub4 mode)
{
	tidbconn = mysql_real_connect(tidbconn, host, user, "", 
                               service_name, atoi(port), NULL, 0);
    if(tidbconn == NULL)
    {
        printf("mysql_real_connect() error\n");
        return -1;
    }		
    printf("mysql api使用的默认编码: %s\n", mysql_character_set_name(tidbconn));

    // 设置编码为utf8
    mysql_set_character_set(tidbconn, "utf8mb4");
    //mysql_options(tidbconn, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    printf("mysql api使用的修改之后的编码: %s\n", mysql_character_set_name(tidbconn));

    printf("恭喜, 连接数据库服务器成功了...\n");	

        return (sword)0;
}

sword   OCISessionEnd   (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
                         ub4 mode)
{
        return (sword)0;
}

sword   OCIStmtPrepare   (OCIStmt *stmtp, OCIError *errhp, const OraText *orastmt,
                          ub4 stmt_len, ub4 language, ub4 mode)
{
    // 创建预处理语句对象
    stmt = mysql_stmt_init(tidbconn);
    if (stmt == NULL) {
        fprintf(stderr, "mysql_stmt_init(), out of memory\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
    }

    //    char sql[] = "INSERT INTO emp(empno, ename, job, sal, deptno) VALUES (:empno, :ename, :job, :sal, :deptno)";
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
    
    
    bindFNUM=parse_sql(orastmt);

    // prepare
    if (mysql_stmt_prepare(stmt, result, strlen(result))) {
        fprintf(stderr, "mysql_stmt_prepare(), INSERT failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
 //       exit(1);
    }



        return (sword)0;
}

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


// 函数用于解析SQL语句并填充StmtFields数组
int parse_sql(const char *orastmt ) {
    //reset stmtF for every parsql;
     memset(stmtF, 0, sizeof(stmtF)); 
    
    //parse insert sql 
    const char *insert_pos = stristr(orastmt, "values");
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
	    printf("stmtF[field_count].fields=%s,stmtF[field_count].indexno=%d \n", stmtF[field_count].fields,stmtF[field_count].indexno);
            field_count++;
        }
        if (*start == ',') start++; // 跳过逗号
    }
    return field_count; // 返回找到的字段数量
}


/*sword   OCIStmtPrepare   (OCIStmt *stmtp, OCIError *errhp, const OraText *orastmt,
                          ub4 stmt_len, ub4 language, ub4 mode)
{
    
        int deptno=1000;
    char dname[32]="dnwrap";
    char loc[32]="locwrap";
	    //dml with bind
    // 创建预处理语句对象
    stmt = mysql_stmt_init(tidbconn);
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

        return (sword)0;
}*/


long  g_int;
float g_float;
char* g_string;
long long g_longlong;


//bind the parameter by pos
sword   OCIBindByPos  (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
                       ub4 position, void  *valuep, sb4 value_sz,
                       ub2 dty, void  *indp, ub2 *alenp, ub2 *rcodep,
                       ub4 maxarr_len, ub4 *curelep, ub4 mode)
{
    memset(&bindByname[bindbnCnt], 0, sizeof(bindByname[bindbnCnt]));


    switch (dty) {
    case SQLT_INT:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_LONG;
        bindByname[bindbnCnt].buffer = (int *)valuep;
        bindByname[bindbnCnt].buffer_length = value_sz;
        break;
    case SQLT_FLT:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_FLOAT;
        bindByname[bindbnCnt].buffer = (float *)valuep;
        bindByname[bindbnCnt].buffer_length = value_sz;
        break;
    case SQLT_STR:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_STRING;
        bindByname[bindbnCnt].buffer = (char *)valuep;
        bindByname[bindbnCnt].buffer_length = value_sz;
        break;
    case SQLT_LNG:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_LONGLONG;
        break;
    case SQLT_DAT:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_DATE;
        break;
    case SQLT_TIMESTAMP:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_TIMESTAMP;
        break;
    case SQLT_BLOB:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_BLOB;
        break;
    case SQLT_CLOB:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_STRING; // CLOB在MySQL中通常作为长文本处理
        break;
    default:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_STRING; // 默认类型
        break;
    }

    //to know if all bind fields are done
    bindbnCnt++;
    printf("bindbnCnt=%d, bindFNUM = %d \n", bindbnCnt, bindFNUM);

    if (bindbnCnt == bindFNUM){
        printf("Call mysql_stmt_bind_param  when bindbnCnt=%d, bindFNUM = %d \n", bindbnCnt, bindFNUM);
        if (mysql_stmt_bind_param(stmt, bindByname)) {
                fprintf(stderr, "mysql_stmt_bind_param() failed\n");
                fprintf(stderr, "Errorno = %d , Err info is %s\n", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
        //exit(1);
        }
    }


    return (sword)mysql_stmt_errno(stmt);
}


//bind the parameter by name 
sword   OCIBindByName   (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
                         const OraText *placeholder, sb4 placeh_len,
                         void  *valuep, sb4 value_sz, ub2 dty,
                         void  *indp, ub2 *alenp, ub2 *rcodep,
                         ub4 maxarr_len, ub4 *curelep, ub4 mode)
{
	
    memset(&bindByname[bindbnCnt], 0, sizeof(bindByname[bindbnCnt]));
    int bindIdx=0;

   //find the index through  name  in based on  parsesql 
   for (int i = 0; i < MAX_FIELDS; ++i) {
        if (strstr(stmtF[i].fields, placeholder) != NULL) {
            bindIdx = stmtF[i].indexno;
            break; // 找到后退出循环
        }
    }

    switch (dty) {
    case SQLT_INT:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_LONG;
        bindByname[bindbnCnt].buffer = (int *)valuep;
        bindByname[bindbnCnt].buffer_length = value_sz;
        break;
    case SQLT_FLT:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_FLOAT;
        bindByname[bindbnCnt].buffer = (float *)valuep;
        bindByname[bindbnCnt].buffer_length = value_sz;
        break;
    case SQLT_STR:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_STRING;
        bindByname[bindbnCnt].buffer = (char *)valuep;
        bindByname[bindbnCnt].buffer_length = value_sz;
        break;
    case SQLT_LNG:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_LONGLONG;
        break;
    case SQLT_DAT:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_DATE;
        break;
    case SQLT_TIMESTAMP:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_TIMESTAMP;
        break;
    case SQLT_BLOB:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_BLOB;
        break;
    case SQLT_CLOB:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_STRING; // CLOB在MySQL中通常作为长文本处理
        break;
    default:
        bindByname[bindbnCnt].buffer_type = MYSQL_TYPE_STRING; // 默认类型
        break;
    }

    //to know if all bind fields are done
    bindbnCnt++;
    printf("bindbnCnt=%d, bindFNUM = %d \n", bindbnCnt, bindFNUM);

    if (bindbnCnt == bindFNUM){
	printf("Call mysql_stmt_bind_param  when bindbnCnt=%d, bindFNUM = %d \n", bindbnCnt, bindFNUM);
	if (mysql_stmt_bind_param(stmt, bindByname)) {
        	fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        	fprintf(stderr, "Errorno = %d , Err info is %s\n", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
        //exit(1);
	}
    }
    
    
    return (sword)mysql_stmt_errno(stmt);
		
}


//define the resultset 
sword   OCIDefineByPos  (OCIStmt *stmtp, OCIDefine **defnp, OCIError *errhp,
                         ub4 position, void  *valuep, sb4 value_sz, ub2 dty,
                         void  *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode)
{
    memset(&bind, 0, sizeof(bind));

    switch (dty) {
    case SQLT_INT:
        bind.buffer_type = MYSQL_TYPE_LONG;
	bind.buffer = (char *)valuep;
	bind.buffer_length = value_sz;	
        break;
    case SQLT_FLT:
        bind.buffer_type = MYSQL_TYPE_FLOAT;
        break;
    case SQLT_STR:
        bind.buffer_type = MYSQL_TYPE_STRING;
	bind.buffer = (char *)valuep;
	bind.buffer_length = value_sz;	
        break;
    case SQLT_LNG:
        bind.buffer_type = MYSQL_TYPE_LONGLONG;
        break;
    case SQLT_DAT:
        bind.buffer_type = MYSQL_TYPE_DATE;
        break;
    case SQLT_TIMESTAMP:
        bind.buffer_type = MYSQL_TYPE_TIMESTAMP;
        break;
    case SQLT_BLOB:
        bind.buffer_type = MYSQL_TYPE_BLOB;
        break;
    case SQLT_CLOB:
        bind.buffer_type = MYSQL_TYPE_STRING; // CLOB在MySQL中通常作为长文本处理
        break;
    default:
        bind.buffer_type = MYSQL_TYPE_STRING; // 默认类型
        break;
    }
    
    

    //if (mysql_stmt_bind_param(stmt, &bind)) {
   if (mysql_stmt_bind_result(stmt, &bind)) { 
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        fprintf(stderr, "Errorno = %d , Err info is %s\n", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
        //exit(1);
    }
        return (sword)mysql_stmt_errno(stmt);
}


sword   OCIStmtExecute  (OCISvcCtx *svchp, OCIStmt *stmtp, OCIError *errhp,
                         ub4 iters, ub4 rowoff, const OCISnapshot *snap_in,
                         OCISnapshot *snap_out, ub4 mode)
{

    // 插入数据
    //deptno = 1000;
    //strcpy(dname, "ACCOUNTING");
    //strcpy(loc, "NEW YORK");
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
        //exit(1);
        return mysql_stmt_errno(stmt);
    }


        return (sword)0;
}


sword   OCIStmtFetch   (OCIStmt *stmtp, OCIError *errhp, ub4 nrows,
                        ub2 orientation, ub4 mode)
{
	sword fetchS=mysql_stmt_fetch(stmt) ;
	if (fetchS != 0) 
	
	{	
	    if (fetchS ==  MYSQL_NO_DATA) return OCI_NO_DATA;
	    else  
            {
		fprintf(stderr, "OCIStmtFetch() failed\n");
            	fprintf(stderr, "OCIStmtFetch() returncode is %d , Errorno = %d , Err info is %s\n", fetchS , mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
		return mysql_stmt_errno(stmt);
	    }
       }
        return (sword)fetchS;
}

sword   OCITransStart  (OCISvcCtx *svchp, OCIError *errhp,
                        uword timeout, ub4 flags )
{
        return (sword)0;
}

sword   OCITransDetach  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags )
{
        return (sword)0;
}

sword   OCITransCommit  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags)
{
        return (sword)0;
}

sword   OCITransRollback  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags)
{
        return (sword)0;
}

sword   OCIErrorGet   (void  *hndlp, ub4 recordno, OraText *sqlstate,
                       sb4 *errcodep, OraText *bufp, ub4 bufsiz, ub4 type)
{
        return (sword)0;
}


sword   OCILobRead  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                     ub4 *amtp, ub4 offset, void  *bufp, ub4 bufl, void  *ctxp,
                     OCICallbackLobRead cbfp, ub2 csid, ub1 csfrm)
{
        return (sword)0;
}


sword   OCILobWrite  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                      ub4 *amtp, ub4 offset, void  *bufp, ub4 buflen,
                      ub1 piece,  void  *ctxp, OCICallbackLobWrite cbfp,
                      ub2 csid, ub1 csfrm)
{
        return (sword)0;
}





/*
int main()
{
    // 1. 初始化连接环境
    MYSQL* mysql = mysql_init(NULL);
    if(mysql == NULL)\
    {
        printf("mysql_init() error\n");
        return -1;
    }

    // 2. 连接数据库服务器
    mysql = mysql_real_connect(mysql, "10.2.103.28", "root", "", 
                               "testly", 4000, NULL, 0);
    if(mysql == NULL)
    {
        printf("mysql_real_connect() error\n");
        return -1;
    }

    printf("mysql api使用的默认编码: %s\n", mysql_character_set_name(mysql));

    // 设置编码为utf8
    mysql_set_character_set(mysql, "utf8");

    printf("mysql api使用的修改之后的编码: %s\n", mysql_character_set_name(mysql));

    printf("恭喜, 连接数据库服务器成功了...\n");

    // 3. 执行一个sql语句
    // 查询scott数据库下的dept部门表
    const char* sql = "select * from dept";
    // 执行这个sql语句
    int ret = mysql_query(mysql, sql);
    if(ret != 0)
    {
        printf("mysql_query() a失败了, 原因: %s\n", mysql_error(mysql));
        return -1;
    }

    // 4. 取出结果集
    MYSQL_RES* res = mysql_store_result(mysql);
    if(res == NULL)
    {
        printf("mysql_store_result() 失败了, 原因: %s\n", mysql_error(mysql));
        return -1;
    }

    // 5. 得到结果集中的列数
    int num = mysql_num_fields(res);

    // 6. 得到所有列的名字, 并且输出
    MYSQL_FIELD * fields = mysql_fetch_fields(res);
    for(int i=0; i<num; ++i)
    {
        printf("%s\t\t", fields[i].name);
    }
    printf("\n");

    // 7. 遍历结果集中所有的行
    MYSQL_ROW row;
    while( (row = mysql_fetch_row(res)) != NULL)
    {
        // 将当前行中的每一列信息读出
        for(int i=0; i<num; ++i)
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
    mysql_autocommit(mysql, 0); 
    int ret1 = mysql_query(mysql, "insert into dept values(61, '海军', '圣地玛丽乔亚')");
    int ret2 = mysql_query(mysql, "insert into dept values(62, '七武海', '世界各地')");
    int ret3 = mysql_query(mysql, "insert into dept values(63, '四皇', '新世界')");
    printf("ret1 = %d, ret2 = %d, ret3 = %d\n", ret1, ret2, ret3);

    if(ret1==0 && ret2==0 && ret3==0)
    {
        // 提交事务
        mysql_commit(mysql);
    }
    else
    {
        mysql_rollback(mysql);
    }

    // 释放数据库资源
    mysql_close(mysql);
    
  return 0;
}
*/
