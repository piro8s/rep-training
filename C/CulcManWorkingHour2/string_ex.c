#include "string_ex.h"

void lntrim(char *str) {
	int i = 0;
	while(1) {
	if(str[i] == '\n') {
		str[i] = '\0';
		break;
	}
	i++;
	}
}

int search(char *str, const char *c){
	int i;
	int num = 0;

	for(i=0; i<strlen(str); i++){
		if(str[i] == *c) num++;
	}
	return num;
}

int split(char *str, const char *delim, char *outlist[] ) {
	char *tk;
	int cnt = search(str, delim);
	int i;

	if (cnt < 1) return cnt;
	tk = strtok(str, delim);
	outlist[0] = tk;
	for (i=1; i<cnt+1; i++) {
		tk = strtok(NULL, delim);
		outlist[i] = tk;
	}

	return cnt;
}