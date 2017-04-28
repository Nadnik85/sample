!include "../global.mak"

ALL : "$(OUTDIR)\MQ2WinXP.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2WinXP.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2WinXP.dll"
	-@erase "$(OUTDIR)\MQ2WinXP.exp"
	-@erase "$(OUTDIR)\MQ2WinXP.lib"
	-@erase "$(OUTDIR)\MQ2WinXP.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2WinXP.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2WinXP.dll" /implib:"$(OUTDIR)\MQ2WinXP.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2WinXP.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2WinXP.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2WinXP.dep")
!INCLUDE "MQ2WinXP.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2WinXP.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2WinXP.cpp

"$(INTDIR)\MQ2WinXP.obj" : $(SOURCE) "$(INTDIR)"

