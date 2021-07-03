/*
 * 本程序用于处理测试数据, 并保存到数据库的t_surfdata表中
 * 作者: 陈锟  日期: 2019-12-01
*/

#include "_ooci.h"
#include "_public.h"
#include "_shqx.h"

// 实例化日志文件类
CLogFile logfile;

// 实例化目录操作类
CDir Dir;

// oracle数据库连接池
connection conn;

// 处理数据文件
bool _psurfdata();

// 程序退出
void EXIT(int sig);

int main(int argc, char *argv[])
{
	if (argc!=5)
	{
		printf("\n本程序用于处理全国气象站点观测的分钟数据, 保存到数据库的t_surfdata表中.\n");
		printf("\n用法: /ProDir/shqx/bin/psurfdata 数据文件存放的目录 日志文件名 数据库连接参数 程序运行时间间隔\n");
		printf("\n例如: /ProDir/shqx/bin/psurfdata /data/shqx/sdata/surfdata /log/shqx/psurfdata.log shqx/pwdidc@snorcl11g_31 10\n\n");
		return -1;
	}

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[2], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[2]); return -1;
	}

	logfile.Write("程序启动.\n");

	while (1)
	{
		/* 扫描数据文件存放的目录 */
		if (Dir.OpenDir(argv[1], "SURF_ZH_*.txt", 100, false, true) == false)
		{
			logfile.Write("Dir.OpenDir(%s) failed.\n", argv[1]); sleep(atoi(argv[4])); continue;
		}

		/* 逐个处理目录中的数据文件 */
		while (1)
		{
			if (Dir.ReadDir() == false) break;

			if (conn.m_state == 0)
			{
				if (conn.connecttodb(argv[3], "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
				{
					logfile.Write("connect database(%s) failed.\n%s\n", argv[3], conn.m_cda.message); break;
				}
			}

			logfile.Write("开始处理文件 %s...", Dir.m_FullFileName);

			// 处理数据文件
			if (_psurfdata() == false)
			{
				logfile.WriteEx("处理文件失败\n"); break;
			}

			logfile.WriteEx("成功.\n");
		}

		// 断开与数据库的连接
		if (conn.m_state == 1) conn.disconnect();

		sleep(atoi(argv[4]));
	}

	return 0;
}

// 处理数据文件
bool _psurfdata()
{
	// 打开文件
	CFile File;
	if (File.Open(Dir.m_FullFileName, "r") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", Dir.m_FullFileName); return false;
	}

	// 读取文件中的每一行记录
	// 写入数据库的表中
	char strbuffer[301];
	CCmdStr Cmdstr;
	struct st_surfdata stsurfdata;

	int iccount = 0;
	sqlstatement stmtsel(&conn);
	stmtsel.prepare("select count(*) from t_surfdata where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
	stmtsel.bindin(1,  stsurfdata.obtid, 5);
	stmtsel.bindin(2,  stsurfdata.ddatetime, 19);
	stmtsel.bindout(1, &iccount);

	sqlstatement stmtins(&conn);
	stmtins.prepare("insert into t_surfdata(keyid, crttime, obtid, ddatetime, t, p, u, wd, wf, r, vis) values(SEQ_SURFDATA.nextval, sysdate, :1, to_date(:2, 'yyyy-mm-dd hh24:mi:ss'), :3, :4, :5, :6, :7, :8, :9)");
	stmtins.bindin(1,  stsurfdata.obtid, 5);
	stmtins.bindin(2,  stsurfdata.ddatetime, 19);
	stmtins.bindin(3, &stsurfdata.t);
	stmtins.bindin(4, &stsurfdata.p);
	stmtins.bindin(5, &stsurfdata.u);
	stmtins.bindin(6, &stsurfdata.wd);
	stmtins.bindin(7, &stsurfdata.wf);
	stmtins.bindin(8, &stsurfdata.r);
	stmtins.bindin(9, &stsurfdata.vis);

	while (1)
	{
		if (File.Fgets(strbuffer, sizeof(strbuffer)) == false) break;

		// logfile.Write("%s\n", strbuffer);
		Cmdstr.SplitToCmd(strbuffer, ",");
		if (Cmdstr.CmdCount() != 9)
		{
			logfile.Write("%s\n", strbuffer); continue;
		}

		double ttmp;

		memset(&stsurfdata, 0, sizeof(struct st_surfdata));
		Cmdstr.GetValue(0,  stsurfdata.obtid);						// 站点代码
		Cmdstr.GetValue(1,  stsurfdata.ddatetime);					// 数据时间: 格式yyyy-mm-dd hh:mi:ss
		Cmdstr.GetValue(2, &ttmp); stsurfdata.t   = (int)(ttmp*10); // 气温: 单位, 0.1摄氏度
		Cmdstr.GetValue(3, &ttmp); stsurfdata.p   = (int)(ttmp*10); // 气压: 单位: 0.1百帕
		Cmdstr.GetValue(4, &stsurfdata.u);							// 相对湿度, 0-100之间的值
		Cmdstr.GetValue(5, &stsurfdata.wd);							// 风向, 0-360之间的值
		Cmdstr.GetValue(6, &ttmp); stsurfdata.wf  = (int)(ttmp*10); // 风速: 单位: 0.1m/s
		Cmdstr.GetValue(7, &ttmp); stsurfdata.r   = (int)(ttmp*10); // 降雨量: 0.1mm
		Cmdstr.GetValue(8, &ttmp); stsurfdata.vis = (int)(ttmp*10); // 能见度: 0.1m

		/* 判断记录是否重复 */
		if (stmtsel.execute() != 0)
		{
			logfile.Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
			if ( (stmtsel.m_cda.rc>=3113) && (stmtsel.m_cda.rc<=3115) ) return false;
			continue;
		}

		iccount = 0;
		stmtsel.next();

		if (iccount > 0) continue;

		/* 执行SQL语句, 一定要判断返回值, 0 - 成功, 其它 - 失败 */
		if (stmtins.execute() != 0)
		{
			if (stmtins.m_cda.rc != 1) // 若数据在数据库已经存在(主键一致时), 则跳过该数据继续录入
			{
				logfile.Write("%s\n", strbuffer);
				logfile.Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
				if ( (stmtsel.m_cda.rc>=3113) && (stmtsel.m_cda.rc<=3115) ) return false;
			}
		}
	}

	File.Close();
	// 提交事务
	conn.commit();
	// 删除文件
	remove(Dir.m_FullFileName);

	return true;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n", sig);
	exit(0);
}
