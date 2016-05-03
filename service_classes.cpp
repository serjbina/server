#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>


class SocketAddress {
public:	struct sockaddr_in addr;
public:
	SocketAddress(const char *hostname, short port)
	{
		addr.sin_family = AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	struct sockaddr *getAddress();
};

class Socket{
	protected:
		int _sd;
	public:
		Socket() { _sd = socket(AF_INET, SOCK_STREAM, 0); }
		~Socket() { if( _sd >= 0) { shutdown(_sd, 2); close(_sd); }}
	};

class IOSocket: public Socket{
public:
	int Send(const void *buffer, size_t n)
		{	
			printf("send done\n");
			int p=send(_sd,buffer,n,0); //_sd?
			printf("send done done %d\n",p);
			printf("send to socket %d\n",_sd);
			return p;
		}
	
	int Receive(void *buffer, size_t n)
		{	int p;
			p = recv(_sd,buffer,n,0);
			return p;
		}
	IOSocket(int sd) {close(_sd);
		_sd=sd;
		}
};

class ServerSocket: public Socket{
public:
	
	void Bind(SocketAddress *pAddr)
	{
		const struct sockaddr_in t= pAddr->addr;
		const struct sockaddr p=(sockaddr&)t;
		int p1;
		p1= bind(_sd,  &p,sizeof(sockaddr));//
		printf("bind done %d\n",p1);
	};
	
	
	void Listen()
	{	printf("Listen done\n");
		listen(_sd,8);
		printf("Listen done done %d\n",_sd);
	}
	virtual void Accept(void);

protected:
		virtual void OnAccept(IOSocket *pSocket) {
		
		//analizis of request
		char response[] = "404 Not Found HTTP /1.1\r\n\r\n";
		pSocket->Send(response, strlen(response)-1);
		delete pSocket;
	}	
};