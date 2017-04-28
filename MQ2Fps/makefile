!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Fps.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Fps.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Fps.dll"
	-@erase "$(OUTDIR)\MQ2Fps.exp"
	-@erase "$(OUTDIR)\MQ2Fps.lib"
	-@erase "$(OUTDIR)\MQ2Fps.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Detours\lib60\detours.lib ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Fps.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Fps.dll" /implib:"$(OUTDIR)\MQ2Fps.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Fps.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Fps.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Fps.dep")
!INCLUDE "MQ2Fps.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Fps.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Fps.cpp

"$(INTDIR)\MQ2Fps.obj" : $(SOURCE) "$(INTDIR)"

