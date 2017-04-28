!include "../global.mak"

ALL : "$(OUTDIR)\MQ2PlatTracker.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2PlatTracker.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2PlatTracker.dll"
	-@erase "$(OUTDIR)\MQ2PlatTracker.exp"
	-@erase "$(OUTDIR)\MQ2PlatTracker.lib"
	-@erase "$(OUTDIR)\MQ2PlatTracker.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2PlatTracker.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2PlatTracker.dll" /implib:"$(OUTDIR)\MQ2PlatTracker.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2PlatTracker.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2PlatTracker.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2PlatTracker.dep")
!INCLUDE "MQ2PlatTracker.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2PlatTracker.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2PlatTracker.cpp

"$(INTDIR)\MQ2PlatTracker.obj" : $(SOURCE) "$(INTDIR)"

