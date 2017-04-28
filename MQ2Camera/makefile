!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Camera.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Camera.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Camera.dll"
	-@erase "$(OUTDIR)\MQ2Camera.exp"
	-@erase "$(OUTDIR)\MQ2Camera.lib"
	-@erase "$(OUTDIR)\MQ2Camera.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Camera.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Camera.dll" /implib:"$(OUTDIR)\MQ2Camera.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Camera.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Camera.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Camera.dep")
!INCLUDE "MQ2Camera.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Camera.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Camera.cpp

"$(INTDIR)\MQ2Camera.obj" : $(SOURCE) "$(INTDIR)"

