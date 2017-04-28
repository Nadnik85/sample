!include "../global.mak"

ALL : "$(OUTDIR)\MQ2AFNuke.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2AFNuke.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2AFNuke.dll"
	-@erase "$(OUTDIR)\MQ2AFNuke.exp"
	-@erase "$(OUTDIR)\MQ2AFNuke.lib"
	-@erase "$(OUTDIR)\MQ2AFNuke.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2AFNuke.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2AFNuke.dll" /implib:"$(OUTDIR)\MQ2AFNuke.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2AFNuke.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2AFNuke.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2AFNuke.dep")
!INCLUDE "MQ2AFNuke.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2AFNuke.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2AFNuke.cpp

"$(INTDIR)\MQ2AFNuke.obj" : $(SOURCE) "$(INTDIR)"

