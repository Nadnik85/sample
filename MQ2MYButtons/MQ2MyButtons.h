//MQ2MyButtons.h 
//9/26/2004 
void MyButtonWindowShow(PSPAWNINFO pChar, PCHAR szLine); 
void CreateButtonWindow(); 
void DestroyButtonWindow(); 
void CmdMyButtonClick1(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick2(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick3(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick4(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick5(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick6(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick7(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick8(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick9(PSPAWNINFO pChar, PCHAR szLine); 
void CmdMyButtonClick10(PSPAWNINFO pChar, PCHAR szLine); 
void ReadWindowINI(PCSIDLWND pWindow); 
void SaveWindowINI(PCSIDLWND pWindow); 
void LoadMyButtons(); 
char szMyCommand1[MAX_STRING]= {0}; 
char szMyCommand2[MAX_STRING]= {0}; 
char szMyCommand3[MAX_STRING]= {0}; 
char szMyCommand4[MAX_STRING]= {0}; 
char szMyCommand5[MAX_STRING]= {0}; 
char szMyCommand6[MAX_STRING]= {0}; 
char szMyCommand7[MAX_STRING]= {0}; 
char szMyCommand8[MAX_STRING]= {0}; 
char szMyCommand9[MAX_STRING]= {0}; 
char szMyCommand10[MAX_STRING]= {0}; 
bool winActive=true; 