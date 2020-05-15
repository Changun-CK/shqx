#ifndef __OOCI_H
#define __OOCI_H

// C/C++库常用头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// oracle数据库接口库头文件
#include <oci.h>

// OCI登录环境
struct LOGINENV
{
  char user[32];     // 数据库的用户名
  char pass[32];     // 数据库的密码
  char tnsname[51];  // 数据库的tnsname，在ORACLE_HOME/network/admin/tnsnames.ora中配置

  OCIEnv *envhp; // 环境变量的句柄
};

// OCI上下文
struct OCI_CXT
{
  OCISvcCtx  *svchp;
  OCIError   *errhp;
  OCIEnv     *envhp;   // 环境变量的句柄
};

// 语句
struct OCI_HANDLE
{
  OCISvcCtx  *svchp;  // 服务器上下文的句柄引用context句柄
  OCIStmt    *smthp;

  OCIBind    *bindhp;
  OCIDefine  *defhp;

  OCIError   *errhp;  // 错误句柄引用context句柄

  OCIEnv     *envhp; // 环境变量的句柄 引用context句柄
};

// OCI执行的结果
struct CDA_DEF
{
  int      rc;             // 执行结果
  unsigned long rpc;       // 执行SQL语句后，影响记录的行数
  char     message[2048];  // 执行SQL语句如果失败，存放错误信息
};

int oci_init(LOGINENV *env);
int oci_close(LOGINENV *env); 
int oci_context_create(LOGINENV *env,OCI_CXT *cxt);
int oci_context_close(OCI_CXT *cxt);

int oci_stmt_create(OCI_CXT *cxt,OCI_HANDLE *handle);
int oci_stmt_close(OCI_HANDLE *handle);

// 数据库连接池类
class connection
{
private:
  // 服务器环境句柄
  LOGINENV m_env;

public:
  // 服务器上下文
  OCI_CXT m_cxt;

  // 连接状态，0-未连接，1-已连接
  int m_state;

  // 自动提交标志，0-关闭自动提交；1-开启自动提交
  int m_autocommitopt; 

  // 数据库种类，固定取值为oracle
  char m_dbtype[21];

  // 用于存放connection操作数据库的错误或最后一次执行SQL语句的结果
  CDA_DEF m_cda;

  connection();

 ~connection();

  // 设置字符集，如果客户端的字符集与数据库的不一致，就会出现乱码。
  // 本方法要在connecttodb()之前调用。
  void character(char *charset);

  // 连接数据库，connstr的格式为：username/password@tnsname，autocommitopt：0-关闭自动提交，1-启用自动提交
  int connecttodb(char *connstr,char *charset,int autocommitopt=0);

  // 从connstr中解析username,password,tnsname
  void setdbopt(char *connstr);

  // 断开与数据库的连接
  int  disconnect();

  // 提交事务
  int  commit(); 

  // 回滚事务
  int  rollback();

  // 获取错误信息
  void err_report();
};

// SQL语言数据操作类
class sqlstatement
{
public:
  // 与数据库连接池的状态，0-未绑定，1-已绑定
  int m_state;    

  // 语句句柄
  OCI_HANDLE m_handle;

  connection *m_conn;

  // SQL语句
  char m_sql[10240];

  // 执行结果
  CDA_DEF m_cda;

  int m_sqltype;  // 待执行的SQL语句的类型，0-查询语句；1-非查询语句

  // 自动提交标志，0-关闭自动提交；1-开启自动提交
  int m_autocommitopt; 

  sqlstatement();
  sqlstatement(connection *conn);

  void initial();

 ~sqlstatement();

  // 设定数据库连接池
  int  connect(connection *conn); 

  // 断开与数据库连接池的连接
  int  disconnect();

  // 分析SQL语句,支持存储过程,采用立刻分析的方式,即时返回分析结果
  int  prepare(const char *fmt,...);

  // 绑定输入变量的地址
  int  bindin(unsigned int position,int    *value);
  int  bindin(unsigned int position,long   *value);
  int  bindin(unsigned int position,unsigned int  *value);
  int  bindin(unsigned int position,unsigned long *value);
  int  bindin(unsigned int position,float *value);
  int  bindin(unsigned int position,double *value);
  int  bindin(unsigned int position,char   *value,unsigned int len);

  // 绑定输出变量的地址
  int  bindout(unsigned int position,int    *value);
  int  bindout(unsigned int position,long   *value);
  int  bindout(unsigned int position,unsigned int  *value);
  int  bindout(unsigned int position,unsigned long *value);
  int  bindout(unsigned int position,float *value);
  int  bindout(unsigned int position,double *value);
  int  bindout(unsigned int position,char   *value,unsigned int len);

  // 执行SQL语句
  int  execute();
  
  // 如果SQL语句不需要输入和输出变量，可以直接执行。
  int  execute(const char *fmt,...);

  // 取下一条记录,只有当SQL语句是查询语句时才有意义 
  int  next();

  // 错误报告
  void err_report();

  // 指向LOB字段的指针，在执行execute后，用bindblob或bindclob绑定，再用next获取指针
  OCILobLocator *m_lob;

  // 初始化lob指针，在sqlstatement::connect()中调用
  int  alloclob();
  
  // 释放lob指针，在sqlstatement::disconnect()中调用
  void freelob();

  // 绑定lob指针
  int  bindblob();
  int  bindclob();

  // 以下两个函数都是把文件中的内容写入LOB字段，第二个函数是被第一个函数调用的
  int  filetolob(char *filename);
  int  filetolob(FILE *fp);

  // 把LOB字段中的内容写入文件
  int  lobtofile(char *filename);
  int  lobtofile(FILE *fp);
};

/*
create or replace function to_null(in_value in varchar2) return varchar2
is
begin
  return in_value;
end;
/
*/

#endif 

