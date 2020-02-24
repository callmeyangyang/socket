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
    
    closesocket(socketServer);
    WSACleanup();

    system("pause");
    return  0;
}

