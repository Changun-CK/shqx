#ifndef _SHQX_H
#define _SHQX_H

#include "_public.h"
#include "_ooci.h"
//计算
//日数据结构
struct st_day
{
    char obtid[11];         // 区站代码
    char ddatetime[20];     // 观测时间
    int  r0808;          // 昨08-今08时累计雨量
    int  r2008;          // 昨20-今08时累计雨量
    int  r0820;          // 今08-今20时累计雨量
    int  r24h;           //日雨量 
    int maxr10m;        // 最大十分钟滑动雨量
    char maxr10mtime[20];   // 最大十分钟滑动雨量出现时间
    int maxr01h;        // 最大小时滑动雨量
    char maxr01htime[20];   //最大小时滑动雨量出现时间
    int maxt;          // 最高气温
    int maxttime;          // 最高气温出现时间
    int mint;          // 最低气温
    int minttime;         // 最低气温出现时间
    int avgt;         //平均气温
    int maxrh;      // 最大湿度
    char maxrhtime[20];      // 最大湿度出现时间
    int minrh;      // 最小湿度
    int minrhtime;      // 最小湿度出现时间
    int avgrh;         // 平均湿度
    int maxp0;      // 最高海面气压
    int maxp0time;      // 最高海面气压出现时间
    int minp0;      // 最低海平面气压
    int minp0time;      // 最低海平面气压出现时间
    int avgp0;         // 平均海面气压
    int maxp;      // 最高气压
    int maxptime;      // 最高气压出现时间
    int minp;      // 最低气压
    int minptime;      // 最低气压出现时间
    int avgp;         // 平均气压
    int wd2maxdf;        // 最大两分钟风速
    int wd2maxdd;        // 最大两分钟风速风向
    int wd2maxtime;        // 最大两分钟风速出现时间
    int avgwd2df;    // 平均两分钟风速
    int wd10maxdf;        // 最大十分钟风速
    int wd10maxdd;        // 最大十分钟风速风向
    int wd10maxtime;        // 最大十分钟风速出现时间
    int avgwd10df;    // 平均十分钟风速
    int mdcount;             // 分钟数据记录总数
    int hdconut;             // 整点数据记录总数
};

struct st_rain
{
    char ddatetime[20];          // 数据时间
    char obtid[11];         // 区站代码
    int minr;   //分钟雨量，单位：0.1mm，由本小时累计雨量计算出来的，用当前分钟的小时雨量减本小时上一分钟的小时雨量，如果本小时上一分钟的数据缺失，就用本小时上上一分钟的数据，如果本小时上上一分钟的数据缺失，就用本小时上上上一分钟的数据，采用真实值
    int r05m;     //05分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近05分钟的累计雨量。采用真实值
    char hdrbz[21]; //标志位
    int r10m; 
    int r30m; 
    int r01h; 
    int r02h; 
    int r03h; 
    int r06h; 
    int r12h; 
    int r24h; 
    int r48h; 
    int r72h; 
    int hourrf;   //小时雨量，单位：0.1mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。采用真实值
};
struct st_jisuan
{
    char ddatetime[20];          // 数据时间
    char obtid[11];         // 区站代码
    char station[3];        // 自动站编号，数值为0-99等。
    char baoformat[2];      // 资料类型
    char bps[7];            // 资料类型，0为正点报告，1 为瞬时报告。
    char ys[9];             // 自动站探测项目,标志自动站是否有某气象要素的探测，采用权位表示：0-没有，1-有，2-故障。
    char w3a[7];            // 自动站型号
    char wdidf[7];          // 瞬时风速，单位：0.1m/s。
    char wdidd[7];          // 瞬时风向，取值0到360。
    char wd2df[7];          // 二分钟风速，单位：0.1m/s。
    char wd2dd[7];          // 二分钟风向，取值0到360。
    char wd10df[7];         // 十分钟风速，单位：0.1m/s。
    char wd10dd[7];         // 十分钟风向，取值0到360。
    char wd3smaxdf[7];      // 极大风速，本小时内出现的最大瞬时风速值，单位：0.1m/s。
    char wd3smaxdd[7];      // 极大风向，极大风速的风向，取值0到360。
    char wd3smaxtime[3];    // 极大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char wd10maxdf[7];      // 最大风速，本小时内出现的最大10分钟平均风速值，单位：0.1m/s。
    char wd10maxdd[7];      // 最大风向，最大风速的风向，取值0到360。
    char wd10maxtime[3];    // 最大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char wd3daymax[7];      // 日瞬时极大风速时的风向，取值0到360。
    char wf3daymax[7];      // 日瞬时极大风速，单位：0.1m/s。
    char wd3daymaxtime[5];  // 日瞬时极大风速出现时间，用时分表示，如：2230为22时30分。
    char wd10daymax[7];     // 日十分钟最大风速时的风向。
    char wf10daymax[7];     // 日十分钟最大风速，单位：0.1m/s。
    char wd10daymaxtime[5]; // 日十分钟最大风速出现时间，用时分表示，如：2230为22时30分。
    char t[7];              // 空气温度，单位：0.1摄氏度。
    char maxt[7];           // 最高气温，本小时内的最高气温，单位：0.1摄氏度。
    char maxttime[3];       // 最高气温出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char mint[7];           // 最低气温，本小时内的最低气温，单位：0.1摄氏度。
    char minttime[3];       // 最低气温出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char daymaxt[7];        // 日最高温度，单位：0.1摄氏度。
    char daymaxttime[5];    // 日最高温度出现时间,用时分表示，用时分表示，如：2230为22时30分。
    char daymint[7];        // 日最低温度，单位：0.1摄氏度。
    char dayminttime[5];    // 日最低温度出现时间,用时分表示，用时分表示，如：2230为22时30分。
    char rh[7];             // 相对湿度，一个百分比值，取值1到100。
    char maxrh[7];          // 最大湿度，本小时内的最大湿度，一个百分比值，取值1到100。
    char maxrhtime[3];      // 最大湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char minrh[7];          // 最小湿度，本小时内的最小湿度，一个百分比值，取值1到100。
    char minrhtime[3];      // 最小湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char dp[7];             // 露点温度，单位：0.1摄氏度。
    char p[7];              // 本站气压，单位：0.1百帕。
    char maxp[7];           // 最高气压，本小时内的最高气压，单位：0.1百帕。
    char maxptime[3];       // 最高气压出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char minp[7];           // 最低气压，本小时内的最低气压，单位：0.1百帕。
    char minptime[3];       // 最低气压出现时间，整数，从0-59，只保留了时间中的分钟部分。
    char p0[7];             // 海平面气压，单位：百帕，海平台气压需要根据自动站安装的海拔高度参数计算，目前，深圳市自动站只有一部分站点有海拔高度参数>
    char hourrf[7];         // 小时雨量，单位：mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。
    char dayrf[7];          // 日雨量累计，日界为：08:00-08:00,即每天的08:00清零。
    char rfmark1[7];        // 当前一分钟雨量指示,没有说明。
    char minrf[51];         // 一小时的每五分钟的雨量分布。
    char wpv[7];            // 自动站电池电压，单位：0.01伏。
    char othfields[2001];   // 其它要素，采用XML表达。
    char rddatetime[3];      // 实际时间，当前记录采用的本小时内数据的实际时间，整数，从1-59、00，表示本小时的分钟部分加1和下一小时的00分钟，该字段不启
    char procsts[2];        // 处理状态，default 1，1-未处理，2-已处理。
};


// 全国气象站点参数数据结构
struct st_stcode
{
  char provname[31];   // 省名称
  char obtid[11];      // 站点代码
  char cityname[31];   // 城市名
  double lat;          // 纬度
  double lon;          // 经度
  double height;       // 海拔高度
};

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


//台风数据结构
struct st_typthon1
{ 
  char ddatetime[21];   // YYYY年, MM月, DD日，HH时(世界时);
  char h_level[2];      //强度标记
  double lat;          // 纬度
  double lon;          // 经度
  char pres[2];     //中心最低气压(hPa);
  int  wnd;     //2分钟平均近中心最大风速(MSW, m/s)
  int  owd;     //2分钟平均风速(m/s)
  int t_no;  //自定义台风编号，从第0个开始
};

struct st_typthon
{
  char cyclone[5];       //气旋序号
  char num[5];        //国内编号
  char ws[2];            //热带气旋终结标志
  char hours[2];         //每行路径间隔小时数, 目前均为6(小时);
  char t_name[30];         //热带气旋的英文名称, 名称后加 “(-1)n” 表示副中心及其序号;
  char datasettime[21];  //数据集形成的日期
};
//全国精确预报

struct st_forecast
{
   char product[6];                                   
   char foretime[20];                                  
};
struct st_forecast1
{
   char obtid[6];                                     
};
struct st_forecast2
{
   char sx[4];                                        
   char wd[6];                   
   char csd[6];            
   char wind[6];                  
   char speed[6];                 
   char pressure[6];              
   char p[6]; 
   char cloud[6];                 
   char lcloud[6];                
   char weather[6];               
   char vis[6];                   
   char ht[6];                    
   char lt[6];                    
   char maxsd[6];                 
   char minsd[6];                 
   char p2[6];                    
   char p1[6];                    
   char c2[6];                    
   char c1[6];                    
   char weather1[6];             
   char wind1[6];                 
   char speed1[6];                
};

struct st_adid
{
  char obtid[6];
  char lat[7];
  char lon[7];
  char height[7];
  char mode[2];
};

struct st_adid1
{
  char adidate[20];
  char astart[7];
  char aend[7];
  char r[6];
  char t[4];
  char ph1[5];
  char ph2[5];
  char ph3[5];
  char ph_av[5];
  char w[4];
  char s[4];
  char pheno[9];
};
struct st_visbility
{
  char obtid[6];
  char time[20];
};
struct st_visbility1
{
  char vtime[20];
  char vseeing[7];
  char max_vseeing[7];
  char max_vseetime[20];
  char min_vseeing[7];
  char min_vseetime[20];
  char vseeing10[7];
  char max_vseeing10[7];
  char max_vseetime10[20];
  char min_vseeing10[7];
  char min_vseetime10[20];
  char qcc[11];
};

struct st_autostation
{
  char obtid[6];
  char ddatetime[20];
  char lat[7];
  char lon[7];
  char height[6];
  char pheight[6];
  char mode[2];
  char qcc[4];
  char modify[4];

};
struct st_autostation1
{
  char p[6];
  char sp[6];
  char p3[5];
  char p24[5];
  char hp[6];
  char hpt[5];
  char lp[6];
  char lpt[5];
};

class CSURFDATA
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  struct st_surfdata m_stsurfdata;

  CSURFDATA(connection *conn,CLogFile *logfile);
 ~CSURFDATA();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;

  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把用逗号分隔的记录拆分到m_stsurfdata结构中。
  bool SplitBuffer(const char *strBuffer);

  // 把xml格式的记录拆分到m_stsurfdata结构中。
  bool SplitBuffer1(const char *strBuffer);

  // 把m_stsurfdata结构中的值更新到T_SURFDATA表中。
  long InsertTable();
};

struct st_signallog
{
  char obtid[11];
  char ddatetime[20];
  char signalname[2];
  char signalcolor[2];
};

class CSIGNALLOG
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  struct st_signallog m_stsignallog;
  vector<struct st_signallog> vsignallog;   // 容器存放一个文件的全部记录

  CSIGNALLOG(connection *conn,CLogFile *logfile);
 ~CSIGNALLOG();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;

  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把记录拆分到vsignallog容器中。
  bool SplitBuffer(const char *strBuffer);

  // 把vsignallog容器中的值更新到T_SIGNALDATA表中。
  long InsertTable();
};

class CTYPTHON
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  int c;//个数
  struct st_typthon m_sttypthon;
  struct st_typthon1 m_sttypthon1;
  vector<struct st_typthon> vtypthon;   // 容器存放一个文件的全部记录

  CTYPTHON(connection *conn,CLogFile *logfile);
 ~CTYPTHON();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;
  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把记录拆分到vsignallog容器中。
  bool SplitBuffer(const char *strBuffer);

  // 把vsignallog容器中的值更新到T_SIGNALDATA表中。
  long InsertTable();
};

class CFORECAST
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  struct st_forecast m_stforecast;
  struct st_forecast1 m_stforecast1;
  struct st_forecast2 m_stforecast2;

  CFORECAST(connection *conn,CLogFile *logfile);
 ~CFORECAST();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;
  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把记录拆分到vsignallog容器中。
  bool SplitBuffer(const char *strBuffer);

  // 把vsignallog容器中的值更新到T_SIGNALDATA表中。
  long InsertTable();
};



// 把非结构化数据文件写入oracle数据库的表中
int FileToTable(connection *in_conn,CLogFile *in_logfile,char *in_tname,char *in_filename,char *in_ddatetime);
//酸雨CADID
class CADID
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  int c;//个数
  struct st_adid m_stadid;
  struct st_adid1 m_stadid1;
  vector<struct st_adid> vadid;   // 容器存放一个文件的全部记录

  CADID(connection *conn,CLogFile *logfile);
 ~CADID();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;
  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把记录拆分到vsignallog容器中。
  bool SplitBuffer(const char *strBuffer);

  // 把vsignallog容器中的值更新到T_SIGNALDATA表中。
  long InsertTable();
};

class CVISIBILITY
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  int c;//个数
  struct st_visbility m_stvis;
  struct st_visbility1 m_stvis1;
  vector<struct st_typthon> vtypthon;   // 容器存放一个文件的全部记录

  CVISIBILITY(connection *conn,CLogFile *logfile);
 ~CVISIBILITY();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;
  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把记录拆分到vsignallog容器中。
  bool SplitBuffer(const char *strBuffer);

  // 把vsignallog容器中的值更新到T_SIGNALDATA表中。
  long InsertTable();
};


//自动站Z文件
class CAUTOSTATION
{
public:
  int totalcount,insertcount,updatecount,invalidcount;  // 记录总数据、插入数、更新数、无效记录数。
  int c;//个数
  struct st_autostation m_stauto;
  struct st_autostation1 m_stauto1;
  vector<struct st_adid> vadid;   // 容器存放一个文件的全部记录

  CAUTOSTATION(connection *conn,CLogFile *logfile);
 ~CAUTOSTATION();

  void initdata();  // 数据初始化

  connection *m_conn;
  CLogFile   *m_logfile;
  int iccount;
  sqlstatement stmtsel,stmtins,stmtupt;

  // 把记录拆分到vsignallog容器中。
  bool SplitBuffer(const char *strBuffer);

  // 把vsignallog容器中的值更新到T_SIGNALDATA表中。
  long InsertTable();
};
class Cjisuan
{
public:
    struct st_jisuan m_stjisuan;
    struct st_rain m_strain;
    struct st_day m_day;

    Cjisuan(connection *conn, CLogFile *logfile);
    ~Cjisuan();
    void initdata();

    vector<struct st_jisuan> vjisuan; //将分钟数据存入容器

    connection *m_conn;
    CLogFile   *m_logfile;
    /* 查询语句结果集 */
    int iccount;

    /* SQL语言数据操作类 */
    sqlstatement stmtsel, stmtins, stmtsel_n, stmtins_n, stmtupd_n, stmtins_d, stmtsel_rain, stmtsel_rain1, stmtsel_rain2, stmtsel_rain3;

    /* 更新容器 */
    bool selectforjs(const char *cols, const char *tname, const char *andstr=0);
    /* 计算整点表 */
    long thour(const char *cols, const char *tname, const char *andstr=0);

    /* 计算最新表 */
    long tnew(const char *cols, const char *tname, const char *andstr=0);

    /* 计算日数据表 */
    long tday(const char *cols, const char *tname, const char *andstr=0);
};
#endif
