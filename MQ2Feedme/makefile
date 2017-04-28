!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Feedme.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Feedme.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Feedme.dll"
	-@erase "$(OUTDIR)\MQ2Feedme.exp"
	-@erase "$(OUTDIR)\MQ2Feedme.lib"
	-@erase "$(OUTDIR)\MQ2Feedme.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Detours\lib60\detours.lib ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Feedme.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Feedme.dll" /implib:"$(OUTDIR)\MQ2Feedme.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Feedme.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Feedme.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Feedme.dep")
!INCLUDE "MQ2Feedme.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Feedme.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Feedme.cpp

"$(INTDIR)\MQ2Feedme.obj" : $(SOURCE) "$(INTDIR)"

