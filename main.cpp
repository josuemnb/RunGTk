#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>

#include "Tokenizer.h"
#include "Structs.h"
#include "Error.h"
#include "Gui.h"

#ifndef nil
#define nil "\0"
#endif

FILE *fp;//este file passará a ser usado como ficheiro para guardar o codec

//fpos_t fpVarPos;//posição sobre a posicao do ficheiro no header das variaveis

char * 	sChar;
int 	pChar=0;
char * 	tempChar;
int * 	tempInt;
int *  	sInt;
short	pInt=0;
short 	lastInt=0;
short 	lastChar=0;


char mainType;//valor dado pelo Main as ...
bool insideFunction=false;
short level=0;

char VARTYPES[][10]= {"number","string","const","real","bool"};
char MAINTYPES[][10]= {"Gui","Console","Service","Web","Mobile","Library"};

Tokenizer token;
extern Erros ERROS;

int LOOP[10];
bool smallSize=false;
clock_t start,stop;

void evalAttrib();
void evalIf();
int evalBlock();
void evalVar();
void evalConst();
void evalDebug();
void evalPuts();
void evalRead();
void evalSpin();
void evalReadIn();
void evalLog();
void evalBreak();
void evalMain();
void evalGui();
void evalConsole();
void evalService();
void evalWeb();
void evalMobile();
void evalLibrary();
int evalExpression(int level);
void processSpin(int);
void processIf(char);
void processFunction(FUNCTIONt*);
void printAll();

int checkVarTypes(char name[10])
{
    int i;
    for(i=0; VARTYPES[i]; i++)
        if(!strcmp(name,VARTYPES[i]))
            return i+1;
    return 0;
}

int checkMainTypes(char name[10])
{
    int i;
    for(i=0; MAINTYPES[i]; i++)
        if(!strcmp(name,MAINTYPES[i]))
            return i+1;
    return 0;

}

void evalVar()
{
    char ch;
    char type=0;
    char name[20];
    if (!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    if(CheckVars(token.ReturnTheToken()))
        showErr(ERROS.VARIABLE_EXISTS,token.ReturnActualLine(),token.ReturnTheToken());
    if(token.IsTheEndOfTheLine())
    {
        AddVar(token.ReturnTheToken(),0,nil);
        return;
    }
    strcpy(name,token.ReturnTheToken());
    ch=token.GetTheNextChar();
    if (ch==':') //se encontra :
    {
        if (!token.GetTheNextName())
            showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
        if (!strcmp(token.ReturnTheToken(), "string"))
            type = 1;
        else if (!strcmp(token.ReturnTheToken(), "real"))
            type = 2;
        else
            showErr(ERROS.UNKNOW_TYPE,token.ReturnActualLine(),token.ReturnTheToken());
        if(!token.IsTheEndOfTheLine())
            showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
        AddVar(name,type,nil);
        //puts("variavel adicionada");
        return;
    }
    else if (ch == '<') //se encontra <<
    {
        if(token.GetTheNextChar()!='<')
            showErr(ERROS.EXPECTED_ATTRIB,token.ReturnActualLine(),token.ReturnTheToken());
        if(token.IsThisTheNextChar('\''))
        {
            //se detecta que é uma string
            if(!token.GetStringInTheLine())
                showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            AddVar(name,1,token.ReturnTheString());
            return;
        }
        else if(!token.GetTheNextNumber())
            showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
        if(!token.IsTheEndOfTheLine())
            showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
        AddVar(name,2,token.ReturnTheToken());
        return;
    }
    else
        showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());
    return;
}

void evalBreak()
{
    if(!token.IsTheEndOfTheLine())
        showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
    sChar[pChar++]='B';
    return;
}

void evalExit()
{
    if(!token.IsTheEndOfTheLine())
        showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
    sChar[pChar++]='\0';
    return;
}

void evalLog()
{
    if(token.GetTheNextName())
    {
        vV=CheckVars(token.ReturnTheToken());
        if(vV==NULL)
            showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
        if(!token.IsTheEndOfTheLine())
            showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
        sChar[pChar++]='L';
        sChar[pChar++]='V';
        sInt[pInt++]=vV->index;
        return;
    }
    else showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    return;
}

void evalDebug()
{

    if(token.GetTheNextName())
    {
        vV=CheckVars(token.ReturnTheToken());
        if(vV==NULL)
        {
            showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            return;
        }
        if(!token.IsTheEndOfTheLine())
            showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
        sChar[pChar++]='D';
        sChar[pChar++]='V';
        sInt[pInt++]=vV->index;
        return;
    }
    showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    return;
}

void evalRead()
{

    if (!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    vV=CheckVars(token.ReturnTheToken());
    if(vV==NULL)
        showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
    if(vV->type==3)
        showErr(ERROS.CONSTANT_UNMODIFIABLE,token.ReturnActualLine(),token.ReturnTheToken());
    if(!token.IsTheEndOfTheLine())
        showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
    sChar[pChar++]='R';
    sInt[pInt++]=vV->index;
    return;
}

void evalReadIn()
{
    int tok;
    char ch;
    char temp[100];

    if(insideFunction==true)//está a processar uma função
    {
        if(vF->ret==0)
            showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());
        sprintf(temp,"F%s%d",vF->name,vF->index);
//        puts(temp);
        vV=CheckVars(temp);
        if(vV==NULL)
            showErr(ERROS.TYPE_NOT_MATCH,token.ReturnActualLine(),token.ReturnTheToken());
        sChar[pChar++]='A';
        sInt[pInt++]=vV->index;

        tok=token.GetTheNextToken();
        if(tok==NUMBER)
        {
            if(vF->ret!=1)
                showErr(ERROS.TYPE_NOT_MATCH,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='N';
            sInt[pInt++]=atoi(token.ReturnTheToken());
            return;
        }
        else if(tok==STRING)//é uma string, precisa criar a variavel
        {
            sprintf(temp,"%d",pChar);
            AddVar(temp,3,token.ReturnTheString());
            vV=CheckVars(temp);
            if(vV==NULL)
                showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='V';
            sInt[pInt++]=vV->index;
            return;
        }
        else if(tok==NAME)//tem que ser uma variavel ou função
        {
            ch=token.GetTheNextChar();
            if(ch=='(')//é uma funcao
            {
                if(token.GetTheNextChar()!=')')
                    showErr(ERROS.EXPECTED_END_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                vF=CheckFunction(token.ReturnTheToken());
                if(vF==NULL)
                    showErr(ERROS.UNKNOW_FUNCTION,token.ReturnActualLine(),token.ReturnTheToken());
                sprintf(temp,"F%s%d",vF->name,vF->index);
                vV=CheckVars(temp);
                if(vV==NULL)
                    showErr(ERROS.TYPE_NOT_MATCH,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='V';
                sInt[pInt++]=vV->index;
                return;
            }
            else if(ch=='\n' || ch=='\r' || ch==';')//tem que ser uma variavek
            {
                vV=CheckVars(token.ReturnTheToken());
                if(vV==NULL)
                    showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='V';
                sInt[pInt++]=vV->index;
                return;
            }
        }
        else
            showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());
        return;
    }
    else
        showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());
    return;
}

int evalExpression(int level=1)
//analise expressao matematica na linha e determina a sua construcao no codigo final

//Operator	Meaning	                    Priority
//=	        assignment*	                1
//+	        addition	                2
//-	        subtraction	                2
//*	        multiplication	            3
///	        division                    3
//^	        raise x to the power of y	3
//()        parentenses                 4
{

    int tok,indexEx,i;
    char ch;
    char tempEx[100];
    switch(level)
    {
    case 1://atribuição
        tok=token.GetTheNextToken();
        if(tok!=ATTRIB && tok!=READIN)//= ou <<
            showErr(ERROS.EXPECTED_ASSIGN,token.ReturnActualLine(),token.ReturnTheToken());
        tok=token.GetTheNextToken();
        if(tok==NAME || tok==NUMBER)
        {
            ch=token.GetTheNextChar();
            if(ch=='\n' || ch=='\r' || ch==';')//o valor de atribuicao vem de variavel ou numero
            {
                sChar[pChar++]='A';
                sInt[pInt++]=vV->index;
                if(tok==NAME)//tem que ser uma variavel pois a linha termina aqui
                {
                    vV=CheckVars(token.ReturnTheToken());
                    if(vV==NULL)
                        showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
                    sChar[pChar++]='V';
                    sInt[pInt++]=vV->index;
                    return 1;
                }
                else // tem que ser um numero
                {
                    sChar[pChar++]='N';
                    sInt[pInt++]=atoi(token.ReturnTheToken());
                    return 1;
                }
            }
            else if(ch=='(')//é o valor de uma função
            {
                indexEx=vV->index;
                if(token.GetTheNextChar()!=')')
                    showErr(ERROS.EXPECTED_END_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                vF=CheckFunction(token.ReturnTheToken());
                if(vF==NULL)
                    showErr(ERROS.UNKNOW_FUNCTION,token.ReturnActualLine(),token.ReturnTheToken());
                //adiciona o código da funcçao
//                if(smallSize==true)
//                {
//                    sChar[pChar++]='F';
//                	    sInt[pInt++]=vF->index;
//                }
//                else if(smallSize==false)
//                {
//                    for(i=0; i<=vF->sCharLen; i++)
//                        sChar[pChar++]=vF->sChar[i];
//                    for(i=0; i<vF->pInt; i++)
//                        sInt[pInt++]=vF->sInt[i];
//                }
                sprintf(tempEx,"F%s%d",vF->name,vF->index);
                vV=CheckVars(tempEx);
                if(vV==NULL)
                    showErr(ERROS.TYPE_NOT_MATCH,token.ReturnActualLine(),token.ReturnTheToken());
                if(token.IsTheEndOfTheLine())
                {
                    sChar[pChar++]='A';
                    sInt[pInt++]=indexEx;
                    sChar[pChar++]='V';
                    sInt[pInt++]=vV->index;
//                printf("valor de test: %d\n",vV->value);
                    return 1;
                }
                else return (evalExpression(2));
            }
            else return (evalExpression(2));
        }
        break;
    case 2://adicao e subtracao
        tok=token.GetTheNextToken();
        if(tok==NAME)//ou é uma variavel ou funcção
        {
            if(token.IsThisTheNextChar('('))// é uma função
            {
                if(token.GetTheNextChar()!=')')
                    showErr(ERROS.EXPECTED_END_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                vF=CheckFunction(token.ReturnTheToken());
                if(vF==NULL)
                    showErr(ERROS.UNKNOW_FUNCTION,token.ReturnActualLine(),token.ReturnTheToken());

            }
            else
            {
                vV=CheckVars(token.ReturnTheToken());
            }
        }
    case 3://multiplicação e divisão
    case 4://entre parenteses
    default:
        break;
    }
}

void evalAttrib()
{
    int tok;

    tok=token.GetTheNextToken();
    if(tok==ATTRIB || tok==READIN)//= ou <<
    {
        sChar[pChar++]='A';
        sInt[pInt++]=vV->index;
        tok=token.GetTheNextToken();
        if(tok==NAME)
        {
            vV=CheckVars(token.ReturnTheToken());
            if(vV==NULL)
                showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='V';
            sInt[pInt++]=vV->index;
            return;
        }
        else if(tok==NUMBER)
        {
            sChar[pChar++]='N';
            sInt[pInt++]=atoi(token.ReturnTheToken());
            return;
        }
    }
    else showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());
    return;
}

void evalConst()
{
    int j;
    char temp[20];

    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    if(CheckVars(token.ReturnTheToken()))
        showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
    strcpy(temp,token.ReturnTheToken());
    if(!token.GetAttrib())
        showErr(ERROS.EXPECTED_ASSIGN,token.ReturnActualLine(),token.ReturnTheToken());
    j=token.GetTheNextToken();
    if (j == STRING)
    {
        if (!token.GetStringInTheLine())
            showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
        if (!token.IsTheEndOfTheLine())
            showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
        token.IncreaseLineCounter();
        AddVar(temp,3,token.ReturnTheString());
        return;
    }
    else if (j == NUMBER)
    {
        if (!token.IsTheEndOfTheLine())
            showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
        token.IncreaseLineCounter();
        AddVar(temp,3,token.ReturnTheToken());
        return;
    }
    showErr(ERROS.EXPECTED_NAME_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
    return;
}

void evalIf()   //esta função vai desenvolver sobre a keyword IF, ELSE e ELSEIF
{
    char temp[100];
    int elsePos, endPos,tChar,elseInt,endInt,tok=1;

    //reconhece a variavel
    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    vV=CheckVars(token.ReturnTheToken());
    if(vV==NULL)
        showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
    //reconhece a condição
    int cond=token.GetComparasionSymbol();//passa o valor de comparação para o COND
    if(!cond)
    {
        //confirma se a variavel é do tipo numerico
        if(vV->type!=2)
            showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());
        if(!token.GetTheNextName())
            showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
        if(strcmp(token.ReturnTheToken(),"do"))
            showErr(ERROS.EXPECTED_DO,token.ReturnActualLine(),token.ReturnTheToken());

        sChar[pChar++]='I';
        sChar[pChar++]='C';
        sInt[pInt++]=vV->index;

        if(token.IsTheEndOfTheLine())//é um bloco
        {
            elsePos=pInt++;
            elseInt=pInt++;
            endPos=pInt++;
            endInt=pInt++;
            tChar=pChar++;
            while(tok<3 && tok>5)
                tok=evalBlock();
            if(tok==3 || tok==4)//encontrou em elseif ou else
            {
                sInt[elsePos]=pChar;
                sInt[elseInt]=pInt;
                while(tok!=5)//procura pelo endif
                    tok=evalBlock();
                sInt[endPos]=pChar;
                sInt[endInt]=pInt;
                sChar[tChar]='E';
            }
            else if(tok==5)//encontrou o endif sem else ou elseif
            {
                sInt[elsePos]=0;
                sInt[elseInt]=0;
                sInt[endPos]=pChar;
                sInt[endInt]=pInt;
                sChar[tChar]='F';
            }
            return;
        }
        else
        {
            sChar[pChar++]='L';//está na mesma linha
            endPos=pInt++;
            endInt=pInt++;
            evalBlock();
            sInt[endPos]=pChar;
            sInt[endInt]=pInt;
        }
        return;
    }
    sChar[pChar++]='I';
    //reconhece se a comparação é entre string ou numeros
    if(vV->type==1 )
    {
        if(token.GetTheNextChar()!='\'')
            showErr(ERROS.EXPECTED_SETENCE,token.ReturnActualLine(),token.ReturnTheToken());
        if(!token.GetStringInTheLine())
            showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
        sChar[pChar++]='S';
    }
    else if(vV->type==2)
    {
        if(!token.GetTheNextNumber())
            showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
        strcpy(temp,token.ReturnTheToken());
        sChar[pChar++]='N';
    }
    else if(vV->type==0 || vV->type==3)
    {
        if(vV->value[0]=='\0')
            showErr(ERROS.UNRESOLVED_VALUE,token.ReturnActualLine(),token.ReturnTheToken());

        //controla os valores de constante
        if(token.IsThisTheNextChar('\''))
        {
            if(!token.GetStringInTheLine())
                showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='S';
        }
        else
        {
            if(!token.GetTheNextNumber())
                showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='N';
        }
        strcpy(temp,token.ReturnTheToken());
    }
    //confirma a presença do DO
    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_DO,token.ReturnActualLine(),token.ReturnTheToken());
    if(strcmp(token.ReturnTheToken(),"do"))
        showErr(ERROS.EXPECTED_DO,token.ReturnActualLine(),token.ReturnTheToken());
    //reconhece o tipo de bloco
    //śo com uma linha ou um bloco

    sInt[pInt++]=vV->index;
//    printf("index: %d\n",vV->index);
    sInt[pInt++]=cond;
//    printf("cond: %d\n",cond);
    //FIXME
    if(vV->type==1)
        //tem que criar outra variavel com este valor
        sInt[pInt++]=strlen(token.ReturnTheString());
    else if(vV->type==2)
        sInt[pInt++]=atoi(temp);
//    puts(vV->name);
    if(token.IsTheEndOfTheLine())//é um bloco
    {
        elsePos=pInt++;
        elseInt=pInt++;
        endPos=pInt++;
        endInt=pInt++;
        tChar=pChar++;
        while(tok!=3 && tok!=5 && tok!=4)
            tok=evalBlock();
        if(tok==3 || tok==4)//encontrou em elseif ou else
        {
            sInt[elsePos]=pChar;
            sInt[elseInt]=pInt;
            while(tok!=5)//procura pelo endif
                tok=evalBlock();
            sInt[endPos]=pChar;
            sInt[endInt]=pInt;
            sChar[tChar]='E';
        }
        else if(tok==5)//encontrou o endif somente
        {
            //puts(sChar);
            sInt[elsePos]=0;
            sInt[elseInt]=0;
            sInt[endPos]=pChar;
            sInt[endInt]=pInt;
            sChar[tChar]='F';
        }
        return;
    }
    else
    {
        sChar[pChar++]='L';
        endPos=pInt++;
        endInt=pInt++;
        evalBlock();
        sInt[endPos]=pChar;
        sInt[endInt]=pInt;
    }
    return;
}

void evalPuts()
{
    char ch;
    int j;
    char tempStr[20];
    sChar[pChar++]='P';
    if(!token.GetStringInTheLine())
        showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
    if(strlen(token.ReturnTheString())>0)
    {
        sprintf(tempStr,"%d",pChar);
        AddVar(tempStr,3,token.ReturnTheString());
        vV=CheckVars(tempStr);
        sChar[pChar++]='V';
        sInt[pInt++]=vV->index;
//        printf("posicao: %d\n",pChar);
    }
    ch=token.GetTheNextChar();
    while(ch!='\n')
    {
        if(ch=='+')
        {
            j = token.GetTheNextToken();
            if (j == NAME)
            {
                vV=CheckVars(token.ReturnTheToken());
                if(vV==NULL)
                    showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='V';
                sInt[pInt++]=vV->index;
            }
            else if(j == STRING)
            {
                if(!token.GetStringInTheLine())
                    showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
                sprintf(tempStr,"%d",pChar);
                AddVar(tempStr,3,token.ReturnTheString());
                vV=CheckVars(tempStr);
                sChar[pChar++]='V';
                sInt[pInt++]=vV->index;
            }
        }
        else if(ch=='\'')
        {
            if(!token.GetStringInTheLine())
                showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
            if(strlen(token.ReturnTheString())>0)
            {
                sprintf(tempStr,"%d",pChar);
                AddVar(tempStr,3,token.ReturnTheString());
                vV=CheckVars(tempStr);
                sChar[pChar++]='V';
                sInt[pInt++]=vV->index;
            }
        }
        else if(ch==';')
        {
            token.GotoTheNextLine();
            sChar[pChar++]='N';
            break;
        }
        else if(ch=='>')
        {
            if(token.GetTheNextChar()!='>')
                showErr(ERROS.EXPECTED_ATTRIB,token.ReturnActualLine(),token.ReturnTheToken());
            //espera-se que seja uma variavel para receber o valor tipo read
            if(!token.GetTheNextName())
                showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
            vV=CheckVars(token.ReturnTheToken());
            if(vV==NULL)
                showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            if(vV->type==3)
                showErr(ERROS.CONSTANT_UNMODIFIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            //não precisa de mudar de linha porque o controlo de fim da linha já garantiu isso
            sChar[pChar++]='A';
            sInt[pInt++]=vV->index;
            break;
        }
        else
            showErr(ERROS.UNKNOW_TYPE,token.ReturnActualLine(),token.ReturnTheToken());
        ch=token.GetTheNextChar();
    }
    sChar[pChar++]='\n';
    token.IncreaseLineCounter();
    return;
}

void evalFunction()
{
    char name[100];
    int tok;
    short type;
    char nameV[255]="\0";

    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    strcpy(name,token.ReturnTheToken());
    if(token.GetTheNextChar()!='(')
        showErr(ERROS.EXPECTED_BEGIN_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
    tok=token.GetTheNextToken();
    if(tok==NAME)//é uma nome para parametros
    {
        //precisa de procurar adicionar os parametros á estrutura da função

    }
    else if(tok==CLOSEP)//é o fecho da função
    {
        //precisa somente de confirmar se ela tem retorno
        tok=token.GetTheNextChar();
        if(tok=='\n')//não tem retorno nem tem parametros
        {
            vF=AddFunction(name,0,NULL);
            processFunction(vF);
//			puts(vF->sChar);
            return;
        }
        else if(tok==':')//tem retorno definido
        {
            if(!token.GetTheNextName())
                showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
            type=checkVarTypes(token.ReturnTheToken());
            if(!type)
                showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
            vF=AddFunction(name,type,"\0");
            sprintf(nameV,"F%s%d",name,vF->index);
            AddVar(nameV,type,"\0");
            insideFunction=true;
            processFunction(vF);
            return;
        }
    }
}

void processFunction(FUNCTIONt* F)
{
	int tok=1;

    F->pChar=lastChar;
    F->pInt=lastInt;
    while(tok!=8)
        tok=evalBlock();
	sChar[pChar++]='%';
	lastChar=pChar;
	lastInt=pInt;
//
//    tempChar=new char[strlen(sChar)+1];
//    tempInt=new int[pInt];
//    int p=pInt;
//    int s=pChar;
//
//    memset((int*)tempInt,0,pInt*sizeof(int));
//    memset((char*)tempChar,0,strlen(sChar)+1);
//
//    memcpy((int*)tempInt,sInt,pInt*sizeof(int));
//    strcpy(tempChar,sChar);
//
//    memset((char*)sChar,0,strlen(sChar)+1);
//    memset((int*)sInt,0,pInt*sizeof(int));
//    pInt=0;
//    pChar=0;
//    while(tok!=8)
//        tok=evalBlock();
//
//    F->sInt=new int[pInt];
//    F->sChar=new char[pChar];
//    F->sCharLen=pChar-1;
//    memcpy((int*)F->sInt,sInt,pInt*sizeof(int));
//    F->pInt=pInt;
//    memcpy((int*)sInt,tempInt,p);
//    pInt=p;
//    pChar=s;
//    strcpy(F->sChar,sChar);
//    strcpy(sChar,tempChar);
//    delete []tempChar;
//    delete []tempInt;
//    insideFunction=false;
//    return;
}

void evalSpin()
{
    int posActual,j,tok=1;

    int init;
    sChar[pChar++]='S';
    j=token.GetTheNextToken();
    if(j==NAME)
    {
        if(!strcmp(token.ReturnTheToken(),"to"))// é modelo V
            //spin to A
        {
            j=token.GetTheNextToken();
            if(j==NAME)
            {
                vV=CheckVars(token.ReturnTheToken());
                if(vV==NULL)
                    showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
                vV->type=2;
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='V';
                sInt[pInt++]=vV->index;
                posActual=pChar;
                j=pInt++;
                while(tok!=9)
                    tok=evalBlock();
                sInt[j]=pChar-posActual;
                return;
            }
            else if(j==NUMBER)// é o modelo C
                //spin to 100
            {
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='C';
                sInt[pInt++]=atoi(token.ReturnTheToken());
                posActual=pChar;
                j=pInt++;
                while(tok!=9)
                    tok=evalBlock();
                sInt[j]=pChar-posActual;
            }
        }
        else if(!strcmp(token.ReturnTheToken(),"ever"))
            //spin ever
        {
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='P';
            posActual=pChar;
            j=pInt++;
            while(tok!=9)
                tok=evalBlock();
            sInt[j]=pChar-posActual;
            return;
        }
        else if(!strcmp(token.ReturnTheToken(),"from"))
            //spin from 0 to 10
        {
            if(!token.GetTheNextNumber())
                showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
            init=atoi(token.ReturnTheToken());
            if(!token.GetTheNextName())
                showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
            if(strcmp(token.ReturnTheToken(),"to"))
                showErr(ERROS.EXPECTED_TO,token.ReturnActualLine(),token.ReturnTheToken());
            if(!token.GetTheNextNumber())
                showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='S';
            sInt[pInt++]=init;
            sInt[pInt++]=atoi(token.ReturnTheToken());
            posActual=pChar;
            j=pInt++;
            while(tok!=9)
                tok=evalBlock();
            sInt[j]=pChar-posActual;
            return;
        }
        else
        {
            vV=CheckVars(token.ReturnTheToken());
            if(vV==NULL)
                showErr(ERROS.UNKNOW_VARIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            vV->type=2;
            if(!token.GetTheNextName())
                showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
//puts(token.ReturnTheToken());
            if(!strcmp(token.ReturnTheToken(),"from"))
                //spin A from 0
                //spin A from 0 to 10
            {
                if(!token.GetTheNextNumber())
                    showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
//puts(token.ReturnTheToken());
                init=atoi(token.ReturnTheToken());
                if(token.IsTheEndOfTheLine())
                    //spin A from 0
                {
                    sChar[pChar++]='I';
                    sInt[pInt++]=vV->index;
                    sInt[pInt++]=init;
                    posActual=pChar;
                    j=pInt++;
                    while(tok!=9)
                        tok=evalBlock();
                    sInt[j]=pChar-posActual;
                    return;
                }
                //spin A from 0 to 10
                if(!token.GetTheNextName())
                    showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                if(strcmp(token.ReturnTheToken(),"to"))
                    showErr(ERROS.EXPECTED_TO,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.GetTheNextNumber())
                    showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='N';
                sInt[pInt++]=vV->index;
                sInt[pInt++]=init;
                sInt[pInt++]=atoi(token.ReturnTheToken());
                posActual=pChar;
                j=pInt++;
                while(tok!=9)
                    tok=evalBlock();
                sInt[j]=pChar-posActual;
                return;
            }
            else if(!strcmp(token.ReturnTheToken(),"to"))
                //spin A to 10
            {
                if(!token.GetTheNextNumber())
                    showErr(ERROS.EXPECTED_NUMBER,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='U';
                sInt[pInt++]=vV->index;
                sInt[pInt++]=atoi(token.ReturnTheToken());
                posActual=pChar;
                j=pInt++;
                while(tok!=9)
                    tok=evalBlock();
                sInt[j]=pChar-posActual;
                return;
            }
            else showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
            return;
        }
        return;
    }
}

void evalConsole()
{

}

void evalService()
{

}

void evalWeb()
{

}

void evalMobile()
{

}

void evalLibrary()
{

}

void evalGui()
{
    int obj;
    int tok;
    int prop;
    int i;
    char ch,tempStr[10];

    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    obj=checkGuiObjects(token.ReturnTheToken());
    if(!obj)
        showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    tok=token.GetTheNextToken();
    if(tok==NL)
    {
        if(token.GetTheNextChar()!='{')
            showErr(ERROS.EXPECTED_BLOCK_BEGIN,token.ReturnActualLine(),token.ReturnTheToken());
    }
    else if(tok!=OPENB)
        showErr(ERROS.EXPECTED_BLOCK_BEGIN,token.ReturnActualLine(),token.ReturnTheToken());
    if(!token.IsTheEndOfTheLine())
        showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
    sChar[pChar++]=returnGuiId(obj);
    sChar[pChar++]='+';
    level++;
    while(level)
    {
        tok=token.GetTheNextToken();
        if(tok==NAME)//deve ser um objecto
        {
            //puts(token.ReturnTheToken());
            obj=checkGuiObjects(token.ReturnTheToken());
            if(!obj)
                showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
//            if(!token.IsTheEndOfTheLine())
//                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            tok=token.GetTheNextToken();
            if(tok==NL)
            {
                if(token.GetTheNextChar()!='{')
                    showErr(ERROS.EXPECTED_BLOCK_BEGIN,token.ReturnActualLine(),token.ReturnTheToken());
            }
            else if(tok!=OPENB)
                showErr(ERROS.EXPECTED_BLOCK_BEGIN,token.ReturnActualLine(),token.ReturnTheToken());
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());

            sChar[pChar++]=returnGuiId(obj);
            sChar[pChar++]='+';
            level++;
        }
        else if(tok==CLOSEB)
        {
            if(!token.IsTheEndOfTheLine())
                showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
            sChar[pChar++]='-';
            level--;
        }
        else if(tok==PONTUATOR)//é uma propriedade
        {
            if(!token.GetTheNextName())
                showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
            prop=checkGuiProperties(token.ReturnTheToken(),obj);
            if(!prop)
                showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
//            if(prop==1)//encontrou o end
//            {
//                if(!token.IsTheEndOfTheLine())
//                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
//                sChar[pChar++]='-';
//                level--;
//            }
//            else //é outra propriedade, precisa de se verificar a atribuição
//            {
            sChar[pChar++]=prop;
            switch(returnPropParam(obj,prop))
            {
            case 1://é uma string
                if(token.GetTheNextChar()!=':')
                    showErr(ERROS.EXPECTED_ATTRIB,token.ReturnActualLine(),token.ReturnTheToken());
                if(token.GetTheNextChar()!='\'')
                    showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.GetStringInTheLine())
                    showErr(ERROS.MISSING_QUOTE,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sprintf(tempStr,"%d",pChar);
                AddVar(tempStr,3,token.ReturnTheString());
                vV=CheckVars(tempStr);
                sInt[pInt++]=vV->index;
                break;
            case 2://é um number
                if(token.GetTheNextChar()!=':')
                    showErr(ERROS.EXPECTED_ATTRIB,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.GetTheNextNumber())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sInt[pInt++]=atoi(token.ReturnTheToken());
                break;
            case 3://é um boolean
                if(token.GetTheNextChar()!=':')
                    showErr(ERROS.EXPECTED_ATTRIB,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.GetTheNextName())
                    showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                if(!strcmp(token.ReturnTheToken(),"false"))
                    //sInt[pInt++]=0;
                    sChar[pChar++]=0;
                else if(!strcmp(token.ReturnTheToken(),"true"))
                    //sInt[pInt++]=1;
                    sChar[pChar++]=1;
                else
                    showErr(ERROS.UNKNOW_TYPE,token.ReturnActualLine(),token.ReturnTheToken());
                break;
            case 4://é um evento
                if(token.GetTheNextChar()!='.')
                    showErr(ERROS.EXPECTED_POINT,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.GetTheNextName())
                    showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                prop=checkGuiEvents(token.ReturnTheToken());
                if(!prop)
                    showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]=prop;
                if(token.GetTheNextChar()!=':')
                    showErr(ERROS.EXPECTED_ATTRIB,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.GetTheNextName())
                    showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                //precisa de ser uma função para já
                vF=CheckFunction(token.ReturnTheToken());
                if(vF==NULL)
                    showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                if(token.GetTheNextChar()!='(')
                    showErr(ERROS.EXPECTED_BEGIN_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                if(token.GetTheNextChar()!=')')
                    showErr(ERROS.EXPECTED_END_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
                sChar[pChar++]='F';
				sInt[pInt++]=vF->pChar;
                sInt[pInt++]=vF->pInt;
                break;
            default:
                showErr(ERROS.UNKNOW_TYPE,token.ReturnActualLine(),token.ReturnTheToken());
                break;
            }
//            }

        }
        else
            showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    }
    return;
}

void evalMain()
{
    int tok;

    insideFunction=true;
    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_KEY_AS,token.ReturnActualLine(),token.ReturnTheToken());
    if(strcmp(token.ReturnTheToken(),"as"))
        showErr(ERROS.EXPECTED_KEY_AS,token.ReturnActualLine(),token.ReturnTheToken());
    if(!token.GetTheNextName())
        showErr(ERROS.EXPECTED_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    tok=checkMainTypes(token.ReturnTheToken());
    if(!tok)
        showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
    if(!token.IsTheEndOfTheLine())
        showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
    switch(tok)
    {
    case 1:
        mainType='G';
        evalGui();
        break;
    case 2:
        mainType='C';
        evalConsole();
        break;
    case 3:
        mainType='S';
        evalService();
        break;
    case 4:
        mainType='W';
        evalWeb();
        break;
    case 5:
        mainType='M';
        evalMobile();
        break;
    case 6:
        mainType='L';
        evalLibrary();
        break;
    default:
        showErr(ERROS.UNKNOW_TYPE,token.ReturnActualLine(),token.ReturnTheToken());
        break;
    }
}

int evalBlock()
{
    int tok,i;
    char temp[100];

    tok=token.GetTheNextToken();
    if (tok== NAME)
    {
        if (!strcmp(token.ReturnTheToken(), "debug"))
        {
            evalDebug();
            return 1;
        }
        else if (!strcmp(token.ReturnTheToken(), "var"))
        {
            evalVar();
            return 1;
        }
        else if (!strcmp(token.ReturnTheToken(),"const"))
        {
            evalConst();
            return 1;
        }
        else if (!strcmp(token.ReturnTheToken(),"spin"))
        {
            evalSpin();
            return 1;
        }
        else if (!strcmp(token.ReturnTheToken(),"if"))
        {
            evalIf();
            return 1;
        }
        else if (!strcmp(token.ReturnTheToken(),"log"))
        {
            evalLog();
            return 1;
        }
        else if(!strcmp(token.ReturnTheToken(),"elseif"))
            return 3;
        else if (!strcmp(token.ReturnTheToken(),"read"))
        {
            evalRead();
            return 1;
        }
        else if(!strcmp(token.ReturnTheToken(),"else"))
            return 4;
        else if(!strcmp(token.ReturnTheToken(),"function"))
        {
            evalFunction();
            return 7;
        }
        else if(!strcmp(token.ReturnTheToken(),"endf"))
            return 8;
        else if(!strcmp(token.ReturnTheToken(),"endif"))
            return 5;
        else if(!strcmp(token.ReturnTheToken(),"exit"))
		{
            evalExit();
            return 21;
		}
        else if(!strcmp(token.ReturnTheToken(),"break"))
        {
            evalBreak();
            return 10;
        }
        else if(!strcmp(token.ReturnTheToken(),"main"))
        {
            evalMain();
            return 20;

        }
        else if(!strcmp(token.ReturnTheToken(),"continue"))
        {
            return 6;
        }
        else if(!strcmp(token.ReturnTheToken(),"next"))
            return 9;
        else
        {
            vV=CheckVars(token.ReturnTheToken());
            if(vV==NULL)
            {
                vF=CheckFunction(token.ReturnTheToken());
                if(vF==NULL)
                    showErr(ERROS.UNKNOW_NAME,token.ReturnActualLine(),token.ReturnTheToken());
                if(token.GetTheNextChar()!='(')
                    showErr(ERROS.EXPECTED_BEGIN_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                if(token.GetTheNextChar()!=')')
                    showErr(ERROS.EXPECTED_END_BRACKET,token.ReturnActualLine(),token.ReturnTheToken());
                if(!token.IsTheEndOfTheLine())
                    showErr(ERROS.EXPECTED_END_OF_LINE,token.ReturnActualLine(),token.ReturnTheToken());
//                if(smallSize==true)
//                {
//                    sChar[pChar++]='F';
//                    sInt[pInt++]=vF->index;
//                    return 1;
//                }
//                else if(smallSize==false)
//                {
//                    for(i=0; i<=vF->sCharLen; i++)
//                        sChar[pChar++]=vF->sChar[i];
//                    for(i=0; i<vF->pInt; i++)
//                        sInt[pInt++]=vF->sInt[i];
//                    return 1;
//                }

            }
            if(vV->type==3)
                showErr(ERROS.CONSTANT_UNMODIFIABLE,token.ReturnActualLine(),token.ReturnTheToken());
            //evalAttrib();
            evalExpression();
        }
    }
    else if(tok==STRING)
    {
        evalPuts();
        return 1;
    }
    else if(tok==END)
        exit(1);
    else if(tok==COMM)
    {
        token.GotoTheNextLine();
        return 1;
    }
    else if(tok==READIN)
    {
        evalReadIn();
        return 11;
    }
    else if(tok==NL)
        return 100;
    else if(tok==ERROR)
        return -1;
    return 1;
}

void saveAll()
{

    short sz=ReturnMaxSize(),mx=ReturnTotalVars();
    printf("Programa Compilado - %s\n",token.ReturnTheFileName());
    stop=clock();
    clock_t elapsed;
    elapsed = 1000 * (stop - start) / (CLOCKS_PER_SEC);
    printf("Demorou %ld ms a compilar\n",elapsed);
    rewind(fp);
    fprintf(fp,"RUN0.2");
    fwrite(&mx,sizeof(short),1,fp);
    fwrite(&sz,sizeof(short),1,fp);
	//PrintFunctions();
    SaveVarTable();
//    if(ReturnTotalFunctions()>0 && smallSize==true)
//    {
//        fputs("$",fp);
//        SaveFunctionTable();
//    }
    fputc('#',fp);
    fputc(mainType,fp);
    fwrite(&lastChar,sizeof(short),1,fp);
    fwrite(&lastInt,sizeof(short),1,fp);
    fwrite(&pChar,sizeof(short),1,fp);
    fwrite(&pInt,sizeof(short),1,fp);
    fprintf(fp,"I");
    fwrite(sInt,sizeof(int),pInt,fp);
    fprintf(fp,"C");
    fwrite(sChar,sizeof(char),pChar,fp);
    fprintf(fp,"\n");
    CloseTables();
    fclose(fp);
    free(sChar);
    free(sInt);
}


int main()
{
    char fname[10]="test.cod";

    int tok=0;//variavel de controlo de retorno do tokenizer
    InitTables();
    atexit(saveAll);
    token.LoadThisFile("test.run");
    //printf("o ficheiro tem o tamanho de %ld\n", token.ReturnTheSizeOfTheFile());

    sChar=new char[token.ReturnTheSizeOfTheFile()];
    if(sChar==NULL)
    {
        printf("ERRO: Não é possivel alocar memoria temporária para o codec");
        exit(-1);
    }
    memset((char*)sChar,0,token.ReturnTheSizeOfTheFile());

    sInt=new int[token.ReturnTheSizeOfTheFile()];
    if(sInt==NULL)
    {
        printf("ERRO: Não é possivel alocar memoria temporária para o loop spin");
        exit(-1);
    }
    memset((int*)sInt,0,token.ReturnTheSizeOfTheFile()*sizeof(int));

    printf("RUN v0.2\n\n");

    start=clock();
    //time(&start);
    if((fp=fopen(fname,"wb"))==NULL)
    {
        puts("ERRO: Não é permitido criar ficheiro com codec");
        exit(-1);
    }
    while(tok!=-1)
        tok=evalBlock();

    return 0;
}
