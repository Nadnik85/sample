!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Spellhotkeys.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Spellhotkeys.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Spellhotkeys.dll"
	-@erase "$(OUTDIR)\MQ2Spellhotkeys.exp"
	-@erase "$(OUTDIR)\MQ2Spellhotkeys.lib"
	-@erase "$(OUTDIR)\MQ2Spellhotkeys.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Spellhotkeys.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Spellhotkeys.dll" /implib:"$(OUTDIR)\MQ2Spellhotkeys.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Spellhotkeys.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Spellhotkeys.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Spellhotkeys.dep")
!INCLUDE "MQ2Spellhotkeys.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Spellhotkeys.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Spellhotkeys.cpp

"$(INTDIR)\MQ2Spellhotkeys.obj" : $(SOURCE) "$(INTDIR)"

