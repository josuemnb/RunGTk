#include <string.h>

#include "Gui.h"


char GUIOBJS[][10]={"","Window","Button","Label","TextBox","CheckBox","Image"};
char GUIID[10]={'0','W','B','L','T','C','I'};

char GUIPROPS[7][10][10]=
{
	{""},
	{"","title","id","width","height","top","left","visible","state","On"},//WINDOW
	{"","title","id","width","height","top","left","visible","state","On"},//BUTTON
	{"","title","id","width","height","top","left","visible","state","On"},//LABEL
	{"","text","id","width","height","top","left","visible","enable","On"},//TEXTBOX
	{"","text","id","width","height","top","left","visible","enable","On"},//CHECKBOX
	{"","file","id","width","height","top","left","visible","enable","On"}//IMAGE
};

char GUIEVENTS[16][10]=
{"","Click","Down","Up",
	"Enter","Leave","Move",
	"Resize","Showing","Hiding",
	"Paint","FocusOut","FocusIn",
	"Press","Release"};

short GUIPARAM[7][10]=
{
	//1-STRING, 2-NUMBER, 3-BOOL, 4-EVENT
    {0},//TIPO DE ENTRADA PARA AS PROPRIEDADES DOS OBJECTOS
    {0,1,1,2,2,2,2,3,2,4},//WINDOW
    {0,1,1,2,2,2,2,3,2,4},//BUTTON
    {0,1,1,2,2,2,2,3,3,0},//LABEL
    {0,1,1,2,2,2,2,3,3,4},//TEXT
    {0,1,1,2,2,2,2,3,3,4},//CHECKBOX
    {0,1,1,2,2,2,2,3,3,0}//IMAGE
};

int checkGuiObjects(char name[10])
{
	int i;
    for(i=1;GUIOBJS[i];i++)
        if(!strcmp(name,GUIOBJS[i]))
            return i;
    return 0;
}

int checkGuiEvents(char name[10])
{
	int i;
    for(i=1;GUIEVENTS[i];i++)
        if(!strcmp(name,GUIEVENTS[i]))
            return i;
    return 0;
}

char returnGuiId(int loc)
{
	return GUIID[loc];
}

int checkGuiProperties(char name[10], int loc)
{
	int i;
    for(i=1;GUIPROPS[loc][i];i++)
        if(!strcmp(name,GUIPROPS[loc][i]))
            return i;
    return 0;
}

int returnPropParam(int obj, int loc)
{
	return GUIPARAM[obj][loc];
}
