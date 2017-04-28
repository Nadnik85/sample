!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Casttimer.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Casttimer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Casttimer.dll"
	-@erase "$(OUTDIR)\MQ2Casttimer.exp"
	-@erase "$(OUTDIR)\MQ2Casttimer.lib"
	-@erase "$(OUTDIR)\MQ2Casttimer.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Casttimer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Casttimer.dll" /implib:"$(OUTDIR)\MQ2Casttimer.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Casttimer.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Casttimer.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Casttimer.dep")
!INCLUDE "MQ2Casttimer.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Casttimer.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Casttimer.cpp

"$(INTDIR)\MQ2Casttimer.obj" : $(SOURCE) "$(INTDIR)"

