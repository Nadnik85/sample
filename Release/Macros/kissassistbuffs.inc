| KissAssist Ultimate Buff System Inc file v 1.1 09/07/2013
| kissassistbuffs.inc
| Redguides Very Vanilla Compile required.
| Add this include file to any macro you want toons using kissassist to buff
| Usage:
| #include kissassistbuffs.inc
| /call WriteBuffs

|----------------------------------------------------------------------------
| SUB: Write Buffs
|----------------------------------------------------------------------------
Sub WriteBuffs
    /if (!${Defined[WriteBuffsTimer]}) /declare WriteBuffsTimer timer outer 0
    /if (!${Defined[BBWindowOpen]}) /declare BBWindowOpen int outer 0
    /if (${WriteBuffsTimer}) /return
    /declare i int local 0
    /declare k int local 0
    /declare Blockedbuff string local
    /declare Blockedbufflist string local
    /declare BuffList string local
    /declare Writebufflist string local
    /call CleanBuffsFile
    | Write day of the month and hour. Entry will be deleted if more than a hour old
        /if (!${Ini["KissAssist_Buffs.ini",${Me.ID},Day].Length}) /ini "KissAssist_Buffs.ini" "${Me.ID}" Day "${Time.Day}"
        /if (!${Ini["KissAssist_Buffs.ini",${Me.ID},Hour].Length}) /ini "KissAssist_Buffs.ini" "${Me.ID}" Hour "${Time.Hour}"
        /if (!${Ini["KissAssist_Buffs.ini",${Me.ID},Zone].Length}) /ini "KissAssist_Buffs.ini" "${Me.ID}" Zone "${Zone.ID}"
        /if (!${Ini["KissAssist_Buffs.ini",${Me.ID},Buffs].Length}) /ini "KissAssist_Buffs.ini" "${Me.ID}" Buffs
        /if (!${Ini["KissAssist_Buffs.ini",${Me.ID},Blockedbuffs].Length}) /ini "KissAssist_Buffs.ini" "${Me.ID}" Blockedbuffs
    | Write all current buffs to Kissassist_Buffs.ini file
    /for i 0 to 40 step 1
        /if (${Window[BuffWindow].Child[BW_Buff${i}_Button].Child[Buff${i}].Tooltip.Find[(]}) {
            /varset BuffList ${Window[BuffWindow].Child[BW_Buff${i}_Button].Child[Buff${i}].Tooltip.Arg[1,(].Left[-1]} 
        } else {
            /varset BuffList ${Window[BuffWindow].Child[BW_Buff${i}_Button].Child[Buff${i}].Tooltip} 
        }        
        /if (${BuffList.Length}) {
            /varset Writebufflist ${Writebufflist}|${BuffList}
        }
    /next i
    /ini "KissAssist_Buffs.ini" "${Me.ID}" Buffs "${Writebufflist}"
    /delay 15
    | Open Blocked Buffs window temporarily to read blocked buffs
     /if (!${Window[BlockedBuffWnd].Open} && !${BBWindowOpen}) {
        /windowstate BlockedBuffWnd open
        /varset BBWindowOpen 300m
    }
    | Write all blocked buffs to Kissassist_Buffs.ini file
    /for k 0 to 29
        /varset Blockedbuff ${Window[BlockedBuffWnd].Child[BW_BLOCKEDBUFF${k}_Button].Child[BBuff${k}].Tooltip} 
        /if (${Blockedbuff.Length}) {
            | /echo ${Window[BlockedBuffWnd].Child[BW_BLOCKEDBUFF${k}_Button].Child[BBuff${k}].Tooltip} 
            /varset Blockedbufflist ${Blockedbufflist}|${Blockedbuff}
        }
    /next k
    /if (${Blockedbufflist.Length}) /ini "KissAssist_Buffs.ini" "${Me.ID}" Blockedbuffs "${Blockedbufflist}"
    /if (${Window[BlockedBuffWnd].Open}) /windowstate BlockedBuffWnd close
    /varset WriteBuffsTimer 1m
/return
|----------------------------------------------------------------------------
| SUB: Clean Buffs File
|----------------------------------------------------------------------------
Sub CleanBuffsFile
    /declare i int local
    /declare SectionList string local ${Ini["KissAssist_Buffs.ini"]}
    /declare SectionName string local
    /for i 1 to ${SectionList.Count[|]}
        /varset SectionName ${SectionList.Arg[${i},|]}
        /if (${DebugBuffs}) /echo ${i} ${SectionName} ${Math.Calc[${Ini["KissAssist_Buffs.ini",${SectionName},Day]}-${Time.Day}]} ${Math.Calc[${Time.Hour}-${Ini["KissAssist_Buffs.ini",${SectionName},Hour]}]}
        /if (${Ini["KissAssist_Buffs.ini",${SectionName},Day].NotEqual[${Time.Day}]}) /ini "KissAssist_Buffs.ini" ${SectionName} NULL NULL
        /if (${Math.Calc[${Time.Hour}-${Ini["KissAssist_Buffs.ini",${SectionName},Hour]}]}!=0) /ini "KissAssist_Buffs.ini" ${SectionName} NULL NULL
    /next i
/return