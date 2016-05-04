
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#include "response_code.h" //Содержит заголовочные файлы функций, определяющих код ответа
#include "service_classes.cpp" //Содержит описание служебных классов

#define BUFFSIZE 256

using namespace std;

void response(int code, char* name, IOSocket *pSocket){
	stringstream resp;

	time_t rawtime;
 	struct tm * timeinfo;
 	time(&rawtime);
 	timeinfo = localtime(&rawtime);

	if (code == 404){
		resp << "HTTP/1.1 404 Not Found \r\n"; 
		resp << "Server: Model HTTP Server/0.1\r\n";
		resp << "Date: ";
 		resp << asctime(timeinfo);
		int f = open("responses/404.html", O_RDONLY);
 		resp <<"Content-type: text/html\r\n";
 		resp <<"Content-length: " << lseek(f,0,2) << "\r\n\r\n";
 		pSocket->Send(resp.str().c_str(), resp.str().size());
 		char buff[BUFFSIZE];
 		lseek(f,0,0);
 		int k;
 		while(k = read(f, buff, BUFFSIZE)){
 			pSocket->Send(buff, k);
 		}
 		close(f);

	}
	else if(code == 200){
		resp << "HTTP/1.1 200 OK \r\n";
		resp << "Server: Model HTTP Server/0.1\r\n";
		resp << "Date: ";
 		resp << asctime(timeinfo);
		int f = open(name, O_RDONLY);
 		resp <<"Content-type: "<< define_type(name) <<" \r\n";
 		resp <<"Content-length: " << lseek(f,0,2) << "\r\n\r\n";
 		pSocket->Send(resp.str().c_str(), resp.str().size());

 		char buff[BUFFSIZE];
 		lseek(f,0,0);
 		int k;
 		while(k = read(f, buff, BUFFSIZE)){
 			pSocket->Send(buff, k);
 		}
 		close(f);
	}
	else if (code == 403){
		resp << "HTTP/1.1 403 Forbidden \r\n"; 
		resp << "Server: Model HTTP Server/0.1\r\n";
		resp << "Date: ";
 		resp << asctime(timeinfo);
		int f = open("responses/403.html", O_RDONLY);
 		resp <<"Content-type: text/html\r\n";
 		resp <<"Content-length: " << lseek(f,0,2) << "\r\n\r\n";
 		pSocket->Send(resp.str().c_str(), resp.str().size());
 		char buff[BUFFSIZE];
 		lseek(f,0,0);
 		int k;
 		while(k = read(f, buff, BUFFSIZE)){
 			pSocket->Send(buff, k);
 		}
 		close(f);
	}

}

class MySocket: public ServerSocket {
protected:


	virtual void OnAccept(IOSocket *pSocket) {
		
		char buff[BUFFSIZE];
		int n = pSocket->Receive(buff, sizeof(buff)-1);
		buff[n] = '\0';

		char * target_name = find_name(buff);
		char * target_type = define_type(target_name);

		int r_code;
		if (if_exist(target_name)){
			if(if_availible(target_name))
				r_code = 200;
			else
				r_code = 403;
		}
		else 
			r_code = 404;
		
		printf("Respone code = %d, Required name = %s\n", r_code, target_name);
		response(r_code, target_name, pSocket);
		printf("Responsed succesfully\n");
			
		delete pSocket;		
	}
};

void Run(ServerSocket *pSocket) {
	pSocket->Listen();
	for(;;) {
		pSocket->Accept();
		printf("Connection Accepted\n");
	}
}


int main() {

	MySocket s;
	SocketAddress a("localhost", 8080);
	s.Bind(&a);
	printf("Binded succesfully\n");
	Run(&s);
	return 0;
}

