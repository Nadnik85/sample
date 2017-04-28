!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Targets.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Targets.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Targets.dll"
	-@erase "$(OUTDIR)\MQ2Targets.exp"
	-@erase "$(OUTDIR)\MQ2Targets.lib"
	-@erase "$(OUTDIR)\MQ2Targets.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Targets.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Targets.dll" /implib:"$(OUTDIR)\MQ2Targets.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Targets.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Targets.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Targets.dep")
!INCLUDE "MQ2Targets.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Targets.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Targets.cpp

"$(INTDIR)\MQ2Targets.obj" : $(SOURCE) "$(INTDIR)"

