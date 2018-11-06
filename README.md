# Introduction
Updating code library before every onsite contest is a nightmare. A brute force solution is
presented here. This procedure takes all source files from given location and makes a tex
file, then compiles that file using a tex distribution and generates a cool looking pdf file.
# Setup Environment
* This program only works on linux distribution because it uses some system calls which are
not available on windows.
* Install texlie(a TeX distribution)  "sudo apt-get install texlive-full" Instead, you can use this online tool https://www.sharelatex.com
* Install g++ compiler "sudo apt-get install g++"
# How to use
Run the source code (Codebocks/Terminal both works ne). This program scans two input
string from standard input stream. First one is absolute location, that contains all your souce
codes. Second one is the file name, that you want to save as. If no error occures, outputs
(.tex, .pdf, .log, .aux, .toc) will be saved in 'tex output' under 'Documents' directory.
# Program Behaviour
* Notice that your given source codes' location can contain both sub-folders and source
code files. Folder name will be used as section name like given screenshot.

image is not ready

* If a source code is located at 4 layer(by layer I mean folder under folder) further than
given location, that file will be ignored.
* Files that do not have any of .cpp , .py , .c , .java extensions will be ignored.
* Highly Recommanded not to use special characters in source file name and anywhere
of code.
* Ingore White Spaces in path name and file names.
* Program may contain bugs.
