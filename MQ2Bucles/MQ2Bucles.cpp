
/****************************************************************************
 Plugin Name  : MQ2Bucles
 Version      : 1.0
 Made by      : el_nene
 EMail        : eq.nene@gmail.com
 Description  : MQ2Bucles adds while and do bucles to macros
*****************************************************************************
 Commands :
-----------------------------------------------------------------------------
 /while (<condition>)
 ...
 ...
 /endwhile
  - Executes a bucle, evaluating the condition on each iteration.
-----------------------------------------------------------------------------
 /do
 ...
 ...
 /until (<condition>)
 - Executes a bucle, evaluating the condition on each iteration but on first
 one.
-----------------------------------------------------------------------------
 /break
 - Stops a /while, /do or /for bucle.
-----------------------------------------------------------------------------
 /continue
  - Stops a /while, /do or /for bucle, reevaluating the condition.
*****************************************************************************
 History      :
-----------------------------------------------------------------------------
 - Version 1.0 - Date: 21-01-2008
 -> ADDED Functions
 --> /while
 --> /endwhile
 --> /do
 --> /until
 --> /break
 --> /continue
*****************************************************************************/
//Updated by eqmule on sep 1 2017 for new macrostack changes, although this plugin is obsolete now since its functionality are included in core.


#include "../MQ2Plugin.h"


PreSetup("MQ2Bucles");

// While commands
VOID CommandWhile(PSPAWNINFO pChar, PCHAR szLine);
VOID CommandEndWhile(PSPAWNINFO pChar, PCHAR szLine);

// Do commands
VOID CommandDo(PSPAWNINFO pChar, PCHAR szLine);
VOID CommandUntil(PSPAWNINFO pChar, PCHAR szLine);

// Break commands
VOID CommandBreak(PSPAWNINFO pChar, PCHAR szLine);
VOID CommandContinue(PSPAWNINFO pChar, PCHAR szLine);

// Helpers
VOID EndWhile(PSPAWNINFO pChar, int StartLine);
VOID DoBreak(PSPAWNINFO pChar, int StartLine, BOOL evaluate = false);
VOID ContinueDo(PSPAWNINFO pChar, int StartLine);
VOID ContinueWhile(PSPAWNINFO pChar, int StartLine);

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
   AddCommand("/while", CommandWhile);
   AddCommand("/endwhile", CommandEndWhile);
   AddCommand("/do", CommandDo);
   AddCommand("/until", CommandUntil);
   AddCommand("/break", CommandBreak);
   AddCommand("/continue", CommandContinue);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
   RemoveCommand("/while");
   RemoveCommand("/endwhile");
   RemoveCommand("/do");
   RemoveCommand("/until");
   RemoveCommand("/break");
   RemoveCommand("/continue");
}

// ***************************************************************************
// Function:    ContinueWhile
// Description: Continue a /while bucle execution.Searchs the /while command
//            and set the execution pointer over that command.
// ***************************************************************************
VOID ContinueWhile(PSPAWNINFO pChar, int StartLine)
{
	if (!gMacroBlock) {
		DebugSpewNoFile("EndWhile - Macro was ended before we could handle the end of while command");
		return;
	}
	std::map<int, MACROLINE>::iterator i = gMacroBlock->Line.find(StartLine);
	std::map<int, MACROLINE>::reverse_iterator ri(i);
	bool bFound = false;
	for(;ri!=gMacroBlock->Line.rend();ri++)
	{
		if (!_strnicmp(ri->second.Command.c_str(),"sub ",4))
		{
			gMacroBlock->CurrIndex=StartLine;
			FatalError("/while ran into another subroutine");
            return;
		}
		if (!_strnicmp(ri->second.Command.c_str(),"/while",6))
		{
			ri--;
			if (ri != gMacroBlock->Line.rend())
			{
				bFound = true;
				gMacroBlock->CurrIndex = ri->first;
			}
			break;
		}
	}
	if (!bFound) {
		gMacroBlock->CurrIndex=StartLine;
		FatalError("/while without pairing /endwhile");
	}
}


// ***************************************************************************
// Function:    EndWhile
// Description: Ends a /while bucle execution. Searchs the /endwhile command
//            and set the execution pointer at that command.
// ***************************************************************************
VOID EndWhile(PSPAWNINFO pChar, int StartLine)
{
	if (!gMacroBlock) {
		DebugSpewNoFile("EndWhile - Macro was ended before we could handle the end of while command");
		return;
	}
	std::map<int, MACROLINE>::iterator i = gMacroBlock->Line.find(StartLine);
	bool bFound = false;
	for (;i!=gMacroBlock->Line.end();i++)
	{
		if (!_strnicmp(i->second.Command.c_str(),"sub ",4))
		{
			gMacroBlock->CurrIndex=StartLine;
			FatalError("/while ran into another subroutine");
			return;
		}
		if (!_strnicmp(i->second.Command.c_str(), "/endwhile", 9)) {
			gMacroBlock->CurrIndex = i->first;
			bFound = true;
			break;
		}
	}
	if (!bFound) {
		gMacroBlock->CurrIndex=StartLine;
		FatalError("/while without pairing /endwhile");
	}
}

// ***************************************************************************
// Function:    ContinueDo
// Description: Continues a /do bucle execution. Searchs the /do command
//            and set the execution pointer at that command.
// ***************************************************************************
VOID ContinueDo(PSPAWNINFO pChar, int StartLine)
{
	if (!gMacroBlock) {
		DebugSpewNoFile("ContinueDo - Macro was ended before we could handle the do command");
		return;
	}
	std::map<int, MACROLINE>::iterator i = gMacroBlock->Line.find(StartLine);
	std::map<int, MACROLINE>::reverse_iterator ri(i);
	bool bFound = false;
	for (;ri!=gMacroBlock->Line.rend();ri++)
	{
		if (!_strnicmp(ri->second.Command.c_str(),"sub ",4))
		{
			gMacroBlock->CurrIndex=StartLine;
			FatalError("/until without pairing /do");
			return;
		}
		if (!_strnicmp(ri->second.Command.c_str(), "/do", 3)) {
			gMacroBlock->CurrIndex = ri->first;
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		gMacroBlock->CurrIndex = StartLine;
		FatalError("/until without pairing /do");
	}
}

// ***************************************************************************
// Function:    DoBreak
// Description: Breaks the execution flow within a bucle.
//            If evaluate is set, the function set the execution pointer at
//            the end of the bucle, so the condition is evaluate.
// ***************************************************************************
VOID DoBreak(PSPAWNINFO pChar, int StartLine, BOOL evaluate)
{
	if (!gMacroBlock)
	{
		DebugSpewNoFile("DoBreak - Macro was ended before we could handle the break command");
		return;
	}
	std::map<int, MACROLINE>::iterator i = gMacroBlock->Line.find(StartLine);
	bool bFound = false;
	for (; i != gMacroBlock->Line.end(); i++)
	{
		if (!_strnicmp(i->second.Command.c_str(), "Sub ", 4))
		{
			gMacroBlock->CurrIndex = StartLine;
			FatalError("/break or /continue called outside a bucle");
			return;
		}
		if (!_strnicmp(i->second.Command.c_str(), "/until", 6) ||
			!_strnicmp(i->second.Command.c_str(), "/endwhile", 9) ||
			!_strnicmp(i->second.Command.c_str(), "/next", 4))
		{
			int index = i->first;
			i--;
			if (i != gMacroBlock->Line.end()) {
				int previndex = i->first;
				//We are on the /until or /endwhile command
				gMacroBlock->CurrIndex = (evaluate) ? previndex : index;
				bFound = true;
			}
			break;
		}
	}
	if (!bFound) {
		gMacroBlock->CurrIndex = StartLine;
		FatalError("/break or /continue called outside a bucle");
	}
}

// ***************************************************************************
// Function:    CommandContinue
// Description: Our '/continue' command
// Usage:       /continue
// ***************************************************************************
VOID CommandContinue(PSPAWNINFO pChar, PCHAR szLine)
{
   if (!gMacroBlock)
   {
        MacroError("Can only use /continue during a macro.");
        return;
    }
   bRunNextCommand = TRUE;

   if (szLine[0])
   {
       FatalError("Failed to parse /continue command. Unexpected parameters.");
      SyntaxError("Usage: /continue");
        return;
   }

   DoBreak(pChar, gMacroBlock->CurrIndex, true);
}

// ***************************************************************************
// Function:    CommandBreak
// Description: Our '/break' command
// Usage:       /break
// ***************************************************************************
VOID CommandBreak(PSPAWNINFO pChar, PCHAR szLine)
{
   if (!gMacroBlock)
   {
        MacroError("Can only use /break during a macro.");
        return;
    }
   bRunNextCommand = TRUE;

   if (szLine[0])
   {
       FatalError("Failed to parse /break command. Unexpected parameters.");
      SyntaxError("Usage: /break");
        return;
   }

   DoBreak(pChar, gMacroBlock->CurrIndex);
}

// ***************************************************************************
// Function:    CommandDo
// Description: Our '/do' command
// Usage:       /do
// ***************************************************************************
VOID CommandDo(PSPAWNINFO pChar, PCHAR szLine)
{
    if (!gMacroBlock)
   {
        MacroError("Can only use /do during a macro.");
        return;
    }
   bRunNextCommand = TRUE;
   if (szLine[0])
   {
       FatalError("Failed to parse /do command. Unexpected parameters.");
      SyntaxError("Usage: /do");
        return;
   }
}

// ***************************************************************************
// Function:    CommandUntil
// Description: Our '/until' command
// Usage:       /until (<conditions>)
// ***************************************************************************
VOID CommandUntil(PSPAWNINFO pChar, PCHAR szLine)
{
    CHAR szCond[MAX_STRING] = {0};
   CHAR WhileLine[MAX_STRING] = {0};

    if (!gMacroBlock)
   {
        MacroError("Can only use /until during a macro.");
        return;
    }
   bRunNextCommand = TRUE;
   if (szLine[0]!='(')
   {
       FatalError("Failed to parse /until command.  Expected () around conditions.");
      SyntaxError("Usage: /until (<conditions>)");
        return;
   }
   PCHAR pEnd=&szLine[1];
   DWORD nParens=1;
   while(1)
   {
      if (*pEnd=='(')
         nParens++;
      else if (*pEnd==')')
      {
         nParens--;
         if (nParens==0)
         {
            pEnd++;
            if (*pEnd!=0)
            {
               FatalError("Failed to parse /until command. Parameters after conditions.");
               SyntaxError("Usage: /until (<conditions>)");
               return;
            }
            break;
         }
      }
      else if (*pEnd==0)
      {
         FatalError("Failed to parse /until command.  Could not find condition to evaluate.");
         SyntaxError("Usage: /until (<conditions>)");
         return;
      }
      ++pEnd;
   } // while
   strcpy_s(szCond,szLine);

   DOUBLE Result=0;
   if (!Calculate(szCond,Result))
   {
      FatalError("Failed to parse /until condition '%s', non-numeric encountered",szCond);
      return;
   }
   if (Result==0)
      ContinueDo(pChar, gMacroBlock->CurrIndex);
      
}

// ***************************************************************************
// Function:    CommandWhile
// Description: Our '/while' command
// Usage:       /while (<conditions>)
// ***************************************************************************
VOID CommandWhile(PSPAWNINFO pChar, PCHAR szLine)
{
    CHAR szCond[MAX_STRING] = {0};
    if (!gMacroBlock) {
        MacroError("Can only use /while during a macro.");
        return;
    }
   bRunNextCommand = TRUE;
   if (szLine[0]!='(')
   {
       FatalError("Failed to parse /while command.  Expected () around conditions.");
      SyntaxError("Usage: /while (<conditions>)");
        return;
   }
   PCHAR pEnd=&szLine[1];
   DWORD nParens=1;
   while(1)
   {
      if (*pEnd=='(')
         nParens++;
      else if (*pEnd==')')
      {
         nParens--;
         if (nParens==0)
         {
            pEnd++;
            if (*pEnd!=0)
            {
               FatalError("Failed to parse /while command. Parameters after conditions.");
               SyntaxError("Usage: /while (<conditions>)");
               return;
            }
            break;
         }
      }
      else if (*pEnd==0)
      {
         FatalError("Failed to parse /while command.  Could not find conditions to evaluate.");
         SyntaxError("Usage: /while (<conditions>)");
         return;
      }
      ++pEnd;
   } // while

   strcpy_s(szCond,szLine);

   DOUBLE Result=0;
   if (!Calculate(szCond,Result))
   {
      FatalError("Failed to parse /while condition '%s', non-numeric encountered",szCond);
      return;
   }
   if (Result==0)
      EndWhile(pChar, gMacroBlock->CurrIndex);
}

// ***************************************************************************
// Function:    CommandEndWhile
// Description: Our '/endwhile' command
// Usage:       /endwhile
// ***************************************************************************
VOID CommandEndWhile(PSPAWNINFO pChar, PCHAR szLine)
{
    if (!gMacroBlock) {
        MacroError("Can only use /endwhile during a macro.");
        return;
    }

    bRunNextCommand = TRUE;
   if (szLine[0])
   {
       FatalError("Failed to parse /endwhile command. Unexpected parameters.");
      SyntaxError("Usage: /endwhile");
        return;
   }

   ContinueWhile(pChar, gMacroBlock->CurrIndex);
} 