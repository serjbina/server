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
#include <iostream>
#include <sstream>
#include <string>


using namespace std;

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

char * find_name(char * buff); //Обнаруживает имя запрашиваемого файла в буфере данных
char * define_type(char * name); //Определяет тип запрошенного файла по его имени
int if_exist(char *name);
int if_availible(char *name);


class MySocket: public ServerSocket {
protected:
	virtual void OnAccept(IOSocket *pSocket) {
		
		char buff[256];
		int n = pSocket->Receive(buff, sizeof(buff)-1);
		buff[n] = '\0';

		char * target_name = find_name(buff);
		char * target_type = define_type(target_name);
/*
		if(if_exist(target_name) && if_availible(target_name)){

		}
		else if(if_exist()){

		}
*/

/*
		int op = open(buffer2,O_RDONLY);
		printf("%d\n",op);
		//analizis of request
		
		char response[] = "404 Not Found HTTP /1.1\r\n\r\n";
		if (op==-1)
			pSocket->Send(response, strlen(response)-1);
		else
		{	struct stat f1;
			fstat(op,&f1);
			int size = f1.st_size;
			stringstream s;
			if(buffer2[strlen(buffer2)-1] == 't')
			s << "HTTP/1.1 200 Ok\r\nContent-type:text/plain\r\nContent-length:"<<size<<"\r\n\r\n";
			else if(buffer2[strlen(buffer2)-1] == 'l')
			s << "HTTP/1.1 200 Ok\r\nContent-type:text/html\r\nContent-length:"<<size<<"\r\n\r\n";
			else
			s << "HTTP/1.1 200 Ok\r\nContent-type:image/jpeg\r\nContent-length:"<<size<<"\r\n\r\n";
			
			
			
			pSocket->Send(s.str().c_str(), s.str().size());
			int k;
			while((k=read(op,buffer2,10240))!=0)
				pSocket->Send(buffer2, k);
		}
*/
		delete pSocket;
		printf("On Accept done done\n");
		
	}
};

void Run(ServerSocket *pSocket) {
	pSocket->Listen();
	for(;;) {
		pSocket->Accept();
	}
}

void ServerSocket::Accept(void) {
	printf("Accept done\n");
	int sd;
	
	sd = accept(_sd, NULL, NULL);
	if(sd < 0) 
		printf("error\n");
	if(!fork()){
		printf("Accept done done  %d\n",sd);
		IOSocket *pSocket = new IOSocket(sd);
		OnAccept(pSocket);
	}
	else{
		close(sd);
 	}
}




int main() {

	MySocket s;
	printf("Socket created\n");
	SocketAddress a("localhost", 8080);
	printf("Address done\n");
	s.Bind(&a);
	printf("Bind done\n");
	Run(&s);
printf("End\n");
	return 0;
}

char* find_name(char * buff)
{
		char *name = (char*) malloc (80*sizeof(char));
		char *p = name;
		while(*buff != '/')
			buff++;
		buff++;

		while(*buff == ' ')
			buff++;

		while(*buff != ' '){//} and *str != '/'){
			*p = *buff;
			p++;
			buff++;
		}

		*p = '\0';

		if (!strcmp(name, "HTTP/1.1\r\nHost:"))
			name = "index.html";

		return name;

}

char *define_type(char *name){
	char *p = name;
	while (*p != '.')
		p++;
	p++;
	char buff[50];
	char *a = buff;
	while(*p != '\0'){
		*a = *p;
		a++;
		p++;
	}
	*a = '\0';
	char *type = (char*) malloc (12*sizeof(char));
	if(!strcmp(buff, "html"))
		type = "text/html";
	else if(!strcmp(buff, "jpeg") or (!strcmp(buff, "jpg")))
		type = "image/jpeg";		
	else
		type = "text/plain";
	return type;
}