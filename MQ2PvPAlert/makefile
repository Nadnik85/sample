!include "../global.mak"

ALL : "$(OUTDIR)\MQ2PvPAlert.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2PvPAlert.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2PvPAlert.dll"
	-@erase "$(OUTDIR)\MQ2PvPAlert.exp"
	-@erase "$(OUTDIR)\MQ2PvPAlert.lib"
	-@erase "$(OUTDIR)\MQ2PvPAlert.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2PvPAlert.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2PvPAlert.dll" /implib:"$(OUTDIR)\MQ2PvPAlert.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2PvPAlert.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2PvPAlert.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2PvPAlert.dep")
!INCLUDE "MQ2PvPAlert.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2PvPAlert.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2PvPAlert.cpp

"$(INTDIR)\MQ2PvPAlert.obj" : $(SOURCE) "$(INTDIR)"

