/*
 * 本程序用于删除指定时间前的文件, 参数可配置
 * 作者: 陈锟, 时间: 2020-01-31
 */
#include "_freecplus.h"

/* 程序退出 */
void EXIT(int sig);

/* 显示程序帮助 */
void Help(char *argv[]);

/* 主函数 */
bool _deletefiles(const char *path, const char *OutTime, const char *matchstr);

int main(int argc, char *argv[])
{
	if ( !((argc == 3) || (argc == 4)) ) { Help(argv); return -1; }

	/* 关闭全部的信号 */
	//CloseIOAndSignal();

	/* 处理程序退出的信号 */
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	char PathName[201];
	char MatchStr[201];
	char outtime[201];
	double Ddayout = 0;

	memset(PathName, 0, sizeof(PathName));
	memset(outtime, 0, sizeof(outtime));
	memset(MatchStr, 0, sizeof(MatchStr));

	strcpy(PathName, argv[1]);

	Ddayout = atof(argv[2]);
	LocalTime(outtime, "yyyy-mm-dd hh24:mi:ss", 0 - (int)(Ddayout*24*60*60));

	if (argc == 3) strcpy(MatchStr, "*");
	else strcpy(MatchStr, argv[3]);

	/* 主函数 */
	_deletefiles(PathName, outtime, MatchStr);

	return 0;
}

/* 程序退出 */
void EXIT(int sig)
{
	printf("程序退出, sig = %d\n", sig);
	exit(0);
}

/* 显示程序帮助 */
void Help(char *argv[])
{
	printf("\n本程序用于删除指定时间前的文件, 参数可配置.\n");
	printf("\nUsing:/ProDir/public/bin/deletefiles pathname dayout [matchstr]\n");
	printf("\nSample:/ProDir/public/bin/deletefiles /data/shqx/ftp/surfdata 0.1 SURF_ZH*.TXT\n\n");
	printf("pathname:   待删除文件的目录.\n");
	printf("dayout:     待删除文件的保留天数, 可用小数.\n");
	printf("[matchstr]: 待删除文件的匹配规则, 这是一个可选参数, 默认为所有文件.\n\n");
}

/* 主函数 */
bool _deletefiles(const char *path, const char *OutTime, const char *matchstr)
{
	CDir Dir;

	/* 扫描数据文件存放的目录, 包括子目录 */
	if (Dir.OpenDir(path, matchstr, 100000, true) == false) { printf("Dir.OpenDir(%s) failed.\n", path); return false; }

	while (1)
	{
		/* 读取文件 */
		if (Dir.ReadDir() == false) break;
		/* 如果文件时间大于或等于指定时间, 则放弃删除
		 * 应注意此时等于指定时间我们仍保留该文件 */
		if (strcmp(Dir.m_ModifyTime, OutTime) >= 0) continue;
		remove(Dir.m_FullFileName);
		//printf("delete %s is ok\n", Dir.m_ModifyTime);
	}

	return true;
}
