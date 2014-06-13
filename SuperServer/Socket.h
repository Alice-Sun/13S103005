//Socket.h
#pragma once
#include <iostream>
#include "WinSock2.h"
#include <stdio.h>

/* 定义常量 */

#define FTP_DATA_PORT 	2101
#define FTP_CTRL_PORT 	2102
#define HTTP_DEF_PORT        80     /* 连接的缺省端口 */
#define HTTP_BUF_SIZE      1024     /* 缓冲区的大小 */
#define HTTP_FILENAME_LEN   256     /* 文件名长度 */

using namespace std;
#define STRLEN 256
//const int STRLEN = 256;

/* 定义文件类型对应的 Content-Type */
struct doc_type
{
    char *suffix; /* 文件后缀 */
    char *type;   /* Content-Type */
};

class Socket
{
protected:
    WSADATA wsaData;
    SOCKET mySocket;
    SOCKET myBackup;
    SOCKET acceptSocket;
    sockaddr_in myAddress;
    int myAddress_len;
    int type;
	int protocol;
public:

    Socket();
    ~Socket();
    void Init(int, int);
    bool SendMsg( char* );
    int RecvMsg( char*, int );
    void CloseConnection();
    void GetAndSendMessage();
};

class ServerSocket : public Socket
{
public:
    void Listen();
    void Bind( int port );
    void StartHosting( int port );
    void Accept(struct sockaddr_in from_addr);
};

class WebServerSocket : public ServerSocket
{
public:
    char *http_get_type_by_suffix(const char *suffix);
    void http_parse_request_cmd(char *buf, int buflen, char *file_name, char *suffix);
    int http_send_response(char *buf, int buf_len);
};

class ClientSocket : public Socket
{
public:
    int ConnectToServer( const char *ipAddress, int port );
};











