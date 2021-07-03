#include "_public.h"
#include "_postgresql.h"   // oracle数据库是_ooci.h,mysql数据库换成_mysql.h,PostgreSQL数据库换成_postgresql.h

// 主程序的参数
struct st_arg
{
  char connstr[101];
  char charset[51];
  char tname[51];
  char cols[1001];
  char fieldname[1001];
  char fieldlen[501];
  int  exptype;
  char andstr[501];
  char bname[51];
  char ename[51];
  char idfieldname[51];
  char idfilename[301];
  char exppath[301];
  int  timetvl;
} starg;

CLogFile logfile;

connection conn;

// 本程序的业务流程主函数
bool _dmintables();

void EXIT(int sig);

vector<string> vfieldname; // 存放拆分fieldname后的容器
vector<int>    vfieldlen;  // 存放拆分fieldlen后的容器
int maxfieldlen;           // 存放fieldlen的最大值
void SplitFields();        // 拆分fieldname和fieldlen

// 显示程序的帮助
void _help(char *argv[]);

long maxkeyid;   // 已挖掘数据的最大的id
bool LoadMaxKeyid(); // 从记录已获取数据最大id的文件中加载已挖掘数据的最大的id
bool UptMaxKeyid();  // 更新已挖掘数据的最大的id到文件中
  
// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

int main(int argc,char *argv[])
{
  if (argc!=3) { _help(argv); return -1; }

  // 关闭全部的信号和输入输出
  CloseIOAndSignal();

  // 处理程序退出的信号
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[1],"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv[1]); return -1;
  }

  // 把xml解析到参数starg结构中
  if (_xmltoarg(argv[2])==false) return -1;

  while (true)
  {
    // 连接数据库
    if (conn.connecttodb(starg.connstr,starg.charset) != 0)
    {
      logfile.Write("connect database %s failed.\n",starg.connstr); sleep(starg.timetvl); continue;
    }

    // logfile.Write("export table %s.\n",starg.tname);

    // 挖掘数据的主函数
    if (_dmintables() == false) logfile.Write("export tables failed.\n");

    conn.disconnect();   // 断开与数据库的连接

    sleep(starg.timetvl);
  }

  return 0;
}

void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n",sig);

  exit(0);
}

// 显示程序的帮助
void _help(char *argv[])
{
  printf("\n");
  printf("Using:/htidc/public/bin/dminoracle logfilename xmlbuffer\n\n");

  printf("增量挖掘示例：\n");
  printf("Sample:/htidc/public/bin/dminoracle /log/shqx/dminoracle_surfdata_from_qx.log \"<connstr>shqx/pwdidc@snorcl11g_198</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_SURFDATA</tname><cols>obtid,to_char(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis</cols><fieldname>obtid,ddatetime,t,p,u,wd,wf,r,vis</fieldname><fieldlen>5,14,8,8,8,8,8,8,8</fieldlen><exptype>1</exptype><andstr> and obtid in ('59293','50745')</andstr><bname>SURFDATA_</bname><ename>_from_qx</ename><idfilename>/data/dmin/SURFDATA_from_qx.txt</idfilename><idfieldname>keyid</idfieldname><exppath>/data/shqx/sdata/fromqx</exppath><timetvl>30</timetvl>\"\n\n");
  printf("全量挖掘示例：\n");
  printf("Sample:/htidc/public/bin/dminoracle /log/shqx/dminoracle_obtcode_from_qx.log \"<connstr>shqx/pwdidc@snorcl11g_198</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_OBTCODE</tname><cols>obtid,obtname,provname,lat,lon,height</cols><fieldname>obtid,obtname,provname,lat,lon,height</fieldname><fieldlen>5,30,30,8,8,8</fieldlen><exptype>2</exptype><andstr> and rsts=1 and obtid in ('59293','50745')</andstr><bname>OBTCODE_</bname><ename>_from_qx</ename><exppath>/data/shqx/sdata/fromqx</exppath><timetvl>300</timetvl>\"\n\n");

  printf("本程序是数据中心的公共功能模块，从其它业务系统的数据库中挖掘数据，用于入库到数据中心。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件传输的参数，如下：\n");
  printf("数据库的连接参数 <connstr>shqx/pwdidc@snorcl11g_198</connstr>\n");
  printf("数据库的字符集 <charset>Simplified Chinese_China.ZHS16GBK</charset> 这个参数要与数据源数据库保持>一致，否则会出现中文乱码的情况。\n");
  printf("待挖掘数据的表名 <tname>T_SURFDATA</tname>\n");
  printf("需要挖掘字段的列表 <cols>obtid,to_char(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis</cols> 可以采用函数。\n");
  printf("挖掘字段的别名列表 <fieldname>obtid,ddatetime,t,p,u,wd,wf,r,vis</fieldname> 必须与cols一一对应。\n");
  printf("挖掘字段的长度列表 <fieldlen>5,14,8,8,8,8,8,8,8</fieldlen> 必须与cols一一对应。\n");
  printf("挖掘数据的方式 <exptype>1</exptype> 1-增量挖掘；2-全量挖掘，如果是增量挖掘，要求表一定要有表达记录序号的id字段。\n");
  printf("挖掘数据的附加条件 <andstr> and obtid in ('59293','50745')</andstr> 注意，关键字and不能少。\n");
  printf("数据文件的命名的前部分 <bname>SURFDATA_</bname>\n");
  printf("数据文件的命名的后部分 <ename>_from_qx</ename>\n");
  printf("挖掘数据表记录号字段名 <idfieldname>keyid</idfieldname> 当exptype=1时该参数有效。\n");
  printf("已挖掘数据id保存的文件名 <idfilename>/data/dmin/SURFDATA_from_qx.txt</idfilename> 当exptype=1时该参数有效。\n");
  printf("挖掘文件存放的目录 <exppath>/data/shqx/sdata/fromqx</exppath>\n");
  printf("挖掘数据的时间间隔 <timetvl>30</timetvl> 单位：秒，建议大于10。\n");
  printf("以上参数，除了idfieldname、idfilename和andstr，其它字段都不允许为空。\n\n\n");
}

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"connstr",starg.connstr);
  if (strlen(starg.connstr)==0) { logfile.Write("connstr is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"charset",starg.charset);
  if (strlen(starg.charset)==0) { logfile.Write("charset is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"tname",starg.tname);
  if (strlen(starg.tname)==0) { logfile.Write("tname is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"cols",starg.cols);
  if (strlen(starg.cols)==0) { logfile.Write("cols is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"fieldname",starg.fieldname);
  if (strlen(starg.fieldname)==0) { logfile.Write("fieldname is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"fieldlen",starg.fieldlen);
  if (strlen(starg.fieldlen)==0) { logfile.Write("fieldlen is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"exptype",&starg.exptype);
  if ( (starg.exptype!=1) && (starg.exptype!=2) ) { logfile.Write("exptype is not in (1,2).\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"andstr",starg.andstr);
  if (strlen(starg.andstr)==0) { logfile.Write("andstr is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"bname",starg.bname);
  if (strlen(starg.bname)==0) { logfile.Write("bname is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"ename",starg.ename);
  if (strlen(starg.ename)==0) { logfile.Write("ename is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"idfieldname",starg.idfieldname);
  if ( (starg.exptype==1) && (strlen(starg.idfieldname)==0) ) { logfile.Write("idfieldname is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"idfilename",starg.idfilename);
  if ( (starg.exptype==1) && (strlen(starg.idfilename)==0) ) { logfile.Write("idfilename is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"exppath",starg.exppath);
  if (strlen(starg.exppath)==0) { logfile.Write("exppath is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if (starg.timetvl==0) { logfile.Write("timetvl is null.\n"); return false; }

  // 拆分fieldname和fieldlen
  SplitFields();

  // 判断fieldname和fieldlen中元素的个数一定要相同
  if (vfieldname.size() != vfieldlen.size() ) { logfile.Write("fieldname和fieldlen的元素个数不同。.\n"); return false; }

  return true;
}

// 本程序的业务流程主函数
bool _dmintables()
{
  // 从记录已获取数据最大id的文件中加载已挖掘数据的最大的id
  if (LoadMaxKeyid()==false) { logfile.Write("LoadMaxKeyid() failed.\n"); return false; }

  // 生成挖掘数据的SQL语句
  char strsql[4096];   
  char fieldvalue[vfieldname.size()][maxfieldlen+1]; // 输出变量定义为一个二维数组
  memset(strsql,0,sizeof(strsql));
  if (starg.exptype==1)
    sprintf(strsql,"select %s,%s from %s where 1=1 and %s>%ld %s order by %s",starg.cols,starg.idfieldname,starg.tname,starg.idfieldname,maxkeyid,starg.andstr,starg.idfieldname);
  else
    sprintf(strsql,"select %s from %s where 1=1 %s",starg.cols,starg.tname,starg.andstr);
  sqlstatement stmt(&conn);
  stmt.prepare(strsql);
  for (int ii=0;ii<vfieldname.size();ii++)
  {
    stmt.bindout(ii+1,fieldvalue[ii],vfieldlen[ii]);
  }
  // 如果是增量挖掘，还要绑定id字段
  if (starg.exptype==1) stmt.bindout(vfieldname.size()+1,&maxkeyid);
  
  // 执行挖掘数据的SQL
  if (stmt.execute() != 0)
  {
    logfile.Write("select %s failed.\n%s\n%s\n",starg.tname,stmt.m_cda.message,stmt.m_sql); return false;
  }

  int  iFileSeq=1;   // 待生成文件的序号
  char strFileName[301],strLocalTime[21];
  CFile File;

  while (true)
  {
    memset(fieldvalue,0,sizeof(fieldvalue));
   
    if (stmt.next() !=0) break;

    // 把数据写入文件
    if (File.IsOpened()==false)
    {
      memset(strLocalTime,0,sizeof(strLocalTime));
      LocalTime(strLocalTime,"yyyymmddhh24miss");
      memset(strFileName,0,sizeof(strFileName));
      sprintf(strFileName,"%s/%s%s%s_%d.xml",starg.exppath,starg.bname,strLocalTime,starg.ename,iFileSeq++);
      if (File.OpenForRename(strFileName,"w")==false)
      {
        logfile.Write("File.OpenForRename(%s) failed.\n",strFileName); return false;
      }
      File.Fprintf("<data>\n");
    }

    for (int ii=0;ii<vfieldname.size();ii++)
    {
      File.Fprintf("<%s>%s</%s>",vfieldname[ii].c_str(),fieldvalue[ii],vfieldname[ii].c_str());
    }
    File.Fprintf("<endl/>\n");

    // 1000条记录写入一个文件完成
    if (stmt.m_cda.rpc%1000==0)
    {
      File.Fprintf("</data>\n");
      if (File.CloseAndRename()==false)
      {
        logfile.Write("File.CloseAndRename(%s) failed.\n",strFileName); return false;
      }

      // 更新已挖掘数据的最大的id到文件中
      if (UptMaxKeyid()==false) { logfile.Write("UptMaxKeyid() failed.\n"); return false; }

      logfile.Write("create file %s ok.\n",strFileName);
    }
  }

  // 不够1000条的写入一个文件
  if (File.IsOpened()==true)
  {
    File.Fprintf("</data>\n");
    if (File.CloseAndRename()==false)
    {
      logfile.Write("File.CloseAndRename(%s) failed.\n",strFileName); return false;
    }

    // 更新已挖掘数据的最大的id到文件中
    if (UptMaxKeyid()==false) { logfile.Write("UptMaxKeyid() failed.\n"); return false; }

    logfile.Write("create file %s ok.\n",strFileName);
  }

  if (stmt.m_cda.rpc>0) logfile.Write("本次挖掘了%d条记录。\n",stmt.m_cda.rpc);

  return true;
}

// 拆分fieldname和fieldlen
void SplitFields()
{
  vfieldname.clear(); vfieldlen.clear(); maxfieldlen=0;
  
  CCmdStr CmdStr;
  CmdStr.SplitToCmd(starg.fieldname,",");
  vfieldname.swap(CmdStr.m_vCmdStr);

  int ifieldlen=0;
  CmdStr.SplitToCmd(starg.fieldlen,",");
  for (int ii=0;ii<CmdStr.CmdCount();ii++)
  {  
    CmdStr.GetValue(ii,&ifieldlen);
    if (ifieldlen>maxfieldlen) maxfieldlen=ifieldlen;   // 得到fieldlen的最大值
    vfieldlen.push_back(ifieldlen);
  }
}

// 从记录已获取数据最大id的文件中加载已挖掘数据的最大的id
bool LoadMaxKeyid()
{
  if (starg.exptype!=1) return true;

  CFile File;

  if (File.Open(starg.idfilename,"r")==false) 
  {
    logfile.Write("注意，%s文件不存在，程序将从新开始挖掘数据。\n",starg.idfilename); return true;
  }

  char strBuf[21];
  memset(strBuf,0,sizeof(strBuf));

  File.Fread(strBuf,20);

  maxkeyid=atol(strBuf);

  logfile.Write("maxkeyid=%d\n",maxkeyid);

  return true;
}

// 更新已挖掘数据的最大的id到文件中
bool UptMaxKeyid()
{
  if (starg.exptype!=1) return true;

  CFile File;

  if (File.Open(starg.idfilename,"w")==false) 
  {
    logfile.Write("File.Open(%s) failed.\n",starg.idfilename); return false;
  }

  File.Fprintf("%ld",maxkeyid);

  return true;
}
