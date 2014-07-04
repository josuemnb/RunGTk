#ifndef GUI_H
#define GUI_H

int checkGuiObjects(char name[10]);
int checkGuiEvents(char name[10]);
int checkGuiProperties(char name[10], int loc);
int returnPropParam(int obj,int loc);
char returnGuiId(int loc);
#endif // GUI_H_INCLUDED
