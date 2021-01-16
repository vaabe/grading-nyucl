#!/bin/bash

inprogressfile=./$1/inprogress

if [ ! -f "$inprogressfile" ]; then
	echo "Cannot finish. Problem set directory not initialised."
else
	firstline=$(head -n 1 ./$1/grades.csv)

	if [[ $firstline == *"ID"* ]]; then
		echo "One sec... first let me convert the grades.csv file back to NYU's atrocious format..."
		echo '""' | cat - ./$1/grades.csv > tmp && mv tmp ./$1/grades.csv
		gradefilemeta=$(head -n 1 $inprogressfile)
		echo "$gradefilemeta" | cat - ./$1/grades.csv > tmp2 && mv tmp2 ./$1/grades.csv
	fi

	rm $inprogressfile

	echo "Zipping..."
	zip -r $1.zip ./$1/*
fi
