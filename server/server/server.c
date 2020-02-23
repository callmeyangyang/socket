#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

// 初始化
int init(); 

int main(void)
{
    // 初始化，打开网络库
    int iRes1 = init();
    if (0 != iRes1)
    {
        printf("打开网络库失败.\n");
    }
    

    system("pause");
    return  0;
}

int init()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA lpWsaData;
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
        return iRes1;
    }

    return iRes1;
}