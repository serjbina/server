#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

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


int if_exist(char *name){
	int marker = open(name, O_RDONLY);
	if (marker == -1)
		return 0;
	else{
		close(marker);
		return 1;
	}
}