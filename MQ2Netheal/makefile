!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Netheal.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Netheal.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Netheal.dll"
	-@erase "$(OUTDIR)\MQ2Netheal.exp"
	-@erase "$(OUTDIR)\MQ2Netheal.lib"
	-@erase "$(OUTDIR)\MQ2Netheal.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Netheal.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Netheal.dll" /implib:"$(OUTDIR)\MQ2Netheal.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Netheal.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Netheal.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Netheal.dep")
!INCLUDE "MQ2Netheal.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Netheal.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Netheal.cpp

"$(INTDIR)\MQ2Netheal.obj" : $(SOURCE) "$(INTDIR)"

