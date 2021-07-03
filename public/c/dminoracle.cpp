/*
 * 本程序是数据中心的公共功能模块，用于数据挖掘, 从oracle数据库的表中导出数据生成xml文件
 * 作者: 陈锟, 时间: 2020-03-21
 */
#include "_public.h"
#include "_ooci.h"

// 主程序的参数
struct st_arg
{
	char connstr[101];
	char charset[51];
	char tname[51];
	char cols[1001];
	char fieldname[1001];
	char fieldlen[501];
	int  digtype;
	char andstr[501];
	char bname[51];
	char ename[51];
	char qsfilename[51];
	char keyidname[51];
	char digpath[301];
	int  timetvl;
} starg;

CLogFile logfile;

connection conn;

// 本程序的业务流程主函数
bool _dminoracle();

vector<string> vfieldname;  // 存放拆分fieldname后的容器
vector<int>    vfieldlen;   // 存放拆分fieldlen后的容器
int maxfieldlen;            // 存放fieldlen的最大值
void SplitFields();         // 拆分fieldname和fieldlen

long maxkeyid;              // 已导出数据的最大keyid
bool LoadMaxKeyid();        // 从文件中加载已导出的数据的最大keyid
bool UptMaxKeyid();         // 更新文件中已导出数据的最大keyid

// 程序退出
void EXIT(int sig);

// 显示程序帮助
void Help(char *argv[]);

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer);

int main(int argc, char *argv[])
{
	if (argc != 3) { Help(argv); return -1; }

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[1]); return -1;
	}

	// 将main函数参数分解保存到starg结构体中
	if (xmltoarg(argv[2]) == false) return -1;

	while (1)
	{
		// 连接数据库
		if (conn.connecttodb(starg.connstr, starg.charset) != 0)
		{
			logfile.Write("connect database %s failed.\n", starg.connstr); sleep(starg.timetvl); continue;
		}

		// 本程序的业务流程主函数
		if (_dminoracle() == false) logfile.Write("Data mining failed.\n");

		conn.disconnect();

		sleep(starg.timetvl);
	}

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n", sig);
	exit(0);
}

// 本程序的业务流程主函数
bool _dminoracle()
{
	// 从文件中加载已导出数据的最大keyid
	if (LoadMaxKeyid() == false) { logfile.Write("LoadMaxKeyid() failed.\n"); return false; }

	// 生成导出数据的SQL语句
	char strsql[4096];
	char fieldvalue[vfieldname.size()][maxfieldlen + 1]; // 输出变量定义为一个二维数组
	memset(strsql, 0, sizeof(strsql));
	if (starg.digtype == 1)
		sprintf(strsql, "select %s, %s from %s where %s > %ld %s order by %s", starg.cols, starg.keyidname, starg.tname, starg.keyidname, maxkeyid, starg.andstr, starg.keyidname);

	if (starg.digtype == 2)
		sprintf(strsql, "select %s from %s where 1 = 1 %s", starg.cols, starg.tname, starg.andstr);

	sqlstatement stmtsel(&conn);
	stmtsel.prepare(strsql);
	for (int ii = 0; ii < vfieldname.size(); ii++)
	{
		stmtsel.bindout(ii + 1, fieldvalue[ii], vfieldlen[ii]);
	}
	// 如果是增量导出, 还需加上keyidname字段
	if (starg.digtype == 1) stmtsel.bindout(vfieldname.size() + 1, &maxkeyid);

	// 执行导出数据的SQL
	if (stmtsel.execute() != 0)
	{
		logfile.Write("select %s failed.\n%s\n%s\n", starg.tname,stmtsel.m_cda.message,stmtsel.m_sql); return false;
	}

	int iFileSeq = 1;  // 待生成文件的序号
	char strFileName[301], strLocalTime[21];
	CFile File;

	while (1)
	{
		memset(fieldvalue, 0, sizeof(fieldvalue));
		if (stmtsel.next() != 0) break;

		// 把数据写入文件
		if (File.IsOpened() == false)
		{
			// 时间
			memset(strLocalTime, 0, sizeof(strLocalTime));
			LocalTime(strLocalTime, "yyyymmddhhmiss");    // 取当前时间

			// 文件名
			memset(strFileName, 0, sizeof(strFileName));
			sprintf(strFileName, "%s/%s%s%s_%d.xml", starg.digpath, starg.bname, strLocalTime, starg.ename, iFileSeq++);

			// 打开或创建文件
			if (File.OpenForRename(strFileName, "w") == false)
			{
				logfile.Write("File.OpenForRename(%s) failed.\n",strFileName); return false;
			}

			File.Fprintf("<data>\n"); // 写入第一行
		}

		for (int ii = 0; ii < vfieldname.size(); ii++)
		{
			File.Fprintf("<%s>%s</%s>",vfieldname[ii].c_str(),fieldvalue[ii],vfieldname[ii].c_str());
		}

		File.Fprintf("<endl/>\n"); // 至此一行结束

		// 1000条记录写入一个文件完成
		if (stmtsel.m_cda.rpc % 1000 == 0)
		{
			File.Fprintf("</data>\n"); // 至此全文内容结束
			if (File.CloseAndRename() == false)
			{
				logfile.Write("File.CloseAndRename(%s) failed.\n",strFileName); return false;
			}

			// 更新文件中已导出数据的最大值keyid
			if (UptMaxKeyid() == false) { logfile.Write("UptMaxKeyid() failed.\n"); return false; }

			logfile.Write("create file %s ok.\n", strFileName);
		}
	}

	// 不足1000条的写入一个文件,注意,此时应该在while循环之外
	if (File.IsOpened() == true)
	{
		File.Fprintf("</data>\n"); // 若还是打开状态,说明有余下的文件,结束
		if (File.CloseAndRename() == false)
		{
			logfile.Write("File.CloseAndRename(%s) failed.\n",strFileName); return false;
		}

		// 更新文件中已导出数据的最大keyid
		if (UptMaxKeyid() == false) { logfile.Write("UptMaxKeyid() failed.\n"); return false; }

		logfile.Write("create file %s ok.\n",strFileName);
	}

	if (stmtsel.m_cda.rpc > 0) logfile.Write("本次导出了%d条记录。\n",stmtsel.m_cda.rpc);

	return true;
}

// 显示程序帮助
void Help(char *argv[])
{
	printf("\n本程序是数据中心的公共功能模块, 数据挖掘系统, 从oracle数据库中导出数据生成xml文件.\n");
	printf("\nUsing: /ProDir/public/bin/dminoracle logfilename xmlbuffer\n");

	printf("\n增量挖掘Sample: /ProDir/public/bin/dminoracle /log/shqx/dminoracle.log \"<connstr>shqx/pwdidc@snorcl11g_31</connstr><charset>SIMPLIFIED CHINESE_CHINA.AL32UTF8</charset><tname>T_SURFDATA</tname><cols>obtid, to_char(ddatetime, 'yyyymmddhh24miss'), t, p, u, wd, wf, r, vis</cols><fieldname>obtid, ddatetime, t, p, u, wd, wf, r, vis</fieldname><fieldlen>5, 14, 8, 8, 8, 8, 8, 8, 8</fieldlen><digtype>1</digtype><andstr>and obtid in ('59293' ,'50745')</andstr><bname>SURFDATA_</bname><ename>_datadig</ename><qsfilename>maxkeyid.dat</qsfilename><keyidname>keyid</keyidname><digpath>/data/shqx/dig/oracle</digpath><timetvl>30</timetvl>\"\n");

	printf("\n全量挖掘Sample: /ProDir/public/bin/dminoracle /log/shqx/dminoracle.log \"<connstr>shqx/pwdidc@snorcl11g_31</connstr><charset>SIMPLIFIED CHINESE_CHINA.AL32UTF8</charset><tname>T_OBTCODE</tname><cols>obtid, obtname, provname, lat, lon, height</cols><fieldname>obtid, obtname, provname, lat, lon, height</fieldname><fieldlen>5, 30, 30, 8, 8, 8</fieldlen><digtype>2</digtype><andstr>and obtid in ('59293' ,'50745')</andstr><bname>OBTCODE_</bname><ename>_datadig</ename><digpath>/data/shqx/dig/oracle</digpath><timetvl>300</timetvl>\"\n");

	printf("logfilename 程序运行的日志文件名.\n");
	printf("xmlbuffer 程序的传输方式, 如下:\n");
	printf("<connstr>shqx/pwdidc@snorcl11g_31</connstr> 数据库的连接参数.\n");
	printf("<charset>SIMPLIFIED CHINESE_CHINA.AL32UTF8</charset> 数据库的字符集, 这个参数要与数据源数据库保持一致, 否则会出现中文乱码.\n");
	printf("<tname>T_SURFDATA</tname> 待导出数据的表名.\n");
	printf("<cols>obtid, to_char(ddatetime, 'yyyymmddhh24miss'), t, p, u, wd, wf, r, vis</cols> 需要导出字段的列表, 可以采用函数.\n");
	printf("<fieldname>obtid, ddatetime, t, p, u, wd, wf, r, vis</fieldname> 导出字段的别名列表, 必须与cols一一对应.\n");
	printf("<fieldlen>5,14,8,8,8,8,8,8,8</fieldlen> 导出字段的长度列表, 必须与cols一一对应.\n");
	printf("<digtype>1</digtype> 导出数据的方式: 1-增量导出; 2-全量导出; 如果是增量导出, 要求表一定要有keyid字段.\n");
	printf("<andstr> and obtid in ('59293','50745')</andstr> 导出数据的附加条件, 注意, 关键字and不能缺.\n");
	printf("<bname>SURFDATA_</bname> 导出文件的命名的前部分.\n");
	printf("<ename>_for_hb</ename> 导出文件的命名的后部分.\n");
	printf("<qsfilename>obt.txt</qsfilename> 存放最大keyid的文件名, 当digtype==1时, 此参数才有意义.\n");
	printf("<keyidname>keyid</keyidname> keyid的名字, 当digtype==1时, 此参数才有意义.\n");
	printf("<digpath>/data/shqx/exp/tohb</digpath> 导出文件存放的目录.\n");
	printf("<timetvl>30</timetvl> 导出数据的时间间隔, 单位:秒, 建议大于10.\n\n");
	printf("以上参数，除了andstr，其它字段都不允许为空.\n\n");
}

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer)
{
	memset(&starg, 0, sizeof(struct st_arg));

	GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr);
	if (strlen(starg.connstr) == 0) { logfile.Write("connstr is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "charset", starg.charset);
	if (strlen(starg.charset) == 0) { logfile.Write("charset is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "tname", starg.tname);
	if (strlen(starg.tname) == 0) { logfile.Write("tname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "cols", starg.cols);
	if (strlen(starg.cols) == 0) { logfile.Write("cols is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "fieldname", starg.fieldname);
	if (strlen(starg.fieldname) == 0) { logfile.Write("fieldname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "fieldlen", starg.fieldlen);
	if (strlen(starg.fieldlen) == 0) { logfile.Write("fieldlen is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "digtype", &starg.digtype);
	if ( (starg.digtype != 1) && (starg.digtype != 2) ) { logfile.Write("digtype is not in (1, 2).\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "andstr", starg.andstr);

	GetXMLBuffer(strxmlbuffer, "bname", starg.bname);
	if (strlen(starg.bname) == 0) { logfile.Write("bname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "ename", starg.ename);
	if (strlen(starg.ename) == 0) { logfile.Write("ename is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "qsfilename", starg.qsfilename);
	if ( (starg.digtype == 1) && (strlen(starg.qsfilename) == 0) ) { logfile.Write("qsfilename is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "keyidname", starg.keyidname);
	if ( (starg.digtype == 1) && (strlen(starg.keyidname) == 0) ) { logfile.Write("keyidname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "digpath", starg.digpath);
	if (strlen(starg.digpath) == 0) { logfile.Write("digpath is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
	if (starg.timetvl == 0) { logfile.Write("timetvl is NULL.\n");  return false; }

	// 拆分fieldname和fieldlen
	SplitFields();

	// 判断fieldname和fieldlen中元素的个数是否相同
	if (vfieldname.size() != vfieldlen.size()) { logfile.Write("fieldname和fieldlen的元素个数不同."); return false; }

	return true;
}

// 拆分fieldname和fieldlen
void SplitFields()
{
	vfieldname.clear(); vfieldlen.clear(); maxfieldlen = 0;

	CCmdStr CmdStr;
	CmdStr.SplitToCmd(starg.fieldname, ",");
	vfieldname.swap(CmdStr.m_vCmdStr);

	int ifieldlen = 0;
	CmdStr.SplitToCmd(starg.fieldlen, ",");
	for (int ii = 0; ii < CmdStr.CmdCount(); ii++)
	{
		CmdStr.GetValue(ii, &ifieldlen);
		if (ifieldlen > maxfieldlen) maxfieldlen = ifieldlen;  // 得到fieldlen的最大值
		vfieldlen.push_back(ifieldlen);
	}
}

// 从文件中加载已导出数据的最大的keyid
bool LoadMaxKeyid()
{
	if (starg.digtype != 1) return true;

	CFile File;

	char strbuffer[30];
	memset(strbuffer, 0, sizeof(strbuffer));
	if (File.Open(starg.qsfilename, "r") == false)
	{ logfile.Write("注意，%s文件不存在，程序将从新开始挖掘数据。\n",starg.qsfilename); return true; }

	File.Fgets(strbuffer, sizeof(strbuffer), true);

	maxkeyid = atol(strbuffer);
	logfile.Write("maxkeyid = %ld\n", maxkeyid);
	File.Close();

	return true;
}

// 更新文件中已导出数据的最大keyid
bool UptMaxKeyid()
{
	if (starg.digtype != 1) return true;

	CFile File;
	if (File.Open(starg.qsfilename, "w") == false) { logfile.Write("File.Open(%s) failed.\n",starg.qsfilename); return false; }
	File.Fprintf("%ld", maxkeyid); // 将最大keyid写入文件
	File.Close();

	return true;
}
