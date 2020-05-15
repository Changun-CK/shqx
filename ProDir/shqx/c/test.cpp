// 演示客户端socket
#include <_freecplus.h>

int main()
{
	CTcpClient TcpClient;
	if (TcpClient.ConnectToServer("39.108.127.31", 5050) == false) {
		printf("TcpClient.ConnectToServer(\"39.108.127.31\", 5050) failed.\n");
	}

	char str[10241];
	for (int ii = 0; ii < 5; ii++) {
		memset(str, 0, sizeof(str));
		sprintf(str, "这是第%d个报文.", ii + 1);
		if (TcpClient.Write(str) == false) continue;
		printf("发送: %s\n", str);

		memset(str, 0, sizeof(str));
		if (TcpClient.Read(str, 20) == false) break;
		printf("接收: %s\n", str);
		sleep(1);
	}

	return 0;
}
