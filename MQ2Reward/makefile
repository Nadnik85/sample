!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Reward.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Reward.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Reward.dll"
	-@erase "$(OUTDIR)\MQ2Reward.exp"
	-@erase "$(OUTDIR)\MQ2Reward.lib"
	-@erase "$(OUTDIR)\MQ2Reward.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Reward.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Reward.dll" /implib:"$(OUTDIR)\MQ2Reward.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Reward.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Reward.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Reward.dep")
!INCLUDE "MQ2Reward.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Reward.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Reward.cpp

"$(INTDIR)\MQ2Reward.obj" : $(SOURCE) "$(INTDIR)"

