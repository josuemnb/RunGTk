#include "Structs.h"
#include "Error.h"

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

VARt *V_TABLE;// listas principais
FUNCTIONt *F_TABLE;
LIBt *L_TABLE;

VARt* vV;// listas auxiliares
LIBt* vL;
FUNCTIONt* vF;

extern FILE *fp;

int maxSize=0;
int totalVars=0;
int totalFunctions=0;

char *result;
//obrigatorio executar esta função no inicio do main
void InitTables(void)
{
	V_TABLE=NULL;
	//L_TABLE=NULL;
	F_TABLE=NULL;
	vV=NULL;
	vF=NULL;
	//vL=NULL;
}

void CloseTables()
{
    VARt *start, *list_ptr;
    start=vV;
    while (start) {
        list_ptr = start;
        start = list_ptr->next;
        free(list_ptr);
    }
    free(result);
}

int SaveVarTable(void)
{
	sVAR *V;
	int sInt[totalVars*3];
    char sChar[maxSize*totalVars];
    int i=0,l;

    memset((char*)sChar,0,maxSize*totalVars);
    memset((int*)sInt,0,totalVars*3);

	if(V_TABLE==NULL)
		return 0;

	V=V_TABLE;
	while(V)
	{
        sInt[i++]=V->index;
//        printf("index: %d\n",V->index);
        sInt[i++]=V->type;
        sInt[i++]=strlen(V->value);
//        printf("Value: %s\n",V->value);
        if(strlen(V->value)>0)
            strcat(sChar,V->value);
        V=V->next;
	}
	l=strlen(sChar);
//	printf("tamanho: %d\n",strlen(sChar));
	//escreve a quantidade de valores na stack int
	fwrite(&i,sizeof(short),1,fp);
	//escreve a quantidade de strings na stack char
	fwrite(&l,sizeof(short),1,fp);
    fprintf(fp,"\n");
    //escreve o array de ints
    fwrite(sInt,sizeof(int),i,fp);
    fprintf(fp,"\n");
    //escreve o array de chars
    fwrite(sChar,sizeof(char),l,fp);
    fprintf(fp,"\n");
	return 1;
}

int SaveFunctionTable()
{
//	FUNCTIONt *F;
//    int l;
//
//	if(F_TABLE==NULL)
//		return 0;
//
//	l=ReturnTotalFunctions();
//	//escreve o numero total de funções
//	fwrite(&l,sizeof(short),1,fp);
//
//	F=F_TABLE;
//	while(F)
//	{
//		fwrite(&F->index,sizeof(short),1,fp);
////		printf("index: %d\n",F->index);
////        sInt[i++]=F->index;
//        fwrite(&F->pInt,sizeof(short),1,fp);
////        printf("pint: %d\n",F->pInt);
////		sInt[i++]=F->pInt;
//		fwrite(&F->sCharLen,sizeof(short),1,fp);
////		printf("scharlen: %d\n",F->sCharLen);
////        sInt[i++]=F->sCharLen;
//        fwrite(&F->ret,sizeof(short),1,fp);
////        sInt[i++]=F->ret;
//		fwrite(&F->n_param,sizeof(short),1,fp);
////        sInt[i++]=F->n_param;
//		fwrite(F->param,sizeof(char),10,fp);
////			strcat(sChar,F->param);
//		fwrite(F->sInt,sizeof(int),F->pInt,fp);
//		fwrite(F->sChar,sizeof(char),strlen(F->sChar),fp);
//		fputc('\n',fp);
//
//        F=F->next;
//	}
	return 1;
}

void PrintVars(void)
{
	sVAR *V;

	char str[10];

	if(V_TABLE==NULL)
		return;

	V=V_TABLE;
	while(V)
	{
		printf("Name: %s, ",V->name);
		printf("Index: %d, ",V->index);
		if(V->type==1)strcpy(str,"STRING");
		else if(V->type==2)strcpy(str,"INT");
		else if(V->type==3)strcpy(str,"CONST");
		else strcpy(str,"UNDEF");
		printf("Type: %s, ", str);
		if(strlen(V->value)<1)
            puts("Valor: NULL");
        else
            printf("Valor: %s\n",V->value);

		V=V->next;
	}
	return;
}

void PrintFunctions()
{
    FUNCTIONt *F;

    if(F_TABLE==NULL)
		return;

	F=F_TABLE;
	while(F)
	{
        printf("Name: %s, ", F->name);
        printf("Valor em string: %s, ",F->pChar);
        //printf("Tamanho do sChar: %d, ",F->sCharLen);
        printf("Tamanho dos inteiros: %d\n", F->pInt);
		F=F->next;
	}
	return;
}

FUNCTIONt* CheckLibFunction(LIBt L,char *name)// percorre todo o vector de variaveis e verifica se existe
{
	FUNCTIONt *V;

	if(L.n_functions ==0)
		return 0;
	V=L.functions;
	while(V!='\0')
	{
		if(!strcmp(V->name,name))
			return V;
		V=V->next;
	}
	return '\0';
}

int AddLibFunction(LIBt* L,char name[100])
{
	FUNCTIONt *V;

		//é necessário abrir o ficheiro e pedir as definicoes com respeito á funcao

		V=(FUNCTIONt*)malloc(sizeof(FUNCTIONt));
		if(V=='\0')
		{
			printf("ERRO: Não é possivel alocar memoria para a função %s dentro da biblioteca %s",name,L->name);
			exit(-1);
		}
		strcpy(V->name,name);
		V->next='\0';
		if(L_TABLE->functions =='\0')
			L_TABLE->functions =V;
		else
		{
			V->next =L_TABLE->functions;
			L_TABLE->functions =V;
		}
		return 1;
}

FUNCTIONt* CheckFunction(char *name)// percorre todo o vector de variaveis e verifica se existe
{
	FUNCTIONt *V;

	if(F_TABLE=='\0')
		return 0;
	V=F_TABLE;
	while(V!='\0')
	{
		if(!strcmp(V->name,name))
			return V;
		V=V->next;
	}
	return '\0';
}


FUNCTIONt* AddFunction(char name[100],short ret, char param[10])
{
	FUNCTIONt *V;

    V=(FUNCTIONt*)malloc(sizeof(FUNCTIONt));
    if(V=='\0')
    {
        printf("Não é possivel alocar memoria para a função %s",name);
        exit(-1);
    }
    if(param==NULL)
        V->n_param=0;
    else
        V->n_param=strlen(param);
    strcpy(V->name,name);
    if(param!=NULL)
		strcpy(V->param,param);
	else V->param[0]='\0';
    V->ret=ret;
	V->next='\0';
	V->index=totalFunctions;
    if(F_TABLE=='\0')
        F_TABLE=V;
    else
    {
        V->next =F_TABLE;
        F_TABLE=V;
    }
    totalFunctions++;
    return V;
}

VARt* CheckVars(char *name)// percorre todo o vector de variaveis e verifica se existe
{
	VARt *V=V_TABLE;

	if(V=='\0')
		return NULL;
	while(V!=NULL)
	{
		if(!strcmp(V->name,name))
			return V;
        V=V->next;
	}
	return NULL;
}

void UpdateVar(VARt * V ){

    int l=strlen(V->value);

    if(V->maxSize<l)
        V->maxSize=l;
    V->hasChanged=true;
    V->nChanged++;
    if(isdigit(vV->value[0]))
        vV->type=2;
    else
        vV->type=1;
    return;
}

int AddVar(char name[20],char type, const char value[100])
{
    VARt *V;
    V=(VARt*)malloc(sizeof(VARt));
    if(V=='\0'){
        printf("ERRO: Não foi possivel alocar memoria para a variavel %s",name);
        exit(-1);
    }
    V->index=totalVars;
    V->type=type;
    strcpy(V->name,name);
    strcpy(V->value,value);
    V->next='\0';
    if(V_TABLE=='\0')
        V_TABLE=V;
    else {
        V->next =V_TABLE;
        V_TABLE=V;
    }
    int size=strlen(value);
    if(size>maxSize)
        maxSize=size;
    totalVars++;
    return 1;
}

short ReturnTotalVars(){ return totalVars; }
short ReturnMaxSize(){ return maxSize; }
short ReturnTotalFunctions(){ return totalFunctions; }

LIBt* CheckLibrary(char *name)// percorre todo o vector de bibiotecas e verifica se existe
{
	LIBt *V;

	if(L_TABLE=='\0')
		return 0;
	V=L_TABLE;
	while(V!='\0')
	{
		if(!strcmp(V->name,name))
			return V;
		else V=V->next;
	}
	return '\0';
}

int AddLib(char name[100], char address[1000])
{
	LIBt *V;

		V=(LIBt*)malloc(sizeof(LIBt));
		if(V=='\0'){
			printf("Não é possivel alocar memoria para a biblioteca %s",name);
			exit(-1);
		}
		strcpy(V->name,name);
		strcpy(V->address,address);
		V->n_functions=0;
		V->status=1;
		V->next='\0';
		if(L_TABLE=='\0')
			L_TABLE=V;
		else
		{
			V->next =L_TABLE;
			L_TABLE=V;
		}
		return 1;
}
