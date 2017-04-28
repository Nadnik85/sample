!include "../global.mak"

ALL : "$(OUTDIR)\MQ2AutoCamp.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2AutoCamp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2AutoCamp.dll"
	-@erase "$(OUTDIR)\MQ2AutoCamp.exp"
	-@erase "$(OUTDIR)\MQ2AutoCamp.lib"
	-@erase "$(OUTDIR)\MQ2AutoCamp.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2AutoCamp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2AutoCamp.dll" /implib:"$(OUTDIR)\MQ2AutoCamp.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2AutoCamp.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2AutoCamp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2AutoCamp.dep")
!INCLUDE "MQ2AutoCamp.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2AutoCamp.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2AutoCamp.cpp

"$(INTDIR)\MQ2AutoCamp.obj" : $(SOURCE) "$(INTDIR)"

