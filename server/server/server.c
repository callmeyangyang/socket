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

    // 创建服务器端socket
    SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == socketServer)
    {
        // socket()执行失败
        int error = WSAGetLastError();
        printf("socket failed and error code = %d.\n", error);

        // 清理网络库
        WSACleanup();
        return 0;
    }

    // 绑定具体的地址和端口号
    struct sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    si.sin_port = htons(12345);
    int iRes2 = bind(socketServer, (const struct sockaddr *)&si, sizeof(struct sockaddr_in));
    if (SOCKET_ERROR == iRes2)
    {
        int error = WSAGetLastError();
        printf("bind() failed and error code = %d.\n",error);

        closesocket(socketServer);
        WSACleanup();
        return 0;
    }

    // 启动服务端socket，接收客户端连接
    int iRes3 = listen(socketServer, SOMAXCONN);
    if (SOCKET_ERROR == iRes3)
    {
        int error = WSAGetLastError();
        printf("listen() failed and error code = %d.\n", error);

        closesocket(socketServer);
        WSACleanup();
        return 0;
    }

    // 得到连接服务器的客户端的socket
    struct sockaddr_in clientMsg;
    int iLen = sizeof(struct sockaddr_in);
    SOCKET socketClient = accept(socketServer, (struct sockaddr *)&clientMsg, &iLen);
    if (INVALID_SOCKET == socketClient)
    {
        int error = WSAGetLastError();
        printf("accept() failed and error code = %d.\n", error);
        printf("client link server failed.\n");

        closesocket(socketServer);
        WSACleanup();
        return 0;
    }
    printf("client link server success.\n"); 

    // 接收从客户端发送来的数据
    char buf[1500];
    int iRes4 = recv(socketClient, buf, 1499, 0);
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
        printf("%s,%d.\n", buf, iRes4);
    }
    
    // 给客户端发送数据
    int iRes5 = send(socketClient, "abcd", sizeof("abcd"), 0);
    if (SOCKET_ERROR == iRes5)
    {
        int error = WSAGetLastError();
        printf("send() failed and error code = %d.\n", error);
    }


    closesocket(socketClient);
    closesocket(socketServer);
    WSACleanup();

    system("pause");
    return  0;
}

