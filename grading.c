#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <dirent.h>

#define MBUF 200

struct Student {
	char *dirpath; 
	char buffer[MBUF]; 
	char *token[7]; /* key: 0-displayid 1-id 2-lastname 
					   3-firstname 4-grade 5-subdate 6-late */ 
	char *displayid; 
	char *id; 
	char *lastname; 
	char *firstname; 
	char *grade; 
	char *subdate; 
	char *late; 
}; 

struct Submission {
	char *dir; 
	char *id; 
}; 

char *join_paths(char c1[], char c2[]) {
	char *c3 = malloc(strlen(c1) + strlen(c2) + 1);  
	strcpy(c3, c1); 
	strcat(c3, c2); 
	return c3; 
}

int get_num_students(char reportDirPath[]) {
	FILE *gradeFile = fopen(join_paths(reportDirPath, "grades.csv"), "r"); 
	if (!gradeFile) {
		perror("No grade file (grades.csv)"); 
		exit(0); 
	}

	char buffer[MBUF]; 
	int n = 0; 
	while (fgets(buffer, sizeof(buffer), gradeFile) != NULL) {
		n++; 
	}

	fclose(gradeFile); 
	return n - 3; 
}

int main(int argc, char *argv[]) 
{
	char reportDirPath[MBUF]; 
	snprintf(
		reportDirPath, 
		sizeof(reportDirPath), 
		"/home/v/gp/rep/02/"
		); 

	FILE *gradeFile = fopen(join_paths(reportDirPath, "grades.csv"), "r"); 
	if (!gradeFile) {
		perror("No grade file (grades.csv)"); 
		exit(0); 
	}

	char meta[3][MBUF]; // metadata at top of grades.csv file
	for (int i = 0; i < 3; i++) {
		fgets(meta[i], sizeof(meta[i]), gradeFile); 
	}

	int numStudents = get_num_students(reportDirPath); 
	struct Student *student = malloc(numStudents * sizeof(struct Student)); 
	char studentDirPath[numStudents][MBUF]; 	

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
		student[i].dirpath = join_paths(reportDirPath, studentDirPath[i]); 
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
				join_paths(reportDirPath, "inprogress"), "r")) {
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
			char commentsPath[MBUF]; 
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
				join_paths(reportDirPath, "inprogress"), "w"); 

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

