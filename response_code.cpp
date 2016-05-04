#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

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
	int marker = open(name, O_RDONLY);
	if (marker == -1)
		return 0;
	else{
		close(marker);
		return 1;
	}
}

/*
int main(int argc, char const *argv[])
{
	make_dir_list();
	return 0;
}*/