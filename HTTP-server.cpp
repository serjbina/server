
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#include "response_code.h" //Содержит заголовочные файлы функций, определяющих код ответа
#include "service_classes.cpp" //Содержит описание служебных классов

#define BUFFSIZE 1024

using namespace std;

void response(int code, char* name, IOSocket *pSocket){
	stringstream resp;

	time_t rawtime;
 	struct tm * timeinfo;
 	time(&rawtime);
 	timeinfo = localtime(&rawtime);

 	stringstream descript, myname, type;

	if (code == 404){
		descript << "404 Not Found";
		myname << "responses/404.html";
		type << "text/html";
	}
	else if(code == 200 ){//|| code == 201){
		descript << "200 OK";
		myname << name;
		type << define_type(name);
	}
	else if (code == 403){
		descript << "403 Forbidden";
		myname << "responses/403.html";
		type << "text/html";
	}
	else if (code == 501){
		descript << "501 Not Implemented";
		myname << "responses/501.html";
		type << "text/plain";
	}

		resp << "HTTP/1.1 "<< descript <<" \r\n"; 
		resp << "Server: Model HTTP Server/0.1\r\n";
		if (code == 501)
			resp << "Allow: GET,HEAD\r\n";

		resp << "Date: ";
 		resp << asctime(timeinfo);
		int f = open(myname.str().c_str(), O_RDONLY);
 		resp <<"Content-type: "<< type << " \r\n";
 		resp <<"Content-length: " << lseek(f,0,2) << "\r\n\r\n";
 		pSocket->Send(resp.str().c_str(), resp.str().size());
 		char buff[BUFFSIZE];
 		lseek(f,0,0);
 		int k;
 		while(k = read(f, buff, BUFFSIZE)){
 			pSocket->Send(buff, k);
 		}
 		close(f);

 		//if (code == 201)
 			//unlink(name);
}


char * CGI(char * target_name, char *target_type, char * buff){ //Возвращает имя файла, в котором содержится результат работы CGI-программы

	char tempfile[PATH_MAX] = "iXXXXXX";
	mktemp(tempfile);
	int op = open(tempfile,O_RDWR | O_CREAT, 0666);
	printf("Current tempfile name = %s\n", tempfile);		
					
			//if(strstr(buff,"cgi-bin") != NULL)
			//{	printf("Hello\n");
				char *cgipar[16];
				for (int i = 0; i <= 15; i++)
					cgipar[i] = (char*) malloc (128*sizeof(char));

				cgipar[15]=NULL;

				strcpy(cgipar[0],"GATEWAY_INTERFACE=CGI/1.1");
				strcpy(cgipar[1],"REMOTE_ADDR=127.0.0.1");
				strcpy(cgipar[2],"REMOTE_PORT=8845");
				strcpy(cgipar[3],"SERVER_ADDR=127.0.0.5");
				strcpy(cgipar[4],"SERVER_NAME=WWW.MYSERVER.RU");
				strcpy(cgipar[5],"SERVER_PORT=9005");
				strcpy(cgipar[6],"SERVER_PROTOCOL=HTTP/1.1");
				strcpy(cgipar[7],"SERVER_SOFTWARE=Apache/1.3.12(Unix) PHP/3.0.17");
				strcpy(cgipar[8],"HTTP_USER_AGENT=");
				strcat(cgipar[8],"FIREFOX");
				strcpy(cgipar[9],"QUERY_STRING=");
				strcat(cgipar[9],buff);
				
			char PathName[PATH_MAX];
			char *PN;

			PN = getwd(PathName);

			if (PN == NULL) printf("Ошибка определения пути\n");
			else{
				strcpy(cgipar[10],"DOCUMENT_ROOT=");
				strcat(cgipar[10],PathName);
			}
			//strcpy(cgipar[11],"/0");
			
				char bufferpath[50],bufferparam[50];
				int u = 0;
				while((buff[u] != '?') && (buff[u] != ' '))
				//while((buffer2[u] != '?'))	
				{
					bufferpath[u]=buff[u];
					u++;
				}
				bufferpath[u]=0;
				strcat(bufferparam,"/");
				strcat(bufferparam,bufferpath);
				printf("%s\n",bufferpath);
				strcat(PathName,"/");
				strcat(PathName,bufferpath);
				printf("%s\n",PathName);
				
				strcpy(cgipar[11],"SCRIPT_FILENAME=");
				strcat(cgipar[11],PathName);
				strcpy(cgipar[12],"CONTENT_TYPE=");
				strcat(cgipar[12],target_type);
				strcpy(cgipar[13],"SCRIPT_NAME=");
				strcat(cgipar[13],bufferpath);
				strcpy(cgipar[14],"HTTP_REFERER=http://localhost:8080/testpage.html");
				
				for(int y=0; y<15; y++)
					printf("%s\n",cgipar[y]);

	//}

	return (char *) tempfile;

}

class MySocket: public ServerSocket{
protected:
	virtual void OnAccept(IOSocket *pSocket){
		
		char buff[BUFFSIZE];
		int n = pSocket->Receive(buff, sizeof(buff)-1);
		buff[n] = '\0';

		char * target_name = find_name(buff);
		char * target_type = define_type(target_name);
		
		int r_code;
		
		if (!strcmp(target_type, "Not/Implemented"))
			r_code = 501;

		else if(if_exist(target_name))
					if(if_availible(target_name))
						r_code = 200;
					else
						r_code = 403;
			else 
				r_code = 404;

		if (!strcmp(target_type, "CGI")){
			printf("CGI is running\n");
			r_code = 201;
			target_name = CGI(target_name, target_type, buff);
		}
		
		printf("Respone code = %d, Required name = %s, Type = %s\n", r_code, target_name, target_type);
		response(r_code, target_name, pSocket);
		printf("Responsed succesfully\n");
		delete pSocket;		
	}
};

void Run(ServerSocket *pSocket){
	pSocket->Listen();
	for(;;) {
		pSocket->Accept();
		printf("Connection Accepted\n");
	}
}


int main(){

	MySocket s;
	SocketAddress a("localhost", 8080);
	s.Bind(&a);
	printf("Binded succesfully\n");
	Run(&s);
	return 0;
}

