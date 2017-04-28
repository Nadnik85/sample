!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Combat.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Combat.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Combat.dll"
	-@erase "$(OUTDIR)\MQ2Combat.exp"
	-@erase "$(OUTDIR)\MQ2Combat.lib"
	-@erase "$(OUTDIR)\MQ2Combat.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Detours\lib60\detours.lib ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Combat.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Combat.dll" /implib:"$(OUTDIR)\MQ2Combat.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Combat.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Combat.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Combat.dep")
!INCLUDE "MQ2Combat.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Combat.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Combat.cpp

"$(INTDIR)\MQ2Combat.obj" : $(SOURCE) "$(INTDIR)"

