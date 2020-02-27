#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

fd_set allSockets;

BOOL WINAPI fun(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
        case CTRL_CLOSE_EVENT:
        {
            for (u_int i = 0; i < allSockets.fd_count; i++)
            {
                closesocket(allSockets.fd_array[i]);
            }
            WSACleanup();
        }
    }

    return TRUE;
}

int main(void)
{
    SetConsoleCtrlHandler(fun, TRUE);

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
        printf("bind() failed and error code = %d.\n", error);

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

    // 定义一个装客户端的数据结构
    // fd_set allSockets;

    //FD_ZERO(&allSockets);                // 清零
    //FD_SET(socketServer, &allSockets);   // 向集合中添加一个socket

    //FD_CLR(socketServer, &allSockets);   // 删除集合中指定的socket
    //closesocket(socketServer);

    //FD_ISSET(socketServer, &allSockets); // 判断一个socket是否在集合中

    // 清零
    FD_ZERO(&allSockets);                
    // 将服务器socket装进fd_set集合中
    FD_SET(socketServer, &allSockets);

    while (1)
    {
        // 中间集合fd_set变量
        fd_set readSockets = allSockets;

        fd_set writeSockets = allSockets;
        FD_CLR(socketServer, &writeSockets);    // 可以有也可以没有

        fd_set errorSockets = allSockets;       // 参数4

        struct timeval st;
        st.tv_sec = 3;
        st.tv_usec = 0;
        int iRes4 = select(0, &readSockets, &writeSockets, &errorSockets, &st);
        if (0 == iRes4)
        {
            // 客户端在等待时间内没有反应
            continue;
        }
        else if (iRes4 > 0)
        {
            // 处理select()参数4
            for (u_int i = 0; i < errorSockets.fd_count; i++)
            {
                char strBuf[100] = { 0 };
                int len = 99;
                if (SOCKET_ERROR == getsockopt(errorSockets.fd_array[i], SOL_SOCKET, SO_ERROR, strBuf, &len))
                {
                    printf("无法得到异常socket.\n");
                }
                printf("异常socket：%s.\n",strBuf);
            }

            // 处理select()参数3
            for (u_int i = 0; i < writeSockets.fd_count; i++)
            {
                // 此时writeSockets中装的有服务器socket和客户端socket
                // printf("服务器%d , %d：可写.\n", socketServer, writeSockets.fd_array[i]);

                int sendRes = send(writeSockets.fd_array[i], "ok", 2, 0);
                if (SOCKET_ERROR == sendRes)
                {
                    // 客户端正常退出，send()也返回SOCKET_ERROR
                    int error = WSAGetLastError();
                    printf("send() failed and error code = %d.\n", error);
                }
            }

            // 有客户端请求交流
            // 遍历有响应的socket
            for (u_int i = 0; i < readSockets.fd_count; i++)
            {
                // 取出有响应的socket
                // 判断取出的socket是服务器socket还是客户端socket
                if (readSockets.fd_array[i] == socketServer)
                {
                    SOCKET socketClient = accept(socketServer, NULL, NULL);
                    if (INVALID_SOCKET == socketClient)
                    {
                        continue;
                    }
                    
                    // 将连接服务器的客户端socket装进集合中fd_set
                    FD_SET(socketClient, &allSockets);

                    // send()给客户端发送一条消息：“服务器收到客户端的消息，连接成功”
                    /*int iRes6 = send(socketClient, "我是服务器，我收到了你的消息", sizeof("我是服务器，我收到了你的消息"), 0);
                    if (SOCKET_ERROR == iRes6)
                    {
                        int error = WSAGetLastError();
                        printf("send() failed and error code = %d.\n", error);
                    }*/
                }
                else
                {
                    // 取出的socket是客户端socket
                    char strBuf[1500] = { 0 };
                    int iRes5 = recv(readSockets.fd_array[i], strBuf, 1500, 0);
                    if (0 == iRes5)
                    {
                        // 客户端下线【客户端正常退出】
                        // 从集合fd_set中去除下线的客户端socket
                        SOCKET socketTemp = readSockets.fd_array[i];
                        FD_CLR(readSockets.fd_array[i], &allSockets);
                        // 释放下线的客户端socket
                        closesocket(socketTemp);
                    }
                    else if (iRes5 > 0)
                    {
                        // 成功接收到了消息
                        // 打印接收到的消息
                        printf("服务器收到来自客户端的消息是：%s.\n", strBuf);
                    }
                    else
                    {
                        // 出错了 SOCKET_ERROR
                        // 强制关闭客户端也是出错，10054
                        int error = WSAGetLastError();
                        switch (error)
                        {
                            case 10054:
                            {
                                // 从集合fd_set中去除下线的客户端socket
                                SOCKET socketTemp = readSockets.fd_array[i];
                                FD_CLR(readSockets.fd_array[i], &allSockets);
                                // 释放下线的客户端socket
                                closesocket(socketTemp);
                            }
                        }
                        // printf("recv failed and error code = %d.\n",error);
                    }
                }         
            }
        }
        else
        {
            // 出错了
            int error = WSAGetLastError();
            printf("select() failed and error code = %d.\n", error);

            break;  // 跳出while(1)循环
        }
    }

    // 什么时候执行到这一步？？？
    // 需要跳出while()循环
    // 释放集合中的所有socket
    for (u_int i = 0; i < allSockets.fd_count; i++)
    {
        closesocket(allSockets.fd_array[i]);
    }


    closesocket(socketServer);
    WSACleanup();

    system("pause");
    return  0;
}

