# grading

A (slightly absurd) terminal grading workflow for NYU Classes. I made this to speed up grading long-form assignments (e.g. lab reports, project essays, etc). 

`./init` to add scripts to `$PATH`. 

```
Usage: grade <mode> <dir>

modes:
	-i		initialize (make comments.txt file for each student) 
	-g		parse comments for each student and save grades
	-p		made pdfs (convert markdown files to pdf)
	-u		create upload directory (required format for uploading)	
```
