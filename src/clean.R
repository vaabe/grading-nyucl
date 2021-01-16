#!/usr/bin/env Rscript 

options(warn = -1)

reportNum = commandArgs(trailingOnly = TRUE)
filePath = paste("./", reportNum, "/grades.csv", sep = "")
grades = read.csv(filePath, skip = 2)

write.csv(grades, filePath, row.names = F)
