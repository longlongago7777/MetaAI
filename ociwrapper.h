#include <stdio.h>
#include <mysql.h>
#include <oci.h>

//#include "oratypes.h"

//#define sword  signed int 


MYSQL *conn ;

sword   OCIInitialize   (ub4 mode, void  *ctxp,
                 void  *(*malocfp)(void  *ctxp, size_t size),
                 void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
                 void   (*mfreefp)(void  *ctxp, void  *memptr) );

sword   OCITerminate( ub4 mode);


sword   OCIEnvCreate (OCIEnv **envp, ub4 mode, void  *ctxp,
                 void  *(*malocfp)(void  *ctxp, size_t size),
                 void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
                 void   (*mfreefp)(void  *ctxp, void  *memptr),
                 size_t xtramem_sz, void  **usrmempp);

sword   OCIHandleAlloc(const void  *parenth, void  **hndlpp, const ub4 type,
                       const size_t xtramem_sz, void  **usrmempp);

sword   OCIHandleFree(void  *hndlp, const ub4 type);

sword   OCIDescriptorAlloc(const void  *parenth, void  **descpp,
                           const ub4 type, const size_t xtramem_sz,
                           void  **usrmempp);

sword   OCIEnvInit (OCIEnv **envp, ub4 mode,
                    size_t xtramem_sz, void  **usrmempp);

sword   OCIServerAttach  (OCIServer *srvhp, OCIError *errhp,
                          const OraText *dblink, sb4 dblink_len, ub4 mode);

sword   OCIServerDetach  (OCIServer *srvhp, OCIError *errhp, ub4 mode);

sword   OCISessionBegin  (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
                          ub4 credt, ub4 mode);

sword   OCISessionEnd   (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
                         ub4 mode);

sword   OCIStmtPrepare   (OCIStmt *stmtp, OCIError *errhp, const OraText *stmt,
                          ub4 stmt_len, ub4 language, ub4 mode);

sword   OCIBindByPos  (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
                       ub4 position, void  *valuep, sb4 value_sz,
                       ub2 dty, void  *indp, ub2 *alenp, ub2 *rcodep,
                       ub4 maxarr_len, ub4 *curelep, ub4 mode);


sword   OCIBindByName   (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
                         const OraText *placeholder, sb4 placeh_len,
                         void  *valuep, sb4 value_sz, ub2 dty,
                         void  *indp, ub2 *alenp, ub2 *rcodep,
                         ub4 maxarr_len, ub4 *curelep, ub4 mode);


sword   OCIStmtExecute  (OCISvcCtx *svchp, OCIStmt *stmtp, OCIError *errhp,
                         ub4 iters, ub4 rowoff, const OCISnapshot *snap_in,
                         OCISnapshot *snap_out, ub4 mode);

sword   OCIDefineByPos  (OCIStmt *stmtp, OCIDefine **defnp, OCIError *errhp,
                         ub4 position, void  *valuep, sb4 value_sz, ub2 dty,
                         void  *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode);

sword   OCIStmtFetch   (OCIStmt *stmtp, OCIError *errhp, ub4 nrows,
                        ub2 orientation, ub4 mode);

sword   OCITransStart  (OCISvcCtx *svchp, OCIError *errhp,
                        uword timeout, ub4 flags );

sword   OCITransDetach  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags );

sword   OCITransCommit  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

sword   OCITransRollback  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

sword   OCIErrorGet   (void  *hndlp, ub4 recordno, OraText *sqlstate,
                       sb4 *errcodep, OraText *bufp, ub4 bufsiz, ub4 type);

#define OCICallbackLobRead void* 
sword   OCILobRead  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                     ub4 *amtp, ub4 offset, void  *bufp, ub4 bufl, void  *ctxp,
                     OCICallbackLobRead cbfp, ub2 csid, ub1 csfrm);


#define OCICallbackLobWrite void*
sword   OCILobWrite  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                      ub4 *amtp, ub4 offset, void  *bufp, ub4 buflen,
                      ub1 piece,  void  *ctxp, OCICallbackLobWrite cbfp,
                      ub2 csid, ub1 csfrm);

sword   OCIAttrSet (void  *trgthndlp, ub4 trghndltyp, void  *attributep,
                    ub4 size, ub4 attrtype, OCIError *errhp);




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
