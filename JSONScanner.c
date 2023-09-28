/*
* Name: Jordan Scherf, 5005736913, Assignment  2
* Description: This program acts as an interpreter for a JSON file to ensure it's formatted correctly.
* Input: valid named JSON file.
* Output: A list of tokens and confirmation of successful parsing and scanning.
*/
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define N 32 // Longest String of Characters

const int EOT = 0; // end of file
const int STRING = 1; // sequence of chars
const int IDENT = 2; // identifier
const int COMMA = 3; // ’,’
const int COLON = 4; // ’:’
const int LBRACE = 5; // ’{’
const int RBRACE = 6; // ’}’
const int LBRACK = 7; // ’[’
const int RBRACK = 8; // ’]
const int CONST = 9; // Any Const like a bool, number, keyword stuff

const char* tokenNames[] = { // C-string array of token names
    "EOF", "STRING", "IDENT", "COMMA", "COLON",
    "LBRACE", "RBRACE", "LBRACK", "RBRACK", "CONST"
    };

const char* tokenNamesSymbols[] = { // C-string array of token symbols to output erros
    "END OF FILE", "STRING", "IDENTIFIER", ",", ":",
    "{", "}", "[", "]", "CONSTANT"
};


/*
* Token: Struct of a token which holds what kind of token it is(int), what line number its was on (int), and an array to hold the symbol or string
* constructors: a vertical list of constructor prototypes
* Variables: int kind, int lineNo, char lexeme[N]
*/
struct Token // What different Kind of Token
    {
        int kind; //What kind of token
        int lineNo; // What line number it is on
        char lexeme[N];
    };


struct Token getNextToken(FILE* file); // Gets the Next token
void file(); // derivation starting at the top of the file
void array(); // if there are brackets involved, match them
void object(); // if therje is an object made, match them
void literal(); // this goes down to the base values of ident, string, and const
void value(); // this decideds what value it is
void match(int x); // ensures the token is correct in its placement
void consume(); // moves onto the next token
void memeber();

int arrayCounter; // counter for the array
int lineNumber = 1; // counts the line number in the file
int objectCount = 0; // object count for comma checking
bool stringError = false; // tells if there is a string error
bool parseError = false; // tels if there is a parse error
bool inObject = false;
struct Token tokenArray[400]; // array to hold tokens, limit 400
struct Token token; // This is a token im using now that im losing my mind
    
int main(int argc, char* argv[]) {
    
    
    bool identifier = false; // If there is an identifier 

    struct Token nextToken; // Variable that represents the current lookahead token
    //struct Token token; 
    //struct Token tokenArray[200];
   


        // Check for the correct number of command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the input file for reading
    FILE* inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) { // if file doesnt open, tell them       
     perror("Failed to open file");
        return 1;
    }
    
    // Lets start to scan the file
    printf("\nScanning the file...\n");
    token = getNextToken(inputFile);
    while(token.kind != EOT && !stringError){
      //printf("Doing stuff \n");
        nextToken = getNextToken(inputFile);
        // write output code for getting the next token
        
        if(token.kind == STRING){ // this changes strings to ident if the token in front of them is a colon
            if(nextToken.kind == COLON){
                token.kind = IDENT;
            }
        }

        if(nextToken.kind == LBRACE){
            if(token.kind == STRING){
            token.kind = IDENT;
            }
        }
        
        // adds the token to the token array
        tokenArray[arrayCounter] = token;
        arrayCounter++;
        // if there is an issue, output it
        if(stringError == true){
            printf("Something went Wrong! Unterminated %s on line %d\n\n", tokenNames[nextToken.kind], nextToken.lineNo);
        }

        token = nextToken;
    }
    



    // Scanning is Successful
    if(stringError == false){
        printf("Scanning done!\n\n");
    }


    // Begin Printing all the tokens
    
  printf("List of tokens:\n");
    for(int i = 0; i < arrayCounter; i++){
      printf("<@%d,<%d,%s>=\"%s\">\n", tokenArray[i].lineNo, tokenArray[i].kind, tokenNames[tokenArray[i].kind], tokenArray[i].lexeme);
    }
    

   

    if(token.kind == EOT){ // To output the end of file token if the file is good
        printf("<@%d,<%d,%s>=\"%s\">\n", token.lineNo-1, token.kind, tokenNames[token.kind], token.lexeme);
    }

  

    //Now lets parse the file starting with that first bracket
    if(!stringError){
    printf("\nParsing file...\n");
    arrayCounter = 0;
    token = tokenArray[arrayCounter++];
    file();
    }else{
      printf("\nCould not parse file.");
    }

    // Lets do a check for proper comma placement
    /*
    for(int i = 0; i < arrayCounter; i++){
      token = tokenArray[i];

      if(token.kind == STRING || token.kind == CONST || token.kind == RBRACK || token.kind == RBRACE){
        if(tokenArray[i+1].kind == IDENT){
            printf("Something went Wrong! Expecting a COMMA on line %d\n\n", token.lineNo);
            arrayCounter = i++;
            arrayCounter++;
            parseError = true;
        }
      }
    }
    */
    // if parsing is successful, output
    if(!parseError && !stringError){
      printf("Parsing done!\n");
    }
  
    return 0;
}

/*
* file(): Takes in the first token in the json file 
* parameters: nothing
* return value: nothing
*/

void file(){
    if (token.kind == LBRACE){
        object();
    }else if (token.kind == LBRACK){
        array();
    }else{
        //Do error Message
    }
}

/*
* member(): Derives the Derrivation of an member of an object
* parameters: nothing
* return value: nothing
*/

void member() {
    if (token.kind == IDENT) {
        match(IDENT);
        match(COLON);
        value();
        if (token.kind == COMMA) {
            match(COMMA);
            member(); // Recursively handle additional members
        }
    } else {
        // Handle error or other cases
    }
}

/*
* object(): Derives the Derrivation of an object
* parameters: nothing
* return value: nothing
*/

void object() {
    int x;
    if (token.kind == LBRACE) {
        x = objectCount;
        objectCount++;
        match(LBRACE);
        if (token.kind != RBRACE) {
            member();
            while (token.kind == COMMA) {
                match(COMMA);
                member(); // Recursively handle additional members
            }
        }
        if (objectCount == x) {
            match(COMMA);
        }
        objectCount--;
        match(RBRACE);
    }
}

/*
* array: derives the array and matches expected input
* parameters: nothing
* return value: nothing
*/
void array(){
    if(token.kind == LBRACK){
        match(LBRACK);
        if(token.kind == RBRACK){
            match(RBRACK);
            }else{
                do{
                value();
                }while(token.kind == COMMA);
            }
        match(RBRACK);
    }
}

/*
* value(): derives the values into literals
* parameters: nothing
* return value: nothing
*/
void value(){

    if(token.kind == STRING){
      literal();
    }else if(token.kind == CONST){
      literal();
    }else if(token.kind == IDENT){
      literal();
    }else if(token.kind ==  LBRACE){
        object();
    }else if (token.kind == LBRACK){
        array();
    }else{
        match(COMMA);
        value();
    }
}

/*
* literal: derives the values into terminals (strings, const, ident)
* parameters: nothing
* return value: nothing
*/
void literal(){
    if(token.kind == STRING){
        match(STRING);

    }else if(token.kind == CONST){
        match(CONST);

    }else if(token.kind == IDENT){
      match(IDENT);
      match(COLON);
      value();
    }
}

/*
* match(): if the token matches the expected, it consumes it
* parameters: and integer from the token names list
* return value: if there is an issue, output an erro
*/
void match(int x){
    if (token.kind == x){
        consume();
    }else{
        if(!parseError)
        fprintf(stderr, "Something Went Wrong! Expecting '%s' but found '%s' in line %d \n", tokenNamesSymbols[x], token.lexeme, token.lineNo);
      parseError = true;
      consume();
    }
}

/*
* consume(): moves the token to the next on in the token array
* parameters: nothing
* return value: nothing
*/

void consume(){
    token = tokenArray[arrayCounter++];
}



/*
* getNextToken(): reads in the next string, int, symbol, anything really and turns it into a token to be used
* parameters: the file so we can read from it inside
* return value: a Token
*/

struct Token getNextToken(FILE* file){
struct Token T;
int c;

while((c = fgetc(file)) != EOF){
    switch (c)
    {
    case ' ':
    case '\n':
    case '\t':
        if(c == '\n'){
            lineNumber++;
        }
        while ((c = fgetc(file)) != EOF) {
            if(c == '\n'){
                lineNumber++;
            }else if (c != ' ' ) {
                ungetc(c, file); // Put the non-whitespace character back into the file stream
                break;
            }
        }
        continue;
    
    case ',':
        T.kind = COMMA;
        T.lineNo = lineNumber;
        T.lexeme[0] = ',';
        T.lexeme[1] = '\0';
        return T;

    case ':':
        T.kind = COLON;
        T.lineNo = lineNumber;
        T.lexeme[0] = ':';
        T.lexeme[1] = '\0';
        return T;

    case '{':
        T.kind = LBRACE;
        T.lineNo = lineNumber;
        T.lexeme[0] = '{';
        T.lexeme[1] = '\0';
        return T;

    case '}':
        T.kind = RBRACE;
        T.lineNo = lineNumber;
        T.lexeme[0] = '}';
        T.lexeme[1] = '\0';
        return T;
    
    case '[':
        T.kind = LBRACK;
        T.lineNo = lineNumber;
        T.lexeme[0] = '[';
        T.lexeme[1] = '\0';
        return T;
    
    case ']':
        T.kind = RBRACK;
        T.lineNo = lineNumber;
        T.lexeme[0] = ']';
        T.lexeme[1] = '\0';
        return T;

    case '"': // Handle strings enclosed in double quotes
            T.kind = STRING;
            T.lineNo = lineNumber;
            int i = 0;
            while ((c = fgetc(file)) != EOF && c != '"') {
                if (i < N - 1) {
                    T.lexeme[i++] = c;
                    if(c == '\n'){
                        stringError = true;
                        return T;
                    }
                }

                
            }
            T.lexeme[i] = '\0'; // Null-terminate the string
            return T;
    
    default:
       if(c >= 'A' || c <=  'Z' || c >= 'a' || c <= 'z' || c >= '0' || c <= '9'){
            //printf("In this bitch");
            T.kind = CONST;
            T.lineNo = lineNumber;
            int i = 0;
            while(c != ' ' && c != '\n' && c != ','){
                
                if (i < N - 1) {
                    T.lexeme[i++] = c;
                }
                c = fgetc(file);
            }
            T.lexeme[i] = '\0';
            ungetc(c, file);
            
            return T;
       }else{
            printf("Error, something bad");
       }
        break;
    }
}

T.kind = EOT;
T.lineNo = lineNumber;
T.lexeme[0] = 'E';
T.lexeme[1] = 'O';
T.lexeme[2] = 'F';
T.lexeme[3] = '\0';

return T;
}
