/*
 * File:   structs.h
 * Author: jony
 *
 * Created on 12 de Junho de 2013, 22:46
 */

#ifndef STRUCTS_H
#define	STRUCTS_H

//estruturas para tabelas de variaveis, bibliotecas e funcoes

struct RESERVED {
    char name[10];
    char type;
    char group;
};

typedef struct sVAR{// não precisa de valores pois não é executado, só conferido

	char name[20];
	short type;
	sVAR *next;
	char value[100];
    short index;
    bool hasChanged;
    short nChanged;
    short maxSize;
}VARt;

typedef struct sFUNCTION {
	char name[100];
	short ret;//tipo de retorno
	char param[10];// definição dos tipos de parametros
	short n_param;//numero de parametros que pede
	sFUNCTION *next;
	short index;
    short pInt;
    short pChar;
}FUNCTIONt;

typedef struct sLIB{
	char name[100];
	char address[1000];
	int n_functions;
	sFUNCTION *functions;
	sLIB *next;
	char status;
	int lenName;
	int lenAddress;
    int index;
}LIBt;

int AddLib(char name[100], char address[1000]);
LIBt* CheckLibrary(char *name);
int AddVar(char name[20],char type, const char value[100]);
VARt* CheckVars(char *name);
FUNCTIONt* AddFunction(char name[100],short ret, char param[10]);
FUNCTIONt* CheckFunction(char *name);
FUNCTIONt* CheckLibFunction(LIBt L,char *name);
int AddLibFunction(LIBt* L,char name[100]);
void InitTables(void);
void PrintVars(void);
void PrintFunctions();
void CloseTables(void);
int SaveVarTable(void);
int SaveFunctionTable(void);
sVAR * GetVars(void);
short ReturnTotalVars();
short ReturnMaxSize();
short ReturnTotalFunctions();
void UpdateVar(VARt *V);//estruturas para o tokenizer, parser, scanner

extern VARt *V_TABLE;// listas principais
extern FUNCTIONt *F_TABLE;
extern LIBt *L_TABLE;

extern VARt* vV;// listas auxiliares
extern LIBt* vL;
extern FUNCTIONt* vF;

#endif	/* STRUCTS_H */

