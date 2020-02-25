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

    
    closesocket(socketServer);
    WSACleanup();

    system("pause");
    return  0;
}

