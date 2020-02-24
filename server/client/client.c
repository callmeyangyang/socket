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

    // �����ǿͻ���
    // ��������socket�Ƿ�������socket
    SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == socketServer)
    {
        // socket()ִ��ʧ��
        int error = WSAGetLastError();
        printf("socket() failed and error code = %d.\n", error);

        // ���������
        WSACleanup();
        return 0;
    }

    // ���ӷ��������ѷ�������Ϣ�������SOCKET�󶨵�һ��
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
        // ��������շ���Ϣ
        char buf[1500] = { 0 };
        int iRes4 = recv(socketServer, buf, 50, 0);
        if (0 == iRes4)
        {
            printf("�����жϣ��ͻ�������.\n");
        }
        else if (SOCKET_ERROR == iRes4)
        {
            int error = WSAGetLastError();
            printf("recv() failed and error code = %d.\n", error);
            // ���ﲻ��Ҫ�ر�socket
            // ��������ʾ�շ����ݵĹ����г�����

        }
        else
        {
            // ִ�гɹ������յ��ͻ��˷��͹���������
            printf("%s, %d.\n", buf, iRes4);
        }

        // ����������������
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

