#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>


#include <string>
#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

char* find_name(char * buff)
{
		char *name = (char*) malloc (80*sizeof(char));
		char *p = name;
		while(*buff != '/')
			buff++;
		buff++;

		while(*buff == ' ')
			buff++;

		while(*buff != ' '){				
			if (*buff == '?'){
				*p = *buff;
				p++;
				buff++;
				break;
			}

			*p = *buff;
			p++;
			buff++;
		}

		*p = '\0';

		if (!strcmp(name, "HTTP/1.1\r\nHost:"))
			name = (char *) "index.html";

		return name;

}

char *define_type( char *name){
	char *p = (char *) name;
	char *type = (char*) malloc (12*sizeof(char));
	while (*p != '.' && *p != '?')
		p++;

	if(*p == '?'){
		type = (char *) "CGI";
		return type;
	}

	p++;
	char buff[50];
	char *a = buff;
	while(*p != '\0'){
		*a = *p;
		a++;
		p++;
	}
	*a = '\0';

	
	//string type;
	if(!strcmp(buff, "html"))
		type =  (char *) "text/html";
	else if(!strcmp(buff, "jpeg") or (!strcmp(buff, "jpg")))
		type = (char *) "image/jpeg";
	else if(!strcmp(buff, "png"))
		type = (char *) "image/png";		
	else if(!strcmp(buff, "txt"))
		type = (char *) "text/plain";
	else 
		type = (char *) "Not/Implemented";
	return type;
}


void make_dir_list(void){
	int pid;
	if(pid = fork()){
		int fd = open("dirlist.txt", O_WRONLY|O_TRUNC|O_CREAT, 0666);
		dup2(fd, 1);
		execlp("ls", "ls", "-R", "-l", NULL);
		printf("Exec failed!\n");
	}
	else
	{
		wait(NULL);
	}
}

int if_exist(char *name){
	struct stat buff;
	int marker = stat(name, &buff);
	if (marker == -1)
		return 0;
	else
		return 1;
}

int if_availible(char *name){
	int f = open(name, O_RDONLY);
	if (f == -1)
		return 0;
	else
		return 1;
}

/*
int main(int argc, char const *argv[])
{
	int f = if_exist("example.txt");
	printf("%d\n", f);
	return 0;
}*/