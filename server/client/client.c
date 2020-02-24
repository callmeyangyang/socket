#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

int main(void)
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA lpWsaData;

    // 打开网络库
    int iRes1 = WSAStartup(wVersionRequested, &lpWsaData);
    // WSAStartup执行出错
    if (0 != iRes1)
    {
        switch (iRes1)
        {
        case WSASYSNOTREADY:
            printf("重启电脑.\n");
            break;
        case WSAVERNOTSUPPORTED:
            printf("版本不对.\n");
            break;
        case WSAEPROCLIM:
            printf("重启该软件.\n");
            break;
        case WSAEINPROGRESS:
            printf("关掉不用的软件.\n");
            break;
        case WSAEFAULT:
            printf("WSAStartup的第个参数填写错误.\n");
            break;
        }
        return 0;
    }

    // 版本校验
    if (2 != HIBYTE(lpWsaData.wVersion) || 2 != LOBYTE(lpWsaData.wVersion))
    {
        // 版本不对
        // 关闭网络库
        WSACleanup();
        return 0;   // 结束主函数
    }

    // 这里是客户端
    // 但创建的socket是服务器端socket
    SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == socketServer)
    {
        // socket()执行失败
        int error = WSAGetLastError();
        printf("socket() failed and error code = %d.\n", error);

        // 清理网络库
        WSACleanup();
        return 0;
    }

    // 连接服务器并把服务器信息与服务器SOCKET绑定到一起
    struct sockaddr_in serverMsg;
    serverMsg.sin_family = AF_INET;
    serverMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverMsg.sin_port = htons(12345);
    int iRes2 = connect(socketServer, (const struct sockaddr*)&serverMsg, sizeof(struct sockaddr_in));
    if (SOCKET_ERROR == iRes2)
    {
        int error = WSAGetLastError();
        printf("connect() failed and error code = %d.\n", error);

        closesocket(socketServer);
        WSACleanup();
        return 0;
    }
    
    while (1)
    {
        // 与服务器收发消息
        char buf[1500] = { 0 };
        int iRes4 = recv(socketServer, buf, 50, 0);
        if (0 == iRes4)
        {
            printf("链接中断，客户端下线.\n");
        }
        else if (SOCKET_ERROR == iRes4)
        {
            int error = WSAGetLastError();
            printf("recv() failed and error code = %d.\n", error);
            // 这里不需要关闭socket
            // 这里出错表示收发数据的过程中出错了

        }
        else
        {
            // 执行成功，接收到客户端发送过来的数据
            printf("%s, %d.\n", buf, iRes4);
        }

        // 给服务器发送数据
        scanf("%s", buf);
        int iRes5 = send(socketServer, buf, strlen(buf), 0);
        if (SOCKET_ERROR == iRes5)
        {
            int error = WSAGetLastError();
            printf("send() failed and error code = %d.\n", error);
        }
    }
      
    closesocket(socketServer);
    WSACleanup();

    system("pause");
    return  0;
}

