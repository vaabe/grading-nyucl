#!/bin/bash

inprogressfile=./$1/inprogress

if [ ! -f "$inprogressfile" ]; then
	echo "Cannot grade. Problem set directory not initialised."
else
	direcs=("./$1/*/")
	Rscript parse.R $direcs
fi
