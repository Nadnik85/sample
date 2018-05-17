#include "../MQ2Plugin.h"

PLUGIN_VERSION(0.1);
PreSetup("MQ2Clipboard");

VOID ClipCommand(PSPAWNINFO pChar, PCHAR szLine);

bool clipping = false;
bool append = false;
std::string	buffer;

void StartClipping();
void AddToBuffer(PCHAR Line);
void SetClipboardString(const std::string &str);
void StopClipping();

HANDLE SetClipboardDataEx(UINT uFormat, const void *pvData, DWORD cbData);

PLUGIN_API VOID InitializePlugin(VOID)
{
	AddCommand("/clip", ClipCommand);
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	RemoveCommand("/clip");
}

PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	AddToBuffer(Line);
	return 0;
}
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	AddToBuffer(Line);
	return 0;
}

VOID ClipCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	char arg[MAX_STRING] = { 0 };

	GetArg(arg, szLine, 1);
	if (!_stricmp(arg, "append"))
	{
		append = true;
		szLine = GetNextArg(szLine, 1);
	}
	else
	{
		append = false;
	}

	if (!_stricmp(szLine, "start"))
	{
		WriteChatf("MQ2Clipboard starting clipping");
		StartClipping();
	}
	else if (!_stricmp(szLine, "stop"))
	{
		StopClipping();
	}
	else if (szLine[0] == '/')
	{
		StartClipping();
		HideDoCommand(pChar, szLine, FALSE);
		StopClipping();
	}
	else if (!_stricmp(szLine, "command"))
	{
		buffer = (append ? buffer + "\r\n" : "") + szLastCommand;
		SetClipboardString(buffer);
	}
	else
	{
		buffer = (append ? buffer + "\r\n" : "") + szLine;
		SetClipboardString(buffer);
	}
}

void StartClipping()
{
	if (!append)
		buffer = "";
	clipping = true;
}

void StopClipping()
{
	clipping = false;
	SetClipboardString(buffer);
}

void AddToBuffer(PCHAR Line)
{
	if (clipping)
	{
		char strippedLine[MAX_STRING] = { 0 };
		StripMQChat(Line, strippedLine);

		if (buffer.length() > 0)
			buffer.append("\r\n");
		buffer.append(strippedLine);
	}
}

void SetClipboardString(const std::string &str)
{
	if (str.length() == 0)
		return;

	auto nLines = std::count(str.begin(), str.end(), '\n') + 1;

	std::string strToCopy = str;

	if (!OpenClipboard(NULL))
	{
		WriteChatf("MQ2Clipboard failed to open clipboard");
		return;
	}

	if (!EmptyClipboard())
	{
		WriteChatf("MQ2Clipboard failed to clear existing clipboard content");
	}
	else
	{
		if (!SetClipboardDataEx(CF_TEXT, strToCopy.c_str(), strToCopy.length() + 1))
		{
			WriteChatf("MQ2Clipboard failed to clear existing clipboard content");
		}
		else
		{
			WriteChatf("MQ2Clipboard copied %d %s to the clipboard", nLines, nLines > 1 ? "lines" : "line");
		}
	}
	if (!CloseClipboard())
		WriteChatf("MQ2Clipboard failed to close clipboard");
}

// https://blogs.msdn.microsoft.com/oldnewthing/20100125-00/?p=15183/
// Added const on pvData argument
HANDLE SetClipboardDataEx(UINT uFormat, const void *pvData, DWORD cbData)
{
	if (uFormat == CF_BITMAP ||
		uFormat == CF_DSPBITMAP ||
		uFormat == CF_PALETTE ||
		uFormat == CF_METAFILEPICT ||
		uFormat == CF_DSPMETAFILEPICT ||
		uFormat == CF_ENHMETAFILE ||
		uFormat == CF_DSPENHMETAFILE ||
		uFormat == CF_OWNERDISPLAY) {
		return NULL; // these are not HGLOBAL format
	}

	HANDLE hRc = NULL;
	HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT,
		cbData);
	if (hglob) {
		void *pvGlob = GlobalLock(hglob);
		if (pvGlob) {
			CopyMemory(pvGlob, pvData, cbData);
			GlobalUnlock(hglob);
			hRc = SetClipboardData(uFormat, hglob);
		}
		if (!hRc) {
			GlobalFree(hglob);
		}
	}
	return hRc;
}