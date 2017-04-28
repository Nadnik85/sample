!include "../global.mak"

ALL : "$(OUTDIR)\MQ2AutoLogin.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2AutoLogin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2AutoLogin.dll"
	-@erase "$(OUTDIR)\MQ2AutoLogin.exp"
	-@erase "$(OUTDIR)\MQ2AutoLogin.lib"
	-@erase "$(OUTDIR)\MQ2AutoLogin.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2AutoLogin.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2AutoLogin.dll" /implib:"$(OUTDIR)\MQ2AutoLogin.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2AutoLogin.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2AutoLogin.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2AutoLogin.dep")
!INCLUDE "MQ2AutoLogin.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2AutoLogin.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2AutoLogin.cpp

"$(INTDIR)\MQ2AutoLogin.obj" : $(SOURCE) "$(INTDIR)"

