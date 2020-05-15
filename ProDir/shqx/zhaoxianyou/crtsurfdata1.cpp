/*
 *  本程序用于生成全国气象站点观测的分钟数据。
 *  与crtsurfdata程序不同，本程序生成的数据文件支持txt和xml两种格式。
 *  作者：码农有道 日期：20190901
*/
#include "_public.h"
#include "_shqx.h"

vector<struct st_stcode> vstcode;   // 存放全国站点参数的容器
vector<struct st_surfdata> vsurfdata;   // 存放全国气象站点分钟观测数据的容器

// 从站点参数文件中加载到vstcode容器中
bool LoadSTCode(const char *inifile);

// 创建全国气象站点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData();

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath,const char *datafmt);

CLogFile logfile;

void EXIT(int sig);

int main(int argc,char *argv[],char *envp[])
{
  if (argc!=5)
  {
    printf("\n本程序用于生成全国气象站点观测的分钟数据，与crtsurfdata程序不同，本程序生成的数据文件支持txt和xml两种格式。\n");
    printf("/htidc/shqx/bin/crtsurfdata1 站点参数 数据文件存放的目录 日志文件名 数据格式[txt|xml]\n");
    printf("例如：/htidc/shqx/bin/crtsurfdata1 /htidc/shqx/ini/stcode.ini /data/shqx/ftp/surfdata1 /log/shqx/crtsurfdata1.log txt\n");
    printf("      /htidc/shqx/bin/crtsurfdata1 /htidc/shqx/ini/stcode.ini /data/shqx/ftp/surfdata1 /log/shqx/crtsurfdata1.log xml\n");
    return -1;
  }

  // 关闭全部的信号和输入输出
  CloseIOAndSignal();

  // 处理程序退出的信号
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[3],"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv[3]); return -1;
  }

  while (true)
  {
    // 从站点参数文件中加载到vstcode容器中
    if (LoadSTCode(argv[1])==false) { sleep(60); continue; }

    logfile.Write("加载参数文件（%s）成功！\n",argv[1]);

    CrtSurfData();  // 创建全国气象站点分钟观测数据，存放在vsurfdata容器中

    // 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
    if (CrtSurfFile(argv[2],argv[4])==false) { sleep(60); continue; }

    sleep(60);
  }

  return 0;
}

// 从站点参数文件中加载到vstcode容器中
bool LoadSTCode(const char *inifile)
{
  vstcode.clear();

  CCmdStr CmdStr;
  struct st_stcode stcode;

  CFile File;

  if (File.Open(inifile,"r") == false)
  {
    logfile.Write("File.Open(%s) 失败。\n",inifile); return false;
  }

  char strbuffer[101];

  while (true)
  {
    memset(&stcode,0,sizeof(struct st_stcode));

    if (File.Fgets(strbuffer,100)==false) break;

    CmdStr.SplitToCmd(strbuffer,",",true);

    CmdStr.GetValue(0, stcode.provname);
    CmdStr.GetValue(1, stcode.obtid);
    CmdStr.GetValue(2, stcode.cityname);
    CmdStr.GetValue(3,&stcode.lat);
    CmdStr.GetValue(4,&stcode.lon);
    CmdStr.GetValue(5,&stcode.height);

    vstcode.push_back(stcode);
  }

  return true;
}

// 创建全国气象站点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData()
{
  vsurfdata.clear();  // 清空容器

  srand(time(0));     // 播随机数种子

  char strLocalTime[21];
  LocalTime(strLocalTime,"yyyy-mm-dd hh24:mi");
  strcat(strLocalTime,":00");

  struct st_surfdata stsurfdata;
  for (int ii=0;ii<vstcode.size();ii++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));
    STRCPY(stsurfdata.obtid,10,vstcode[ii].obtid);  // 站点代码

    STRCPY(stsurfdata.ddatetime,20,strLocalTime);   // 数据时间采用当前时间

    stsurfdata.t=rand()%351;       // 气温：单位，0.1摄氏度
    stsurfdata.p=rand()%265+10000; // 气压：0.1百帕
    stsurfdata.u=rand()%100+1;     // 相对湿度，0-100之间的值。
    stsurfdata.wd=rand()%360;      // 风向，0-360之间的值。
    stsurfdata.wf=rand()%150;      // 风速：单位0.1m/s
    stsurfdata.r=rand()%16;        // 降雨量：0.1mm
    stsurfdata.vis=rand()%5001+100000;  // 能见度：0.1米

    vsurfdata.push_back(stsurfdata);
  }
}

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath,const char *datafmt)
{
  CFile File;

  char strLocalTime[21];
  LocalTime(strLocalTime,"yyyymmddhh24miss");

  char strFileName[301];
  SNPRINTF(strFileName,300,"%s/SURF_ZH_%s_%d.%s",outpath,strLocalTime,getpid(),datafmt);
   
  if (File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.Open(%s) 失败!\n",strFileName); return false;
  }

  if (strcmp(datafmt,"xml")==0) File.Fprintf("<data>\n");

  for (int ii=0;ii<vsurfdata.size();ii++)
  {
    // 站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度
    if (strcmp(datafmt,"xml")==0)
    {
      File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p><u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    }
    else
    {
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    }
  }

  if (strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");

  File.CloseAndRename();   // 关闭文件

  logfile.Write("生成数据文件（%s）成功，数据时间=%s，记录数=%d！\n\n",strFileName,vsurfdata[0].ddatetime,vsurfdata.size());

  vstcode.clear(); vsurfdata.clear();

  return true;
}

void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n",sig);

  exit(0);
}
