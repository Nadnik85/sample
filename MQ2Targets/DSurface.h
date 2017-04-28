/*
// Direct Surface 0.127
// Function Definition Header
// Dec 27/2006
// By Odessa (eqplugins@gmail.com)
*/

#pragma once
#pragma pack(push)
#pragma pack(1)

// defines a 3d point
struct point {
	float x;
	float y;
	float z;
};

//defines a line in 3d space
struct line {
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;
};

//defines an intersection line in 3d space
struct iline {
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;
	unsigned short intersections;
};

#pragma pack(pop)

typedef int sprite;
typedef int surface;

// int DSCreateSurface()
// Creates a surface to work with
// Parameters: empty
// Return: Number of the assigned surface
typedef surface (*_DSCreateSurface)();

// bool DSDestroySurface(surface dsurface)
// Destroys a surface
// Parameters: 
// surface dsurface - surface to destroy
// Return: true - success, false - failure
typedef bool (*_DSDestroySurface)(surface dsurface);

// bool DSCreateSprite(surface dsurface, HMODULE handle, LPCTSTR sprite, float width, float height, float fwidth, float fheight, int frames, float angle, DWORD filter, DWORD mipfilter, DWORD colorkey)
// Creates a sprite to be used on a surface
// Parameters: 
// surface dsurface - surface
// HMODULE handle - handle to the module containing a sprite resource
// LPCTSTR sprite - name of the sprite resource
// int width - sprite width
// int height - sprite height
// int fwidth - sprite frame width
// int fheight - sprite frame height
// int frames - number of frames in a sprite
// float angle - angle of rotation of sprite frame
// DWORD filter - sprite filter
// DWORD mipfilter - sprite mipmap filter
// DWORD colorkey - sprite color key
// Return: true - success, false - failure
typedef sprite (*_DSCreateSprite) (surface dsurface, HMODULE handle, LPCTSTR sprite, float width, float height, float fwidth, float fheight, int frames, float angle, DWORD filter, DWORD mipfilter, DWORD colorkey);

// char* DSGetVersion()
// Returns a string with DirectSurface version
// Parameters: empty
// Return: a string with DirectSurface version
typedef char* (*_DSGetVersion)();

// bool DSAddSprite(surface dsurface, float x, float y, DWORD color, sprite ssprite, float angle, float scale)
// Adds a sprite to a surface
// Parameters:
// surface dsurface - surface
// float x - x-coordinated of the sprite
// float y - y-coordinate of the sprite
// DWORD color - color of the sprite
// sprite ssprite - sprite
// float angle - angle of the sprite
// float scale - scale of the sprite
// Return: true - success, false - failure
typedef bool (*_DSAddSprite)(surface dsurface, float x, float y, DWORD color, sprite ssprite, float angle, float scale);

// bool DSClearSprites(surface dsurface)
// Clears out all the sprites on a surface
// Parameters:
// surface dsurface - surface
// Return: true - success, false - failure
typedef bool (*_DSClearSprites)(surface dsurface);

// bool DSAddLine(surface dsurface, float x1, float y1, float x2, float y2, DWORD color)
// Adds a line to the surface
// Parameters:
// surface dsurface - surface
// float x1, float y1 - start of the line
// float x2, float y2 - end of the line
// DWORD color - color of the line
typedef bool (*_DSAddLine)(surface dsurface, float x1, float y1, float x2, float y2, DWORD color);

// bool DSClearLines(surface dsurface)
// Clears out all the lines on a surface
// Parameters:
// surface dsurface - surface
// Return: true - success, false - failure
typedef bool (*_DSClearLines)(surface dsurface);

// bool DSAddText(surface dsurface, float x1, float y1, float x2, float y2, char* text, DWORD color)
// Adds text to the surface
// Parameters:
// surface dsurface - surface
// float x1, float y1 - top of textbox
// float x2, float y2 - bottom of textbox
// char* text - text
// DWORD color - text color
// Return: true - success, false - failure
typedef bool (*_DSAddText)(surface dsurface, float x1, float y1, float x2, float y2, char* text, DWORD color);

// bool DSClearText(surface dsurface)
// Clears out all the text on a surface
// Parameters:
// surface dsurface - surface
// Return: true - success, false - failure
typedef bool (*_DSClearText)(surface dsurface);

// bool DSChangeAngle(surface dsurface, float angle)
// Changes the angle of a surface
// Parameters:
// surface dsurface - surface
// angle - angle
// Return: true - success, false - failure
typedef bool (*_DSChangeAngle)(surface dsurface, float angle);

// bool DSChangeCenter(surface dsurface, float x, float y)
// Changes the center of a surface
// Parameters:
// surface dsurface - surface
// float x, float y - center
// Return: true - success, false - failure
typedef bool (*_DSChangeCenter)(surface dsurface, float x, float y);

// bool DSChangeScale(surface dsurface, float scale)
// Changes the scale of a surface
// Parameters:
// surface dsurface - surface
// scale - scale
// Return: true - success, false - failure
typedef bool (*_DSChangeScale)(surface dsurface, float scale);

// bool DSDrawCircle(surface dsurface, float x, float y, float radius, sprite ssprite, DWORD color)
// Draws a circle of sprites
// Parameters:
// surface dsurface - surface
// float x, float y - center of a circle
// float radius - radius of a circle
// sprite ssprite - sprite
// DWORD color - sprite color
// Return: true - success, false - failure
typedef bool (*_DSDrawCircle)(surface dsurface, float x, float y, float radius, sprite ssprite, DWORD color);

// iline DSCLIntersection(float cx, float cy, float radius, float x1, float y1, float z1, float x2, float y2, float z2)
// Returns intersecion(s) of a line and a circle
// Parameters:
// float cx, float cy - center of circle
// float radius - radius of circle
// float x1, float y1, float z1 - beginning of a line
// float x2, float y2, float z2 - end of a line
// Return: iline of intersection(s)
typedef iline (*_DSCLIntersection)(float cx, float cy, float radius, float x1, float y1, float z1, float x2, float y2, float z2);

// bool DSPointInCircle(float cx, float cy, float radius, float x1, float y1)
// Checks if the point is inside a circle
// Parameters:
// float cx, float cy - center of circle 
// float radius - radius of circle
// float x1, float y1 - point
// Return: true - if a point is inside a circle, false - if a point is outside a circle
typedef bool (*_DSPointInCircle)(float cx, float cy, float radius, float x1, float y1);

// bool DSPointInCircleBorder(float cx, float cy, float radius, float x1, float y1, float bordersize)
// Checks if the point is inside a circle border
// Parameters:
// float cx, float cy - center of circle 
// float radius - radius of circle
// float x1, float y1 - point
// float bordersize - size of the border
// Return: true - if a point is inside circle border, false - if a point is outside circle border
typedef bool (*_DSPointInCircleBorder) (float cx, float cy, float radius, float x, float y, float bordersize);

// bool DSPointDegInCircle(float cx, float cy, float radius, float x1, float y1, float degangle)
// Checks if the point rotated degangle degrees is inside a circle
// Parameters:
// float cx, float cy - center of circle 
// float radius - radius of circle
// float x1, float y1 - point
// float degangle - angle in degrees
// Return: true - if a point is inside a circle, false - if a point is outside a circle
typedef bool (*_DSPointDegInCircle)(float cx, float cy, float radius, float x1, float y1, float degangle);

// point DSPointDeg(float x,float y, float degangle)
// Returns a point rotated degangle degrees
// Parameters:
// float x, float y - coordinates of a point
// float degangle - angle in degrees
// Return: rotated point
typedef point (*_DSPointDeg)(float x,float y, float degangle);

// point DSLineDeg(float x1,float y1, float x2,float y2,float degangle)
// Returns a line rotated degangle degrees
// Parameters:
// float x1, float y1 - beginning of the line
// float x2, float y2 - end of the line
// float degangle - angle in degrees
// Return: rotated line
typedef line (*_DSLineDeg)(float x1,float y1, float x2,float y2, float degangle);

// void DSWireFrame(bool wireframe)
// Sets wireframe mode
// Parameters:
// bool wireframe - if true enable wireframe mode, if false disable
// Return: true - success, false - failure

typedef bool (*_DSWireFrame)(bool wireframe);

_DSCreateSurface DSCreateSurface;
_DSDestroySurface DSDestroySurface;
_DSCreateSprite DSCreateSprite;
_DSGetVersion DSGetVersion;
_DSAddSprite DSAddSprite;
_DSClearSprites DSClearSprites;
_DSAddLine DSAddLine;
_DSClearLines DSClearLines;
_DSClearText DSClearText;
_DSAddText DSAddText;
_DSChangeAngle DSChangeAngle;
_DSChangeCenter DSChangeCenter;
_DSChangeScale DSChangeScale;
_DSDrawCircle DSDrawCircle;
_DSCLIntersection DSCLIntersection;
_DSPointInCircle DSPointInCircle;
_DSPointInCircleBorder DSPointInCircleBorder;
_DSPointDegInCircle DSPointDegInCircle;
_DSPointDeg DSPointDeg;
_DSLineDeg DSLineDeg;
_DSWireFrame DSWireFrame;


bool InitializeDS()
{
	HMODULE hDSurface = GetModuleHandle("DSurface.dll");
	OutputDebugString("Checking DirectSurface...");
	if (!GetProcAddress(hDSurface, "DSGetVersion")) 
	{
		OutputDebugString("Failed!\r\n");
		return false;
	}
	DSCreateSurface = (_DSCreateSurface)GetProcAddress(hDSurface, "DSCreateSurface");
	DSDestroySurface = (_DSDestroySurface)GetProcAddress(hDSurface, "DSDestroySurface");
	DSCreateSprite = (_DSCreateSprite)GetProcAddress(hDSurface, "DSCreateSprite");
	DSGetVersion = (_DSGetVersion)GetProcAddress(hDSurface, "DSGetVersion");
	DSAddSprite = (_DSAddSprite)GetProcAddress(hDSurface, "DSAddSprite");
	DSClearSprites = (_DSClearSprites)GetProcAddress(hDSurface, "DSClearSprites");
	DSAddLine = (_DSAddLine)GetProcAddress(hDSurface, "DSAddLine");
	DSClearLines = (_DSClearLines)GetProcAddress(hDSurface, "DSClearLines");
	DSAddText = (_DSAddText)GetProcAddress(hDSurface, "DSAddText");
	DSClearText = (_DSClearText)GetProcAddress(hDSurface, "DSClearText");
	DSChangeAngle = (_DSChangeAngle)GetProcAddress(hDSurface, "DSChangeAngle");
	DSChangeCenter = (_DSChangeCenter)GetProcAddress(hDSurface, "DSChangeCenter");
	DSChangeScale = (_DSChangeScale)GetProcAddress(hDSurface, "DSChangeScale");
	DSDrawCircle = (_DSDrawCircle)GetProcAddress(hDSurface, "DSDrawCircle");
	DSCLIntersection = (_DSCLIntersection)GetProcAddress(hDSurface, "DSCLIntersection");
	DSPointInCircle = (_DSPointInCircle)GetProcAddress(hDSurface, "DSPointInCircle");
	DSPointInCircleBorder = (_DSPointInCircleBorder)GetProcAddress(hDSurface, "DSPointInCircleBorder");
	DSPointDegInCircle = (_DSPointDegInCircle)GetProcAddress(hDSurface, "DSPointDegInCircle");
	DSPointDeg = (_DSPointDeg)GetProcAddress(hDSurface, "DSPointDeg");
	DSLineDeg = (_DSLineDeg)GetProcAddress(hDSurface, "DSLineDeg");	
	DSWireFrame = (_DSWireFrame)GetProcAddress(hDSurface, "DSWireFrame");	

	OutputDebugString("Found!\r\n");
	return true;
}