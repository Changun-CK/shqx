// 演示服务端socket
#include <_freecplus.h>

int main()
{
	CTcpServer TcpServer;
	char str[10241];
	if (TcpServer.InitServer(5050) == false) {
		printf("TcpServer.InitServer(5050) falied.\n"); return -1;
	}

	if (TcpServer.Accept() == false) { printf("TcpServer.Accept() failed.\n"); return -1; }

	printf("client's IP is %d\n", TcpServer.GetIP());

	while (1) {
		memset(str, 0, sizeof(str));

		if (TcpServer.Read(str, 20) == false) break;
		printf("接收: %s\n", str);

		strcat(str, " ok.\n");

		if (TcpServer.Write(str) == false) break;
		printf("发送: %s\n");
	}
	printf("客户端已断开.\n");

	return 0;
}
