#include "Tokenizer.h"
#include "Error.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>


int Tokenizer::GetLengthOfTheToken(){
    return LenghtOfToken;
}

char *Tokenizer::ReturnTheToken(){
    return Token;
}

int Tokenizer::GetTheNextName(){
     int i=0;
     while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t' || FileStream[Pointer]=='\r'){
        Pointer++;
//        if(NumberOfTokensInTheLine==0 && FileStream[Pointer]=='\t')
//            NumberOfTabs++;
     }
     if(FileStream[Pointer]=='\0')
        exit(-1);

     TotalOfTokens++;

     if(FileStream[Pointer]==';')GotoTheNextLine();

     if(isalpha(FileStream[Pointer])){
        while(isalnum(FileStream[Pointer]))
           Token[i++]=FileStream[Pointer++];
        Token[i]='\0';
        LenghtOfToken=i-1;
        return 1;
     }
     TotalOfTokens--;
     return 0;
}

int Tokenizer::GetTheNextNumber(){
     int i=0;
     while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t' || FileStream[Pointer]=='\r'){
        Pointer++;
//        if(NumberOfTokensInTheLine==0 && FileStream[Pointer]=='\t')
//            NumberOfTabs++;
     }
     TotalOfTokens++;

     if(FileStream[Pointer]==';')
        GotoTheNextLine();

     if(isdigit(FileStream[Pointer])){
            while(isdigit(FileStream[Pointer]))
                    Token[i++]=FileStream[Pointer++];
            Token[i]='\0';
            LenghtOfToken=i-1;
            return 1;
     }
     TotalOfTokens--;
     return 0;
}

int Tokenizer::GetBeginOfBlock(char ch){

}

int Tokenizer::GetTheNextToken(){
     int i=0;
     while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t' || FileStream[Pointer]=='\r'){
        Pointer++;
//        if(NumberOfTokensInTheLine==0 && FileStream[Pointer]=='\t')
//            NumberOfTabs++;
     }
     TotalOfTokens++;

     if(isalpha(FileStream[Pointer])){
        while(isalnum(FileStream[Pointer]))
           Token[i++]=FileStream[Pointer++];
        Token[i++]='\0';
        LenghtOfToken=i-1;
        return NAME;
     }
     else if(isdigit(FileStream[Pointer])){
            while(isdigit(FileStream[Pointer]))
                    Token[i++]=FileStream[Pointer++];
            Token[i++]='\0';
            LenghtOfToken=i;
            return NUMBER;
     }
     else if(FileStream[Pointer]=='+'){
        Pointer++;
        if(FileStream[Pointer]=='+'){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=INC;
            Token[1]='\0';
            return ARITMETIC;
        }
        LenghtOfToken=1;
        Token[0]=ADD;
        Token[1]='\0';
        return ARITMETIC;
     }
     else if(FileStream[Pointer]=='-'){
        Pointer++;
        if(FileStream[Pointer]=='-'){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=DEC;
            Token[1]='\0';
            return ARITMETIC;
        }
        LenghtOfToken=1;
        Token[0]=SUB;
        Token[1]='\0';
        return ARITMETIC;
     }
     else if(FileStream[Pointer]=='<'){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=LE;
            Token[1]='\0';
            return COMPARE;
        }
        else if(FileStream[Pointer]=='<'){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=READIN;
            Token[1]='\0';
            return READIN;
        }
        LenghtOfToken=1;
        Token[0]=LT;
        Token[1]='\0';
        return COMPARE;
    }
    else if(FileStream[Pointer]=='>'){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=GE;
            Token[1]='\0';
            return COMPARE;
        }
        LenghtOfToken=1;
        Token[0]=GT;
        Token[1]='\0';
        return COMPARE;
    }
    else if(FileStream[Pointer]=='='){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=EQ;
            Token[1]='\0';
            return COMPARE;
        }
        LenghtOfToken=1;
        return ATTRIB;
    }
    else if(FileStream[Pointer]=='!'){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            LenghtOfToken=2;
            Token[0]=NE;
            Token[1]='\0';
            return COMPARE;
        }
        LenghtOfToken=1;
        Token[0]=NOT;
        Token[1]='\0';
        return LOGICAL;
    }
    switch(FileStream[Pointer]){
        case '\0':TotalOfTokens--;return END;break;
        case '\r':
        case '\n':TotalOfTokens--;Pointer++;LineCounter++;return NL;break;

        case ';':LenghtOfToken=1;Pointer++;;return COMM;break;

        case '/':LenghtOfToken=1;Pointer++;return DIV;break;
        case '*':LenghtOfToken=1;Pointer++;return MUL;break;
        case '^':LenghtOfToken=1;Pointer++;return POW;break;
        case '%':LenghtOfToken=1;Pointer++;return MOD;break;
        case '(':LenghtOfToken=1;Pointer++;return OPENP;break;
        case '+':LenghtOfToken=1;Pointer++;return ADD;break;
        case '-':LenghtOfToken=1;Pointer++;return SUB;break;
        case ')':LenghtOfToken=1;Pointer++;return CLOSEP;break;

        case '{':LenghtOfToken=1;Pointer++;return OPENB;break;
        case '}':LenghtOfToken=1;Pointer++;return CLOSEB;break;

        case '"':
        case '\'':LenghtOfToken=1;Pointer++;return STRING;break;

        case '#':LenghtOfToken=1;Pointer++;return COMPARE;break;

        case ':':LenghtOfToken=1;Pointer++;return DEFINE;break;
        case ',':LenghtOfToken=1;Pointer++;return COMMA;break;
        case '.':LenghtOfToken=1;Pointer++;return PONTUATOR;break;
        default:TotalOfTokens--;return ERROR;break;
    }
    return 0;
}

long Tokenizer::ReturnTheSizeOfTheFile(){//retorna o tamanho do texto
    return SizeOfFile;
}
int Tokenizer::GetTheTotalOfTokens(){
    return TotalOfTokens;
}

int Tokenizer::GetTheTotalOfLines(){
    return LineCounter;
}

void Tokenizer::SkipSpaces(){
    while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t')
        Pointer++;
}

int Tokenizer::IsThisTheNextChar(char c){
    SkipSpaces();
    if(FileStream[Pointer]!=c){
        //Pointer--;
        return 0;
    }
    TotalOfTokens++;
    if(c=='\n'){
        LineCounter++;
//        NumberOfTokensInTheLine=0;
//        NumberOfTabs=0;
        TotalOfTokens--;
    }
    Pointer++;
    return 1;
}

int Tokenizer::GetAttrib(){

    SkipSpaces();
    if(FileStream[Pointer]=='<'){
        Pointer++;
        if(FileStream[Pointer]=='<'){
            Pointer++;
            return 1;
        }
        Pointer--;
        return 0;
    }
    return 0;

}

void Tokenizer::WalkInThPosition(int step)
{
    Pointer=Pointer+step;
}

void Tokenizer::GotoToPosition(long pos){
    Pointer=pos;
}

long Tokenizer::ReturnActualPosition(){
    return Pointer;
}

char Tokenizer::GetTheNextChar(){
    while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t' || FileStream[Pointer]=='\r')
        Pointer++;
    return FileStream[Pointer++];
}

int Tokenizer::GetStringInTheLine(){
    int i=0;

    //SkipSpaces();
    while((FileStream[Pointer]!='\''&& FileStream[Pointer]!='"') && i<255)
    {
        if(FileStream[Pointer]=='\0'||FileStream[Pointer]=='\n'|| FileStream[Pointer]=='\r'){
            if(FileStream[Pointer]=='\n'){
                LineCounter++;
//                NumberOfTokensInTheLine=0;
//                NumberOfTabs=0;
            }
            return 0;
        }
        StringInTheLine[i++]=FileStream[Pointer++];
    }
    Pointer++;
    StringInTheLine[i++]='\0';
    return 1;
}

void Tokenizer::GotoTheNextLine(){
    SkipLineInCode();
}

char *Tokenizer::ReturnTheFileStream(void){
    return FileStream;
}

char *Tokenizer::ReturnTheString(){
    return StringInTheLine;
}
int Tokenizer::LoadThisFile(const char *str){
    FILE *fp;

    if((fp=fopen(str,"rt"))==NULL){
        puts("ERRO: Ficheiro não existe");
        exit(-1);
    }
    fseek(fp,0,2);
    SizeOfFile=ftell(fp);
    fseek(fp,0,0);
    if((FileStream=(char*)calloc(sizeof(char),SizeOfFile))==NULL){
        puts("ERRO: Memoria insuficiente");
           exit(-1);
    }
    fread((char*)FileStream,SizeOfFile,1,fp);
    fclose(fp);
    FileLoaded=1;
    Pointer=0;
    StateOfFileStream=1;
    strcpy(FileName,str);
//printf("Ficheiro Carregado\n\n");

    return 1;
}

void Tokenizer::PullBackToken(){
    int i;

    for(i=0;i<=LenghtOfToken;i++)
        Pointer--;
}

int Tokenizer::ReturnActualLine(){
    return LineCounter;
}

int Tokenizer::IsTheEndOfTheLine(void){
//    if(FileStream[Pointer]=='\n' || FileStream[Pointer]=='\0' || FileStream[Pointer]==';' || FileStream[Pointer]=='\r')
//        return 1;
    SkipSpaces();
    //Pointer++;
    if(FileStream[Pointer]=='\n' || FileStream[Pointer]=='\0' || FileStream[Pointer]==';' || FileStream[Pointer]=='\r'){
        Pointer++;
        LineCounter++;
//        NumberOfTabs=0;
        return 1;
    }
    return 0;
}

int  Tokenizer::ExtractLineFrom(char *src, int pos, char delim){
    int i=0;
    //printf("%d\n",pos);
    if(pos>255)return -1;

    if(StateOfFileStream==0)
    {
       FileStream=(char*)calloc(sizeof(char),255);
       StateOfFileStream=1;
    }
    Pointer=0;
    if(src[pos]=='\0')return 0;
    for(i=pos;*src!='\0' && *src!=delim;i++)
        FileStream[Pointer++]=*src++;
    FileStream[Pointer]='\0';
    //printf("Retorn da extração: %s",FileStream);
    Pointer=0;
    ++i;
    return i;
}


int Tokenizer::LoadThisLine(char *str){
   if(StateOfFileStream==0)
   {
      FileStream=(char*)calloc(sizeof(char),255);
      StateOfFileStream=1;
   }

   Pointer=0;
   strcpy(FileStream,str);
   return 1;
}

int  Tokenizer::GetAritmeticSymbol()
{
    while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t' || FileStream[Pointer]=='\r')
        Pointer++;
    TotalOfTokens++;

     if(FileStream[Pointer]=='+'){
        Pointer++;
        if(FileStream[Pointer]=='+'){
            Pointer++;
            LenghtOfToken=2;
            return INC;
        }
        LenghtOfToken=1;
        return ADD;
     }
     else if(FileStream[Pointer]=='-'){
        Pointer++;
        if(FileStream[Pointer]=='-'){
            Pointer++;
            LenghtOfToken=2;
            return DEC;
        }
        LenghtOfToken=1;
        return SUB;
    }
    else if(FileStream[Pointer]=='*'){
        Pointer++;
        LenghtOfToken=1;
        return MUL;
    }
    else if(FileStream[Pointer]=='/'){
        Pointer++;
        LenghtOfToken=1;
        return DIV;
    }
    else if(FileStream[Pointer]=='%'){
        Pointer++;
        LenghtOfToken=1;
        return MOD;
    }
    else if(FileStream[Pointer]=='^'){
        Pointer++;
        LenghtOfToken=1;
        return POW;
    }
    else return 0;
}

int Tokenizer::GetComparasionSymbol(){

    while(FileStream[Pointer]==' '|| FileStream[Pointer]=='\t' || FileStream[Pointer]=='\r')
        Pointer++;
    TotalOfTokens++;

    if(FileStream[Pointer]=='<'){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            return LE;
        }
        return LT;
    }
    else if(FileStream[Pointer]=='>'){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            return GE;
        }
        return GT;
    }
    else if(FileStream[Pointer]=='='){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            return EQ;
        }
        else return 0;
    }
    else if(FileStream[Pointer]=='!'){
        Pointer++;
        if(FileStream[Pointer]=='='){
            Pointer++;
            return NE;
        }
        return 0;
    }
    else return 0;
}

void Tokenizer::SkipLineInCode(){
    while(FileStream[Pointer]!='\n' && FileStream[Pointer]!='\0')
        Pointer++;
//    if(FileStream[Pointer]=='\0')
//        exit(0);
    Pointer++;
    LineCounter++;

//    NumberOfTokensInTheLine=0;
//    NumberOfTabs=0;
    return;
}

char *Tokenizer::ReturnTheFileName(){
    return FileName;
}

Tokenizer::Tokenizer() {
    StateOfFileStream=0;
    LenghtOfToken=0;//tamanho do token
    SizeOfFile=0;//tamanho do ficheiro
    FileLoaded=0;//confirma que o ficheiro foi carregado
    TotalOfTokens=0;//total de token obtidos
    Pointer=0;//posicao na linha
    LineCounter=1;//contador de linhas
    NumberOfTokensInTheLine=0;//quantidades
}

Tokenizer::~Tokenizer(){
   if(StateOfFileStream==1)
       free(FileStream);
}

