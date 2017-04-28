!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Kiss.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Kiss.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Kiss.dll"
	-@erase "$(OUTDIR)\MQ2Kiss.exp"
	-@erase "$(OUTDIR)\MQ2Kiss.lib"
	-@erase "$(OUTDIR)\MQ2Kiss.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Kiss.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Kiss.dll" /implib:"$(OUTDIR)\MQ2Kiss.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Kiss.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Kiss.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Kiss.dep")
!INCLUDE "MQ2Kiss.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Kiss.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Kiss.cpp

"$(INTDIR)\MQ2Kiss.obj" : $(SOURCE) "$(INTDIR)"

