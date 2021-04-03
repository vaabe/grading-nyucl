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
	int done; 
	int numComments; 
	int numGrades; 
	int numExtras; 
	struct Feedback feedback;  
	char *gradestring[7]; 
	char *displayid; 
	char *id; 
	char *lastname; 
	char *firstname; 
	char *grade; 
	char *subdate; 
	char *late; 
}; 

char hwDirPath[MAXCHARS]; 
char meta[3][MAXCHARS]; 
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

void grading_load(char hwDirPath[])
{
	numStudents = get_num_students(hwDirPath); 
	student = malloc(numStudents * sizeof(struct Student)); 

	FILE *gradesFile = fopen(join_paths(hwDirPath, "grades.csv"), "r"); 
	if (!gradesFile) {
		perror("No grades file in directory (grades.csv)"); 
		exit(0); 
	}

	// metadata (at top of grades.csv file)
	for (int i = 0; i < 3; i++) {
		fgets(meta[i], sizeof(meta[i]), gradesFile); 
	}

	// student info 
	char buffer[MAXCHARS]; 
	for (int i = 0; i < numStudents; i++) {
		fgets(buffer, sizeof(buffer), gradesFile); 

		int j = 0; 
		student[i].gradestring[j] = strtok(buffer, ","); 
		while (student[i].gradestring[j] != NULL) {
			student[i].gradestring[++j] = strtok(NULL, ","); 
		}

		student[i].displayid = remove_quotes(student[i].gradestring[0]); 
		student[i].id = remove_quotes(student[i].gradestring[1]); 
		student[i].lastname = remove_quotes(student[i].gradestring[2]); 
		student[i].firstname = remove_quotes(student[i].gradestring[3]); 
		student[i].grade = remove_quotes(student[i].gradestring[4]); 
		student[i].subdate = remove_quotes(student[i].gradestring[5]); 
		student[i].late = student[i].gradestring[6]; 
		student[i].late = "On time"; 

		snprintf(
			student[i].dirname, 
			sizeof(student[i].dirname), 
			"%s, %s(%s)", 
			student[i].lastname, 
			student[i].firstname, 
			student[i].id
			); 

		student[i].dirpath = join_paths(hwDirPath, student[i].dirname); 
		student[i].dirpath = join_paths(student[i].dirpath, "/"); 
	}

	fclose(gradesFile); 

	// printf("%s\n", student[0].late); 
}

void configure_hw_directory() 
{
	printf("\nConfiguring grading directory...\n\n"); 

	FILE *progressFile; 
	int progressIndicator; 
	if (progressFile = fopen(join_paths(hwDirPath, "progress.csv"), "r")) {
		progressIndicator = 1; 
		fclose(progressFile); 
	}
	else {
		progressIndicator = 0; 
	}

	if (progressIndicator == 0) {
		printf("Creating comments.txt file in each student directory... "); 

		for (int i = 0; i < numStudents; i++) {
			char commentsPath[MAXCHARS]; 
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
				"## COMMENTS\n\n\n", 
				"## GRADE\n\n",
				"theory, -\n", 
				"analysis, -\n", 
				"total, -\n\n",
				"## EXTRAS\n\n\n",
				""
				); 

			fclose(commentsFile); 
		}
		printf("Done.\n"); 

		printf("Creating progress.csv file... "); 
		progressFile = fopen(join_paths(hwDirPath, "progress.csv"), "w"); 

		for (int i = 0; i < numStudents; i++) {
			student[i].done = 0; 
			fprintf(
				progressFile, 
				"%d, \"%s %s\", %s, %d, %s\n", 
				i+1, 
				student[i].firstname,
				student[i].lastname, 
				student[i].id, 
				student[i].done,
				student[i].grade
				); 
		}

		fclose(progressFile); 
		printf("Done.\n"); 

		printf("Creating commentsbuffer.txt file... "); 
		FILE *commentsBuffer = fopen(
			join_paths(hwDirPath, "commentsbuffer.txt"), "w"); 
		fclose(commentsBuffer); 
		printf("Done\n"); 

		printf(
			"\nThe bollards are down! "
			"Ready to start grading.\n\n"
			); 

	}

	else {
		printf("Halt! Grading in progress.\n"); 
		printf(
			"If you actually want to delete all progress "
		   	"and start again, delete the \"progress\" file "
			"and rerun this command.\n\n"
			); 	
		exit(0); 
	}
}

void save_progress()
{
	printf("\nSaving progress..."); 
#ifdef VERBOSE
	printf("\nReading the \"comments.txt\" file for each student... "); 
#endif

	FILE *commentsBuffer = fopen(
		join_paths(hwDirPath, "commentsbuffer.txt"), "w"); 

	for (int i = 0; i < numStudents; i++) {
		char feedbackPath[MAXCHARS]; 
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
			perror("No feedback file"); 
			exit(0); 
		}

		int numLines = get_num_lines(feedbackPath); 
		enum LineType sectiontype; 
		enum LineType linetype[numLines]; 
		char buffer[MAXCHARS]; 
		student[i].numComments = 0; 
		student[i].numGrades = 0; 
		student[i].numExtras = 0; 
		student[i].done = 0; 

#ifdef DEBUG
		printf("\n"); 
		printf("  [%d] %s\n", i+1, student[i].dirname); 
#endif

		// get line types in comments.txt
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
		fclose(feedbackFile); 

		// save comments from comments.txt in student struct
		feedbackFile = fopen(feedbackPath, "r"); 
		int cind = 0; 
		int gind = 0; 
		int eind = 0; 
		for (int j = 0; j < numLines; j++) {
			fgets(buffer, sizeof(buffer), feedbackFile); 
			if (linetype[j] == comment) {
				strcpy(student[i].feedback.comment[cind], buffer); 

				// save comment in comments buffer
				fprintf(commentsBuffer, "%s\n", buffer); 

				cind++; 
			}
			if (linetype[j] == grade) {
				strcpy(student[i].feedback.grade[gind], buffer); 
				gind++; 
			}
			if (linetype[j] == extra) {
				strcpy(student[i].feedback.extra[eind], buffer); 
				eind++; 
			}
		}
		fclose(feedbackFile); 

		// get total grade
		int totalind = student[i].numGrades - 1; 
		char *gradetoken[2]; 
 		int j = 0; 
 		gradetoken[j] = strtok(student[i].feedback.grade[totalind], ","); 
 		while (gradetoken[j] != NULL) {
 			gradetoken[++j] = strtok(NULL, ", "); 
 		}

		student[i].grade = gradetoken[1]; 
		student[i].grade[strlen(student[i].grade) - 1] = 0; 

		if (strcmp(student[i].grade, "-") != 0) {
			student[i].done = 1; 
		}

		char *gradetmp1 = malloc(sizeof(gradetoken[0])); 
		char *gradetmp2 = malloc(sizeof(gradetoken[1]));   

		strcpy(gradetmp1, gradetoken[0]); 
		strcpy(gradetmp2, gradetoken[1]); 
		strcpy(student[i].feedback.grade[totalind], 
				join_paths(gradetmp1, ", "));  
		strcpy(student[i].feedback.grade[totalind], 
				join_paths(student[i].feedback.grade[totalind], gradetmp2)); 
	}

	fclose(commentsBuffer); 

#ifdef VERBOSE
	printf("Done.\n"); 
	printf("Writing grades to file \"grades-tmp.csv\" file...\n\n"); 
#endif

	FILE *gradesFile = fopen(join_paths(hwDirPath, "grades-tmp.csv"), "w"); 

	for (int i = 0; i < 3; i++) {
		fprintf(gradesFile, "%s", meta[i]); 
	}

#ifdef VERBOSE
	printf("   #     Grade        Student\n\n"); 
#endif
	for (int i = 0; i < numStudents; i++) {
		char grade[5]; 
		if (strcmp(student[i].grade, "-") != 0) {
			strcpy(grade, student[i].grade); 
		}
		else {
			strcpy(grade, "\"\""); 
		}

		fprintf(
			gradesFile, 
			"%s,%s,%s,%s,%s,%s,%s\n", 
			add_quotes(student[i].displayid),
			add_quotes(student[i].id), 
			add_quotes(student[i].lastname), 
			add_quotes(student[i].firstname), 
			grade,
			add_quotes(student[i].subdate), 
			add_quotes(student[i].late)
			); 

#ifdef VERBOSE
		printf(
			"  [%d]     %s 	%s\n",	
			i+1, 
			student[i].grade,
			student[i].dirname 
			); 
#endif
	}
	fclose(gradesFile); 

	FILE *progressFile = fopen(join_paths(hwDirPath, "progress.csv"), "w"); 

	for (int i = 0; i < numStudents; i++) {
		fprintf(
			progressFile, 
			"%d, \"%s %s\", %s, %d, %s\n", 
			i+1, 
			student[i].firstname,
			student[i].lastname, 
			student[i].id, 
			student[i].done, 
			student[i].grade
			); 
	}

	fclose(progressFile); 

	int numCompleted = 0; 
	for (int i = 0; i < numStudents; i++) {
		if (student[i].done == 1) {
			numCompleted++; 
		}
	}

#ifdef VERBOSE
	printf("\nDone. "); 
	printf("%d/%d %s\n", numCompleted, numStudents, "graded."); 
#endif
}

void write_txt_to_md()
{
	printf("\nWriting comments to markdown format... \n"); 
	for (int i = 0; i < numStudents; i++) {
		FILE *mdFile = fopen(join_paths(
				student[i].dirpath, 
				"Feedback Attachment(s)/comments.md"
				), "w"); 
		if (!mdFile) {
			perror("mdfile error"); 
			exit(0); 
		}

		fprintf(mdFile, "%s\n\n", "## COMMENTS"); 

		for (int j = 0; j < student[i].numComments; j++) {
			fprintf(
				mdFile, 
				"%s %s", 
				"-", 
				student[i].feedback.comment[j]
			); 
		}

		fprintf(
			mdFile, 
			"\n%s\n\n%s\n", 
			"## GRADE",
			"----        ----"
		); 

		for (int j = 0; j < student[i].numGrades; j++) {
			char *token[2]; 
			int k = 0; 
			token[k] = strtok(student[i].feedback.grade[j], ","); 
			while (token[k] != NULL) {
				token[++k] = strtok(NULL, ", "); 
			}

			int len0 = strlen(token[0]); 
			int len1 = strlen(token[1]); 
			int diff = 17 - len0 - len1; 

			fprintf(mdFile, "%s", token[0]); 

			for (int l = 0; l < diff; l++) {
				fprintf(mdFile, "%c", ' '); 
			}

			fprintf(mdFile, "%s", token[1]); 
		}

		fprintf(
			mdFile, 
			"\n%s\n", 
			"----        ----"
		); 

		for (int j = 0; j < student[i].numExtras; j++) {
			fprintf(mdFile, "\n%s", student[i].feedback.extra[j]); 
		}
	}

#ifdef VERBOSE
	printf("Done.\n\n"); 
#endif
}

int main(int argc, char *argv[])
{
	snprintf(hwDirPath, sizeof(hwDirPath), argv[1]); 
	grading_load(hwDirPath); 

#ifdef CONFIGURE
	configure_hw_directory(); 
#endif

#ifdef SAVE
	save_progress(); 
	write_txt_to_md(); 
#endif
}
