#include <iostream>
#include "stdafx.h"
#include "ServerFunction.h"

using namespace std;

int main()
{
    HANDLE ThreadHandle;
    DWORD ThreadID;
    //���������������̣߳��������̴߳�����ɶ˿�
    cout<<"WELCOME TO OUR SUPERSERVER"<<endl;
    if ((ThreadHandle = CreateThread(NULL,0,FtpTcpServer,NULL,0,&ThreadID)) == NULL)
    {
        cout<<"CreateThread failed with error:" <<GetLastError()<<endl;
    }
    if ((ThreadHandle = CreateThread(NULL,0,FtpUdpServer,NULL,0,&ThreadID)) == NULL)
    {
        cout<<"CreateThread failed with error:" <<GetLastError()<<endl;
    }
    if ((ThreadHandle = CreateThread(NULL,0,WebTcpServer,NULL,0,&ThreadID)) == NULL)
    {
        cout<<"CreateThread failed with error:" <<GetLastError()<<endl;
    }
    getchar();
    CloseHandle(ThreadHandle);
    return 0;
}
