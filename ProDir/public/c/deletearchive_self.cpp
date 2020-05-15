/*
 * 本程序用于删除指定个数外的归档数据文件,保留近期文件, 参数可配置
 * 作者: 陈锟, 时间: 2020-03-15
 */
#include "_public.h"

/* 程序退出 */
void EXIT(int sig);

/* 显示程序帮助 */
void Help(char *argv[]);

/* 主函数 */
bool _deletearchive(const char *path, const int Countfile, const char *matchstr);

int main(int argc, char *argv[])
{
	if ( !((argc == 3) || (argc == 4)) ) { Help(argv); return -1; }

	/* 关闭全部的信号 */
	CloseIOAndSignal();

	/* 处理程序退出的信号 */
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	char PathName[201];
	char MatchStr[201];
	int  Dcountfiles = 0;

	memset(PathName, 0, sizeof(PathName));
	memset(MatchStr, 0, sizeof(MatchStr));

	strcpy(PathName, argv[1]);

	Dcountfiles = atoi(argv[2]);

	if (argc == 3) strcpy(MatchStr, "*");
	else strcpy(MatchStr, argv[3]);

	/* 主函数 */
	_deletearchive(PathName, Dcountfiles, MatchStr);

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
	printf("\n本程序用于删除指定个数外的归档数据文件,保留近期文件, 参数可配置.\n");
	printf("\nUsing:/ProDir/public/bin/deletearchive pathname countfiles [matchstr]\n");
	printf("\nSample:/ProDir/public/bin/deletearchive /oracle/archive1 2 *.log,*.dbf\n\n");
	printf("pathname:   待删除文件的目录.\n");
	printf("countfiles: 待删除文件的保留个数.\n");
	printf("[matchstr]: 待删除文件的匹配规则, 这是一个可选参数, 默认为所有文件.\n");
}

/* 主函数 */
bool _deletearchive(const char *path, const int Countfile, const char *matchstr)
{
	int icount = 0;
	CDir Dir;

	/* 扫描数据文件存放的目录, 包括子目录 */
	if (Dir.OpenDir(path, matchstr, 100000, false, true) == false) { printf("Dir.OpenDir(%s) failed.\n", path); return false; }
	if ( Dir.m_vFileName.size() < Countfile + 1 ) return true; // 如果文件个数本身小于最大文件数则不删除, 返回结果仍为真

	while (1)
	{
		/* 读取文件 */
		if (Dir.ReadDir() == false) break;
		icount++;
		if (icount <= Countfile) continue;

		remove(Dir.m_FullFileName);
	}

	return true;
}
