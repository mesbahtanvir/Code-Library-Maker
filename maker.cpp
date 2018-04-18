#include <bits/stdc++.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#define _DIR 0
#define _REG 1
using namespace std;

// global Variables
FILE *output;
string OUTPUT_DIR;
string texFileName;



/**
	add respective suffix to allow more language
**/
// to make 1 based indexing empty string is addeds
vector < string > allowedSuffix ({"",".cpp",".c",".py",".java"});
vector < string > language      ({"","C++" , "C", "Python","Java"}); 

/**
	hierarchys
**/
vector < string > level ( {"section", "subsection", "subsubsection", "paragraph", "subparagraph" } );

/**
	get user name
**/
/**
	if you know your username (whoami) then you can 
	directly return your username.
	this is needed for saving file under document folder.
**/
string whoami()
{
   FILE* fp = popen("/usr/bin/whoami", "r");
   if (fp)
   {
      char info[256];
      fgets(info, sizeof(info), fp);
      pclose(fp);
      string res;
      /**
       	removing newline,whitespace from your name
      **/
      string data = info;
      for(int i = 0 ; i < data.size() ; i++ ){
      	char c = data[i];
      	if( c != '\n' and c != ' ') res += c;
      }
      return res;
   }
   cout << "can not get who am i\n" ;
   cout << "Closing .... ... .. .\n" ;
   exit( -1 );
   return "";
}

/**
	store a char to tex output file
**/
void storeData(char c)
{
	// special character for tex
	if(c=='^') {
		fputs("\\textasciicircum",output);
		return ;
	}
	if(c == '_' ) c = ' ';
	if( c == '#' ) fputc(  '\\' , output);
	fputc( c, output);
}

/**
	store string to tex output file
**/
void storeData(string str){
	int sz = str.size();
	for( int i = 0 ; i < sz ; i++ ){
		storeData(str[i]);
	}	
}


/**
	checking if rhs is a suffix of lhs 
**/
bool isSuffix(string lhs , string rhs)
{
	if( lhs.size() < rhs.size()) 
		return false;
	
	if( lhs.substr( lhs.size() - rhs.size(), rhs.size()) == rhs) 
		return true;
	
	return false;
}



/**
	checking if document is a source code,
	return index of language ordered in allowedSuffix
	return 0 if fileName is not a allowed source code
**/
int isSourceCode(string fileName)
{
	int sz = allowedSuffix.size();
	for( int i = 1 ; i < sz;  i++ )
	{
		string suffix = allowedSuffix[i];
		if( isSuffix( fileName, suffix) ) 
			return i;
	}
	return 0;
}

/**
	remove .cpp, .java, .py extension from fileName
**/

string removeFileExtension(string  fileName ){
	int sz = allowedSuffix.size();
	for(int i =1 ; i < sz;i++)
	{
		string suffix = allowedSuffix[i];
		if( isSuffix( fileName, suffix) ) {
			return fileName.substr(0,fileName.size()-suffix.size());
		}
	}
} 


/**
	Get vector of all element (DIRECTORY and REGULAR file) under given path.
	set success to 1 if path can be opened, 0 otherwise.
	First element of pair is a string data type which hold the name of 
	DIRECTORY or REGULAR file. Second element hold 1 if that element is a REGULAR file
	0 if that element is a DIRECTORY.
	
**/
vector < pair < string  , int >  > getElement(string path, bool &success){
    vector < pair < string  , int > > data;
    DIR *dir;
    struct dirent *ent;
    if( ( dir = opendir( path.c_str() ) ) != NULL )
    {
        success = true;
        while( (ent = readdir ( dir )) != NULL )
        {
            if(ent -> d_type == DT_DIR)
            {
                //avoiding hidden folder
                if(ent -> d_name[0] != '.')
                	data.push_back ( make_pair ( ent -> d_name , _DIR ) );
            }
            else if( ent -> d_type == DT_REG ) 
            {
            	//avoiding hidden file
           		 if(ent -> d_name[0] != '.')
           		 {
           		 	if(isSourceCode( ent -> d_name))
           		 	{
           		 		data.push_back ( make_pair ( ent -> d_name , _REG ) );
           		 	}
           		 }
            		 
            }
         }
        closedir ( dir );
    }
    else
    {
        cout << "[Minor Error] Can't Open directory -->" << path << endl;
        success = false;
    }
    return data;
}

void printFile(string path , string fileName, string tagName){
	
	storeData( "\\" );
	storeData( tagName );
	storeData( "{" );
	int lan = isSourceCode(fileName);
	storeData( removeFileExtension( fileName ) ) ;
	storeData( "}\n" );
	storeData( "\\begin{lstlisting}" );
	storeData( "[language =" + language[lan] + " ,linebackgroundcolor={\\ifodd\\value{lstnumber}\\color{gray!10}\\fi}]\n");
	
	FILE *file;
	
	file = fopen(path.c_str(), "r");
	
	if( file ){
		char c;
		while( EOF != (c = getc(file) ) )
		{
			fputc(c, output);
		}
		fclose( file );
	}
	else 
	{
		cout << "[Minor Error] can't not open -->" << path << endl;
		cout << "[Recovery] output field will be blank\n";
	}
	storeData("\\end{lstlisting}\n");
}

/**
	recursivly taking all source code from given path and go deeper directory.
	remember highest depth is 4. File that locate deeper than 4 sub directory 
	will be ignored.
**/

void printDirectoryData(string path, string name, int lvl){
	// maximum depth is 4
	if( lvl >= 4 )  return ; 
	bool success;
	auto element = getElement(path,success);
	
	/**
		 main given directory is not a document element 
	**/
	if( lvl != -1 )
	{
		storeData("\\");
		storeData(level[lvl]);
		storeData("{");
		storeData(name);
		storeData("}\n");
	}
	
	if(success){
		vector < string > dirs, files; 
		for( auto item : element) {
			if(item.second == _DIR ) dirs.push_back(item.first);
			else files.push_back(item.first); 
		}
		// you can change the order
		sort( dirs.begin(), dirs.end() ) ;
		sort( files.begin(), files.end() ) ;
		
		for( auto fileName : files ) {
			printFile(path+"/"+fileName,fileName,level[lvl+1]);
		}
		for( auto dirName : dirs ) {
			printDirectoryData(path+"/"+dirName,dirName,lvl+1);
		}
	}
	else {
		cout<< "[Minor Error] The File of "<< path <<" will be missing on document due to Error\n"; 
	}
}
void header(){
	fputs("\\documentclass[10pt]{article}\n",output);
	fputs("\\usepackage{listings}\n",output);
	fputs("\\usepackage[utf8]{inputenc}\n",output);
	fputs("\\usepackage[english]{babel}\n",output);
	fputs("\\usepackage{courier}\n",output);
	fputs("\\usepackage{lstlinebgrd}\n",output);
	fputs("\\inputencoding{latin1}\n",output);
	
	fputs("\\usepackage[textwidth=6.5in,textheight=9.5in]{geometry}\n",output);
	fputs("\\addcontentsline{toc}{section}{Algorightm}\n",output);
	fputs("\n",output);
	fputs("\\setcounter{tocdepth}{4}\n",output);
	fputs("\\usepackage{fullpage}\n",output);
	fputs("\\usepackage{xcolor}\n",output);
	fputs("\\lstset{ basicstyle = \\ttfamily}\n",output);
	fputs("\\lstset{ keywordstyle  =    \\color{blue}\\ttfamily}\n",output);
	fputs("\\lstset{ stringstyle  =    \\color{red}\\ttfamily}\n",output);
	fputs("\\lstset{ commentstyle = \\color{gray}\\ttfamily}\n",output);
	fputs("\\lstset{ numberstyle    =   \\small }\n",output);
	fputs("\\lstset{ showstringspaces   =   false}\n",output);
	


	fputs("\\lstset{ tabsize    =   4}\n",output);
	fputs("\\lstset{ rulesepcolor   =   \\color{white} captionpos=b}\n",output);
	fputs("\\lstset{ breaklines =   true}\n",output);
	fputs("\\lstset{ breakatwhitespace  =   false}\n",output);
	fputs("\\lstset{ numbers    =   left,   xleftmargin=    0em}\n",output);
	fputs("\\begin{document}\n",output);
	fputs("		\\begin{titlepage}\n",output);
	fputs("       \\centering\n",output);
	fputs("				{\\Large{CODE LIBRARY} }\n",output);
	fputs("		\\end{titlepage}\n",output);
	fputs("\\tableofcontents\n",output);
	fputs("\\clearpage\n",output);
}
void footer(){
	fputs("\\end{document}\n",output);
	fclose(output);
}
void makeLibrary(string &targetLocation){
	header();
	printDirectoryData(targetLocation, "" , -1);
	footer();
}

void  setupOutputDirectory()
{
	// change here to save tex file in another directory
	OUTPUT_DIR = "/home/"+whoami()+"/Documents/tex_output";
	
	if( mkdir( OUTPUT_DIR.c_str(), S_IRWXU ) )
	{
		if( errno != EEXIST )
		{
			cout << " [Major Error] can't touch directory " + OUTPUT_DIR << endl; 
			cout << " Closing Program! .... ... .. .\n";
			exit( -1 );
		}
	}
}


// 
bool openOutputFile( string outputFileName ){
	int cnt = 0;
	texFileName = (OUTPUT_DIR) + string("/") + outputFileName+".tex"; 
	if( (output = fopen(texFileName.c_str(),"w") ) )
	{
	}
	else {
		cout << "[Majog Error] can not open: " + texFileName << endl;
		cout << "Closing Program! .... ... .. .\n" ;
		exit( -1 );
	}
}

// location name, output file name
void takeInput( string &targetLocation, string &outputFileName ){
	cout << "Enter absolute path of source codes:\n" ;
	getline( cin, targetLocation );
	cout<< "Latex file save as?: \n";
	getline( cin, outputFileName );
}

int main()
{
	string targetLocation , outputFileName;
	takeInput( targetLocation, outputFileName );
	setupOutputDirectory();
	openOutputFile( outputFileName );	
	makeLibrary(targetLocation);
	
	
	cout<<"Tex File is successfully generated\n";
	cout<<"Want to generate pdf file? (Y/n):\n";
	string str;
	cin>>str;
	
	if(str[0]=='Y' or str[0] == 'y' or str[0] == '1') {
			/**
				installing texlive-full 
			**/
				//	system("sudo apt-get install texlive-full");
			/**
				command must be run twice to get table of content 
			**/
			system(("pdflatex -output-directory " + OUTPUT_DIR + " " + texFileName).c_str() );
			system(("pdflatex -output-directory " + OUTPUT_DIR + " " + texFileName).c_str() );
	  	return 0;
	}
}
