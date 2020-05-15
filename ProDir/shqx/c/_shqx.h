#ifndef _SHQX_H
#define _SHQX_H

#include "_public.h"
#include "_ooci.h"
#include "cJSON.h"

// 全国气象站点参数数据结构
struct st_code
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
	char ddatetime[21];  // 数据时间：格式yyyy-mm-dd hh:mi:ss
	int  t;              // 气温：单位，0.1摄氏度
	int  p;              // 气压：0.1百帕
	int  u;              // 相对湿度，0-100之间的值。
	int  wd;             // 风向，0-360之间的值。
	int  wf;             // 风速：单位0.1m/s
	int  r;              // 降雨量：0.1mm
	int  vis;            // 能见度：0.1米
};

// 分区预警发布日志数据结构
struct st_signallog
{
	char obtid[5];       // 街道代码, 预警分区分街道字母代码
	char ddatetime[21];  // 发布时间, 预警信号发布的时间, 北京时间
	char signalname[2];  // 信号名称，A－台风，B－暴雨，C－高温，D－寒冷，E－大雾，F－灰霾，H－道路结冰，I－冰雹，J－森林火险，Q－干旱，R－大风，P－雷电。
	char signalcolor[2]; // 信号颜色，1－白色，2－蓝色，3－黄色，4－橙色，5－红色，0－解除。
};

// 台风最佳路径数据结构, "*"注释的是头部文件
struct st_typhoonbest
{
	char cycid[5];       // *国内编号
	char ddatetime[21];  // 数据时间
	char innaid[5];      // *国际编号
	char egname[21];     // *英文名称
	char cycno[5];       // *气旋编号
	char classmk[6];     // *分类标志, 默认66666
	char fmtdate[20];    // *数据集成的日期
	int  pathno;         // *路径数据记录的行数
	int  intervals;      // *数据的间隔时间, 小时, 目前均为6小时
	int  endsign;        // *终结标志, 热带气旋终结标志:0表示消散, 1表示移出西太台风委员会的责任海区, 2表示合并, 3表示准静止。
	int  intensitysign;  // 强度标记, 以正点前2分钟至正点内的平均风速为准
	int  lat;            // 纬度, 0.1°N
	int  lon;            // 经度, 0.1°E
	int  pressure;       // 中心最低气压(hPa); 百帕
	int  wdmaxspeed;     // 2分钟平均近中心最大风速(MSW, m/s)，WND=9 表示 MSW < 10m/s,WND=0 为缺测
	char wdavespeed[4];  // 2分钟平均风速(m/s), 有两种情况:(a) 对登陆我国的热带气旋, 表示沿海大风的风速;(b) 热带气旋位于南海时表示距中心300－500km范围的最大风速, 0值为空.
};

// 云量观测数据结构, "*"的数据在数据库中为number型.
struct st_cloudobt
{
	char statno[7];      // 台站号
	char servtype[3];    // 服务类型，00~99。
	char devflag[5];     // 设备标识位，4位字母。
	char devid[4];       // 设备ID，000~999。
	char ddatetime[20];  // 观测时间，yyyy-mm-dd hh24:mi:ss。
	char framelabel[3];  // 帧标示，分钟数据
	char obfeatures[3];  // 观测要素变量数，00~99。
	char devstatus[3];   // 设备状态变量数，00~99。
	char fname1[6];      // 总云量（AL0B）。
	char fvalue1[5];     // *小数点后扩大10倍。
	char fname2[6];      // 低云量（AL1B）。
	char fvalue2[5];     // *小数点后扩大10倍。
	char fname3[6];      // 中云量（AL2B）。
	char fvalue3[5];     // *小数点后扩大10倍。
	char fname4[6];      // 高云量（AL3B）。
	char fvalue4[5];     // *小数点后扩大10倍。
	char fname5[6];      // 高云量（AL3B）。
	char fvalue5[5];     // *小数点后扩大10倍。
	char fname6[6];      // 高云量（AL3B）。
	char fvalue6[5];     // *小数点后扩大10倍。
	char fname7[6];      // 高云量（AL3B）。
	char fvalue7[5];     // *小数点后扩大10倍。
	char fname8[6];      // 高云量（AL3B）。
	char fvalue8[5];     // *小数点后扩大10倍。
	char qualitybit[11]; // 质量控制位。
	char sname1[6];      // 外接电源（ZDA）。
	char svalue1[2];     // *0交流，1直流
	char sname2[6];      // 蓄电池电压（ZD1A）。
	char svalue2[2];     // *0偏低，1正常。
	char sname3[6];      // 主采主板电压（ZD3A）。
	char svalue3[2];     // *0偏低，1正常，2偏高。
	char sname4[6];      // 遮阳板工作电压（ZD4A）。
	char svalue4[2];     // *0偏低，1正常，2偏高。
	char sname5[6];      // 旋转云台工作电压（ZD5A）。
	char svalue5[2];     // *0偏低，1正常，2偏高。
	char sname6[6];      // 双波段加热（ZD11A）。
	char svalue6[2];     // *0偏低，1正常，2偏高，3停止。
	char sname7[6];      // 双波段通风（ZD17A）。
	char svalue7[2];     // *0偏低，1正常，2偏高，3停止。
	char sname8[6];      // 双波段通风（ZD17A）。
	char svalue8[2];     // *0偏低，1正常，2偏高，3停止。
	char sname9[6];      // 双波段通风（ZD17A）。
	char svalue9[2];     // *0偏低，1正常，2偏高，3停止。
	char sname10[6];     // 双波段通风（ZD17A）。
	char svalue10[2];    // *0偏低，1正常，2偏高，3停止。
	char checkcode[5];   // 校验码。
};

// 全国城市精细化天气预报数据结构
struct st_refinedata1
{
	char prodcode[7];
	char ddatetime[20];
	char totalstat[5];
};

struct st_refinedata2
{
	char obtid[7];
	char lon[7];
	char lat[6];
	char height[7];
	char agings[3];
	char forprods[3];
};

struct st_refinedata3
{
	char timestep[4];
	char temperature[7];
	char rehumid[7];
	char wddirect[7];
	char wdspeed[7];
	char pressure[7];
	char precipit[7];
	char totalcloud[7];
	char lowcloud[7];
	char pweather[7];
	char visibility[7];
	char maxtemper[7];
	char mintemper[7];
	char maxrehumid[7];
	char minrehumid[7];
	char hcp24[7];
	char hcp12[7];
	char tcloud12[7];
	char lcloud12[7];
	char pweath12[7];
	char wddir12[7];
	char wdspeed12[7];
};

// 酸雨观测数据结构
struct st_acidobt
{
	char acidobt[6];
	char lat[8];
	char lon[9];
	char height[7];
	char obmethod[2];
	char ddatetime[20];
	char tbegin[20];
	char tend[20];
	char prept[6];
	char t[4];
	char PH1[5];
	char PH2[5];
	char PH3[5];
	char PHarv[5];
	char K1[6];
	char K2[6];
	char K3[6];
	char Karv[6];
	char ret[4];
	char rePH1[5];
	char rePH2[5];
	char rePH3[5];
	char rePHarv[5];
	char reK1[6];
	char reK2[6];
	char reK3[6];
	char reKarv[6];
	char wddir14[4];
	char wdsp14[4];
	char wddir20[4];
	char wdsp20[4];
	char wddir02[4];
	char wdsp02[4];
	char wddir08[4];
	char wdsp08[4];
	char wtph1[3];
	char wtph2[3];
	char wtph3[3];
	char wtph4[3];
	char acidnote[6];
};

// 自动站能见度数据结构
struct st_autostvis1
{
	char statid[6];
	char ddatetime[11];
};

struct st_autostvis2
{
	char hourminu[6];
	char minuvis[7];
	char minumaxvis[7];
	char minumaxvist[6];
	char minuminvis[7];
	char minuminvist[6];
	char minuvis10[7];
	char minumaxvis10[7];
	char minumaxvist10[6];
	char minuminvis10[7];
	char minuminvist10[6];
	char qccode[11];
};

//// 自动站数据(Z文件)数据结构
//struct st_surfzfiles
//{
//	char
//}

// 计算数据结构
struct st_quyujisuan
{
	char ddatetime[20];     // 数据时间
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
	char p0[7];             // 海平面气压，单位：百帕，海平台气压需要根据自动站安装的海拔高度参数计算，目前，深圳市自动站只有一部分站点有海拔高度参数，省港澳自动站还没有海拔高度参数。
	char hourrf[7];         // 小时雨量，单位：mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。
	char dayrf[7];          // 日雨量累计，日界为：08:00-08:00,即每天的08:00清零。
	char rfmark1[7];        // 当前一分钟雨量指示,没有说明。
	char minrf[51];         // 一小时的每五分钟的雨量分布。
	char wpv[7];            // 自动站电池电压，单位：0.01伏。
	char othfields[2001];   // 其它要素，采用XML表达。
	char rddatetime[3];     // 实际时间，当前记录采用的本小时内数据的实际时间，整数，从1-59、00，表示本小时的分钟部分加1和下一小时的00分钟，该字段不启用。
	char procsts[2];        // 处理状态，default 1，1-未处理，2-已处理。
};

// 区域自动站雨量数据结构
struct st_raind
{
	char ddatetime[20];     // 数据时间
	char obtid[11];         // 区站代码
	int  hourrf;            // 小时雨量，单位：0.1mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。采用真实值
	int  minr;              // 分钟雨量，单位：0.1mm，由本小时累计雨量计算出来的，用当前分钟的小时雨量减本小时上一分钟的小时雨量，如果本小时上一分钟的数据缺失，就用本小时上上一分钟的数据，如果本小时上上一分钟的数据缺失，就用本小时上上上一分钟的数据，采用真实值
	int  r05m;              // 5分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近05分钟的累计雨量。采用真实值
	int  r10m;              // 10分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近10分钟的累计雨量。采用真实值
	int  r30m;              // 30分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近30分钟的累计雨量。采用真实值
	int  r01h;              // 01小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近01小时的累计雨量。采用真实值
	int  r02h;              // 02小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近02小时的累计雨量。采用真实值
	int  r03h;              // 03小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示>该站点最近02小时的累计雨量。采用真实值
	int  r06h;              // 06小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示>该站点最近02小时的累计雨量。采用真实值
	int  r12h;              // 12小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示>该站点最近02小时的累计雨量。采用真实值
	int  r24h;              // 24小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示>该站点最近02小时的累计雨量。采用真实值
	int  r48h;              // 48小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示>该站点最近02小时的累计雨量。采用真实值
	int  r72h;              // 72小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示>该站点最近02小时的累计雨量。采用真实值
	char hdrbz[21];         // 滑动雨量标志，当数据缺失的时候，当前分钟的雨量和各时段滑动雨量的计算结果可能不准确，该字用于标记雨量计算结果是否准确，采用权位表达，一个权位表示一种雨量，1-准确，0-不准确，权位1-当前分钟雨量，权位2-06分钟滑动雨量，...权位12-72小时滑动雨量。
};

// 全国预警信号数据结构
struct st_country
{
	char province[21];     // 预警发布单位的省级名称
	char city[21];         // 预警发布单位的市级名称
	char county[21];       // 预警发布单位的县级名称
	char categoryid[5];    // 预警类别编号
	char categoryname[11]; // 预警类别名称
	char levelid[5];       // 预警级别编号
	char levelname[11];    // 预警级别名称
	char datetime[21];     // 预警发布时间
	char content[1001];    // 预警发布内容 
	char warsignalid[201]; // 预警ID
	char webhtml[201];     // 网页链接
};

class CSURFDATA
{
public:
	struct st_surfdata m_stsurfdata;

	CSURFDATA(connection *conn, CLogFile *logfile);
	~CSURFDATA();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded;
	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;
	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 分割csv字符串 */
	bool SplitBuffer(const char *strBuffer);
	/* 分割xml字符串 */
	bool SplitXmlstr(const char *strBuffer);
	/* 向表中插入数据 */
	long InsertTable();
};

class CSIGNALLOG
{
public:
	struct st_signallog m_stsignallog;
	vector<struct st_signallog> vsignallog;  // 容器存放一个文件的全部记录

	CSIGNALLOG(connection *conn, CLogFile *logfile);
	~CSIGNALLOG();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded;
	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;
	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 解析数据文件 */
	bool SplitBuffer(const char *strBuffer);
	/* 基于容器vsignallog向表中插入数据 */
	long InsertTable();
};

class CTYPHOONBEST
{
public:
	struct st_typhoonbest m_sttyphoonbest;

	CTYPHOONBEST(connection *conn, CLogFile *logfile);
	~CTYPHOONBEST();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded;
	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;
	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 解析数据文件 */
	bool SplithdBuffer(const char *strBuffer);
	bool SplitinBuffer(const char *strBuffer);
	/* 向表中插入数据 */
	long InsertTable();
};

class CCLOUDOBT
{
public:
	struct st_cloudobt m_stcloudobt;

	CCLOUDOBT(connection *conn, CLogFile *logfile);
	~CCLOUDOBT();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded;
	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;
	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 分割字符串 */
	bool SplitBuffer(const char *strBuffer);
	/* 向表中插入数据 */
	long InsertTable();
};

class CPREFINEDATA
{
public:
	/* 文件信息数据结构 */
	struct st_refinedata1 m_strefinedata1;
	struct st_refinedata2 m_strefinedata2;
	struct st_refinedata3 m_strefinedata3;

	CPREFINEDATA(connection *conn, CLogFile *logfile);
	~CPREFINEDATA();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded; 
	/* 查询语句结果集 */
	int iccount;

	connection *m_conn;
	CLogFile   *m_logfile;

	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 解析数据文件 */
	bool SplitBuffer(const char *strBuffer);
	/* 向表中插入数据 */
	long InsertTable();
};

class CACIDOBT
{
public:
	struct st_acidobt m_stacidobt;

	CACIDOBT(connection *conn, CLogFile *logfile);
	~CACIDOBT();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded;
	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;
	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 解析数据文件 */
	bool SplithdBuffer(const char *strBuffer);
	bool SplitinBuffer(const char *strBuffer);
	/* 基于容器vsignallog向表中插入数据 */
	long InsertTable();
};

class CAUTOSTVIS
{
public:
	struct st_autostvis1 m_stautostvis1;
	struct st_autostvis2 m_stautostvis2;

	CAUTOSTVIS(connection *conn, CLogFile *logfile);
	~CAUTOSTVIS();
	void initdata();

	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
	unsigned int m_total, m_inserted, m_updated, m_discarded;
	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;
	/* SQL语言数据操作类 */
	sqlstatement stmtsel, stmtins, stmtupd;

	/* 解析数据文件 */
	bool SplithdBuffer(const char *strBuffer);
	bool SplitinBuffer(const char *strBuffer);
	/* 向表中插入数据 */
	long InsertTable();
};

//class CSURFZFILES
//{
//public:
//	struct st_surfzfiles m_stsurfzfiles;
//
//	CSURFZFILES(connection *conn, CLogFile *logfile);
//	~CSURFZFILES();
//	void initdata();
//
//	/* 用于存放总记录数, 插入数, 更新数, 无效数 */
//	unsigned int m_total, m_inserted, m_updated, m_discarded;
//	connection *m_conn;
//	CLogFile   *m_logfile;
//	/* 查询语句结果集 */
//	int iccount;
//	/* SQL语言数据操作类 */
//	sqlstatement stmtsel, stmtins, stmtupd;
//
//	/* 解析数据文件 */
//	bool SplithdBuffer(const char *strBuffer);
//	bool SplitinBuffer(const char *strBuffer);
//	/* 向表中插入数据 */
//	long InsertTable();
//};

class Cquyujisuan
{
public:
	struct st_quyujisuan m_stquyujisuan;
	struct st_raind m_straind;

	Cquyujisuan(connection *conn, CLogFile *logfile);
	~Cquyujisuan();
	void initdata();

	vector<struct st_quyujisuan> vquyujisuan; //将分钟数据存入容器

	connection *m_conn;
	CLogFile   *m_logfile;
	/* 查询语句结果集 */
	int iccount;

	/* SQL语言数据操作类 */
	sqlstatement stmtsel_real, stmtins_hour, stmtins_real, stmtins_rain, stmtupd_real;

	/* 更新容器 */
	bool pstjisuan(const char *cols, const char *tname, const char *andstr=0);

	/* 计算整点表 */
	long phourd(const char *cols, const char *tname, const char *andstr=0);

	/* 计算最新表 */
	long preald(const char *cols, const char *tname, const char *andstr=0);

	/* 计算雨量表 */
	long praind();
};

// 把非结构化数据文件写入oracle数据库的表中
int FileToTable(connection *in_conn,CLogFile *in_logfile,char *in_tname,char *in_filename,char *in_ddatetime);

// 处理全国预警信号(JSON格式)
class CCOUNTRYSIGNAL
{
private:
    void initdata();
	struct st_country m_stcountry;

    /* 查询语句结果集 */
    int iccount;
	vector<struct st_country> m_vsig;

    /* SQL语言数据操作类 */
    sqlstatement stmtsel, stmtins, stmtupd;

    connection *m_conn;
    CLogFile   *m_logfile;

public:
    /* 用于存放总记录数, 插入数, 更新数, 无效数 */
    unsigned int m_total, m_inserted, m_updated, m_discarded;

    CCOUNTRYSIGNAL(connection *conn, CLogFile *logfile);
    ~CCOUNTRYSIGNAL();

    /* 解析数据文件 */
    bool SplitBuffer(const char *strBuffer);
    /* 向表中插入数据 */
    long InsertTable();
};

#endif
