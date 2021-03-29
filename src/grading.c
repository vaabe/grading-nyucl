#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <dirent.h>
#include "stringing.h"

#define MAXBUF 200

enum LineType {
	empty = 0, 
	comment = 1, 
	grade = 2, 
	extra = 3, 
	header = 4
}; 

struct Feedback {
	char text[MAXBUF]; 
	enum LineType type; 
}; 

struct Student {
	char dirname[MAXBUF]; 
	char *dirpath; 
	char buffer[MAXBUF]; 
	int numComments; 
	int numGrades; 
	int numExtras; 
	struct Feedback *comments;  
	struct Feedback *grades; 
	struct Feedback *extras; 
	char *token[7]; 
	char *displayid; 
	char *id; 
	char *lastname; 
	char *firstname; 
	char *grade; 
	char *subdate; 
	char *late; 
}; 

//struct Submission {
//	char *dir; 
//	char *id; 
//}; 

int get_num_students(char hwDirPath[]) {
	FILE *gradeFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
	if (!gradeFile) {
		perror("No grade file (grades.csv)"); 
		exit(0); 
	}

	char buffer[MAXBUF]; 
	int n = 0; 
	while (fgets(buffer, sizeof(buffer), gradeFile) != NULL) {
		n++; 
	}

	fclose(gradeFile); 
	return n - 3; 
}

int main(int argc, char *argv[])
{
	char hwDirPath[MAXBUF]; 
	snprintf(
		hwDirPath, 
		sizeof(hwDirPath), 
		"/home/v/de/grading/ps3/"
		); 

	FILE *gradeFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
	if (!gradeFile) {
		perror("No grade file (grades.csv)"); 
		exit(0); 
	}

	// metadata at top of grades.csv file
	char meta[3][MAXBUF]; 
	for (int i = 0; i < 3; i++) {
		fgets(meta[i], sizeof(meta[i]), gradeFile); 
	}

	int numStudents = get_num_students(hwDirPath); 
	struct Student *student = malloc(numStudents * sizeof(struct Student)); 

	for (int i = 0; i < numStudents; i++) {
		fgets(student[i].buffer, sizeof(student[i].buffer), gradeFile); 

		int j = 0; 
		student[i].token[j] = strtok(student[i].buffer, ","); 
		while (student[i].token[j] != NULL) {
			student[i].token[++j] = strtok(NULL, ","); 
		}

		student[i].displayid = student[i].token[0]; 
		student[i].id = student[i].token[1]; 
		student[i].lastname = student[i].token[2]; 
		student[i].firstname = student[i].token[3]; 
		student[i].grade = student[i].token[4]; 
		student[i].subdate = student[i].token[5]; 
		student[i].late = student[i].token[6]; 

		snprintf(
			student[i].dirname, 
			sizeof(student[i].dirname), 
			"%s, %s(%s)/", 
			remove_quotes(student[i].lastname), 
			remove_quotes(student[i].firstname), 
			remove_quotes(student[i].id)
			); 

		student[i].dirpath = join_paths(hwDirPath, student[i].dirname); 
	}

#ifdef INIT
	FILE *inprogressFile; 
	int inprogressIndicator;  
	if (inprogressFile = fopen(join_paths(hwDirPath, "inprogress"), "r")) {
		fclose(inprogressFile); 
		inprogressIndicator = 1; 
	}
	else {
		inprogressIndicator = 0; 
	}

	if (inprogressIndicator == 0) {
		printf("Mode: START\n"); 
		printf("Creating comments.md file in each student directory... \n"); 

		for (int i = 0; i < numStudents; i++) {
			char commentsPath[MAXBUF]; 
			snprintf(
				commentsPath, 
				sizeof(commentsPath), 
				"%s%s", 
				student[i].dirpath, 
				"Feedback Attachment(s)/"
				); 

			FILE *commentsFile = fopen(
				join_paths(commentsPath, "comments.txt"), "w"); 

			fprintf(
				commentsFile,
				"%s%s%s%s%s%s%s", 
				"## COMMENTS\n\n\n\n", 
				"## GRADE\n\n",
				"theory, -\n", 
				"analysis, -\n", 
				"total, -\n\n",
				"## EXTRAS\n\n\n",
				""
				); 

			fclose(commentsFile); 
		}

		printf("Creating inprogress file...\n"); 
		FILE *inprogressFile = fopen(
				join_paths(hwDirPath, "inprogress"), "w"); 

		fprintf(inprogressFile, "%s%s%s", meta[0], meta[1], meta[2]); 
		fclose(inprogressFile); 
	}

	else {
		printf("Halt! Grading in progress (inprogress file exists)\n"); 
		exit(0); 
	}
#endif

#ifdef SAVE
	for (int i = 0; i < numStudents; i++) {
		char feedbackPath[MAXBUF]; 
		snprintf(
			feedbackPath, 
			sizeof(feedbackPath), 
			join_paths(
				student[i].dirpath, 
				"Feedback Attachment(s)/comments.txt"
			)
		); 

		FILE *feedbackFile = fopen(feedbackPath, "r"); 
		if (!feedbackFile) {
			perror("File error"); 
			exit(0); 
		}

		int numLines = get_num_lines(feedbackPath); 
		enum LineType sectiontype; 
		enum LineType linetype[numLines]; 
		char buffer[MAXBUF]; 

		student[i].numComments = 0; 
		student[i].numGrades = 0; 
		student[i].numExtras = 0; 

		for (int j = 0; j < numLines; j++) {
			fgets(buffer, sizeof(buffer), feedbackFile); 

			if (strstr(buffer, "## COMMENTS") != NULL) {
				sectiontype = comment; 
				linetype[j] = header; 
			}
			else if (strstr(buffer, "## GRADE") != NULL) {
				sectiontype = grade; 
				linetype[j] = header; 
			}
			else if (strstr(buffer, "## EXTRA") != NULL) {
				sectiontype = extra; 
				linetype[j] = header; 
			}
			else {
			}

			if (strcmp(buffer, "\n") == 0) {
				linetype[j] = empty; 
			}

			if (sectiontype == comment
				&& strcmp(buffer, "\n") != 0
				&& strstr(buffer, "##") == NULL
			) {
				student[i].numComments++; 
				linetype[j] = comment; 
			}

			if (sectiontype == grade 
				&& strcmp(buffer, "\n") != 0
				&& strstr(buffer, "##") == NULL
			) {
				student[i].numGrades++; 
				linetype[j] = grade; 
			}

			if (sectiontype == extra 
				&& strcmp(buffer, "\n") != 0
				&& strstr(buffer, "##") == NULL
			) {
				student[i].numExtras++; 
				linetype[j] = extra; 
			}
		}

		student[i].comments = malloc(
				student[i].numComments 
				* sizeof(struct Feedback)
				); 
		student[i].grades = malloc(
				student[i].numGrades
				* sizeof(struct Feedback)
				); 
		student[i].extras = malloc(
				student[i].numExtras 
				* sizeof(struct Feedback)
				); 

		fclose(feedbackFile); 
	}

#endif

}

#if 0
int main(int argc, char *argv[]) 
{
	char hwDirPath[MAXBUF]; 
	snprintf(
		hwDirPath, 
		sizeof(hwDirPath), 
		"/home/v/gp/rep/03-011/"
		); 

	FILE *gradeFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
	if (!gradeFile) {
		perror("No grade file (grades.csv)"); 
		exit(0); 
	}

	char meta[3][MAXBUF]; // metadata at top of grades.csv file
	for (int i = 0; i < 3; i++) {
		fgets(meta[i], sizeof(meta[i]), gradeFile); 
	}

	int numStudents = get_num_students(hwDirPath); 
	struct Student *student = malloc(numStudents * sizeof(struct Student)); 
	char studentDirPath[numStudents][MAXBUF]; 	

	for (int i = 0; i < numStudents; i++) {
		fgets(student[i].buffer, sizeof(student[i].buffer), gradeFile); 

		int j = 0; 
		student[i].token[j] = strtok(student[i].buffer, ","); 
		while (student[i].token[j] != NULL) {
			student[i].token[++j] = strtok(NULL, ","); 
		}

		student[i].displayid = student[i].token[0]; 
		student[i].id = student[i].token[1]; 
		student[i].lastname = student[i].token[2]; 
		student[i].firstname = student[i].token[3]; 
		student[i].grade = student[i].token[4]; 
		student[i].subdate = student[i].token[5]; 
		student[i].late = student[i].token[6]; 

		char *lastname_clean = malloc(sizeof(student[i].lastname));  
		lastname_clean = student[i].lastname; 
		lastname_clean += 1; 
		lastname_clean[strlen(lastname_clean) - 1] = 0; 

		char *firstname_clean = malloc(sizeof(student[i].firstname));
		firstname_clean = student[i].firstname; 
		firstname_clean += 1; 
		firstname_clean[strlen(firstname_clean) - 1] = 0; 

		char *id_clean = malloc(sizeof(student[i].id));
		id_clean = student[i].id; 
		id_clean += 1; 
		id_clean[strlen(id_clean) - 1] = 0; 

		snprintf(studentDirPath[i], 
				sizeof(studentDirPath[i]), 
				"%s, %s(%s)/", 
				lastname_clean, 
				firstname_clean, 
				id_clean); 
		student[i].dirpath = join_paths(hwDirPath, studentDirPath[i]); 
	}

//	for (int i = 0; i < numStudents; i++)
//		printf("%s\n", student[i].dirpath); 

//	DIR *assignmentDir = opendir(dirpath); 
//	if (!assignmentDir) {
//		perror("Something is wrong"); 
//		exit(0); 
//	}

#ifdef START
	FILE *inprogressFile; 
	int inprogressIndicator;  
	if (inprogressFile = fopen(
				join_paths(hwDirPath, "inprogress"), "r")) {
		fclose(inprogressFile); 
		inprogressIndicator = 1; 
	}
	else {
		inprogressIndicator = 0; 
	}

	if (inprogressIndicator == 0) {
		printf("'START'\n"); 
		printf("Creating comments.md file in each student directory... \n"); 

		for (int i = 0; i < numStudents; i++) {
			char commentsPath[MAXBUF]; 
			snprintf(
				commentsPath, 
				sizeof(commentsPath), 
				"%s%s", 
				student[i].dirpath, 
				"Feedback Attachment(s)/"
				); 

			FILE *commentsFile = fopen(
					join_paths(commentsPath, "comments.md"), "w"); 

			fprintf(
				commentsFile,
				"%s%s%s%s%s%s%s", 
				"## COMMENTS\n\n\n\n", 
				"## GRADE\n\n",
				"----        ----\n",
				"theory      -\n", 
				"analysis    -\n", 
				"total       -\n",
				"----        ----"
				); 

			fclose(commentsFile); 
		}

		printf("Creating inprogress file...\n"); 
		FILE *inprogressFile = fopen(
				join_paths(hwDirPath, "inprogress"), "w"); 

		fprintf(inprogressFile, "%s%s%s", meta[0], meta[1], meta[2]); 
		fclose(inprogressFile); 
	}

	else {
		printf("Halt! Grading in progress (inprogress file exists)\n"); 
		exit(0); 
	}
	
#endif

	fclose(gradeFile); 
	// closedir(reportDir); 
}
#endif
