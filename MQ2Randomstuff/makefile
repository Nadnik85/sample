!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Randomstuff.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Randomstuff.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Randomstuff.dll"
	-@erase "$(OUTDIR)\MQ2Randomstuff.exp"
	-@erase "$(OUTDIR)\MQ2Randomstuff.lib"
	-@erase "$(OUTDIR)\MQ2Randomstuff.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Detours\lib60\detours.lib ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Randomstuff.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Randomstuff.dll" /implib:"$(OUTDIR)\MQ2Randomstuff.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Randomstuff.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Randomstuff.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Randomstuff.dep")
!INCLUDE "MQ2Randomstuff.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Randomstuff.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Randomstuff.cpp

"$(INTDIR)\MQ2Randomstuff.obj" : $(SOURCE) "$(INTDIR)"

