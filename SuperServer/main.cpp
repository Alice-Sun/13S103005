#include <iostream>
#include "stdafx.h"
#include "ServerFunction.h"

using namespace std;

int main()
{
    HANDLE ThreadHandle;
    DWORD ThreadID;
    //创建服务器工作线程，并且向线程传送完成端口
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
