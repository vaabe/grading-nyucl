#ifndef STRINGING_H
#define STRINGING_H

#include <string.h>

#ifndef MAXCHARS
#define MAXCHARS 8192
#endif

char *join_paths(char c1[], char c2[]) {
	char *c3 = malloc(strlen(c1) + strlen(c2) + 1);  
	strcpy(c3, c1); 
	strcat(c3, c2); 
	return c3; 
}

char *remove_quotes(char c1[]) {
	char *c2 = malloc(strlen(c1));  
	strcpy(c2, c1); 
	c2 += 1; 
	c2[strlen(c2) - 1] = 0; 
	return c2; 
}

char *add_quotes(char c1[]) {
	char *c2 = malloc(strlen(c1) + strlen("\"") + strlen("\"")); 
	strcpy(c2, "\""); 
	strcat(c2, c1); 
	strcat(c2, "\""); 
	return c2; 
}

int get_num_lines(char filepath[])
{
	FILE *file = fopen(filepath, "r"); 
	if (!file) {
		perror("File error"); 
		exit(0); 
	}

	int numLines = 0; 
	char buffer[MAXCHARS]; 

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		numLines++; 
	}

	fclose(file); 
	return numLines; 
}

#endif
