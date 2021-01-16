#!/bin/bash

inprogressfile=./$1/inprogress

if [ -f "$inprogressfile" ]; then
	echo "Halt! Grading in progress. Cannot initialise."
else
	for dir in ./$1/*/; do
		touch "$dir/Feedback Attachment(s)/comments.txt"
		echo "0" > "$dir/Feedback Attachment(s)/comments.txt"
	done

	touch $inprogressfile
	gradefilemeta=$(head -n 1 ./$1/grades.csv)
	echo "$gradefilemeta" > $inprogressfile

	Rscript clean.R $1
fi
