!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Missing.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Missing.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Missing.dll"
	-@erase "$(OUTDIR)\MQ2Missing.exp"
	-@erase "$(OUTDIR)\MQ2Missing.lib"
	-@erase "$(OUTDIR)\MQ2Missing.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Missing.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Missing.dll" /implib:"$(OUTDIR)\MQ2Missing.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Missing.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Missing.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Missing.dep")
!INCLUDE "MQ2Missing.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Missing.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Missing.cpp

"$(INTDIR)\MQ2Missing.obj" : $(SOURCE) "$(INTDIR)"

