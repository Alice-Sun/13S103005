//Socket.cpp
#include "Socket.h"

struct doc_type file_type[] =
{
    {"html",    "text/html"  },
    {"gif",     "image/gif"  },
    {"jpeg",    "image/jpeg" },
    { NULL,      NULL        }
};

char *http_res_hdr_tmpl = "HTTP/1.1 200 OK\r\nServer: Huiyong's Server <0.1>\r\n"
                          "Accept-Ranges: bytes\r\nContent-Length: %d\r\nConnection: close\r\n"
                          "Content-Type: %s\r\n\r\n";

Socket::Socket()
{
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
    {
        cerr<<"Socket Initialization: Error with WSAStartup\n";
        system("pause");
        WSACleanup();
        exit(10);
    }

}
void Socket::Init(int pro_type, int pro)
{
	type = pro_type;
	protocol = pro;
	//Create a socket
    mySocket = socket( AF_INET, pro_type, pro);

    if ( mySocket == INVALID_SOCKET )
    {
        cerr<<"Socket Initialization: Error creating socket"<<endl;
        system("pause");
        WSACleanup();
        exit(11);
    }

    myBackup = mySocket;
}
Socket::~Socket()
{
    WSACleanup();
}

bool Socket::SendMsg( char *buffer )
{
	int i;
	myAddress_len = sizeof(myAddress);
    if(type == SOCK_STREAM)
    {
        i = send( mySocket, buffer, strlen( buffer ), 0 );
        if(i<0) cout<<"send error: "<<GetLastError()<<endl;
    }
    else
    {
        i = sendto(mySocket,buffer,strlen(buffer),0,(struct sockaddr *)&myAddress,myAddress_len);
        if(i<0) cout<<"sendto error: "<<GetLastError()<<endl;
    }
    if(strncmp(buffer,"FIN",3)!=0)
		cout<<"Send > "<<buffer<<endl;
    return true;
}

int Socket::RecvMsg( char *buffer, int len )
{
	myAddress_len = sizeof(myAddress);
    memset(buffer,'\0',len);
    cout<<"Recv > " ;
    //cout<<"recv len: "<<len<<endl;
    int i;
    if(type == SOCK_STREAM)
	{
		i = recv( mySocket, buffer, len, 0 );
		if(i<0) cout<<"recv error: "<<GetLastError()<<endl;
	}
	else
	{
		i= recvfrom(mySocket, buffer, len,0,(struct sockaddr *)&myAddress,&myAddress_len);
		if(i<=0) cout<<"recvfrom error: "<<GetLastError()<<endl;
	}

    buffer[i] = '\0';
    if(strncmp(buffer,"FIN",3)==0)
    {
    	cout<<"end!"<<endl;
    	return 0;
    }
    cout<<buffer<<endl;
    return i;
}

void Socket::CloseConnection()
{
    //cout<<"CLOSE CONNECTION"<<endl;
    closesocket( mySocket );
    mySocket = myBackup;
}
/*
void Socket::GetAndSendMessage()
{
    char message[STRLEN];
    cin.ignore();//without this, it gets the return char from the last cin and ignores the following one!
    cout<<"Send > ";
    cin.get( message, STRLEN );
    SendMsg( message );
}
*/
void ServerSocket::StartHosting( int port )
{
    Bind( port );
    if(type == SOCK_STREAM)
    {
        Listen();
    }
}

void ServerSocket::Listen()
{
    if ( listen ( mySocket, 1 ) == SOCKET_ERROR )
    {
        cerr<<"ServerSocket: Error listening on socket\n";
        system("pause");
        WSACleanup();
        exit(15);
    }
	printf("...LISTEN FOR CLIENT...\n");
}

void ServerSocket::Accept(struct sockaddr_in from_addr)
{
    int from_len = sizeof(from_addr);

    acceptSocket = accept(myBackup, (struct sockaddr *) &from_addr, &from_len);
    while ( acceptSocket == SOCKET_ERROR )
    {
        acceptSocket = accept( myBackup, (struct sockaddr *) &from_addr, &from_len );
    }
    printf("...ACCEPT CONNECTION...\n");
    mySocket = acceptSocket;
}

void ServerSocket::Bind( int port )
{
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( "0.0.0.0" );
    myAddress.sin_port = htons( port );

    //cout<<"BIND TO PORT "<<port<<endl;

    if ( bind ( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress) ) == SOCKET_ERROR )
    {
        cout<<"ServerSocket: Failed to connect  "<<GetLastError()<<endl;
        system("pause");
        WSACleanup();
        exit(14);
    }
}

/**************************************************************************
 *
 * ��������: �����ļ���׺���Ҷ�Ӧ�� Content-Type.
 *
 * ����˵��: [IN] suffix, �ļ�����׺;
 *
 * �� �� ֵ: �ɹ������ļ���Ӧ�� Content-Type, ʧ�ܷ��� NULL.
 *
 **************************************************************************/
char * WebServerSocket::http_get_type_by_suffix(const char *suffix)
{
    struct doc_type *type;

    for (type = file_type; type->suffix; type++)
    {
        if (strcmp(type->suffix, suffix) == 0)
            return type->type;
    }

    return NULL;
}

/**************************************************************************
 *
 * ��������: ����������, �õ��ļ��������׺. �����и�ʽ:
 *           [GET http://www.baidu.com:8080/index.html HTTP/1.1]
 *
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [IN]  buflen, buf �ĳ���;
 *           [OUT] file_name, �ļ���;
 *           [OUT] suffix, �ļ�����׺;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void WebServerSocket::http_parse_request_cmd(char *buf, int buflen, char *file_name, char *suffix)
{
    int length = 0;
    char *begin, *end, *bias;

    /* ���� URL �Ŀ�ʼλ�� */
    begin = strchr(buf, ' ');
    begin += 1;

    /* ���� URL �Ľ���λ�� */
    end = strchr(begin, ' ');
    *end = 0;

    bias = strrchr(begin, '/');
    length = end - bias;

    /* �ҵ��ļ����Ŀ�ʼλ�� */
    if ((*bias == '/') || (*bias == '\\'))
    {
        bias++;
        length--;
    }

    /* �õ��ļ��� */
    if (length > 0)
    {
        memcpy(file_name, bias, length);
        file_name[length] = 0;

        begin = strchr(file_name, '.');
        if (begin)
            strcpy(suffix, begin + 1);
    }
}


/**************************************************************************

 * ��������: ��ͻ��˷��� HTTP ��Ӧ.
 *
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [IN]  buf_len, buf �ĳ���;
 *
 * �� �� ֵ: �ɹ����ط�0, ʧ�ܷ���0.
 *
 **************************************************************************/
int WebServerSocket::http_send_response(char *buf, int buf_len)
{
    int read_len, file_len, hdr_len, send_len;
    char *type;
    char read_buf[HTTP_BUF_SIZE];
    char http_header[HTTP_BUF_SIZE];
    char file_name[HTTP_FILENAME_LEN] = "index.html", suffix[16] = "html";
    FILE *res_file;

    /* �õ��ļ����ͺ�׺ */
    http_parse_request_cmd(buf, buf_len, file_name, suffix);

    res_file = fopen(file_name, "rb+"); /* �ö����Ƹ�ʽ���ļ� */
    if (res_file == NULL)
    {
        printf("[Web] The file [%s] is not existed\n", file_name);
        return 0;
    }

    fseek(res_file, 0, SEEK_END);
    file_len = ftell(res_file);
    fseek(res_file, 0, SEEK_SET);

    type = http_get_type_by_suffix(suffix); /* �ļ���Ӧ�� Content-Type */
    if (type == NULL)
    {
        printf("[Web] There is not the related content type\n");
        return 0;
    }

    /* ���� HTTP �ײ��������� */
    hdr_len = sprintf(http_header, http_res_hdr_tmpl, file_len, type);
    send_len = send(mySocket, http_header, hdr_len, 0);
    //send_len=1;
    if (send_len == SOCKET_ERROR)
    {
        fclose(res_file);
        printf("[Web] Fail to send, error = %d\n", WSAGetLastError());
        return 0;
    }

    do /* �����ļ�, HTTP ����Ϣ�� */
    {
        read_len = fread(read_buf, sizeof(char), HTTP_BUF_SIZE, res_file);

        if (read_len > 0)
        {
            send_len = send(mySocket, read_buf, read_len, 0);
            file_len -= read_len;
        }
    }
    while ((read_len > 0) && (file_len > 0));

    fclose(res_file);

    return 1;
}

int ClientSocket::ConnectToServer( const char *ipAddress, int port )
{
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( ipAddress );
    myAddress.sin_port = htons( port );
    if ( connect( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress ) ) == SOCKET_ERROR )
    {
        cerr<<"ClientSocket: Failed to connect\n";
        system("pause");
        WSACleanup();
        exit(13);
    }
    cout<<"CONNECTED "<<"on port: "<<port<<endl;
    return 1;
}
