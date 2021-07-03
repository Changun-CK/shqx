/*
 * 本程序用于入库站点参数日出日落数据
 * 作者: 陈锟  日期: 2020-02-11
*/
#include "_ooci.h"
#include "_public.h"

CLogFile logfile;
connection conn;
/* 处理数据文件 */
bool _pobtsriseset(const char *inifilename);

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("\n本程序用于入库站点参数数据, 保存到数据库的t_obtcode表中.\n");
		printf("\n用法: /ProDir/shqx/bin/pobtsriseset 数据文件name 数据库连接参数.\n");
		printf("\n例如: /ProDir/shqx/bin/pobtsriseset /ProDir/shqx/ini/stcode.ini /log/shqx/obtsriseset.log shqx/pwdidc@snorcl11g_31\n\n");
		return -1;
	}

	logfile.Open(argv[2], "a+");

	if (conn.m_state == 0)
	{
		if (conn.connecttodb(argv[2], "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
		{
			printf("connect database(%s) failed.\n%s\n", argv[2], conn.m_cda.message); return -1;
		}
	}

	logfile.Write("程序启动.\n");

	/* 处理数据文件 */
	if (_pobtsriseset(argv[1]) == false)
	{
		printf("处理文件失败\n"); return -1;
	}
	printf("成功.\n");

	if (conn.m_state == 1) conn.disconnect();

	return 0;
}

bool _pobtsriseset(const char *inifilename)
{
	CFile File;
	CCmdStr CmdStr;

	if (File.Open(inifilename, "r") == false)
	{
		printf("File.Open(%s) failed.\n", inifilename); return false;
	}
	printf("开始处理文件 %s...", inifilename);

	char   strbuffer[201];
	char   obtid[6];
	char   obtname[31];
	char   provname[31];
	double lat = 0;
	double lon = 0;
	double height = 0;
	memset(strbuffer, 0, sizeof(strbuffer));
	memset(obtid, 0, sizeof(obtid));
	memset(obtname, 0, sizeof(obtname));
	memset(provname, 0, sizeof(provname));

	sqlstatement stmt(&conn);

	/* 读取文件中的每一行记录, 写入数据库的表中 */
	while (1)
	{
		if (File.Fgets(strbuffer, sizeof(strbuffer)) == false) break;

		CmdStr.SplitToCmd(strbuffer, ",");

		CmdStr.GetValue(0, provname);
		CmdStr.GetValue(1, obtid, 5);
		CmdStr.GetValue(2, obtname);
		CmdStr.GetValue(3, &lat);
		CmdStr.GetValue(4, &lon);
		CmdStr.GetValue(5, &height);

		stmt.prepare("insert into t_obtcode(provname, obtid, obtname, lat, lon, height, rsts) values(:1, :2, :3, :4, :5, :6, 1)");
		stmt.bindin(1,  provname, 30);
		stmt.bindin(2,  obtid, 5);
		stmt.bindin(3,  obtname, 30);
		stmt.bindin(4, &lat);
		stmt.bindin(5, &lon);
		stmt.bindin(6, &height);

		if (stmt.execute() != 0)
    	{
        	printf("stmtsel.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
        	return false;
    	}

		//if (stmt.m_cda.rc != 0) printf("%s\n", strbuffer);
		//if ( (stmt.m_cda.rc>=3113) && (stmt.m_cda.rc<=3115) ) return false;
	}

	conn.commit();
	File.Close();

	return true;
}
