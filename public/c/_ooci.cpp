// 如果是windows平台，要用以下行，否则会报以下错误
// _ooci.cpp(845) : fatal error C1010: unexpected end of file while looking for precompiled header. Did you forget to add '#include "stdafx.h"' to your source?
// #include "stdafx.h"

#include "_ooci.h"

/*
OCI_SUCCESS                0  // maps to SQL_SUCCESS of SAG CLI  函数执行成功
OCI_SUCCESS_WITH_INFO      1  // maps to SQL_SUCCESS_WITH_INFO   执行成功，但有诊断消息返回，
                              // 可能是警告信息，但是，在测试的时候，我还从未见
                              // 识到OCI_SUCCESS_WITH_INFO是怎么回事
OCI_RESERVED_FOR_INT_USE 200  // reserved 
OCI_NO_DATA              100  // maps to SQL_NO_DATA 函数执行完成，但没有其他数据 
OCI_ERROR                 -1  // maps to SQL_ERROR 函数执行错误 
OCI_INVALID_HANDLE        -2  // maps to SQL_INVALID_HANDLE 传递给函数的参数为无效句柄，
                              // 或传回的句柄无效 
OCI_NEED_DATA             99  // maps to SQL_NEED_DATA 需要应用程序提供运行时刻的数据
OCI_STILL_EXECUTING    -3123  // OCI would block error 服务环境建立在非阻塞模式，
                              // OCI函数调用正在执行中

OCI_CONTINUE          -24200  // Continue with the body of the OCI function 
                              // 说明回调函数需要OCI库恢复其正常的处理操作 
OCI_ROWCBK_DONE       -24201  // done with user row callback 
*/

int oci_init(LOGINENV *env)
{
  //初始化Oracle 环境变量
  int  oci_ret;

  oci_ret = OCIEnvCreate(&env->envhp,OCI_DEFAULT,NULL,NULL,NULL,NULL,0,NULL);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO ) 
  {
    oci_close(env); return -1;
  }

  return 0;
} 

int oci_close(LOGINENV *env)
{
  int oci_ret;

  // 释放Oracle 环境变量
  oci_ret=OCIHandleFree(env->envhp,OCI_HTYPE_ENV);

  env->envhp = 0;

  return oci_ret;
}

int oci_context_create(LOGINENV *env,OCI_CXT *cxt )
{
  // 创建数据库连接的上下文对象，连接服务器，认证并建立会话

  if (env->envhp == 0) return -1;

  int oci_ret;
    
  oci_ret = OCIHandleAlloc(env->envhp,(dvoid**)&cxt->errhp,OCI_HTYPE_ERROR,(size_t) 0,NULL);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO ) 
  {
    oci_context_close(cxt); return -1;
  }

  // 登录
  oci_ret = OCILogon(env->envhp,cxt->errhp,&cxt->svchp,(OraText*)env->user,strlen(env->user),
                     (OraText*)env->pass,strlen(env->pass),(OraText*)env->tnsname,strlen(env->tnsname));

  if( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    oci_context_close(cxt); return -1;
  }

  cxt->envhp = env->envhp;

  return 0;    
}

int oci_context_close(OCI_CXT *cxt)
{
  // 关闭数据库连接的上下文

  int oci_ret;
    
  oci_ret = OCILogoff(cxt->svchp,cxt->errhp);

  oci_ret = OCIHandleFree(cxt->svchp,OCI_HTYPE_SVCCTX);

  oci_ret = OCIHandleFree(cxt->errhp, OCI_HTYPE_ERROR);

  cxt->svchp=0;

  cxt->errhp=0;

  return oci_ret;
}

int oci_stmt_create(OCI_CXT *cxt,OCI_HANDLE *handle )
{
  //创建语句
  int  oci_ret;
    
  oci_ret = OCIHandleAlloc( cxt->envhp, (dvoid**)&handle->smthp, OCI_HTYPE_STMT,(size_t)0, NULL);

  if( oci_ret == OCI_SUCCESS || oci_ret == OCI_SUCCESS_WITH_INFO )
  {
    handle->svchp  = cxt->svchp;
    handle->errhp  = cxt->errhp;
    handle->envhp = cxt->envhp;

    oci_ret = OCI_SUCCESS;
  }

  return oci_ret;
}

int oci_stmt_close(OCI_HANDLE *handle)
{
  // 关闭语句
  int oci_ret=0;

  oci_ret = OCIHandleFree(handle->smthp,OCI_HTYPE_STMT);

  return oci_ret;
}

connection::connection()
{ 
  m_state = 0; 

  memset(&m_cxt,0,sizeof(OCI_CXT));
  memset(&m_env,0,sizeof(LOGINENV));

  memset(&m_cda,0,sizeof(m_cda));

  m_cda.rc=-1;
  strncpy(m_cda.message,"database not open.",128);

  // 数据库种类
  memset(m_dbtype,0,sizeof(m_dbtype));
  strcpy(m_dbtype,"oracle");
}

connection::~connection()
{
  disconnect();
}

// 从connstr中解析username,password,tnsname
// scott/tiger@snorgl11g_128
void connection::setdbopt(char *connstr)
{
  char strtemp[201];

  memset(strtemp,0,sizeof(strtemp));

  strcpy(strtemp,connstr);

  char *pos=0;

  // tnsname
  pos = strstr(strtemp,"@");
  if (pos > 0) 
  {
    strncpy(m_env.tnsname,pos+1,50); pos[0]=0;
  }

  // password
  pos = strstr(strtemp,"/");
  if (pos > 0) 
  {
    strncpy(m_env.pass,pos+1,30); pos[0]=0;
  }

  // username
  strncpy(m_env.user,strtemp,30); 
}


// 设置字符集，如果客户端的字符集与数据库的不一致，就会出现乱码。
void connection::character(char *charset)
{
  // UNIX平台是采用setenv函数
  setenv("NLS_LANG",charset,1);

  // windows是采用putenv，如下
  /*
  char str[100];
  memset(str,0,sizeof(str));
  _snprintf(str,50,"NLS_LANG=%s",charset);
  putenv(str);
  */
}

int connection::connecttodb(char *connstr,char *charset,int autocommitopt)
{
  // 如果已连接上数据库，就不再连接
  // 所以，如果想重连数据库，必须显示的调用disconnect()方法后才能重连
  if (m_state == 1) return 0;

  // 设置字符集
  character(charset);

  // 设置日期字段的输出格式
  
  // UNIX平台是采用setenv函数
  // setenv("NLS_DATE_FORMAT","yyyy-mm-dd hh24:mi:ss",1);
  
  // windows是采用putenv，如下
  // putenv("NLS_DATE_FORMAT=yyyy-mm-dd hh24:mi:ss");
  
  
  // 从connstr中解析username,password,tnsname
  setdbopt(connstr);

  memset(&m_cda,0,sizeof(m_cda));

  // 初始化环境
  int oci_ret = oci_init(&m_env);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    oci_close(&m_env); m_cda.rc=-1; strncpy(m_cda.message,"initialize oracle call interface failed.\n",128); return -1;
  }

  // 创建句柄，登录数据库
  oci_ret = oci_context_create(&m_env,&m_cxt);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    oci_close(&m_env); m_cda.rc=1017; strncpy(m_cda.message,"ORA-01017: invalid username/password,logon denied.\n",128); return -1;
  }

  m_state = 1;

  m_autocommitopt=autocommitopt;

  return 0;
}

int connection::disconnect()
{
  memset(&m_cda,0,sizeof(m_cda));

  if (m_state == 0) 
  { 
    m_cda.rc=-1; strncpy(m_cda.message,"database not open.",128); return -1;
  }

  rollback();

  oci_context_close(&m_cxt);

  oci_close(&m_env);

  m_state = 0;    

  return 0;
}

int connection::rollback()
{ 
  memset(&m_cda,0,sizeof(m_cda));

  if (m_state == 0) 
  { 
    m_cda.rc=-1; strncpy(m_cda.message,"database not open.",128); return -1;
  }

  int oci_ret = OCITransRollback( m_cxt.svchp, m_cxt.errhp, OCI_DEFAULT ); 

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    err_report(); return m_cda.rc;
  }

  return 0;    
}

int connection::commit()
{ 
  memset(&m_cda,0,sizeof(m_cda));

  if (m_state == 0) 
  { 
    m_cda.rc=-1; strncpy(m_cda.message,"database not open.",128); return -1;
  }

  int oci_ret = OCITransCommit( m_cxt.svchp, m_cxt.errhp, OCI_DEFAULT );

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    err_report(); return m_cda.rc;
  }

  return 0;
}

void connection::err_report()
{
  if (m_state == 0) 
  { 
    m_cda.rc=-1; strncpy(m_cda.message,"database not open.",128); return;
  }

  memset(&m_cda,0,sizeof(m_cda));

  m_cda.rc=-1;
  strncpy(m_cda.message,"call err_report failed.",128);

  if (m_cxt.errhp != NULL)
  {
    if (OCIErrorGet(m_cxt.errhp,1,NULL,&m_cda.rc,(OraText*)m_cda.message,sizeof(m_cda.message),OCI_HTYPE_ERROR) == OCI_NO_DATA)
    {
      // 如果获取不到错误信息，就返回正确的
      memset(&m_cda,0,sizeof(m_cda)); return;
    }
  }
}

sqlstatement::sqlstatement()
{
  initial();
}

void sqlstatement::initial()
{
  m_state=0; 

  memset(&m_handle,0,sizeof(m_handle));

  memset(&m_cda,0,sizeof(m_cda));

  memset(m_sql,0,sizeof(m_sql));

  m_cda.rc=-1;
  strncpy(m_cda.message,"sqlstatement not connect to connection.\n",128);
}

sqlstatement::sqlstatement(connection *conn)
{
  initial();

  connect(conn);
}

sqlstatement::~sqlstatement()
{
  disconnect();
}

int sqlstatement::connect(connection *conn)
{
  // 注意，一个sqlstatement在程序中只能连一个connection，不允许连多个connection
  // 所以，只要这个光标已打开，就不允许再次打开，直接返回成功
  if ( m_state == 1 ) return 0;

  memset(&m_cda,0,sizeof(m_cda));

  m_conn=conn;

  // 如果数据库连接类的指针为空，直接返回失败
  if (m_conn == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"database not open.\n",128); return -1;
  }

  // 如果数据库没有连接好，直接返回失败
  if (m_conn->m_state == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"database not open.\n",128); return -1;
  }
    
  m_cda.rc = oci_stmt_create(&m_conn->m_cxt,&m_handle);

  if ( m_cda.rc != OCI_SUCCESS && m_cda.rc != OCI_SUCCESS_WITH_INFO )
  {
    err_report(); return m_cda.rc;
  }

  m_state = 1;  // 光标成功打开

  m_autocommitopt=m_conn->m_autocommitopt;

  m_cda.rc = OCI_SUCCESS; 

  alloclob();

  return 0;
}

int sqlstatement::disconnect()
{
  if (m_state == 0) return 0;

  memset(&m_cda,0,sizeof(m_cda));

  freelob();

  m_cda.rc = oci_stmt_close(&m_handle);

  m_state=0;

  memset(&m_handle,0,sizeof(m_handle));

  memset(&m_cda,0,sizeof(m_cda));

  memset(m_sql,0,sizeof(m_sql));

  m_cda.rc=-1;
  strncpy(m_cda.message,"cursor not open.",128);

  return 0;
}

// 在新的OCI方法中，当SQL语句有错误时，OCIStmtPrepare返回的是0，不是失败
// 所以，程序员在程序中一般不必处理prepare的结果
int sqlstatement::prepare(const char *fmt,...)
{ 
  memset(&m_cda,0,sizeof(m_cda));

  if (m_state == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"cursor not open.\n",128); return -1;
  }

  memset(m_sql,0,sizeof(m_sql));

  va_list ap;
  va_start(ap,fmt);
  vsnprintf(m_sql,10000,fmt,ap);
  va_end(ap);

  int oci_ret = OCIStmtPrepare(m_handle.smthp,m_handle.errhp,(OraText*)m_sql,strlen(m_sql),OCI_NTV_SYNTAX,OCI_DEFAULT);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    err_report(); return m_cda.rc;
  }

  // 判断是否是查询语句，如果是，把m_sqltype设为0，其它语句设为1。
  m_sqltype=1;

  // 从待执行的SQL语句中截取15个字符，如果这15个字符中包括了“select”，就认为是查询语句
  char strtemp[16]; memset(strtemp,0,sizeof(strtemp)); strncpy(strtemp,m_sql,15);

  if ( (strstr(strtemp,"select") > 0) || (strstr(strtemp,"Select") > 0) || (strstr(strtemp,"SELECT") > 0) ) m_sqltype=0; 

  m_cda.rc = OCI_SUCCESS; 

  return 0;
}

int sqlstatement::bindin(unsigned int position,int *value)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, sizeof(int),
                        SQLT_INT, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);  
}

int sqlstatement::bindin(unsigned int position,long *value)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, sizeof(long),
                        SQLT_INT, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);  
}

int sqlstatement::bindin(unsigned int position,unsigned int *value)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp,m_handle.errhp,(ub4)position,value,sizeof(unsigned int),
                        SQLT_INT, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);  
}

int sqlstatement::bindin(unsigned int position,unsigned long *value)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp,m_handle.errhp,(ub4)position,value,sizeof(unsigned long),
                        SQLT_INT, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);
}

int sqlstatement::bindin(unsigned int position,char *value,unsigned int len)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp,m_handle.errhp,(ub4)position,value,len+1,
                        SQLT_STR, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);  
}

int sqlstatement::bindin(unsigned int position,float *value)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, sizeof(float),
                        SQLT_FLT, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);
}

int sqlstatement::bindin(unsigned int position,double *value)
{
  return OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, sizeof(double),
                        SQLT_FLT, NULL, NULL,NULL,0, NULL, OCI_DEFAULT);
}

int sqlstatement::bindout(unsigned int position,int *value)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, sizeof(int), 
                          SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindout(unsigned int position,long *value)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, sizeof(long), 
                          SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindout(unsigned int position,unsigned int *value)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, sizeof(unsigned int), 
                          SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT );
}
int sqlstatement::bindout(unsigned int position,unsigned long *value)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, sizeof(unsigned long), 
                          SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindout(unsigned int position,float *value)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, sizeof(float), 
                          SQLT_FLT, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindout(unsigned int position,double *value)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, sizeof(double), 
                          SQLT_FLT, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindout(unsigned int position,char *value,unsigned int len)
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, len+1, 
                          SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindblob()
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, 1,(dvoid *) &m_lob,-1, 
                          SQLT_BLOB, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::bindclob()
{
  return OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, 1,(dvoid *) &m_lob,-1, 
                          SQLT_CLOB, NULL, NULL, NULL, OCI_DEFAULT );
}

int sqlstatement::execute() 
{
  memset(&m_cda,0,sizeof(m_cda));

  if (m_state == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"cursor not open.\n",128); return -1;
  }

  ub4 mode=OCI_DEFAULT;

  if (m_sqltype==1 && m_autocommitopt==1) mode=OCI_COMMIT_ON_SUCCESS;

  int oci_ret = OCIStmtExecute(m_handle.svchp,m_handle.smthp,m_handle.errhp,m_sqltype,0,NULL,NULL,mode);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    // 发生了错误或查询没有结果
    err_report(); return m_cda.rc;
  }

  // 如果不是查询语句，就获取影响记录的行数
  if (m_sqltype == 1)
  {
    OCIAttrGet((CONST dvoid *)m_handle.smthp,OCI_HTYPE_STMT,(dvoid *)&m_cda.rpc, (ub4 *)0,
                OCI_ATTR_ROW_COUNT, m_handle.errhp);
  }

  return 0;
}

int sqlstatement::execute(const char *fmt,...) 
{
  char strtmpsql[10240];
  memset(strtmpsql,0,sizeof(strtmpsql));

  va_list ap;
  va_start(ap,fmt);
  vsnprintf(strtmpsql,10000,fmt,ap);
  va_end(ap);

  if (prepare(strtmpsql) != 0) return m_cda.rc;

  return execute();
}

int sqlstatement::next() 
{ 
  // 注意，在该函数中，不可随意用memset(&m_cda,0,sizeof(m_cda))，否则会清空m_cda.rpc的内容
  if (m_state == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"cursor not open.\n",128); return -1;
  }

  // 如果语句未执行成功，直接返回失败。
  if (m_cda.rc != 0) return m_cda.rc;

  // 判断是否是查询语句，如果不是，直接返回错误
  if (m_sqltype != 0)
  {
    m_cda.rc=-1; strncpy(m_cda.message,"no recordset found.\n",128); return -1;
  }

  int oci_ret = OCIStmtFetch(m_handle.smthp,m_handle.errhp,1,OCI_FETCH_NEXT,OCI_DEFAULT);

  if ( oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO )
  {
    err_report(); 

    // 只有当m_cda.rc不是1405和1406的时候，才返回错误，1405和1406不算错
    // 并且，返回错误的时候，不要清空了m_cda.rpc
    if (m_cda.rc != 1405 && m_cda.rc != 1406) 
    {
      // 如果返回代码不是0,1403,1405,1406，就表是next出现了其它异常
      // 必须关闭数据库连接，让程序错误退出。
      if ( (m_cda.rc!=0) && (m_cda.rc!=1403) && (m_cda.rc!=1405) && (m_cda.rc!=1406) ) m_conn->disconnect();

      return m_cda.rc;
    }
  }

  // 获取结果集成功

  // 如果返回的是1405或1406，就把它强置为0
  if (m_cda.rc == 1405 || m_cda.rc == 1406) m_cda.rc=0;

  // 获取影响记录的行数据
  OCIAttrGet((CONST dvoid *)m_handle.smthp,OCI_HTYPE_STMT,(dvoid *)&m_cda.rpc, (ub4 *)0,
              OCI_ATTR_ROW_COUNT, m_handle.errhp);

  return 0;
}

void sqlstatement::err_report()
{
  // 注意，在该函数中，不可随意用memset(&m_cda,0,sizeof(m_cda))，否则会清空m_cda.rpc的内容
  if (m_state == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"cursor not open.\n",128); return;
  }

  m_cda.rc=-1;
  strncpy(m_cda.message,"call sqlstatement::err_report() failed.\n",128);

  if (m_handle.errhp != NULL)
  {
    if (OCIErrorGet(m_handle.errhp,1,NULL,&m_cda.rc,(OraText*)m_cda.message,sizeof(m_cda.message),OCI_HTYPE_ERROR) == OCI_NO_DATA)
    {
      // 如果没有获取到任何错误信息，就返回正确的
      // 这里可以用memset清空m_cda，因为如果没有任何错误m_cda.rpc在next中会重新赋值
      m_cda.rc=0; memset(m_cda.message,0,sizeof(m_cda.message)); return;
    }
  }

  m_conn->err_report();
}

int sqlstatement::alloclob()
{
  return OCIDescriptorAlloc((dvoid *)m_handle.envhp,(dvoid **)&m_lob,(ub4)OCI_DTYPE_LOB,(size_t)0,(dvoid **)0);
}

void sqlstatement::freelob()
{
  OCIDescriptorFree((dvoid *)m_lob, (ub4)OCI_DTYPE_LOB);
}


ub4 file_length(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  return (ub4) (ftell(fp));
}

int  sqlstatement::filetolob(char *filename)
{
  FILE *fp=0;

  if ( (fp = fopen(filename,"rb")) == 0) 
  {
    m_cda.rc=-1; strncpy(m_cda.message,"fopen failed",128); return -1;
  }
  
  int iret = filetolob(fp);

  fclose(fp);

  return iret;
}

/* 操作CLOB和BLOB内容时，缓冲区的大小，一般不需要修改。 */
#define LOBMAXBUFLEN  10240

int sqlstatement::filetolob(FILE *fp)
{
  ub4   offset = 1;
  ub4   loblen = 0;
  ub1   bufp[LOBMAXBUFLEN+1];
  ub4   amtp;
  ub1   piece;
  sword retval;
  ub4   nbytes;
  ub4   remainder;

  ub4  filelen = file_length(fp);

  if (filelen == 0) return 0;

  amtp = filelen;

  remainder = filelen;

  (void) OCILobGetLength(m_handle.svchp, m_handle.errhp, m_lob, &loblen);

  (void) fseek(fp, 0, 0);

  if (filelen > LOBMAXBUFLEN)
  {
    nbytes = LOBMAXBUFLEN;
  }
  else
  {
    nbytes = filelen;
  }

  memset(bufp,0,sizeof(bufp));

  if (fread((void *)bufp, (size_t)nbytes, 1, fp) != 1)
  {
    m_cda.rc=-1; strncpy(m_cda.message,"fread failed",128); return -1;
  }

  remainder -= nbytes;

  if (remainder == 0)       
  {
    // exactly one piece in the file
    // Only one piece, no need for stream write
    if ( (retval = OCILobWrite(m_handle.svchp, m_handle.errhp, m_lob, &amtp, offset, (dvoid *) bufp,
                            (ub4) nbytes, OCI_ONE_PIECE, (dvoid *)0,
                            (sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0,
                            (ub2) 0, (ub1) SQLCS_IMPLICIT)) != OCI_SUCCESS)
    {
      err_report(); return m_cda.rc;
    }
  }
  else
  {
    // more than one piece
    if (OCILobWrite(m_handle.svchp, m_handle.errhp, m_lob, &amtp, offset, (dvoid *) bufp,
                    (ub4) LOBMAXBUFLEN, OCI_FIRST_PIECE, (dvoid *)0,
                    (sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0,
                    (ub2) 0, (ub1) SQLCS_IMPLICIT) != OCI_NEED_DATA)
    {
      err_report(); return m_cda.rc;
    }

    piece = OCI_NEXT_PIECE;

    do
    {
      if (remainder > LOBMAXBUFLEN)
      {
        nbytes = LOBMAXBUFLEN;
      }
      else
      {
        nbytes = remainder; piece = OCI_LAST_PIECE;
      }

       memset(bufp,0,sizeof(bufp));

      if (fread((void *)bufp, (size_t)nbytes, 1, fp) != 1)
      {
        m_cda.rc=-1; strncpy(m_cda.message,"fread failed",128); return -1;
      }

      retval = OCILobWrite(m_handle.svchp, m_handle.errhp, m_lob, &amtp, offset, (dvoid *) bufp,
                          (ub4) nbytes, piece, (dvoid *)0,
                          (sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0,
                          (ub2) 0, (ub1) SQLCS_IMPLICIT);
      remainder -= nbytes;

    } while (retval == OCI_NEED_DATA && !feof(fp));
  }

  if (retval != OCI_SUCCESS)
  {
    err_report(); return m_cda.rc;
  }

  (void) OCILobGetLength(m_handle.svchp, m_handle.errhp, m_lob, &loblen);

  return 0;
}

// 把LOB字段中的内容写入文件
int  sqlstatement::lobtofile(char *filename)
{
  FILE *fp=0;

  if ( (fp = fopen(filename,"wb")) == 0)
  {
    if ( (fp = fopen(filename,"wb")) == 0)
    {
      m_cda.rc=-1; strncpy(m_cda.message,"fopen failed",128); return -1;
    }
  }

  fseek(fp, 0, 0);

  int iret = lobtofile(fp);

  fclose(fp);

  // 如果文件在生成的过程中发生了错误，就删除该文件，因为它是一个不完整的文件
  if (iret != 0) remove(filename); 

  return iret;
}

int sqlstatement::lobtofile(FILE *fp)
{
  ub4   offset = 1;
  ub4   loblen = 0;
  ub1   bufp[LOBMAXBUFLEN+1];
  ub4   amtp = 0;
  sword retval;

  OCILobGetLength(m_handle.svchp, m_handle.errhp, m_lob, &loblen);

  if (loblen == 0) return 0;

  amtp = loblen;

  memset(bufp,0,sizeof(bufp));

  retval = OCILobRead(m_handle.svchp, m_handle.errhp, m_lob, &amtp, offset, (dvoid *) bufp,
                     (loblen < LOBMAXBUFLEN ? loblen : LOBMAXBUFLEN), (dvoid *)0,
                     (sb4 (*)(dvoid *, const dvoid *, ub4, ub1)) 0,
                     (ub2) 0, (ub1) SQLCS_IMPLICIT);

  switch (retval)
  {
    case OCI_SUCCESS:             /* only one piece */
      fwrite((void *)bufp, (size_t)amtp, 1, fp);
      break;

    case OCI_NEED_DATA:           /* there are 2 or more pieces */
      fwrite((void *)bufp, amtp, 1, fp); 

      while (1)
      {
        amtp = 0;
        memset(bufp,0,sizeof(bufp));

        retval = OCILobRead(m_handle.svchp, m_handle.errhp, m_lob, &amtp, offset, (dvoid *) bufp,
                           (ub4) LOBMAXBUFLEN, (dvoid *)0,
                           (sb4 (*)(dvoid *, const dvoid *, ub4, ub1)) 0,
                           (ub2) 0, (ub1) SQLCS_IMPLICIT);

        fwrite((void *)bufp, (size_t)amtp, 1, fp);

        if (retval != OCI_NEED_DATA) break;
      } 

      break;

    case OCI_ERROR:
      err_report(); 

      return m_cda.rc;
  }

  return 0;
}
