!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Timestamp.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Timestamp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Timestamp.dll"
	-@erase "$(OUTDIR)\MQ2Timestamp.exp"
	-@erase "$(OUTDIR)\MQ2Timestamp.lib"
	-@erase "$(OUTDIR)\MQ2Timestamp.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Detours\lib60\detours.lib ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Timestamp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Timestamp.dll" /implib:"$(OUTDIR)\MQ2Timestamp.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Timestamp.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Timestamp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Timestamp.dep")
!INCLUDE "MQ2Timestamp.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Timestamp.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Timestamp.cpp

"$(INTDIR)\MQ2Timestamp.obj" : $(SOURCE) "$(INTDIR)"

