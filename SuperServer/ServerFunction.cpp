#include "stdafx.h"

DWORD WINAPI FtpTcpServer(LPVOID lpParamter)
{
	printf("====FtpTcpServer STARTED====\n");
    const char* dirName="C:\\Users\\Administrator\\Desktop";
    struct sockaddr_in from_addr;
    string ipAddress;
    char recMessage[STRLEN];
    char senMessage[STRLEN];
    string input;
    string fileToCopy;

    ServerSocket sockServer;
    ServerSocket DataSockServer;
    sockServer.Init(SOCK_STREAM,IPPROTO_TCP);
    DataSockServer.Init(SOCK_STREAM,IPPROTO_TCP);
    //cout<<"HOSTING..."<<endl;
    sockServer.StartHosting( FTP_CTRL_PORT );
    DataSockServer.StartHosting(FTP_DATA_PORT);

    //Connected
    while ( true )
    {
        sockServer.Accept(from_addr);
        sockServer.SendMsg("WELCOME");
        //cout<<"\t--WAIT--"<<endl;
        while(true)
        {
            sockServer.RecvMsg( recMessage, STRLEN );
            //cout<<"Recv > "<< recMessage << endl;

            input = string(recMessage).substr(0,4);
            if(string(recMessage).size() > 4)
            {
                fileToCopy = string(recMessage).substr(5);
            }

			if(input == "PASV")
			{
				sockServer.SendMsg(itoa(FTP_DATA_PORT, senMessage,10));
				DataSockServer.Accept(from_addr);
			}
            //sends client all the files in our directory
            if(input == "LIST")
            {
                DIR *dir;
                struct dirent *ep;
                char* fileName;
                //char* directory;

                dir = opendir (dirName);

                while ((ep = readdir (dir)))
                {
                    fileName = ep->d_name;
                    DataSockServer.SendMsg(fileName);
                    cout<<"fileName: "<<fileName;
                    DataSockServer.SendMsg("\n");
                }
                continue;
            }

            //sends files to our client
            else if(input == "SEND")
            {
                ifstream fin;
                string line;
                //fileToCopy = "C:/Users/Student/desktop/" + fileToCopy;
                //fileToCopy = fileToCopy;
                cout << fileToCopy << endl;
                fin.open(fileToCopy.c_str());
                cout << "file open" << endl;
                if(!fin.is_open())
                {
                    cout << "File Does Not Exist" << endl;
                }
                char  toSend[1024];
                cout << "before reading loop" << endl;
                while(!fin.eof())
                {
                    getline(fin, line);
                    cout << "after getline" << endl;
                    //toSend = new char[line.length()+1];
                    strcpy(toSend, line.c_str());
                    //append a newline
                    strcat(toSend, "\n");
                    DataSockServer.SendMsg(toSend);
                    cout << "sent data" << endl;
                    memset(&toSend, '\0', sizeof(toSend));
                }
                DataSockServer.SendMsg("EOF");
                cout << "sent eof" << endl;
                continue;
            }
            else if(input == "QUIT")
            {
            	DataSockServer.CloseConnection();
                break;
            }
        }

    }
    WSACleanup();
    printf("[Ftp] The Ftp UDP server is stopped.\n");
    return 0;
}

DWORD WINAPI FtpUdpServer(LPVOID lpParamter)
{
	printf("====FtpUdpServer STARTED====\n");
    const char* dirName="C:\\Users\\Administrator\\Desktop";
    struct sockaddr_in from_addr;
    string ipAddress;
    char recMessage[STRLEN];
    char senMessage[STRLEN];
    string input;
    string fileToCopy;

    ServerSocket sockServer;
    ServerSocket DataSockServer;
    sockServer.Init(SOCK_DGRAM,IPPROTO_UDP);
    //DataSockServer.Init(SOCK_DGRAM,IPPROTO_UDP);
    //cout<<"HOSTING..."<<endl;
    sockServer.StartHosting(FTP_CTRL_PORT );//listen

    //Connected
    while ( true )
    {//user control......
        while(true)
        {
        	//cout<<"Recv > "<<endl;
        	memset(recMessage,0,STRLEN);
            sockServer.RecvMsg( recMessage, STRLEN );
            //cout<<"Recv > "<< recMessage << endl;

            input = string(recMessage).substr(0,4);
            if(string(recMessage).size() > 4)
            {
                fileToCopy = string(recMessage).substr(5);
            }

			if(input == "PASV")
			{
				sockServer.SendMsg(itoa(FTP_DATA_PORT, senMessage,10));
				DataSockServer.Init(SOCK_DGRAM,IPPROTO_UDP);
				DataSockServer.StartHosting(FTP_DATA_PORT);
				memset(recMessage,0,STRLEN);
				DataSockServer.RecvMsg(recMessage,STRLEN);
				//DataSockServer.Accept(from_addr);
				continue;
			}
            //sends client all the files in our directory
            else if(input == "LIST")
            {
                DIR *dir;
                struct dirent *ep;
                char* fileName;
                //char* directory;

                dir = opendir (dirName);

                while ((ep = readdir (dir)))
                {
                    fileName = ep->d_name;
                    DataSockServer.SendMsg(fileName);
                }
                DataSockServer.SendMsg("FIN");
                continue;
            }

            //sends files to our client
            else if(input == "SEND")
            {
                ifstream fin;
                string line;
                //fileToCopy = "C:/Users/Student/desktop/" + fileToCopy;
                //fileToCopy = fileToCopy;
                cout << fileToCopy << endl;
                fin.open(fileToCopy.c_str());
                cout << "file open" << endl;
                if(!fin.is_open())
                {
                    cout << "File Does Not Exist" << endl;
                }
                char  toSend[1024];
                cout << "before reading loop" << endl;
                while(!fin.eof())
                {
                    getline(fin, line);
                    cout << "after getline" << endl;
                    //toSend = new char[line.length()+1];
                    strcpy(toSend, line.c_str());
                    //append a newline
                    strcat(toSend, "\n");
                    DataSockServer.SendMsg(toSend);
                    cout << "sent data" << endl;
                    memset(&toSend, '\0', sizeof(toSend));
                }
                DataSockServer.SendMsg("EOF");
                cout << "sent eof" << endl;
                continue;
            }
            else if(input == "QUIT")
            {
            	DataSockServer.CloseConnection();
                cout<<"Data link closed!"<<endl;
                memset(recMessage,0,sizeof(recMessage));
                break;
            }
            else
			{
				//DataSockServer.SendMsg("ERROR INPUT.");
				memset(recMessage,0,sizeof(recMessage));
				continue;
			}
        }
    }
    WSACleanup();
    printf("[Ftp] The Ftp UDP server is stopped.\n");
    return 0;
}

DWORD WINAPI WebTcpServer(LPVOID lpParamter)
{
	printf("====WebTcpServer STARTED====\n");
    struct sockaddr_in from_addr;
    char recMessage[STRLEN];
    unsigned short port = HTTP_DEF_PORT;

    WebServerSocket sockServer;
    sockServer.Init(SOCK_STREAM,IPPROTO_TCP);
    //cout<<"HOSTING..."<<endl;
    sockServer.StartHosting( port );

    while (1)
    {
    	//sockServer.StartHosting( port );
        sockServer.Accept(from_addr);
        memset(recMessage,0,STRLEN);
        cout<<"[Web] Accepted address:"<<inet_ntoa(from_addr.sin_addr)<<"port:[%d]\n"<<ntohs(from_addr.sin_port)<<endl;
        sockServer.RecvMsg(recMessage, STRLEN);
        while(strlen(recMessage) <= 0)
		{
			sockServer.RecvMsg(recMessage, STRLEN);
			cout<<".";
		}
        /*if(strlen(recMessage) <= 0)
        {
            sockServer.CloseConnection();
            cout<<"[Web] Fail to recv, error = "<<WSAGetLastError()<<endl;
            break;
        }*/
		recMessage[STRLEN] = 0;
		sockServer.http_send_response(recMessage, STRLEN);
		sockServer.CloseConnection();

    }

    WSACleanup();
    printf("[Web] The server is stopped.\n");

    return 0;
}
