!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Heals.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Heals.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Heals.dll"
	-@erase "$(OUTDIR)\MQ2Heals.exp"
	-@erase "$(OUTDIR)\MQ2Heals.lib"
	-@erase "$(OUTDIR)\MQ2Heals.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Heals.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Heals.dll" /implib:"$(OUTDIR)\MQ2Heals.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Heals.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Heals.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Heals.dep")
!INCLUDE "MQ2Heals.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Heals.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Heals.cpp

"$(INTDIR)\MQ2Heals.obj" : $(SOURCE) "$(INTDIR)"

