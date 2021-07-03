/*
 *  程序名：inserttable.cpp，此程序演示freecplus框架操作PostgreSQL数据库（向表中插入5条记录）。
 *  作者：C语言技术网(www.freecplus.net) 日期：20190525
*/
#include "_postgresql.h"   // freecplus框架操作PostgreSQL的头文件。

// 定义用于超女信息的结构，与表中的字段对应。
struct st_girls
{
  long id;        // 超女编号，用long数据类型对应Oracle无小数的number(10)。
  char name[11];  // 超女姓名，用char[31]对应Oracle的varchar2(30)。
  double weight;  // 超女体重，用double数据类型对应Oracle有小数的number(8,2)。
  char btime[20]; // 报名时间，用char对应Oracle的date，格式：'yyyy-mm-dd hh24:mi:ssi'。
} stgirls;

int main(int argc,char *argv[])
{
  connection conn; // 数据库连接类

  // 登录数据库，返回值：0-成功，其它-失败。
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("host=39.108.127.31 user=postgres password=974753ck dbname=postgres port=5432","utf-8")!=0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  sqlstatement stmt(&conn); // 操作SQL语句的对象。

  // 准备插入表的SQL语句。
  stmt.prepare("\
    insert into girls(id,name,weight,btime) \
                values(:1,:2,:3,to_timestamp(:4,'yyyy-mm-dd hh24:mi:ss'))");
  // prepare方法不需要判断返回值。
  // 为SQL语句绑定输入变量的地址，bindin方法不需要判断返回值。
  stmt.bindin(1,&stgirls.id);
  stmt.bindin(2, stgirls.name,10);
  stmt.bindin(3,&stgirls.weight);
  stmt.bindin(4, stgirls.btime,19);

  // 模拟超女数据，向表中插入5条测试信息。
  for (int ii=1;ii<=5;ii++)
  {
    memset(&stgirls,0,sizeof(stgirls)); // 结构体变量初始化。

    // 为结构体变量的成员赋值。
    stgirls.id=ii;                                 // 超女编号。
    sprintf(stgirls.name,"超女%02d",ii);           // 超女姓名。
    stgirls.weight=ii*2.11;                        // 超女体重。
    strcpy(stgirls.btime,"2018-03-01 12:25:31");   // 报名时间。

    // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
    if (stmt.execute() != 0)
    {
      printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
    }

    printf("成功插入了%ld条记录。\n",stmt.m_cda.rpc); // stmt.m_cda.rpc是本次执行SQL影响的记录数。
  }

  printf("insert table girls ok.\n");

  conn.commit(); // 提交数据库事务。
}
