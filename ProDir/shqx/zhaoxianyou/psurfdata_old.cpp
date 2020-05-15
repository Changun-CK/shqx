/*
 *  本程序用于处理全国气象站点观测的分钟数据，并保存到数据库的T_SURFDATA表中。
 *  这个是没有优化前的程序
 *  作者：码农有道 日期：20190905
*/
#include "_public.h"
#include "_ooci.h"

// 全国气象站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];      // 站点代码
  char ddatetime[21];  // 数据时间：格式yyyy-mm-dd hh:mi:ss。
  int  t;              // 气温：单位，0.1摄氏度
  int  p;              // 气压：0.1百帕
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm
  int  vis;            // 能见度：0.1米
};

CLogFile logfile;

CDir Dir;

// 处理数据文件
bool _psurfdata();

connection conn;

void EXIT(int sig);

int main(int argc,char *argv[])
{
  if (argc!=5)
  {
    printf("\n本程序用于处理全国气象站点观测的分钟数据，并保存到数据库的T_SURFDATA表中。\n");
    printf("/htidc/shqx/bin/psurfdata 数据文件存放的目录 日志文件名 数据库连接参数 程序运行时间间隔\n");
    printf("例如：/htidc/shqx/bin/psurfdata /data/shqx/sdata/surfdata /log/shqx/psurfdata.log shqx/pwdidc@snorcl11g_198 10\n");
    return -1;
  }

  // 关闭全部的信号和输入输出
  CloseIOAndSignal();

  // 处理程序退出的信号
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[2],"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv[2]); return -1;
  }

  logfile.Write("程序启动。\n");

  while (true)
  {
    // logfile.Write("开始扫描目录。\n");

    // 扫描数据文件存放的目录
    if (Dir.OpenDir(argv[1],"SURF_ZH_*.txt",1000,true,true)==false)
    {
      logfile.Write("Dir.OpenDir(%s) failed.\n",argv[1]); sleep(atoi(argv[4])); continue;
    }

    // 逐个处理目录中的数据文件
    while (true)
    {
      if (Dir.ReadDir()==false) break;
  
      if (conn.m_state==0)
      {
        if (conn.connecttodb(argv[3],"Simplified Chinese_China.ZHS16GBK")!=0)
        {
          logfile.Write("connect database(%s) failed.\n%s\n",argv[3],conn.m_cda.message); break;
        }
        // logfile.Write("连接数据库成功。\n");
      }
  
      logfile.Write("开始处理文件%s...",Dir.m_FileName);
  
      // 处理数据文件
      if (_psurfdata()==false) 
      {
        logfile.WriteEx("失败。\n"); break;
      }
      
      logfile.WriteEx("成功。\n");
    }

    // 断开与数据库的连接
    if (conn.m_state==1) conn.disconnect(); 

    sleep(atoi(argv[4]));
  }

  return 0;
}

void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n",sig);

  exit(0);
}
     

// 处理数据文件
bool _psurfdata()
{
  // 打开文件
  CFile File;

  if (File.Open(Dir.m_FullFileName,"r")==false)
  {
    logfile.Write("(File.Open(%s) failed.\n",Dir.m_FullFileName); return false;
  }

  // 读取文件中的每一行记录
  // 写入数据库的表中
  char strBuffer[301];
  CCmdStr CmdStr;
  struct st_surfdata stsurfdata;

  int iccount=0;
  sqlstatement stmtsel(&conn);
  stmtsel.prepare("select count(*) from T_SURFDATA where obtid=:1 and ddatetime=to_date(:2,'yyyy-mm-dd hh24:mi:ss')");
  stmtsel.bindin( 1, stsurfdata.obtid,5);
  stmtsel.bindin( 2, stsurfdata.ddatetime,19);
  stmtsel.bindout(1,&iccount);

  sqlstatement stmtins(&conn);
  stmtins.prepare("insert into T_SURFDATA(obtid,ddatetime,t,p,u,wd,wf,r,vis,crttime,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,:4,:5,:6,:7,:8,:9,sysdate,SEQ_SURFDATA.nextval)");
  stmtins.bindin( 1, stsurfdata.obtid,5);
  stmtins.bindin( 2, stsurfdata.ddatetime,19);
  stmtins.bindin( 3,&stsurfdata.t);
  stmtins.bindin( 4,&stsurfdata.p);
  stmtins.bindin( 5,&stsurfdata.u);
  stmtins.bindin( 6,&stsurfdata.wd);
  stmtins.bindin( 7,&stsurfdata.wf);
  stmtins.bindin( 8,&stsurfdata.r);
  stmtins.bindin( 9,&stsurfdata.vis);

  while (true)
  {
    memset(strBuffer,0,sizeof(strBuffer));

    if (File.Fgets(strBuffer,300,true)==false) break;

    // logfile.Write("%s\n",strBuffer);
    CmdStr.SplitToCmd(strBuffer,",",true);
    if (CmdStr.CmdCount()!=9)
    {
      logfile.Write("%s\n",strBuffer); continue;
    }

    memset(&stsurfdata,0,sizeof(struct st_surfdata));
    CmdStr.GetValue(0,stsurfdata.obtid,5);      // 站点代码
    CmdStr.GetValue(1,stsurfdata.ddatetime,19); // 数据时间：格式yyyy-mm-dd hh:mi:ss。
    double dtmp=0;
    CmdStr.GetValue(2,&dtmp); stsurfdata.t=(int)(dtmp*10);  // 气温：单位，0.1摄氏度
    CmdStr.GetValue(3,&dtmp); stsurfdata.p=(int)(dtmp*10);  // 气压：0.1百帕
    CmdStr.GetValue(4,&stsurfdata.u);  // 相对湿度，0-100之间的值。
    CmdStr.GetValue(5,&stsurfdata.wd); // 风向，0-360之间的值。
    CmdStr.GetValue(6,&dtmp); stsurfdata.wf=(int)(dtmp*10);  // 风速：单位0.1m/s
    CmdStr.GetValue(7,&dtmp); stsurfdata.r=(int)(dtmp*10);   // 降雨量：0.1mm
    CmdStr.GetValue(8,&dtmp); stsurfdata.vis=(int)(dtmp*10); // 能见度：0.1米

    if (stmtsel.execute() != 0)
    {
      logfile.Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
      if ( (stmtsel.m_cda.rc>=3113) && (stmtsel.m_cda.rc<=3115) ) return false;
      continue;
    }

    iccount=0;
    stmtsel.next();

    if (iccount>0) continue;

    // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtins.execute() != 0)
    {
      if (stmtins.m_cda.rc!=1)
      {
        logfile.Write("%s\n",strBuffer);
        logfile.Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message); 
        if ( (stmtins.m_cda.rc>=3113) && (stmtins.m_cda.rc<=3115) ) return false;
      }
    }
  }

  // 提交事务
  conn.commit();

  // 关闭文件指针，并删除文件
  File.CloseAndRemove();

  return true;
}


