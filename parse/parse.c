#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXBUF 8192

struct FileInfo {
	int numLines; 
	int numComments; 
	int numGrades; 
	int numExtras; 
}; 

enum LineType {
	empty = 0,
	comment = 1, 
	grade = 2, 
	extra = 3, 
	header = 4
}; 

int get_num_lines(char path[]) 
{
	FILE *commentsFile = fopen(path, "r"); 
	if (!commentsFile) {
		perror("File error"); 
		exit(0); 
	}

	int numLines = 0; 
	char buffer[MAXBUF]; 

	while(fgets(buffer, sizeof(buffer), commentsFile) != NULL) {
		numLines++; 
	}

	fclose(commentsFile); 
	return numLines; 
}

void read_comments_file(char path[], int numLines) 
{
	FILE *commentsFile = fopen(path, "r"); 
	if (!commentsFile) {
		perror("File error"); 
		exit(0); 
	}

	struct FileInfo finfo = { 0, 0, 0, 0 };  
	enum LineType sectiontype; 
	enum LineType linetype[numLines]; 
	char buffer[MAXBUF]; 
	int lineNum = 0; 

	for(int i = 0; i < numLines; i++) {
		fgets(buffer, sizeof(buffer), commentsFile); 
		finfo.numLines++; 

		if(strstr(buffer, "## COMMENTS") != NULL) {
			sectiontype = comment; 
			linetype[i] = header; 
		}
		else if(strstr(buffer, "## GRADE") != NULL) {
			sectiontype = grade; 
			linetype[i] = header; 
		}
		else if(strstr(buffer, "## EXTRA") != NULL) {
			sectiontype = extra; 
			linetype[i] = header; 
		}
		else {
		}

		if(strcmp(buffer, "\n") == 0) {
			linetype[i] = empty; 
		}

		if( sectiontype == comment 
			&& strcmp(buffer, "\n") != 0 
			&& strstr(buffer, "##") == NULL
		) {
			 finfo.numComments++; 
			 linetype[i] = comment; 
		}

		if( sectiontype == grade 
			&& strcmp(buffer, "\n") != 0 
			&& strstr(buffer, "##") == NULL
		) {
			 finfo.numGrades++; 
			 linetype[i] = grade; 
		}

		if( sectiontype == extra
			&& strcmp(buffer, "\n") != 0 
			&& strstr(buffer, "##") == NULL
		) {
			 finfo.numExtras++; 
			 linetype[i] = extra; 
		}
	}
	fclose(commentsFile); 

	commentsFile = fopen(path, "r"); 
	char commentsBuffer[finfo.numComments][MAXBUF]; 
	char gradesBuffer[finfo.numGrades][MAXBUF]; 
	char extrasBuffer[finfo.numExtras][MAXBUF]; 
	int commentsIndex = 0; 
	int gradesIndex = 0; 
	int extrasIndex = 0; 

	for(int i = 0; i < numLines; i++) {
		fgets(buffer, sizeof(buffer), commentsFile); 

		if(linetype[i] == comment) {
			strcpy(commentsBuffer[commentsIndex], buffer); 
			commentsIndex++; 
		}

		if(linetype[i] == grade) {
			strcpy(gradesBuffer[gradesIndex], buffer); 
			gradesIndex++; 
		}

		if(linetype[i] == extra) {
			strcpy(extrasBuffer[extrasIndex], buffer); 
			extrasIndex++; 
		}
	}

	fclose(commentsFile); 
}

int main() 
{
	char path[MAXBUF]; 
	snprintf(path, sizeof(path), "comments.txt"); 

	int numLines = get_num_lines(path); 

	read_comments_file(path, numLines);  
}
