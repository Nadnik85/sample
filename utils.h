

/* Usage

    CVoiceHandler *v = new CVoiceHandler();
    v->BuildMessage((PSPAWNINFO)pCharSpawn, (PSPAWNINFO)pTarget, "Wish to go");
    SendEQPacket(OP_MESSAGE, v->Get(), v->Size());
    delete v;
    v = 0;

*/

class CSerializeBuffer
{
#define MAX_BUFFER_SIZE 512
public:
    CSerializeBuffer()
    {
        pBuf = new unsigned char[MAX_BUFFER_SIZE];
        pPos = pBuf;
        memset(pBuf, 0, MAX_BUFFER_SIZE);
        pGet = 0;
    };

    ~CSerializeBuffer()
    {
        if (pBuf)
        {
            delete[] pBuf;
            pBuf = NULL;
        }
        if (pGet)
        {
            delete[] pGet;
            pGet = NULL;
        }
    };

    void AddString(char* szString)
    {
        if (szString && szString[0])
        {
            unsigned int uiLen = strlen(szString);
            strncpy((char*)pPos, szString, uiLen);
            pPos += uiLen;
        }
        pPos += 1;
    };

    template<typename T>
    void AddData(T data)
    {
        memset(pPos, (int)data, 1);
        pPos += sizeof(T);
    };

    unsigned long Size()
    {
        return (unsigned long)(pPos - pBuf);
    };

    void* Get()
    {
        if (Size())
        {
            pGet = new unsigned char[Size()];
            memcpy(pGet, pBuf, Size());
            return pGet;
        }
        return 0;
    }

protected:
    unsigned char* pBuf;
    unsigned char* pPos;
    unsigned char* pGet;
};

class CVoiceHandler : public CSerializeBuffer
{
public:
    void BuildMessage(PSPAWNINFO pSender, PSPAWNINFO pToTarget, char* szLine)
    {
        AddString(pSender->Name);             // sender
        AddString(pToTarget->Name);           // target
        AddData((unsigned long)0);            // unk
        AddData((unsigned long)0);            // language
        AddData((unsigned long)8);            // channel
        AddData((unsigned long)0);            // unk
        AddData((unsigned char)0);            // unk
        AddData((unsigned long)100);          // skill
        AddString(szLine);                    // message
        AddData((unsigned char)0);            // unk
        AddData((unsigned long)0);            // unk
        AddData((unsigned long)0);            // unk
        AddString(0);                         // unk
        AddData((unsigned char)0);            // unk
        AddData((unsigned long)0);            // unk
    };

    void HailTarget(PSPAWNINFO pSender, PSPAWNINFO pToTarget)
    {
        char szHail[MAX_BUFFER_SIZE] = {0};
        sprintf(szHail, "Hail, %s", pToTarget->DisplayedName);
        BuildMessage(pSender, pToTarget, szHail);
    };
};


class EQVector3
{
public:
    EQVector3(float Y, float X, float Z)
    {
        y = Y;
        x = X;
        z = Z;
    };
    
    EQVector3()
    {
        Clear();
    };
    
    void Clear()
    {
        y = 0;
        x = 0;
        z = 0;
    };

    float y;
    float x;
    float z;
};

class eqswitch
{
public:
    void UseSwitch(DWORD spawnID, int itemRecordNumber, int skill, EQVector3 *switchPos);
};

struct _switch {
    BYTE x[0x70];
    eqswitch *pEQSwitch;
};

struct _door {
/*0x00*/ BYTE  Unknown0x0[0xa4];
/*0xa4*/ _switch *pSwitch;
};