#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

// ��ʼ��
int init(); 

int main(void)
{
    // ��ʼ�����������
    int iRes1 = init();
    if (0 != iRes1)
    {
        printf("�������ʧ��.\n");
    }
    

    system("pause");
    return  0;
}

int init()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA lpWsaData;
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
        return iRes1;
    }

    return iRes1;
}