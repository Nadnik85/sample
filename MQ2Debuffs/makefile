!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Debuffs.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Debuffs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Debuffs.dll"
	-@erase "$(OUTDIR)\MQ2Debuffs.exp"
	-@erase "$(OUTDIR)\MQ2Debuffs.lib"
	-@erase "$(OUTDIR)\MQ2Debuffs.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Debuffs.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Debuffs.dll" /implib:"$(OUTDIR)\MQ2Debuffs.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Debuffs.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Debuffs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Debuffs.dep")
!INCLUDE "MQ2Debuffs.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Debuffs.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Debuffs.cpp

"$(INTDIR)\MQ2Debuffs.obj" : $(SOURCE) "$(INTDIR)"

