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

    // �������
    int iRes1 = WSAStartup(wVersionRequested, &lpWsaData);
    // WSAStartupִ�г���
    if (0 != iRes1)
    {
        switch (iRes1)
        {
        case WSASYSNOTREADY:
            printf("��������.\n");
            break;
        case WSAVERNOTSUPPORTED:
            printf("�汾����.\n");
            break;
        case WSAEPROCLIM:
            printf("���������.\n");
            break;
        case WSAEINPROGRESS:
            printf("�ص����õ����.\n");
            break;
        case WSAEFAULT:
            printf("WSAStartup�ĵڸ�������д����.\n");
            break;
        }
        return 0;
    }

    // �汾У��
    if (2 != HIBYTE(lpWsaData.wVersion) || 2 != LOBYTE(lpWsaData.wVersion))
    {
        // �汾����
        // �ر������
        WSACleanup();
        return 0;   // ����������
    }

    // ������������socket
    SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == socketServer)
    {
        // socket()ִ��ʧ��
        int error = WSAGetLastError();
        printf("socket failed and error code = %d.\n", error);

        // ���������
        WSACleanup();
        return 0;
    }

    // �󶨾���ĵ�ַ�Ͷ˿ں�
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

    // ���������socket�����տͻ�������
    int iRes3 = listen(socketServer, SOMAXCONN);
    if (SOCKET_ERROR == iRes3)
    {
        int error = WSAGetLastError();
        printf("listen() failed and error code = %d.\n", error);

        closesocket(socketServer);
        WSACleanup();
        return 0;
    }

    // ����һ��װ�ͻ��˵����ݽṹ
    // fd_set allSockets;

    //FD_ZERO(&allSockets);                // ����
    //FD_SET(socketServer, &allSockets);   // �򼯺������һ��socket

    //FD_CLR(socketServer, &allSockets);   // ɾ��������ָ����socket
    //closesocket(socketServer);

    //FD_ISSET(socketServer, &allSockets); // �ж�һ��socket�Ƿ��ڼ�����

    // ����
    FD_ZERO(&allSockets);                
    // ��������socketװ��fd_set������
    FD_SET(socketServer, &allSockets);

    while (1)
    {
        // �м伯��fd_set����
        fd_set readSockets = allSockets;

        fd_set writeSockets = allSockets;
        FD_CLR(socketServer, &writeSockets);    // ������Ҳ����û��

        fd_set errorSockets = allSockets;       // ����4

        struct timeval st;
        st.tv_sec = 3;
        st.tv_usec = 0;
        int iRes4 = select(0, &readSockets, &writeSockets, &errorSockets, &st);
        if (0 == iRes4)
        {
            // �ͻ����ڵȴ�ʱ����û�з�Ӧ
            continue;
        }
        else if (iRes4 > 0)
        {
            // ����select()����4
            for (u_int i = 0; i < errorSockets.fd_count; i++)
            {
                char strBuf[100] = { 0 };
                int len = 99;
                if (SOCKET_ERROR == getsockopt(errorSockets.fd_array[i], SOL_SOCKET, SO_ERROR, strBuf, &len))
                {
                    printf("�޷��õ��쳣socket.\n");
                }
                printf("�쳣socket��%s.\n",strBuf);
            }

            // ����select()����3
            for (u_int i = 0; i < writeSockets.fd_count; i++)
            {
                // ��ʱwriteSockets��װ���з�����socket�Ϳͻ���socket
                // printf("������%d , %d����д.\n", socketServer, writeSockets.fd_array[i]);

                int sendRes = send(writeSockets.fd_array[i], "ok", 2, 0);
                if (SOCKET_ERROR == sendRes)
                {
                    // �ͻ��������˳���send()Ҳ����SOCKET_ERROR
                    int error = WSAGetLastError();
                    printf("send() failed and error code = %d.\n", error);
                }
            }

            // �пͻ���������
            // ��������Ӧ��socket
            for (u_int i = 0; i < readSockets.fd_count; i++)
            {
                // ȡ������Ӧ��socket
                // �ж�ȡ����socket�Ƿ�����socket���ǿͻ���socket
                if (readSockets.fd_array[i] == socketServer)
                {
                    SOCKET socketClient = accept(socketServer, NULL, NULL);
                    if (INVALID_SOCKET == socketClient)
                    {
                        continue;
                    }
                    
                    // �����ӷ������Ŀͻ���socketװ��������fd_set
                    FD_SET(socketClient, &allSockets);

                    // send()���ͻ��˷���һ����Ϣ�����������յ��ͻ��˵���Ϣ�����ӳɹ���
                    /*int iRes6 = send(socketClient, "���Ƿ����������յ��������Ϣ", sizeof("���Ƿ����������յ��������Ϣ"), 0);
                    if (SOCKET_ERROR == iRes6)
                    {
                        int error = WSAGetLastError();
                        printf("send() failed and error code = %d.\n", error);
                    }*/
                }
                else
                {
                    // ȡ����socket�ǿͻ���socket
                    char strBuf[1500] = { 0 };
                    int iRes5 = recv(readSockets.fd_array[i], strBuf, 1500, 0);
                    if (0 == iRes5)
                    {
                        // �ͻ������ߡ��ͻ��������˳���
                        // �Ӽ���fd_set��ȥ�����ߵĿͻ���socket
                        SOCKET socketTemp = readSockets.fd_array[i];
                        FD_CLR(readSockets.fd_array[i], &allSockets);
                        // �ͷ����ߵĿͻ���socket
                        closesocket(socketTemp);
                    }
                    else if (iRes5 > 0)
                    {
                        // �ɹ����յ�����Ϣ
                        // ��ӡ���յ�����Ϣ
                        printf("�������յ����Կͻ��˵���Ϣ�ǣ�%s.\n", strBuf);
                    }
                    else
                    {
                        // ������ SOCKET_ERROR
                        // ǿ�ƹرտͻ���Ҳ�ǳ���10054
                        int error = WSAGetLastError();
                        switch (error)
                        {
                            case 10054:
                            {
                                // �Ӽ���fd_set��ȥ�����ߵĿͻ���socket
                                SOCKET socketTemp = readSockets.fd_array[i];
                                FD_CLR(readSockets.fd_array[i], &allSockets);
                                // �ͷ����ߵĿͻ���socket
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
            // ������
            int error = WSAGetLastError();
            printf("select() failed and error code = %d.\n", error);

            break;  // ����while(1)ѭ��
        }
    }

    // ʲôʱ��ִ�е���һ��������
    // ��Ҫ����while()ѭ��
    // �ͷż����е�����socket
    for (u_int i = 0; i < allSockets.fd_count; i++)
    {
        closesocket(allSockets.fd_array[i]);
    }


    closesocket(socketServer);
    WSACleanup();

    system("pause");
    return  0;
}

