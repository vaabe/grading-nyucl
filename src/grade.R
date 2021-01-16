#!/usr/bin/env Rscript 

options(warn = -1)

stu = commandArgs(trailingOnly = TRUE)
numStu = length(stu)

filePath = paste(substr(stu[1], 1, 5), "/grades.csv", sep = "")

grades = read.csv(filePath)
parsedGrades = data.frame(ID = rep(NA, numStu), grade = rep(NA, numStu))

for (i in 1:numStu) {
	path = paste(stu[i], "Feedback Attachment(s)/comments.txt", sep = "")
	id = gsub("\\(([^()]*)\\)|.", "\\1", stu[i], perl=T)
	comments = read.delim(path, header = F)
	grade = comments[dim(comments)[1], ]
	parsedGrades$ID[i] = id
	parsedGrades$grade[i] = grade
}

parsedGrades$ID = as.character(parsedGrades$ID)
parsedGrades$grade = as.numeric(parsedGrades$grade)

grades = merge(grades, parsedGrades, by = "ID")
grades$grade.x = grades$grade.y
grades = grades[ , !(colnames(grades) == "grade.y")]
names(grades)[names(grades) == "grade.x"] = "grade"
grades = grades[order(grades$Last.Name), ]

write.csv(grades, filePath, row.names = F) 
