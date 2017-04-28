!include "../global.mak"

ALL : "$(OUTDIR)\MQ2NoNagWindows.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2NoNagWindows.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2NoNagWindows.dll"
	-@erase "$(OUTDIR)\MQ2NoNagWindows.exp"
	-@erase "$(OUTDIR)\MQ2NoNagWindows.lib"
	-@erase "$(OUTDIR)\MQ2NoNagWindows.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2NoNagWindows.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2NoNagWindows.dll" /implib:"$(OUTDIR)\MQ2NoNagWindows.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2NoNagWindows.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2NoNagWindows.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2NoNagWindows.dep")
!INCLUDE "MQ2NoNagWindows.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2NoNagWindows.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2NoNagWindows.cpp

"$(INTDIR)\MQ2NoNagWindows.obj" : $(SOURCE) "$(INTDIR)"

