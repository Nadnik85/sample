!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Paranoid.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Paranoid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Paranoid.dll"
	-@erase "$(OUTDIR)\MQ2Paranoid.exp"
	-@erase "$(OUTDIR)\MQ2Paranoid.lib"
	-@erase "$(OUTDIR)\MQ2Paranoid.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Paranoid.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Paranoid.dll" /implib:"$(OUTDIR)\MQ2Paranoid.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Paranoid.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Paranoid.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Paranoid.dep")
!INCLUDE "MQ2Paranoid.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Paranoid.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Paranoid.cpp

"$(INTDIR)\MQ2Paranoid.obj" : $(SOURCE) "$(INTDIR)"

