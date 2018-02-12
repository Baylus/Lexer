/*
Baylus Tunnicliff	July 14th, 2017		PRE CS 210 Assignment 1
CCX Lexer
*/
#include <stdio.h>	//standard io stuff;
#include <stdlib.h>	//useful things;
#include <string.h>	//strcmp, 
#include <math.h>	//pow, round/roundf, exp2, 
	#include <tgmath.h>	//exp2 for wormulon.
#include <ctype.h>	//isspace, 
// #include "uthash.h"	//UT_hash_handle

// #define DEBUG
// #define DEBUG_G1	//for debugging group 1 ("...G1") functions.
// #define DEBUG_LOOP	//for debugging lexeme-loop and functions within it 
// #define FREQUENCY


/* Function Groups 
	Group 1 - Organizing and inputting the balanced binary search trees for the keyword and operator instances. Signed with "...G1"		!!!!!!!! REPLACE G1 for something descriptive!!!!!!!
	Group 2 - Category Functions responsible for reading their respective lexeme and calling the print function. Signed with "...Category"
	
	Group 4 - Functions related to counting, and storing, the information regarding the number of times each lexeme occurs. Signed with "...Frequency"
*/



/*
Lexeme Categories
1. Comment - If word starts with "/_*", then read everything until "*_/"
2. Identifier - can be letters, digits, and underscores, but !!MUST!! begin with a letter.
3. String - Begins with a ' "... ' , ends with a ' ..." '
4. Keyword - list of keywords expected of our lexer to recognize.
		accessor and array begin bool case else elsif end exit function if in integer
		interface is loop module mutator natural null of or others out positive procedure
		return struct subtype then type when while
5. Character Literal - any single character enclosed with single quotes; e.g. 'x', 'u', '#'....
6. Operator - list of operators that must be recognized by our lexer. (each operator is surrounded by double-quotes)
		"." "<" ">" "(" ")" "+" "-" "*" "/" "|" "&" ";" "," ":" "[" "]" "=" ":=" ".."
		"<<" ">>" "<>" "<=" ">=" "**" "!=" "=>"
7. Numeric Literal - Each numeric literal encountered by your lexer will start with a decimal digit and will contain only the following:
		– decimal digits (0 through 9)
		– hexadecimal digits (A through F and/or a through f)
		– the special characters ’ ’, ’.’, and ’#’.
8. UNK (Unknown Category)
*/

long frequency[8];	//will contain the value of the frequency of each lexeme

/* Linked List Definition */
typedef struct node {
	char* data;
	struct node* next;
} llnode;

llnode* keywordslist = NULL;
llnode* operatorslist = NULL;

/* Tree node to contain the character*/
typedef struct treenode {
	char* info;
	struct treenode *right;
	struct treenode *left;
} Tnode;
/* Binary Search Trees to store the keywords/operators */
Tnode * keywordstree = NULL;
int keywordsDepth;
Tnode * operatorstree = NULL;
int operatorsDepth;

FILE* outfile = NULL;
FILE* infile = NULL;

char* ReadFile(FILE*);
FILE* getOutput(char* input);
FILE* openinFile(char* x);
void printLexeme(char*, int);
void AddToString(llnode**, char**);
int KeywordsG1();
int OperatorsG1();
char** ListToArrayG1(llnode*);
void searchTreeBalanceG1(Tnode** , float, char**);
int LookupOperatorG1(char*);
int LookupKeywordG1(char*);
void freeList(llnode* listptr);
Tnode* newNode(char**);
void freeTree(Tnode* treeptr);
unsigned int round_closest(unsigned int dividend, unsigned int divisor);

/* Category Functions */
/*
Biggest question is how to pass the file array into the category functions, have them handle finding the whole lexeme,
then be able to exit the functon while incrementing the counter in the main function correctly
I am considering two methods of acheiving this;
	1. Have category functions return an int of # of chars read, then use this number to increment the counter in main.
	2. Or, set the character array passed into the category functions to the address of the correct spot to read in the char array,
		On further thought, this would segment the file array so terribly that it would actually be pretty gross and im not exactly sure what 
			problems it would lead to, but the thought of what could go wrong because of doing it this way is actually terrifying.
		Plus, the first of the two seems to be the much more understandable approach, so i wont lose myself due to a very silly value judgement when
			reading this code in a few years.
Very heavily consider removing the arguments of the form "char**" in some of the functions, these seemed useful, but in practice, were pointless,
	I want to remove them already, before having finished the assignment, but am leaving them around just incase they become useful. Maybe for debugging
	they might be useful.	###################
*/
int commentCategory(char** comment, char* file);
int stringCategory(char* file);
int numericCategory(char** num, char* file);
int operatorCategory(char* file);

int identifyChar(int c);
int identifyNumericLiteral(int c);

void ReadFrequency();
void CountFrequency(int t);
void WriteFrequency();

void error(const char * input);
void PrintTree(Tnode*);
void PrintList(llnode*);
void prints(char*);
void printsG1(char*);
void printsloop(char*);

//http://troydhanson.github.io/uthash/index.html
//hash-table/index information

// char** keywords;
// char** operators;

char* lexemes[8] = {"comment", "identifier", "string", "keyword", "character literal", "operator", "numeric literal", "UNK"};
//!!!!!!!!!!! not used anymore, will keep until trimming stage of code completion just incase i forgot i used it somewhere.

int main(int argc, char* argv[]) {
	//check if arguement was provided
	if (argc < 2) error("Invalid Arguement #.\tExpected 1 command line arguement (input filename)\n");
	
	//outfile = getOutput(argv[1]);
	//if (!outfile) outfile = stdout;		//if outfile failed to open, assume stdout.
	prints("Starting keywords");
	//KeywordsG1();
	prints("Printing keywords");
	// PrintTree(keywordstree);
	
	prints("\nStarting operators");
	//OperatorsG1();
	prints("Printing operators");
	// PrintTree(operatorstree);
	
	/* Moving on to assignment */
	FILE* inputfile = openinFile(argv[1]);
	
	char* file = ReadFile(inputfile);
	if (file == NULL)	error("File is empty. (?)");
	if (inputfile != NULL) { 
	  fclose(inputfile);
	  inputfile = NULL;
	}
	char c;
	int i = 0;
printsloop("Starting Lexeme Loop");

	while (file[i] != '\0' && file[i] != EOF)	{
		c = file[i];	//this line seems a tiny bit pointless to assign a whole variable for a very simple thing.
		//c is the character being analyzed at the current moment.
		if (isspace(c)) {
			//if 'c' is a whitespace character.
			//ignore whitespace.
			++i;
		} else if (file[i] == '/' && file[i + 1] == '*') {	/* Check for comment */
printsloop("PL: Comment");
//char* comment = malloc(sizeof(char*));	//give address to array to pass to function.
				i += commentCategory(NULL, &file[i]);
#ifdef DEBUG
// printLexeme(comment, 0);
#endif
				//free(comment);		//HEAVILY CONSIDER REMOVING THIS WHOLE MECHANISM OF USING THE COMMENT OUTSIDE OF THE FUNCTION IT IS CREATED IN..
				//CONSIDER THIS A RELIC OF POOR FORESIGHT.
		} else if (identifyChar(c) == 0) {	//if c is a letter.
printsloop("LP: Letter");
			//then, the following lexeme is either a identifier or keyword. Going to eliminate keyword if anything other than char found.
			//the following code in this conditional could be replaced with a function to represent current state of identifier/keyword lexeme.
			int bool_letteronly = 1;
			int tmpcounter = 0;
			char word[256] = {0};
			int result = 0;
			while((result = identifyChar(file[i])) != -1) {	//while character is a letter, digit, or underscore.
				word[tmpcounter++] = file[i++];	//scale i with new counter so no housekeeping addition necessary.
				if (result != 0) {	//if char is not a letter
					bool_letteronly = 0;
printsloop("test");
				}
			}
			word[tmpcounter] = '\0';
			if (bool_letteronly == 1) {	//only letters in the word
printsloop("test2");
				//check if word is keyword
				if (LookupKeywordG1(word) == 1) {	//if word is a keyword.
					printLexeme(word, 3);
				} else {	//else, must be an identifier.
					printLexeme(word, 1);
				}
			} else { //if not only letters, must be identifier
				printLexeme(word, 1);
			}
		} else if (c == '\'') {	//if lexeme is character literal.
printsloop("LP: Char Lit");
			char charliteral [3];
			int tmpcount;
			for (tmpcount = 0; tmpcount < 3; ++tmpcount) {
				charliteral[tmpcount] = file[i++];
			}
charliteral[3] = '\0';
			printLexeme(charliteral, 4);
		} else if (identifyNumericLiteral(c) == 0) {	//if c is a decimal digit
			//this lexeme should likely be a numeric literal.
printsloop("LP: Numeric");
//char* numericliteral = malloc(sizeof(char*));
			i += numericCategory(NULL, &file[i]);
			//free(numericliteral);
		} else if (c == '"') {
			//string
printsloop("LP: String");
			i += stringCategory(&file[i]);
		} else	{	//remaining stuff (operators are included in here)
printsloop("LP: End");
			int localOperatorResult = operatorCategory(&file[i]); 
			/* check for operator at file position, return # of char's in operator found, 
			if no operator found, return 0 */
			
			if (localOperatorResult != 0) {	//if an operator was found.
				i += localOperatorResult;
			} else {	//no operator found.
				
				/*	UNK	*/
				int tempcounter = 0;
				while(file[i + tempcounter] != '\0' && !isspace(file[i + tempcounter++])) {	//while file not done, and char isnt a whitespace char.
					//this is to find length of UNK lexeme.
				}
				char* lexemeUNK = malloc(sizeof(char) * tempcounter);
				int templength = tempcounter;
				tempcounter = 0;
				while(i < (i + templength)) {
					lexemeUNK[tempcounter++] = file[i++];	//increment through original file pointer while reccording the unknown lexeme.
				}
				printLexeme(lexemeUNK, 7);	//might have to change this declaration.
				free(lexemeUNK);				
				lexemeUNK = NULL;
			}
		}
	}
	free(file);
	file = NULL;
	return 0;
}	//END main()

/*
Reads from 'file' array. ends when a '*' followed by a '/' is found.
Dynamically allocates the size of array given by address of comment.
	I have forgotten the reason why i have included the ability to pass the address of the comment
		into the function to be able to use the comment outside of this function. It could possibly be useful, but it could
		also be entierly worthless, !!!!!!!!!!!!!!!!!! Consider removing this from the final draft if no purpose discovered !!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
int commentCategory(char** comment, char* file) {
prints("Starting commentCategory()");
	if (file[0] != '/' && file[1] != '*') {	/* If category function was called incorrectly. */
		perror("commentCategory():: Incorrectly called function.\tlexeme does not satisfy comment requirements");
	}
	
	
	char* temptr = NULL;
	if (comment != NULL) {
	  temptr = (*comment);	//make new char* to reference arg1.
	}
	int i = 0;
	int maxSize = 32;
	temptr = malloc(maxSize * sizeof(char));	
	//i believe this should work since comment was initialized with a malloc call for a char*, 
	//therefore it should simply be holding an address
	/* Essentially, I initialize the address of the pointer outside of function, 
		so that i can initialize size of array (dynamically) within this function */
	
	while(1) {
		temptr[i] = file[i++];	//1. assign variable, don't increment.
		//printf("%c %d\n", temptr[i - 1], i - 1);
		if (file[i - 1] == '/' && file[i - 2] == '*') {	//2. check end lexeme condition, post-increment crucial for retaining proper # of int's read.
/* temptr[i - 1] == last char read | temptr[i - 2] == second to last char read */
			//lexeme finished.
			//might have to check here whether or not i == maxSize. Behavior based on implementation when new_size == old_size for realloc function.
#ifdef DEBUG
		  printf("lexeme finished: %s, new size == %d",temptr,  i);
#endif
		  //printf("lexeme finished: %s, new size == %d",temptr,  i);
//temptr = realloc(temptr, sizeof(char) * i);
				//ensure that realloc did not return NULL.
//if (temptr == NULL) perror("Realloc failed memory allocation in func commentCategory.\n");
			break;
		} else if (i == maxSize) {	//or check whether maxSize has been reached.
			/*
			Post increment in comment (2.) two is important for this conditional statement, if there was no increment,
			then 'i' would be too small and would result in the following char "read" replacing the null terminating character at the end of the string.
			*/
			maxSize *= 2;
			char* ptr = realloc(temptr, sizeof(char) * maxSize);
			if (ptr != NULL) {
			  temptr = ptr;
			} else {
			  free(temptr);
			  temptr = NULL;
			  return -1;
			}
		} else {
			
		}
	}
	char* ptr = realloc(temptr, sizeof(char) * (i + 1));
	if (ptr == NULL) {
	  perror("Realloc failed mem alocation in commentCategory.");
	  free(temptr);
	  temptr = NULL;
	  return -1;
	} else {
	  temptr = ptr;
	}
	temptr[i] = '\0';
	printLexeme(temptr, 0);
	free(temptr);
	temptr = NULL;
	return i;	//Return number of int's read.
}	//END commentCategory().
/*
Takes file char pointer into the function to then create the string manually.
Assumes that the string (including the "'s)
*/
int stringCategory(char* file) {
	if (file[0] != '"') {	//Make sure function called correctly.
		perror("Function stringCategory called incorrectly, first char is not '\"'\n");
	}
	int i = 1;
	char string[256] = {0};
	string[0] = file[0];	//include first 
	while(file[i] != '\0') {
		string[i] = file[i];
		if (file[i++] == '"') {	//string done.
			break;
		}
	}
	printLexeme(string, 2);
	return i;
}

int numericCategory(char** numeric, char* file) {
	if (identifyNumericLiteral(file[0]) != 0) {	//if first character is not a decimal digit.
		//checking proper calling of function
		perror("Failed to call numericCategory correctly: first character is not a decimal digit.\n");
	}
	int i = 0;
	int maxSize = 64;
	char* tmptr;
	if (numeric != NULL) {
	  tmptr = (*numeric);	//pointer to make accessing the variable (*numeric)	easier.
	}
	tmptr = calloc(maxSize, sizeof(char));
	while(file[i] != '\0' && identifyNumericLiteral(file[i]) != -1) {
		tmptr[i] = file[i++];
		if (i == maxSize) {
			maxSize *= 2;
			tmptr = realloc(tmptr, sizeof(char) * maxSize);
		} else if (i > maxSize) {	//really bad if this happens. could lead to very bad errors if trying to print the variable that i am editing.
			perror("Error in numericCategory; i (counter) is greater than maxSize!\n");
		}
	}
// printf("Debug numeric is:%s\n", tmptr);
	tmptr = realloc(tmptr, sizeof(char) * (i + 1));
	if (tmptr == NULL) {
	  perror("realloc failed in numericCategory.");
	  return -1;
	}
	tmptr[i] = '\0';
// tmptr[i] = 0;
	printLexeme(tmptr, 6);
	free(tmptr);
	tmptr = NULL;
	return i;
}

/*
returns num of chars taken from the file array, if no operators were found, returns 0.
*/
int operatorCategory(char* file) {
prints("Start operatorCategory");
	// int i = 0;
	char operator[2];
	operator[0] = file[0];
	operator[1] = file[1];
	operator[2] = '\0';
	if (LookupOperatorG1(operator) == 1) {	//found double operator
		printLexeme(operator, 5);
		return 2;
	} else {
		operator[1] = '\0';
		if (LookupOperatorG1(operator) == 1) {	//found single operator
			printLexeme(operator, 5);
			return 1;
		}
	}
	//if program reaches this point, no operators found within the next 2 characters (limit of operators)
	return 0;
}

/*
Takes input file, dynamically allocates size for a character array, reads a character at a time, if needed, more space is allocated. 
Closes the input file after done. 
Returns a char* to the char string that holds the file's contents. 
*/
char* ReadFile(FILE* infile) {
	char c = '\0';
	int i = 0;	//counter for size of current word.
	int maxsize = 256;
	char* temp = malloc(sizeof(char) * maxsize);
	if (temp == NULL) perror("Malloc Returned NULL.");
	while (c != EOF) {
		c = fgetc(infile);
		/* If no room left in buffer */
		if (i == maxsize) {	/* Allocate more room */
			maxsize *= 2;	/* Double maxsize */
			temp = realloc(temp, sizeof(char) * maxsize);
		}
		temp[i++] = c;
		// ++i;
	}
	//temp = realloc(temp, sizeof(char) * (i + 1));
	temp[i] = '\0';		/* NULL-terminate the file string */
	//fclose(infile);
	return temp;
}	/* END ReadFile() */



/* Takes a string and a character
	if string is null, uses last provided string position
	Call function with intial string pointer and after that, simply call with null and the character. 
	actually, just call with the char*, the function creates the string */
void manageStringSize(char** string, char c) {
	static char* tempstr;
	static int counter = 0;
	static int maxsize = 64;
	if (string == NULL) {
		tempstr = (*string);
		counter = 0;
		maxsize = 64;
		tempstr = malloc(sizeof(char) * maxsize);
	}
	
	if (counter == maxsize) {
		maxsize *= 2;
		tempstr = realloc(tempstr, sizeof(char) * maxsize);
	}
	tempstr[counter++] = c;
	// ++counter;
	
}

#define AUTO_OUTPUT_NAME
/*
If this is defined, it takes the input file name, appends ".txt" onto the name and outputs there.
*/
FILE* getOutput(char* inputfile) {
	FILE* tempfileptr = NULL;
#ifdef AUTO_OUTPUT_NAME
//if auto name is desired.
	int inlength = strlen(inputfile);
	char* filename = malloc(sizeof(char) * (inlength + 10));
	strcpy(filename, inputfile);
	strcat(filename, ".txt");
#else
//if desired to provide file name
	char filename[128];
	printf("Output File Name (Empty = standard output) ");
	fgets(filename, 128, stdin);
//https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
	filename[strcspn(filename, "\n")] = 0;	//"chomp" the newline char from the array.
#endif
	if (filename[0] == 0) {
		//Empty input provided; No output file requested
		return NULL;
	} else if (strlen(filename) != 0) {
		//filename provided, try to open.
		if ((tempfileptr = fopen(filename, "w")) == NULL)
			perror("lexer.c:: getOutput() _ Error Opening File");
#ifdef DEBUG
printf("Debug: Input file name = %s\n", inputfile);
#endif
	} else {
		error("lexer.c:: getOutput() _ Really shouldnt ever happen.");
	}
free(filename);
 filename = NULL;
	return tempfileptr;
}	//END getOutput()

FILE* openinFile(char* x) {
	FILE* tempfileptr = NULL;
	if ((tempfileptr = fopen(x, "r")) == NULL)
		error("lexer.c:: outFile() _ Error Opening File");
	else {
#ifdef DEBUG
printf("Succesfully opened %s\n", x);
#endif
	}
	return tempfileptr;
}	//END openFile()

/*
0 == letter (ASCII: 65-90, 97-122)
1 == Digit (48-57) or underscore (95)
-1 == none of the above.
*/
int identifyChar(int c) {
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		//is letter.
		return 0;
	} else if ((c >= '0' && c <= '9') || c == '_') {
		//is a digit or underscore.
		return 1;
	} else {
		//none of the above.
		return -1;
	}
}

/*
Evaluates a char for the purpose of discovering it's validity as a numeric literal
returns 0 if c == 0-9
returns 1 if c is HEX digit (A-F, a-f) 
returns 2 if c is '_', '.', '#' (special char)
returns -1 if c is none of the above.
*/
int identifyNumericLiteral(int c) {
	if (c >= '0' && c <= '9') {
		//if c is a decimal digit
		return 0;
	} else if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
		//if c is a hexadecimal digit
		return 1;
	} else if (c == '_' || c == '.' || c == '#') {
		//if c is a special character
		return 2;
	} else {
		return -1;
	}
}

/*
Lexeme Categories
0. Comment - If lexeme starts with "/_*", then read everything until "*_/"
1. Identifier - can be letters, digits, and underscores, but !!MUST!! begin with a letter.
	Valid ASCII Codes: A-Z (65-90), a-z (97-122 [+32]), 0-9 (48-57), '_' (95).
2. String - Begins with a ' "... ' , ends with a ' ..." '
3. Keyword - list of keywords expected of our lexer to recognize.
		accessor and array begin bool case else elsif end exit function if in integer
		interface is loop module mutator natural null of or others out positive procedure
		return struct subtype then type when while
4. Character Literal - any single character enclosed with single quotes; e.g. 'x', 'u', '#'....
5. Operator - list of operators that must be recognized by our lexer. (each operator is surrounded by double-quotes)
		"." "<" ">" "(" ")" "+" "-" "*" "/" "|" "&" ";" "," ":" "[" "]" "=" ":=" ".."
		"<<" ">>" "<>" "<=" ">=" "**" "!=" "=>"
6. Numeric Literal - Each numeric literal encountered by your lexer will start with a decimal digit and will contain only the following:
		– decimal digits (0 through 9)
		– hexadecimal digits (A through F and/or a through f)
		– the special characters ’ ’, ’.’, and ’#’.
7. UNK (Unknown Category)

Instead of using an integer to designate category, 
it is more readable and easily understood to simply use the string of the category rather than reference an int.
*/
void printLexeme(char* lexeme, int category) {
prints("printLexeme:");
	//old outdated garbage
	if (outfile != NULL) {
		fprintf(outfile, "%s (%s)\n", lexeme, lexemes[category]);	//Output file defined
	}
	printf("%s (%s)\n", lexeme, lexemes[category]);	//no output file given.
	
	CountFrequency(category);
}

/*
Adds string to list. Sorts "Alphabetically", (Actually using strcmp())
!!!!!!!!!!!!!!!!! Return -1 if error.
*/
void AddToString(llnode** head, char** s) {
if (strcmp((*s), "/") != 0) {
}
	// PrintList((*head));	
// prints("Calling AddToString :: ");
// printf("%s\n", (*s));
	llnode *curr = (*head), *prev = curr, *temptr = malloc(sizeof(llnode));
	temptr->data = malloc(sizeof(char*));
	strcpy(temptr->data, (*s));
	//temptr->data = *s;
temptr->data[strcspn(temptr->data, "\n")] = 0;	//"chomp" the newline char from the array.
// prints("Finding Spot");
	
	if ((*head) == NULL) {	//if list was empty
		(*head) = temptr;
		return;
	}
int i = 0;
// printf("strcmp: ");
	while(strcmp(temptr->data, curr->data) > 0 && curr->next != NULL){
// prints(curr->data);
		prev = curr;
		curr = curr->next;
// printf("%s-%s:%d\t", temptr->data, curr->data, strcmp(temptr->data, curr->data));
		if (++i > 35) error("AddToString broke");
	}	//found spot to place or found a repeated word.
	
// prints("Found Spot");
	
	if (curr == prev) {	//word belongs in the front.
		temptr->next = (*head);
		(*head) = temptr;
	} else if (strcmp(temptr->data, curr->data) < 0) {	//temptr belongs in between prev and curr.
		prev->next = temptr;
		temptr->next = curr;
	} else if (curr->next == NULL) { //reached end of list.
		temptr->next = NULL;
		curr->next = temptr;
	} else  if (strcmp(temptr->data, curr->data) == 0) {	//a word was repeated.
		//dont add the word.
		free(temptr->data);	//Can't remember if this is illegal because the freeing of the memory is handled by something else, since the memory comes from getline() and/or (not sure which) strtok()...
		temptr->data = NULL;
		temptr->next = NULL;
		free(temptr);
		temptr = NULL;
	}
}	//END AddToString()

/*
Returns the # of keywords/operators read.
Opens "keywords.txt"/"operators.txt", respectively, and reads them, organizes them by ASCII code value (pseudo alphabetical) in a linked list,
	places words into a array of strings, deconstructs linked list,
	
*/
int KeywordsG1() {
	int keywordCount = 0;
	char* keywordsfilename = "keywords.txt";
	FILE* keyfile = openinFile(keywordsfilename);
	if (!keyfile) error("lexer.c::KeywordsG1 _ file somehow passed incorrectly.");
	
/*
https://stackoverflow.com/questions/16400886/reading-from-a-file-word-by-word
https://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
Solution to read a file line by line without assuming its contents nor it's length.

Not using that solution, but see references if POSIX 7 unusable/undesirable.
*/
	char* line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	read = getline(&line, &len, keyfile);	//this includes a newline character at the end of every line, need to "chomp" that.
	if (read == -1) {
		prints("KeywordsG1 _ read is -1");
	}
	while (read != -1) {
/*
https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
Elegant solution to "chomp" issue, using strcspn.
*/
		if(!line) error("KeywordsG1 _ scary stuff going on here.");
		line[strcspn(line, "\n")] = 0;	//newline character removed from string.
			/*strcspn searches in arg1 for characters in arg2, and returns the value of the first occurence.
				so this line searches for any newlines in "line" and sets the index of the first occurrence to null.*/
		char* temp = strtok(line, " ");
		while (temp != NULL) {	//token up different words.
			AddToString(&keywordslist, &temp);
			// free(temp);
			++keywordCount;
			temp = strtok(NULL, " ");
			// sleep(1);
		}	//put tokened string into list.
		
		// if (line == NULL) break;
		
		
		read = getline(&line, &len, keyfile);
// printf("%s", line);
	}
	//Make the Linked List into an array of strings.
	char** keywords = ListToArrayG1(keywordslist);
	// keywords = ListToArrayG1(keywordslist);
	//use keywords to make tree.
	int n = 0;
//Consider Removing the follwing stuff, it has already been included in the searchTreeBalanceG1, 
//which is the only thing it is used for other than debugging
	while (((++n * n) - 1) < keywordCount)
		//Incrementing clock. Finds the depth of the balanced Binary Search Tree to be made from keywords.
	keywordsDepth = n;
prints("Keywords;");
// prints(keywordCount);
// printf("%d\n", keywordCount);
free(line);
	searchTreeBalanceG1(&keywordstree, keywordCount, keywords);
	return keywordCount;	//###############NOT EVEN IMPORTANT TO RETURN CONSIDER REMOVING.
}


int OperatorsG1() {	//most comments in KeywordsG1() apply to this one, just for operators instead.
	int operatorCount = 0;
	char* operatorsfilename = "operators.txt";
	FILE* opfile = openinFile(operatorsfilename);
	if (!opfile) error("lexer.c::OperatorsG1 _ file somehow passed incorrectly.");
	
	char* line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	read = getline(&line, &len, opfile);
	while (read != -1) {
		if(!line) error("OperatorsG1 _ scary stuff going on here.");
		line[strcspn(line, "\n")] = 0;	
		char* temp = strtok(line, " ");
		while (temp != NULL) {
			AddToString(&operatorslist, &temp);
			++operatorCount;
			temp = strtok(NULL, " ");
			// sleep(3);
		}
		read = getline(&line, &len, opfile);
	}
	char** operators = ListToArrayG1(operatorslist);
	// operators = ListToArrayG1(operatorslist);
	int n = 0;
	while (((++n * n) - 1) < operatorCount)
		//Incrementing clock. Finds the depth of the balanced Binary Search Tree to be made from operators.
	operatorsDepth = n;
prints("Operators;");
// prints(keywordCount);
// printf("%d\n", operatorCount);
	searchTreeBalanceG1(&operatorstree, operatorCount, operators);
free(line);
	return operatorCount;	//!!!!!!!!!!!!!NOT EVEN NECESSARY TO RETURN THIS VALUE!!!!!!!!!!!###############
}


/* Destroys a list and turns it into an array of strings */
char** ListToArrayG1(llnode* head) {
	prints("Calling ListToArrayG1 :: ");
	int i = 0;
	llnode* temptr = head;
	while (temptr != NULL) {
		++i;
		temptr = temptr->next;
	}
	char** words = malloc(sizeof(char*) * i);
	int templen = i;
	i = 0;
	llnode* prev = head;
	while (i < templen) {
		words[i++] = head->data;
		head = head->next;
		prev->data = NULL;
		prev->next = NULL;
		free(prev);
		prev = head;
	}
	return words;
}


// #define LEFT_RIGHT_BALANCE
/* 
calulates optimal indeces for a balanced binary search tree, places words into the tree, 
int num 
*/
void searchTreeBalanceG1(Tnode** p, float num, char** words) {
	prints("Calling searchTreeBalanceG1 ::");
	float root, tempnum, tempnum2;
	root = num / 2.0;
	float x = root;
	tempnum = root;
	tempnum2 = root;
	
	int n = 0;
	while (((++n * n) - 1) < num){}
		//Incrementing clock. Finds the depth of the balanced Binary Search Tree to be made from operators.
	int wordDepth = n;
	
	
	
	Tnode *leftptr = NULL, *rightptr = NULL;
	Tnode* temptr = NULL;
	//make tree
	// (*p) = malloc(sizeof(Tnode));
	// (*p)->info = words[(int) x];
	(*p) = newNode(&words[(int) x]);
#ifdef DEBUG
// printf("x is %d", (int) x);
#endif
	//remove reference to the word itself.
	words[(int) x] = NULL;
	
prints((*p)->info);
	leftptr = rightptr = (*p);
	
	// leftptr = (*p)->left;
	// rightptr = (*p)->right;
#ifdef LEFT_RIGHT_BALANCE
	int counter = 0;
	// char number[64];
	while (words[0] != NULL) {	//insert left side of tree.
		// ++counter;
		// itoa(counter, number, 10);
		// number = itoa(++counter);
		// snprintf(number, 10, "%d", ++counter);
		// prints(number);
// printf("counter is %d\n", ++counter);
		x = tempnum / 2.0;
// printf("x is %d\n", (int) x);
		if (words[(int) x]) {
			// leftptr = malloc(sizeof(Tnode));
			// if ((*p)->left == NULL) {	/* if left pointer unitialized */
				// (*p)->left = leftptr;				
			// }
			// leftptr->info = words[(int) x];	//if words[x] != NULL, move value to tree.
			
			/* Create a new node with value "words[(int) x]" */
			leftptr->left = newNode(&words[(int) x]);
			words[(int) x] = NULL;
			/* Advance leftptr up */
			leftptr = leftptr->left;
prints(leftptr->info);
			
			/*
				We have to do some real funky stuff to balance the inside correctly too. Surprise, Surprise.
			*/
			/* Fill in right node with appropriate value */
			int i = ((int) x) + tempnum; 
			if (words[i / 2] == NULL) {
// printf("i / 2 == %d", i / 2);
				/* 
				This error could be caused by a smaller keyword size. Because the i / 2 should be done before converting 
					the number to a integer. Because of this calculation, this could cause accessing the incorrect array spot.
				At first I thought this would be very bad if it occurred. But now i realize this simply means that there isn't a
					word here. So i am just going to not make a new node if this happens to happen, and if errors start showing up,
					i will look here first.
				*/
				// error("Possibly a very uncommon issue in searchTreeBalanceG1...\nNeed to change calculation timing to be more accurate to prevent this error in bigger keyword sizes");
				
				// leftptr->right = malloc(sizeof(Tnode));
				// temptr = leftptr->right;
				// temptr->info = words[i / 2];	// the /2 is for averaging the x and tempnum
				// temptr->left = NULL;
				// temptr->right = NULL;
				
				// leftptr->right = newNode(&words[i / 2]);
				// words[i / 2] = NULL;
			}
			
			// leftptr->right = NULL;
			// leftptr = leftptr->left;
			tempnum = (int) x;
		}
	}	//left side should be made.
// PrintTree((*p));	

counter = 0;
	x = root;
	// rightptr = (*p)->right;
prints("Starting Right side");
	while (words[(int) num - 1] != NULL) {	//insert right side of tree
	// while (x < num - 1) {	//insert right side of tree
/* 
'x' is updated in the middle of the loop, therefore, we are aiming to let $x reach the value of $(num - 1)
	this way, we only access the final value in the array once.
	
Issues with this code; the left side works fine with checking for the existence of words[0], however the right side
	does not work this way. I am unsure why. But my solution is to simply avoid the 

		this side might take much longer to actually do, especially with larger values of "num"
		it will just waste operation time while waiting to reach the maximum value.
		it doesnt have to reach the maximum value. the equation, (x + num) / 2 = num - 1

*/
// printf("%d\n", ++counter);
		x = (x + num) / 2.0;
// printf("%f", x);
		if (words[(int) x] != NULL) {
// prints("test");
			/* Create new node */
			// rightptr = malloc(sizeof(Tnode));
			// if ((*p)->right == NULL) {
				// (*p)->right = rightptr;				
			// }
			// rightptr->info = words[(int) x];
			rightptr->right = newNode(&words[(int) x]);
			words[(int) x] = NULL;
			rightptr = rightptr->right;
			
prints(rightptr->info);
			/*
				We have to do some real funky stuff to balance the inside correctly too. Surprise, Surprise.
			*/
			/* Fill in one layer of the inside of the tree */
			int i = ((int) x) + tempnum2; 
			if (words[i / 2] == NULL) {
				/* See left side commment above */
				
				// rightptr->left = malloc(sizeof(Tnode));
				// temptr = rightptr->left;
				// temptr->info = words[i / 2];
				// temptr->left = NULL;
				// temptr->right = NULL;
				
				// rightptr->left = newNode(&words[i / 2]);
				// words[i / 2] = NULL;
			}
			
			
			// rightptr->left = NULL;
			// rightptr = rightptr->right;	/* Not needed here, but instead after creating the new node */
			tempnum2 = (int) x;
		}
	}	//right side should be made.
prints("right side finished");
#endif 
	/* Begin placing remaining values */
	temptr = (*p);
	int i = 0;
prints("begin placing remaining values");
	while (i < num) {
// printf("%d\n", i);
		if (words[i] != NULL) {	//if a word has not been placed in tree, find place for it.
prints(words[i]);
			while(temptr != NULL) {	/* Exit conditions are based on break statements. consider cleaning this up*/
				if (temptr == NULL) break;
// prints(temptr->info);
				if (strcmp(words[i], temptr->info) < 0) {	//more words start with letters N-Z than A-M.
					//word belongs to the left
// prints("left/t");
					if (temptr->left == NULL) {
						/* Place word */
						temptr->left = newNode(&words[i]);
						words[i] = NULL;
						break;
					} else
						temptr = temptr->left;
				} else if (strcmp(words[i], temptr->info) > 0) {
					/* Word belongs to the right */
// prints("right\t");
					if (temptr->right == NULL) {
						/* Place word */
						temptr->right = newNode(&words[i]);
						words[i] = NULL;
						break;
					} else
						temptr = temptr->right;
				} else if (strcmp(words[i], temptr->info) == 0) {
					/* Duplicate found */
					break;
				}
			} /* End position loop */
			/* Reset Position of temptr */
			temptr = (*p);
// prints("Found place");
			/* Place word in new tree node */
			/* Transfer word, cleanup array */
			// temptr = malloc(sizeof(Tnode));
			
			// temptr = words[i];
			// temptr->info = malloc(sizeof(char*));
			// strcpy(temptr->info, words[i]);
			// free(words[i]);		
				/* This was dumb, i want to keep the information stored in the
					string, releasing the string here means that the word isnt stored in
					// the tree. */
// prints("Transfered word");
			/* Finish Setting up Tnode */
			// temptr->left = NULL;
			// temptr->right = NULL;
		}
		++i;
	} /* Search through words list and emptied it */
prints("placed all words");
	free(words);
}

#define ARRAY_CHECK

/*
Checks whether or not the word provided is a keyword or just an identifier.
The precompiler directive "ARRAY_CHECK" is intended to circumvent the decision I
made originally to implement the keywords using a Binary Search Tree (BST). 
The solution i made to create the tree was not originally working,
and as i had spent too much time already with the keywords/operators side of it,
i just decided to go for a simpler approach. 
Optimizations could be made by implementing the tree instead.

Returns 1 if keyword is found in keywords tree,
returns 0 if keyword not found.
*/
int LookupKeywordG1(char* word) {
printsG1("Start LookupKeywordG1");
#ifndef ARRAY_CHECK
	Tnode* curr = keywordstree;
	int tmpnum = 0;	//result of "strcmp(word, curr->info)"
	while (curr != NULL && (tmpnum = strcmp(word, curr->info)) != 0) {	//repeat until we find keyword or end of tree
#ifdef DEBUG_G1
	printf("strcmp(%s,%s) = %d\n", word, curr->info, tmpnum);
#endif
		if (tmpnum < 0) {	//more words start with letters N-Z than A-M.
			//word belongs to the left
			curr = curr->left;
		} else if (tmpnum > 0) {
			/* Word belongs to the right */
			curr = curr->right;
		} else if (tmpnum == 0) {
			/* Duplicate found */
			//Not really necessary to have this here since loop will exit upon this condition being met.
		}
		
printsG1("G1: Loop Test");
	}
	if (curr == NULL) {
printsG1("G1: Check Failed");
		return 0;
	} else if (tmpnum == 0) {
		return 1;
	}
#else
	char* keywords[] = {"accessor", "and", "array", "begin", "bool", "case", "character", "constant", "else", 
			    "elsif", "end", "exit", "function", "if", "in", "integer", "interface", "is", "loop", "module",
			    "mutator", "natural", "null", "of", "or", "others", "out", "positive", "procedure", "range", 
			    "return", "struct", "subtype", "then", "type", "when", "while"};
	int i;
	int tmpnum = 1;
	int keywords_num = 37;
	for (i = 0; i < keywords_num; ++i) {
	  if ((tmpnum = strcmp(keywords[i], word)) == 0) break;
	}
	if (i == keywords_num) {
		//no word found.
		return 0;
	} else if (tmpnum == 0) {
		//word found
		return 1;
	}
#endif
}

int LookupOperatorG1(char* word) {
printsG1("G1: Start LookupOperatorG1");
#ifndef ARRAY_CHECK
	Tnode* curr = operatorstree;
	int tmpnum = 0;	//result of "strcmp(word, curr->info)"
	while (curr != NULL && (tmpnum = strcmp(word, curr->info)) != 0) {	//repeat until we find keyword or end of tree
#ifdef DEBUG_G1
	printf("strcmp(%s,%s) = %d\n", word, curr->info, tmpnum);
#endif
// printf("strcmp(%s,%s) = %d\n", word, curr->info, tmpnum);
		if (tmpnum < 0) {	//more words start with letters N-Z than A-M.
			//word belongs to the left
			curr = curr->left;
		} else if (tmpnum > 0) {
			/* Word belongs to the right */
			curr = curr->right;
		} else if (tmpnum == 0) {
			/* Duplicate found */
			//see LookupKeywordG1().
		}
	}
	if (curr == NULL) {
		return 0;
	} else if (tmpnum == 0) {
		return 1;
	}
#else
	char* operators[] = {".", "<", ">", "(", ")", "+", "-", "*", "/", "|",
			     "&", ";", ",", ":", "[", "]", "=", ":=", "..", "<<",
			     ">>", "<>", "<=", ">=", "**", "!=", "=>"};

	int i;
	int operatorsNum = 27;
	for (i = 0; i < operatorsNum; ++i) {
	  if (strcmp(operators[i], word) == 0) break;
	}
	if (i == operatorsNum) {
		//no word found.
		return 0;
	} else {
		//word found
		return 1;
	}
#endif
	
	// while
	// if (strcmp(word, curr->info) < 0) {	//more words start with letters N-Z than A-M.
		// // word belongs to the left
	// } else if (strcmp(word, curr->info) > 0) {
		// /* Word belongs to the right */
	// } else if (strcmp(word, curr->info) == 0) {
		// /* Duplicate found */
	// }
}

//Clean up lists.
void freeList(llnode* listptr) {
	llnode* curr = listptr;
	while(listptr) {
		curr = curr->next;
		free(listptr->data);
		listptr->data = NULL;
		listptr->next = NULL;
		free(listptr);
	}
}

/* Creates a new node with the value of word */
Tnode* newNode(char** word) {
	Tnode* temp = malloc(sizeof(Tnode));
	temp->info = (*word);
	temp->left = temp->right = NULL;
	return temp;
}

void freeTree(Tnode* treeptr) {
	if (treeptr->right) freeTree(treeptr->right);
	if (treeptr->left) freeTree(treeptr->left);
	free(treeptr->info);
	treeptr->info = NULL;
	free(treeptr);
	treeptr = NULL;
}	//END freeTree()

//https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
unsigned int round_closest(unsigned int dividend, unsigned int divisor)
{
    return (dividend + (divisor / 2)) / divisor;
}

void ReadFrequency() {
#ifdef FREQUENCY
prints("Starting Frequency");
	FILE* tmpfp = openinFile("Frequency.txt");
	if (tmpfp != NULL) {
		int i;
		for (i = 0; i < 8; ++i) {
			fscanf(tmpfp, "%li", &frequency[i]);	//"%li" means long int
		}
		fclose(tmpfp);
		tmpfp = NULL;
	}
#endif
}

void CountFrequency(int t) {
#ifdef FREQUENCY
	frequency[t] += 1;	//increment corresponding frequency.
#endif
}

void WriteFrequency() {
#ifdef FREQUENCY
	FILE* tmpfp = fopen("Frequency.txt", "w");
	if (tmpfp != NULL) {
		int i;
		for (i = 0; i < 8; ++i) {
			fprintf(tmpfp, "%li\t", &frequency[i]);	//"%li" means long int
		}
		fclose(tmpfp);
		tmpfp = NULL;
	}
#endif	
}

/*
Better answer used where needed. Use this if POSIX 7 is not allowed/usable. 

https://stackoverflow.com/questions/16400886/reading-from-a-file-word-by-word
https://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
Solution to read a file line by line without assuming its contents nor it's length.
Author: Johannes Schaub (with much help from commentors ;P)

char * getline(void) {
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;	//reset "len" to effectively double immediate allowed space by the variable
            char * linen = realloc(linep, lenmax *= 2);	//create a string capable of storing double previous size.

            if(linen == NULL) {	//if this happens, got some serious problems happening. 
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);	//honestly no clue what this does. it's setting the address of line to the new array, plus the difference in address between the original array and the pointer to that original?
            linep = linen;	//then sets the pointer to the new array to something different than simply line? this seems absolutely screwed.
        }

        if((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}
*/

void error(const char * input) {
	perror(input);
	printf("\nExitingProgram...\n");
	exit(1);
}	//END error()

void PrintTree(Tnode* n) {
	// prints("Calling PrintTree");
	if (n->left != NULL)
		PrintTree(n->left);
	if (n != NULL)
		printf("%s\n", n->info);
	if (n->right != NULL)
		PrintTree(n->right);
	
	// if (n != NULL) {
		// PrintTree(n->left);
		// printf("%s\t", n->info);
		// PrintTree(n->right);
	// }
}

void PrintList(llnode* p) {
	if (p != NULL) {
		prints(p->data);
		PrintList(p->next);
	}
}

void prints(char* x) {
#ifdef DEBUG
	printf("%s\n", x);
#endif
}

/*
Does the same thing as prints(), except this is specifically to be used
	by functions in function group 1. Denoted by suffix of "...G1".
*/
void printsG1(char* s) {
#ifdef DEBUG_G1
	printf("%s\n", s);
#endif
}

/*
Does the same thing as prints(), except this is specifically to be used
	within the lexeme-loop, or the functions called by such loop.
*/
void printsloop(char* s) {
#ifdef DEBUG_LOOP
	printf("%s\n", s);
#endif
}



