#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stringing.h"

#define MAXCHARS 8192
#define MAXFBACK 100

enum LineType {
	empty = 0, 
	comment = 1, 
	grade = 2, 
	extra = 3, 
	header = 4
}; 

struct Feedback {
	char comment[MAXFBACK][MAXCHARS]; 
	char grade[MAXFBACK][MAXCHARS]; 
	char extra[MAXFBACK][MAXCHARS]; 
}; 

struct Student {
	char dirname[MAXCHARS]; 
	char *dirpath; 
	char buffer[MAXCHARS]; 
	int numComments; 
	int numGrades; 
	int numExtras; 
	struct Feedback feedback;  
	float fgrade; 
	char agrade; 
	char *gradestoken[7]; 
	char *displayid; 
	char *id; 
	char *lastname; 
	char *firstname; 
	char *grade; 
	char *subdate; 
	char *late; 
}; 

char hwDirPath[MAXCHARS]; 
int numStudents; 
struct Student *student; 

int get_num_students(char hwDirPath[]) {
	FILE *gradesFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
	if (!gradesFile) {
		perror("No grades file (grades.csv)"); 
		exit(0); 
	}

	char buffer[MAXCHARS]; 
	int n = 0; 
	while (fgets(buffer, sizeof(buffer), gradesFile) != NULL) {
		n++; 
	}

	fclose(gradesFile); 
	return n - 3; 
}

void grading_init(char hwDirPath[])
{
	numStudents = get_num_students(hwDirPath); 
	student = malloc(numStudents * sizeof(struct Student)); 

	FILE *gradesFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
	if (!gradesFile) {
		perror("No grades file in directory (grades.csv)"); 
		exit(0); 
	}

	char buffer[MAXCHARS]; 
	for (int i = 0; i < numStudents; i++) {
		fgets(buffer, sizeof(buffer), gradesFile); 

		int j = 0; 
		student[i].gradestoken[j] = strtok(buffer, ","); 
		while (student[i].gradestoken[j] != NULL) {
			student[i].gradestoken[++j] = strtok(NULL, ","); 
		}

		student[i].displayid = student[i].gradestoken[0]; 
		student[i].id = student[i].gradestoken[1]; 
		student[i].lastname = student[i].gradestoken[2]; 
		student[i].firstname = student[i].gradestoken[3]; 
		student[i].grade = student[i].gradestoken[4]; 
		student[i].subdate = student[i].gradestoken[5]; 
		student[i].late = student[i].gradestoken[6]; 

		snprintf(
			student[i].dirname, 
			sizeof(student[i].dirname), 
			"%s, %s(%s)/", 
			remove_quotes(student[i].lastname), 
			remove_quotes(student[i].firstname), 
			remove_quotes(student[i].id)
			); 

//		student[i].dirpath = join_paths(hwDirPath, student[i].dirname); 
	}

	fclose(gradesFile); 
}

int main(int argc, char *argv[])
{
	snprintf(hwDirPath, sizeof(hwDirPath), argv[1]); 

	grading_init(hwDirPath); 

}

//int main(int argc, char *argv[])
//{
//	char hwDirPath[MAXCHARS]; 
//	snprintf(
//		hwDirPath, 
//		sizeof(hwDirPath), 
//		// argv[1]
//		"/home/v/gp/grading/03-08/"
//		); 
//
//	FILE *gradeFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
//	if (!gradeFile) {
//		perror("No grade file (grades.csv)"); 
//		exit(0); 
//	}
//
//	// metadata at top of grades.csv file
//	char meta[3][MAXCHARS]; 
//	for (int i = 0; i < 3; i++) {
//		fgets(meta[i], sizeof(meta[i]), gradeFile); 
//	}
//
//	int numStudents = get_num_students(hwDirPath); 
//	struct Student *student = malloc(numStudents * sizeof(struct Student)); 
//
//	for (int i = 0; i < numStudents; i++) {
//		fgets(student[i].buffer, sizeof(student[i].buffer), gradeFile); 
//
//		int j = 0; 
//		student[i].token[j] = strtok(student[i].buffer, ","); 
//		while (student[i].token[j] != NULL) {
//			student[i].token[++j] = strtok(NULL, ","); 
//		}
//
//		student[i].displayid = student[i].token[0]; 
//		student[i].id = student[i].token[1]; 
//		student[i].lastname = student[i].token[2]; 
//		student[i].firstname = student[i].token[3]; 
//		student[i].grade = student[i].token[4]; 
//		student[i].subdate = student[i].token[5]; 
//		student[i].late = student[i].token[6]; 
//
//		snprintf(
//			student[i].dirname, 
//			sizeof(student[i].dirname), 
//			"%s, %s(%s)/", 
//			remove_quotes(student[i].lastname), 
//			remove_quotes(student[i].firstname), 
//			remove_quotes(student[i].id)
//			); 
//
//		student[i].dirpath = join_paths(hwDirPath, student[i].dirname); 
//	}
//
//	fclose(gradeFile); 
//
//#ifdef INIT
//
//	FILE *inprogressFile; 
//	int inprogressIndicator;  
//	if (inprogressFile = fopen(join_paths(hwDirPath, "inprogress"), "r")) {
//		fclose(inprogressFile); 
//		inprogressIndicator = 1; 
//	}
//	else {
//		inprogressIndicator = 0; 
//	}
//
//	if (inprogressIndicator == 0) {
//		printf("Creating comments.txt file in each student directory... "); 
//
//		for (int i = 0; i < numStudents; i++) {
//			char commentsPath[MAXCHARS]; 
//			snprintf(
//				commentsPath, 
//				sizeof(commentsPath), 
//				"%s%s", 
//				student[i].dirpath, 
//				"Feedback Attachment(s)/"
//				); 
//
//			FILE *commentsFile = fopen(
//				join_paths(commentsPath, "comments.txt"), "w"); 
//
//			fprintf(
//				commentsFile,
//				"%s%s%s%s%s%s%s%s", 
//				"## COMMENTS\n\n", 
//				"## GRADE\n\n",
//				"theory, -\n", 
//				"analysis, -\n", 
//				"total, -\n\n",
//				"## EXTRAS\n\n\n",
//				""
//				); 
//
//			fclose(commentsFile); 
//		}
//
//		printf("Done.\n"); 
//		printf("Creating \"inprogress\" file... "); 
//
//		FILE *inprogressFile = fopen(
//				join_paths(hwDirPath, "inprogress"), "w"); 
//
//		fprintf(inprogressFile, "%s%s%s", meta[0], meta[1], meta[2]); 
//		fclose(inprogressFile); 
//
//		printf("Done.\n"); 
//		printf(
//			"\nThe bollards are down! "
//			"Ready to start grading.\n"
//			); 
//
//	}
//
//	else {
//		printf("\nHalt! Grading in progress.\n"); 
//		printf(
//			"If you actually want to delete all progress "
//		   	"and start again, delete the \"inprogress\" file.\n\n"
//			); 	
//		exit(0); 
//	}
//
//#endif
//
//#ifdef SAVE
//
//	printf("\nReading the \"comments.txt\" file for each student... "); 
//
//	for (int i = 0; i < numStudents; i++) {
//		char feedbackPath[MAXCHARS]; 
//		snprintf(
//			feedbackPath, 
//			sizeof(feedbackPath), 
//			join_paths(
//				student[i].dirpath, 
//				"Feedback Attachment(s)/comments.txt"
//			)
//		); 
//
//		FILE *feedbackFile = fopen(feedbackPath, "r"); 
//		if (!feedbackFile) {
//			perror("File error"); 
//			exit(0); 
//		}
//
//		int numLines = get_num_lines(feedbackPath); 
//		enum LineType sectiontype; 
//		enum LineType linetype[numLines]; 
//		char buffer[MAXCHARS]; 
//
//		student[i].numComments = 0; 
//		student[i].numGrades = 0; 
//		student[i].numExtras = 0; 
//
//
//		for (int j = 0; j < numLines; j++) {
//			fgets(buffer, sizeof(buffer), feedbackFile); 
//
//			if (strstr(buffer, "## COMMENTS") != NULL) {
//				sectiontype = comment; 
//				linetype[j] = header; 
//			}
//			else if (strstr(buffer, "## GRADE") != NULL) {
//				sectiontype = grade; 
//				linetype[j] = header; 
//			}
//			else if (strstr(buffer, "## EXTRA") != NULL) {
//				sectiontype = extra; 
//				linetype[j] = header; 
//			}
//			else {
//			}
//
//			if (strcmp(buffer, "\n") == 0) {
//				linetype[j] = empty; 
//			}
//
//			if (sectiontype == comment
//				&& strcmp(buffer, "\n") != 0
//				&& strstr(buffer, "##") == NULL
//			) {
//				student[i].numComments++; 
//				linetype[j] = comment; 
//			}
//
//			if (sectiontype == grade 
//				&& strcmp(buffer, "\n") != 0
//				&& strstr(buffer, "##") == NULL
//			) {
//				student[i].numGrades++; 
//				linetype[j] = grade; 
//			}
//
//			if (sectiontype == extra 
//				&& strcmp(buffer, "\n") != 0
//				&& strstr(buffer, "##") == NULL
//			) {
//				student[i].numExtras++; 
//				linetype[j] = extra; 
//			}
//		}
//
//		fclose(feedbackFile); 
//
//		// for (int j = 0; j < numLines; j++) printf("%d\n", linetype[j]); 
//
//		feedbackFile = fopen(feedbackPath, "r"); 
//		int cind = 0; 
//		int gind = 0; 
//		int eind = 0; 
//
//		for (int j = 0; j < numLines; j++) {
//			fgets(buffer, sizeof(buffer), feedbackFile); 
//
//			if (linetype[j] == comment) {
//				strcpy(student[i].feedback.comment[cind], buffer); 
//				cind++; 
//			}
//
//			if (linetype[j] == grade) {
//				strcpy(student[i].feedback.grade[gind], buffer); 
//				gind++; 
//			}
//
//			if (linetype[j] == extra) {
//				strcpy(student[i].feedback.extra[eind], buffer); 
//				eind++; 
//			}
//		}
//
//		fclose(feedbackFile); 
//
//		int gradeind = student[i].numGrades - 1; 
//		char *gradetoken[2]; 
//
//		// printf("%s\n", student[i].feedback.grade[gradeind]); 
//
// 		int j = 0; 
// 		gradetoken[j] = strtok(student[i].feedback.grade[gradeind], ","); 
// 		while (gradetoken[j] != NULL) {
// 			gradetoken[++j] = strtok(NULL, ", "); 
// 		}
//
//		student[i].grade = gradetoken[1]; 
//
//		student[i].grade[strlen(student[i].grade) - 1] = 0; 
//
//		student[i].token[1] = join_paths(student[i].token[1], "\""); 
//		student[i].token[2] = join_paths(student[i].token[2], "\""); 
//		student[i].token[3] = join_paths(student[i].token[3], "\""); 
//
//		if (strcmp(student[i].grade, "-") != 0) {
//			student[i].token[4] = student[i].grade; 
//		}
//
//		char *gradetmp1 = malloc(sizeof(gradetoken[0])); 
//		char *gradetmp2 = malloc(sizeof(gradetoken[1]));   
//
//		strcpy(gradetmp1, gradetoken[0]); 
//		strcpy(gradetmp2, gradetoken[1]); 
//
//		strcpy(student[i].feedback.grade[gradeind], join_paths(gradetmp1, ", "));  
//
//		strcpy(student[i].feedback.grade[gradeind], join_paths(
//				student[i].feedback.grade[gradeind], gradetmp2)); 
//
//	}
//
//	printf("Done.\n"); 
//	printf("Saving grades in \"grades2.csv\" file... "); 
//
//	gradeFile = fopen(join_paths(hwDirPath, "grades2.csv"), "w"); 
//	FILE *inprogressFile = fopen(join_paths(hwDirPath, "inprogress"), "r"); 
//	fgets(meta[0], sizeof(meta[0]), inprogressFile); 
//	fclose(inprogressFile); 
//
//	for (int i = 0; i < 3; i++) {
//		fprintf(gradeFile, "%s", meta[i]); 
//	}
//
//	for (int i = 0; i < numStudents; i++) {
//		fprintf(gradeFile, 
//			"%s,%s,%s,%s,%s,%s,%s", 
//			student[i].token[0],
//			student[i].token[1],
//			student[i].token[2],
//			student[i].token[3],
//			student[i].token[4],
//			student[i].token[5],
//			student[i].token[6]
//		); 
//	}
//
//	fclose(gradeFile); 
//	printf("Done.\n"); 
//	printf("Rewriting comments to a \"comments.md\" file... "); 
//
//	for (int i = 0; i < numStudents; i++) {
//		FILE *mdFile = fopen(join_paths(
//				student[i].dirpath, 
//				"Feedback Attachment(s)/comments.md"
//				), "w"); 
//
//		if (!mdFile) {
//			perror("mdfile error"); 
//			exit(0); 
//		}
//
//		fprintf(mdFile, "%s\n\n", "## COMMENTS"); 
//
//		for (int j = 0; j < student[i].numComments; j++) {
//			fprintf(
//				mdFile, 
//				"%s %s", 
//				"-", 
//				student[i].feedback.comment[j]
//			); 
//		}
//
//		fprintf(
//			mdFile, 
//			"\n%s\n\n%s\n", 
//			"## GRADE",
//			"----        ----"
//		); 
//
//		for (int j = 0; j < student[i].numGrades; j++) {
//			char *token[2]; 
//			int k = 0; 
//			token[k] = strtok(student[i].feedback.grade[j], ","); 
//			while (token[k] != NULL) {
//				token[++k] = strtok(NULL, ", "); 
//			}
//
//			int len0 = strlen(token[0]); 
//			int len1 = strlen(token[1]); 
//			int diff = 17 - len0 - len1; 
//
//			fprintf(mdFile, "%s", token[0]); 
//
//			for (int l = 0; l < diff; l++) {
//				fprintf(mdFile, "%c", ' '); 
//			}
//
//			fprintf(mdFile, "%s", token[1]); 
//		}
//
//		fprintf(
//			mdFile, 
//			"\n%s\n", 
//			"----        ----"
//		); 
//
//		for (int j = 0; j < student[i].numExtras; j++) {
//			fprintf(
//				mdFile, 
//				"\n%s", 
//				student[i].feedback.extra[j]
//			); 
//		}
//	}
//
//	printf("Done.\n"); 
//
//#endif
//
//	printf("\n"); 
//
//}
