# grading

A (slightly absurd) terminal grading workflow^[Not that anyone actually asked for one]. I made this to speed up grading long-form assignments (lab reports, essays, etc) with NYU Classes.  

`./init` to add scripts to `$PATH`. 

```
Usage: grade <mode> <dir>

modes:
	-i		initialize (make comments.txt file for each student) 
	-g		parse comments for each student and save grades
	-p		made pdfs (convert markdown files to pdf)
	-u		create upload directory (required format for uploading)	
```
