#include "string.h"

void _str_init_(String * str) {
    str->str = NULL;
    str->len = 0;
}

void str_free(String * str) {
    free(str->str);
	str->len = 0;
}

void str_append(String * str1, char * str2, int begin, int len) {
	int mult = 1, flag = 0, new_size, str2_size;
	if(!str2 || (begin != -1 && !len)) {
		//printf("line: %s\n", str2);
		return; // second string is NULL or len is 0
	}
	if(begin < 0 || len < 0) {
		str2_size = strlen(str2);
        begin = 0;
    } else str2_size = len;
	if(str1->len == 0) {
		new_size = str2_size + 1;
		str1->len = 1;
		flag = 1;
	} else
		new_size = strlen(str1->str) + str2_size + 1;
	for(int i = 0; i < 100; i++) {
		if(str1->len * mult > new_size) break;
		mult *= 2;
	}
	if(mult > 1)
		str1->str = (char *) realloc(str1->str, (str1->len *= mult) * sizeof(char));
	if(flag) {
		strncpy(str1->str, str2 + begin, str2_size);
		(str1->str)[str2_size] = '\0';
	} else
		strncat(str1->str, str2 + begin, str2_size);
}

void str_append_char(String * str, char c) {
	char tmp[2];
	tmp[0] = c;
	tmp[1] = '\0';
	str_append(str, tmp, -1, 0);
}
