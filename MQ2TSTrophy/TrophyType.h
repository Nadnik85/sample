#pragma once
#include "../MQ2Plugin.h"

class MQ2TrophyType : public MQ2Type
{
public:
	enum TrophyMembers
	{
		Version = 1,
		Matches = 2,
		Alchemy = 3,
		Baking = 4,
		Brewing = 5,
		Fletching = 6,
		Jewelry = 7,
		Tinkering = 8,
		Pottery = 9,
		Research = 10,
		Blacksmithing = 11,
		Tailoring = 12,
		Poison = 13,
		Fishing = 14,
		Container = 15,
	};

	MQ2TrophyType();
	~MQ2TrophyType();
	bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR &Dest);
	bool ToString(MQ2VARPTR VarPtr, char* Destination);
	bool FromData(MQ2VARPTR& VarPtr, MQ2TYPEVAR &Source);
	bool FromString(MQ2VARPTR& VarPtr, char* Source);

};
extern MQ2TrophyType* pTrophyType;

BOOL TrophyData(char* szIndex, MQ2TYPEVAR& Dest);