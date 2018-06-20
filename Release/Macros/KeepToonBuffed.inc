
|-------------------------------------------------------------------------------- 
| SUB: Check My Health
|-------------------------------------------------------------------------------- 
Sub CheckMyHealth

/if (${Me.PctHPs}<=${HealPct}) {
    /if (${Target.ID}!=${Me.ID}) {
         /if (${Debug}) /echo Attempting to target (${Me.CleanName}) for heals!
         /squelch /target id ${Me.ID} pc targetable
         /delay 15 !${Target.ID}   
         /casting "${HealSpell}" gem8
         /call CheckCasting
         /call CheckGlobalCooldown
    }
}

/return
|---------------------------------------------------------------------------------

|-------------------------------------------------------------------------------- 
| SUB: Check Target Health
|-------------------------------------------------------------------------------- 
Sub CheckTargetHealth

/declare j int local

/for j 1 to ${NumberOfToons}
    
    |--------------------------------------------------------------------------------
    /if (${Spawn[=${MyBuffTargetName[${j}]}].ID} || (${Spawn[=${MyBuffTargetName[${j}]}].ID} && ${MyBuffTargetName[${j}].NotEqual[null]})) {
        /if (${Debug}) /echo \ao Attempting to target \at (${MyBuffTargetName[${j}]}) \ao for \ar health \ao check.
        /if (${Target.ID}!=${Spawn[=${MyBuffTargetName[${j}]}].ID}) /squelch /target id ${Spawn[=${MyBuffTargetName[${j}]}].ID} ${Spawn[=${MyBuffTargetName[${j}]}].Type} targetable
        /if (${AdminDebug}) /echo /squelch /target id ${Spawn[=${MyBuffTargetName[${j}]}].ID} ${Spawn[=${MyBuffTargetName[${j}]}].Type} targetable
        /delay 15 !${Target.ID}
        /if (${Target.ID}!=${Spawn[=${MyBuffTargetName[${j}]}].ID} || !${Select[${Target.Type},Mercenary,PC,Pet]}) /next j       

        |--------------------------------------------------------------------------------
        |  ADMIN DEBUG FOR HEAL CODE
        |--------------------------------------------------------------------------------        
        /if (${AdminDebug}) {
            /echo -----------
            /if ((${Target.PctHPs}<=${HealPct}) && (${Target.Distance}<=${HealSpellRange}) && (${Target.Distance}<=${Spell[${HealSpell}].Range} || ${Target.Distance}<=${Spell[${HealSpell}].AERange})) {
                /echo TargetType:${Select[${Target.Type},Mercenary,PC,Pet]} HEAL:${HealSpell} MANA:(${Me.CurrentMana}>${Spell[${HealSpell}].Mana}) INRANGE:(${Target.Distance}<=${Spell[${HealSpell}].Range} || ${Target.Distance}<=${Spell[${HealSpell}].AERange})
                /echo "${TargetBuffName[${j}]}" = Heal Should Cast
            } else {
                /echo TargetType:${Select[${Target.Type},Mercenary,PC,Pet]} HEAL:${HealSpell} MANA:(${Me.CurrentMana}>${Spell[${HealSpell}].Mana}) INRANGE:(${Target.Distance}<=${Spell[${HealSpell}].Range} || ${Target.Distance}<=${Spell[${HealSpell}].AERange})
                /echo "${TargetBuffName[${j}]}" = Heal will NOT Cast  
                /echo -----------
                /delay 10            
            }
        }
        |--------------------------------------------------------------------------------	

        /if ((${Target.PctHPs}<=${HealPct}) && (${Target.Distance}<=${HealSpellRange})) {
            /if (${Debug}) /echo ${MyBuffTargetName[${j}]}'s health at (${Target.PctHPs}), HEAL TIME !!!!
            /casting "${HealSpell}" gem8
            /call CheckCasting
            /call CheckGlobalCooldown
        }
    }
    |--------------------------------------------------------------------------------
    
/next j
/if (${NumberOfToons}>1) /squelch /target clear

/return
|---------------------------------------------------------------------------------

|-------------------------------------------------------------------------------- 
| SUB: Maintain Target buffs
|-------------------------------------------------------------------------------- 
Sub MaintainTargetBuffs

/declare i int local
/declare j int local

/declare SpellToCast string local 0
/declare BuffIDName  string local 0

/for j 1 to ${NumberOfToons}

    /if (${Spawn[=${MyBuffTargetName[${j}]}].ID} || (${Spawn[=${MyBuffTargetName[${j}]}].ID} && ${MyBuffTargetName[${j}].NotEqual[null]})) {
         /if (${Debug}) /echo \ao Attempting to target \at (${MyBuffTargetName[${j}]}) \ao for \ag buff \ao check.
         /if (${Target.ID}!=${Spawn[=${MyBuffTargetName[${j}]}].ID}) /squelch /target id ${Spawn[=${MyBuffTargetName[${j}]}].ID} ${Spawn[=${MyBuffTargetName[${j}]}].Type} targetable
         /if (${AdminDebug}) /echo /squelch /target id ${Spawn[=${MyBuffTargetName[${j}]}].ID} ${Spawn[=${MyBuffTargetName[${j}]}].Type} targetable
         /delay 15 !${Target.ID}
         /if (${Target.ID}!=${Spawn[=${MyBuffTargetName[${j}]}].ID} || !${Select[${Target.Type},Mercenary,PC,Pet]}) /next j 

         /if (${Spawn[=${MyBuffTargetName[${j}]}].ID}!=NULL) /call PopulateBuffs
	 
	 |--------------------------------------------------------------------------------
         /for i 1 to ${NumberOfBuffs} 
                |--------------------------------------------------------------------------------

                |/echo This is target: ${MyBuffTargetName[${j}]}
                |/echo ${TargetBuffName[${i}].Arg[3,|]}
                |/echo ${TargetBuffName[${i}].Arg[3,|].Find[{MyBuffTargetName[${j}]}]}

                /if ( ${TargetBuffName[${i}].Arg[3,|].Length}) {
                    /echo Buffees set:  ${TargetBuffName[${i}].Arg[3,|]}
                    /if (!${TargetBuffName[${i}].Arg[3,|].Find[${MyBuffTargetName[${j}]}]}) {

                        /echo not buffing ${MyBuffTargetName[${j}]}
                        /continue
                    }

                }
        	|If spell name differs from buff name, use argument seperator to extract and set values. 
                /if (${TargetBuffName[${i}].Arg[2,|].Length}) {
                    /varset SpellToCast ${TargetBuffName[${i}].Arg[1,|]}
                    /varset BuffIDName  ${TargetBuffName[${i}].Arg[2,|]}
                } else {
    	            /varset SpellToCast ${TargetBuffName[${i}]}
                    /varset BuffIDName  ${TargetBuffName[${i}]}
                }   
                |--------------------------------------------------------------------------------
	
                |--------------------------------------------------------------------------------
                |  ADMIN DEBUG FOR BUFF CODE
                |--------------------------------------------------------------------------------        
                /if (${AdminDebug}) {
                    /echo -----------
                    /if ((${Target.BuffsPopulated}==TRUE) && !${Target.Buff[${BuffIDName}].ID} && ${Select[${Spawn[${MyBuffTargetName[${j}]}].Type},Mercenary,PC,Pet]} && (${Me.CurrentMana}>${Spell[${SpellToCast}].Mana}) && (${Target.Distance}<=${Spell[${SpellToCast}].Range} || ${Target.Distance}<=${Spell[${SpellToCast}].AERange}) && !${Target.Buff[Resurrection Sickness].ID} && !${Target.Buff[Revival Sickness].ID}) {
                        /echo TargetType:${Select[${Target.Type},Mercenary,PC,Pet]} BUFF:!${Target.Buff[${BuffIDName}].ID} MANA:(${Me.CurrentMana}>${Spell[${SpellToCast}].Mana}) INRANGE:(${Target.Distance}<=${Spell[${SpellToCast}].Range} || ${Target.Distance}<=${Spell[${SpellToCast}].AERange}) RezSick:!${Target.Buff[Resurrection Sickness].ID} RevivSick:!${Target.Buff[Revival Sickness].ID}
                        /echo "${BuffIDName}" = Buff is Needed
                    } else {
                        /echo TargetType:${Select[${Target.Type},Mercenary,PC,Pet]} BUFF:!${Target.Buff[${BuffIDName}].ID} MANA:(${Me.CurrentMana}>${Spell[${SpellToCast}].Mana}) INRANGE:(${Target.Distance}<=${Spell[${SpellToCast}].Range} || ${Target.Distance}<=${Spell[${SpellToCast}].AERange}) RezSick:!${Target.Buff[Resurrection Sickness].ID} RevivSick:!${Target.Buff[Revival Sickness].ID}
                        /echo "${BuffIDName}" = Buff is NOT Needed             
                        /echo -----------
                        /delay 10
                    }
                }
                |--------------------------------------------------------------------------------		
	
                /if (!${Target.Buff[${BuffIDName}].ID} && ${Select[${Spawn[${MyBuffTargetName[${j}]}].Type},Mercenary,PC,Pet]} && (${Me.CurrentMana}>${Spell[${SpellToCast}].Mana}) && (${Target.Distance}<=${Spell[${SpellToCast}].Range} || ${Target.Distance}<=${Spell[${SpellToCast}].AERange}) && !${Target.Buff[Resurrection Sickness].ID} && !${Target.Buff[Revival Sickness].ID}) { 		 
                     /if (${Debug}) /echo Attempting to cast (${SpellToCast}) on (${MyBuffTargetName[${j}]}).
                     /casting "${SpellToCast}" gem8             
                     /call CheckCasting
                     /call CheckGlobalCooldown
                     /doevents
                }
        /next i
        |--------------------------------------------------------------------------------
	
        |/if (${AssistOn} && ${Spawn[=${MyBuffTargetName[${j}]}].ID}!=NULL) /call AssistOnTarget
    
    }
    
/next j
/if (${NumberOfToons}>1) /squelch /target clear
 
/return
|---------------------------------------------------------------------------------

|-------------------------------------------------------------------------------- 
| SUB: Populate Buffs
|-------------------------------------------------------------------------------- 
Sub PopulateBuffs

:GetTargetBuffs
/if ((!${Target.BuffsPopulated}==TRUE) && (${NumberOfToons}>1)) {
    /if (${AdminDebug}) /echo ${Target.CleanName} {Target.BuffsPopulated}=${Target.BuffsPopulated}
    /delay 5
} 
/if ((!${Target.BuffsPopulated}==TRUE) && (${NumberOfToons}>1)) /goto :GetTargetBuffs

/if (${AdminDebug}) /echo ${Target.CleanName} {Target.BuffsPopulated}=${Target.BuffsPopulated}

/return
|-------------------------------------------------------------------------------- 

|-------------------------------------------------------------------------------- 
| SUB: Assist On Target
|-------------------------------------------------------------------------------- 
Sub AssistOnTarget

/echo Testing parms:  ${Param0}

/declare x int local
/declare SpellToCast string local


/target ${MainAssist}

/delay 6

/if (${Me.TargetOfTarget.CleanName.Equal[NULL]} || ${Me.TargetOfTarget.Type.Equal[CORPSE]}) /return	


/for x 1 to ${NumberOfSpells} {

    /echo ${NumberOfSpells}
    /echo here is spell in loop:  ${AssistSpell[${x}].Arg[1,|]}
    /next x
}



/if (${Select[${Me.TargetOfTarget.Type},NPC,PET]}) {

/for x 1 to ${NumberOfSpells} {

    

|-------------------------------------------------------------------------------- 
| SUB: Multiple Spells On Target
|-------------------------------------------------------------------------------- 
|-------------------------------------------------------------------------------- 
| SUB: Multi On Target
|-------------------------------------------------------------------------------- 
Sub AssistOnTarget

/echo Testing parms:  ${Param0}

/declare x int local
/declare SpellToCast string local


/target ${MainAssist}

/delay 1s

/if (${Me.TargetOfTarget.CleanName.Equal[NULL]} || ${Me.TargetOfTarget.Type.Equal[CORPSE]}) /return	

/if (${Select[${Me.TargetOfTarget.Type},NPC,PET]}) {

    /squelch /target id ${Me.TargetOfTarget.ID} ${Me.TargetOfTarget.Type} targetable
    /delay 10
    /for x 1 to ${NumberOfSpells} {


        /echo ${NumberOfSpells}
        /echo ${Target}
        /echo here is spell in loop:  ${AssistSpell[${x}].Arg[1,|]}
        /echo on Spell ${x} 
        /varset SpellToCast ${AssistSpell[${x}].Arg[1,|]}
        /echo ${SpellToCast}

        /if (${AssistSpell[${x}].Arg[2,|].Equal[debuff]} && !${Target.Buff[${SpellToCast}].ID} ) {

            /echo Is A Debuff
            /echo ${AssistPct[${x}]}
            /if (${AssistPct[${x}]} > ${Target.PctHPs}) {

                /echo casting ${AssistSpell[${x}]}
                /call CastIt "${SpellToCast}"

            }



        }  

        /if (${AssistSpell[${x}].Arg[2,|].Equal[nuke]} ) {

            /echo is a nuke
            /if (${AssistPct[${x}]} > ${Target.PctHPs}) {

                /echo casting ${AssistSpell[${x}]}
                /call CastIt "${SpellToCast}"

            }



        }  


    /next x



}

}

       

/return
|---------------------------------------------------------------------------------



|---------------------------------------------------------------------------------
  SUB: CastIt
|---------------------------------------------------------------------------------
Sub CastIt(string SpellName)
/if (${Debug}) /echo Attempting to assist, casting (${SpellName}) on (${Target.CleanName}).
            /echo \ao Assisting: casting  \at (${SpellName}) \ag on \ar (${Me.TargetOfTarget.CleanName})
            /cast "${SpellName}"
            /call CheckCasting
        }


/return
|--------------------------------------------------------------------------------- 
| SUB: Check Mana
|---------------------------------------------------------------------------------
Sub CheckMana

/if (${Select[${Me.Class.ShortName},BRD]}) /return

/echo Checking my Mana . . .

/if (${Me.PctMana}<${MinManaMedPct}) { 
     /echo I'm low on mana (${Me.PctMana}%), medding to (${MinManaMedPct}%).
     /if (${AutoAnnounce}) /tell ${MyBuffTargetName[1]} I'm low on mana, medding to full.      

     /if (${Me.Standing} && !${Me.Mount.ID}) /sit 

    :KeepMeddingForMana
    /if (!${Me.CombatState.Equal[COMBAT]} && (${Me.PctMana}<${MinManaMedPct})) {
        /echo Mana now at (${Me.PctMana}%) ...
        /if (${Me.AltAbilityReady[Cannibalization]}) /call Cast "Cannibalization" alt  	 	
        /if (${HealsOn})  /call CheckMyHealth
        /if (${HealsOn})  /call CheckTargetHealth
        /if (${BuffsOn})  /call MaintainTargetBuffs 
        /if (${FollowGroup}) /call FollowAround
        /delay 6s
        /doevents        
        /if (${Me.Standing} && !${Me.Mount.ID}) /sit
        /goto :KeepMeddingForMana
    } else {
        /if (${Me.Sitting} && !${Me.Mount.ID}) /stand
    }
     
    /echo Mana check done . . .
    /if (${AutoAnnounce}) /tell ${MyBuffTargetName[1]} Done medding!
}
 
/return
|---------------------------------------------------------------------------------

|---------------------------------------------------------------------------------
| SUB: Follow Around
|---------------------------------------------------------------------------------
Sub FollowAround

/if (${Target.ID}!=${Spawn[=${MyBuffTargetName[1]}].ID} && ${MyBuffTargetName[1].NotEqual[null]}) {
     /squelch /target id ${Spawn[=${MyBuffTargetName[1]}].ID} ${Spawn[=${MyBuffTargetName[1]}].Type} targetable
     /delay 10
} 

/if (${AdminDebug}) {
     /echo \atFollowAround: (${FollowGroup}) FollowDIst: (${FollowDist})
}

:CheckDist
/if (${Target.Distance} > ${FollowDist}) {
     /echo *** Moving closer to (${MyBuffTargetName[1]}). ***
     /if (${Me.Sitting} && !${Me.Mount.ID}) /stand
     /moveto id ${Spawn[=${MyBuffTargetName[1]}].ID} mdist ${MyMDist}	
     /delay 10
     /if (${Target.Distance} > ${FollowDist}) /goto :CheckDist
}
/if (${NumberOfToons}>1) /squelch /target clear

/return
|---------------------------------------------------------------------------------

|---------------------------------------------------------------------------------
| SUB: Check Casting
|---------------------------------------------------------------------------------
Sub CheckCasting

/if (${Me.Casting.ID}) {
    /echo *** Waiting to finish casting. ***
    :WaitForCasting
    /delay 5
    /if (${Me.Casting.ID}) /goto :WaitForCasting
}

/return
|---------------------------------------------------------------------------------

|---------------------------------------------------------------------------------
| SUB: Check Global Cooldown
|---------------------------------------------------------------------------------
Sub CheckGlobalCooldown

/if (${Me.SpellInCooldown}) {
    /echo *** Waiting on global spell cool down. ***
    :WaitForCooldown
    /delay 5
    /if (${Me.SpellInCooldown}) /goto :WaitForCooldown
}

/return
|---------------------------------------------------------------------------------

|---------------------------------------------------------------------------------
| SUB: Event_SpellBlocked
|---------------------------------------------------------------------------------
Sub Event_SpellBlocked(string line, string MySPell, string BuffToon, string BlockingSPell)

/if (${Defined[${BlockingSPell}]}) {
    /echo Your ${MySPell} spell did not take hold on ${BuffToon} (Blocked by ${BlockingSPell}.)
} else {
    /echo Your ${MySPell} spell did not take hold on ${BuffToon} (Perhaps he/she is max buffed?)
}
/gu spell ${MySPell} did not take hold. 
/echo You are wasting mana in a casting loop!
    
/return
|---------------------------------------------------------------------------------