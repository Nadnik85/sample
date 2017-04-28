|Auto-Subs Shared routines across Auto series macros by Noobhaxor
| Version 3.0 April Update
| ----------------------------------------------------------------------------
| SUB: Compatibility with KissAssist Chase
| ----------------------------------------------------------------------------
#Event BackOff           "[MQ2] BackOff"
#Event Campfire          "[MQ2] Summon Campfire"
#Event Camping           "#*#seconds to prepare your camp."
#Event GainSomething	 "#*#You have gained|#1#|"
#Event KissShit    		 "[MQ2] Reset #1# #2# #3#"
#Event KissShit      	 "[MQ2] toggle #1#
#Event TaskUpdate		 "#*#Your task |#1#| has been updated#*#" 
#Event Zoned             "LOADING, PLEASE WAIT#*#"
#Event Zoned             "You have entered#*#"
Sub AliasSetup
	/squelch /alias /backoff        /echo BackOff
	/squelch /alias /campfire       /echo Summon Campfire
	/squelch /alias /camphere       /echo toggle ReturnToCamp
	/noparse /alias /camphold       /echo toggle HoldCamp
	/noparse /alias /campoff	    /echo Reset General CampOff
	/squelch /alias /campradius     /echo Reset General CampRadius
	/squelch /alias /chase          /echo toggle ChaseAssist
	/squelch /alias /chaseoff       /echo Reset General ChaseAssist 0        
	/squelch /alias /chaseon        /echo Reset General ChaseAssist 1 
	/squelch /alias /meleeon        /echo toggle MeleeOn
	/squelch /alias /meleeoff       /echo toggle MeleeOff
	/squelch /alias /mezon          /echo Reset Mez MezOn
	/squelch /alias /mezoff         /echo Reset Mez MezOff
	/squelch /alias /slowon         /echo toggle SlowOn
	/squelch /alias /slowoff        /echo toggle SlowOff
	/squelch /alias /returntocamp   /echo toggle ReturnToCamp
	/noparse /alias /instantrelease /echo toggle InstantRelease
	/noparse /alias /usefellowship 	/echo toggle UseFellowship
	
/return
Sub VarSetup
	/declare autotargetid	int outer 0
	/declare sittimer		timer outer 0
	/declare enablesit		int outer 0
	/declare AutoCampReturn	int outer 0
	/declare AutoCampRadius	int outer 50
	/declare AutoCampX		int outer ${Me.X}
	/declare AutoCampY		int outer ${Me.Y}
	/declare changetoini	int outer 0
	/declare targetcleartimer	timer outer 0
	/declare numoftargets	int outer 0
	/echo -----${MacroName} LOADING------
	/if (${AutoCampHold}==1) /echo CAUTION: Auto Camp Hold is set!
	/if (${Me.XTarget[1].TargetType.NotEqual[Auto Hater]}) {
		/echo Extended Target 1 HAS TO BE SET TO Auto Hater
		/echo ENDING MACRO -- Set Extended Target 1 to Auto
		/end
		}
	|-Check for loaded plugins
	/call CheckPlugin MQ2Cast
    /call CheckPlugin MQ2Exchange
	/call CheckPlugin MQ2Rez
	/call CheckPlugin MQ2AdvPath
	/call UnCheckPlugin MQ2Twist
	/squelch /rez accept on
/return
|------------------------KissAssist Alias Changes------------------------------
Sub Event_KissShit(string Line, string ISection, string IName, string IVar)
    /if (${ISection.Equal[ChaseAssist]}&&${Target.Type.Equal[PC]}) {
		/docommand /varset FollowToonName ${Target.Name}
		/docommand /varset changetoini 1
		/echo Following ${If[${FollowToonName.Equal[NULL ]},OFF,${FollowToonName}]}
	} else /if (${ISection.Equal[ChaseAssist]}&&${Target.Type.NotEqual[PC]}) {
		/docommand /varset FollowToonName NULL
		/docommand /varset changetoini 1
		/echo Following OFF
		/if (${AdvPath.Following}) /docommand /afollow off
	}
    /if (${IName.Equal[ChaseAssist]} && ${IVar}==1 &&${Target.Type.Equal[PC]}) {
		/docommand /varset FollowToonName ${Target.Name}
		/docommand /varset changetoini 1
		/echo Following ${If[${FollowToonName.Equal[NULL ]},OFF,${FollowToonName}]}
	} else /if (${IName.Equal[ChaseAssist]} && ${IVar}==1 &&${Target.Type.NotEqual[PC]}) {
		/echo I need a target to follow try again
	}
    /if (${IName.Equal[ChaseAssist]} && ${IVar}==0) {
		/varset FollowToonName NULL
		/docommand /varset changetoini 1
		/echo Following OFF
		/if (${AdvPath.Following}) /docommand /afollow off
	}
	/if (${ISection.Equal[ReturnToCamp]}) {
		/if (${AutoCampReturn}==1) {
			/varset AutoCampReturn 0
		} else {
			/varset AutoCampReturn 1
			/docommand /varset AutoCampX ${Me.X}
			/docommand /varset AutoCampY ${Me.Y}
		}
	}
	/if (${ISection.Equal[HoldCamp]}) {
		/docommand /varset changetoini 1
		/if (${AutoCampHold}==1) {
			/varset AutoCampHold 0
			/if (${AutoCampReturn}!=1) /docommand /varset AutoCampReturn 0
		} else {
			/varset AutoCampHold 1
		}
	}
	/if (${IName.Equal[CampOff]}) {
		/docommand /varset changetoini 1
		/docommand /varset AutoCampHold 0
		/docommand /varset AutoCampReturn 0
	}
	/if (${ISection.Equal[InstantRelease]}) {
		/docommand /varset changetoini 1
		/if (${InstantRelease}==1) {
			/varset InstantRelease 0
			/echo Instant Release on death OFF
		} else {
			/varset InstantRelease 1
			/echo Instant Release on death ON
		}
	}
	/if (${ISection.Equal[UseFellowship]}) {
		/docommand /varset changetoini 1
		/if (${UseFellowship}==1) {
			/varset UseFellowship 0
			/echo Fellowship now OFF
		} else {
			/varset UseFellowship 1
			/echo Fellowship now ON
		}
	}
	/if (${ISection.Equal[MeleeOn]}) {
		/docommand /varset changetoini 1
		/if (${UseMelee}==1) {
			/echo Melee Is already on! Durr!
		} else {
			/varset UseMelee 1
			/echo Melee ON
		}
	}
	/if (${ISection.Equal[MeleeOff]}) {
		/docommand /varset changetoini 1
		/if (${UseMelee}==0) {
			/echo Melee Is already off! Durr!
		} else {
			/varset UseMelee 0
			/echo Melee OFF
		}
	}
	/if (${ISection.Equal[SlowOn]}) {
		/docommand /varset changetoini 1
		/if (${UseSlow}==1) {
			/echo Slow Is already on! Durr!
		} else {
			/varset UseSlow 1
			/echo Slow ON
		}
	}
	/if (${ISection.Equal[SlowOff]}) {
		/docommand /varset changetoini 1
		/if (${UseSlow}==0) {
			/echo Slow Is already off! Durr!
		} else {
			/varset UseSlow 0
			/echo Slow OFF
		}
	}
	/if (${IName.Equal[MezOn]}) {
		/docommand /varset changetoini 1
		/if (${UseMez}==1) {
			/echo Mez Is already on! Durr!
		} else {
			/varset UseMez 1
			/echo Mez ON
		}
	}
	/if (${IName.Equal[MezOff]}) {
		/docommand /varset changetoini 1
		/if (${UseMez}==0) {
			/echo Mez Is already off! Durr!
		} else {
			/varset UseMez 0
			/echo Mez OFF
		}
	}
   /return
|-----------------------------------------------------------------------------  
| SUB: Back Off and stop melee  
| ----------------------------------------------------------------------------      
	Sub Event_BackOff  
		/echo Resetting. No Longer Backing off.  
		/if (${Target.ID}) /target clear
		/if (${Me.Combat}) /squelch /attack off 
		/if (${Stick.Active}) /stick off 
		/call AutoCampCheck
		/doevents flush Event_BackOff  
	/return 
| ----------------------------------------------------------------------------
| SUB: Event Camping - end macro
| ----------------------------------------------------------------------------
    Sub Event_Camping	
		/if (${Me.Casting.ID}) /stopcast
	    /end
    /return
| ----------------------------------------------------------------------------
| SUB: Campfire ${Window[FellowshipWnd].Child[FP_CampPage].Child[FP_CampsiteViewer].Text}
| ----------------------------------------------------------------------------
    Sub Campfire
    /if ((${Me.Fellowship.Campfire} && (${Select[${Me.Fellowship.CampfireZone},${Zone.Name}]} || ${Me.Fellowship.CampfireZone.Name.Find[guild hall]})) || (${Me.InInstance}==FALSE)) /return
        /declare FellowCount int local 0
        /declare i int local
        /declare j int local
        /for i 1 to ${SpawnCount[pc radius 50]}
            /for j 1 to ${Me.Fellowship.Members}
                /if (${NearestSpawn[${i},PC radius 50].CleanName.Equal[${Me.Fellowship.Member[${j}]}]}) /varcalc FellowCount ${FellowCount}+1
            /next j
        /next i
        /if (${FellowCount}>=3) {
            /windowstate FellowshipWnd open
			/delay 10
			/nomodkey /notify FellowshipWnd FP_Subwindows tabselect 2
			/nomodkey /notify FellowshipWnd FP_DestroyCampsite leftmouseup
			/delay 5s ${Window[ConfirmationDialogBox].Open}
			/if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
			/delay 5s !${Me.Fellowship.Campfire}
			/delay 1s
			/nomodkey /notify FellowshipWnd FP_RefreshList leftmouseup
			/delay 1s        
			/nomodkey /notify FellowshipWnd FP_CampsiteKitList listselect 1
			/delay 1s
			/nomodkey /notify FellowshipWnd FP_CreateCampsite leftmouseup
			/delay 5s ${Me.Fellowship.Campfire}
			/windowstate FellowshipWnd close
			/if (${Me.Fellowship.Campfire}) /echo Campfire Dropped
        }
    /return
| ----------------------------------------------------------------------------
| SUB: Event Zoned
| ----------------------------------------------------------------------------
    Sub Event_Zoned(Message)
		/call Zoning
	/return
	
	Sub Zoning
		/delay 1m !${Me.Zoning}
		/if (${Bool[${FollowToonName}]}) /delay 1m ${Spawn[${FollowToonName}].ID}
		/delay 50
		/if (${AutoCampReturn}!=0) /varset AutoCampReturn 0
	/return
|----------------------------------------------------------------------------
| SUB: Load Ini
|----------------------------------------------------------------------------
    Sub LoadIni(string sectionName, string varName, string varType, string varValue, string varArray)
        /if (${${varArray}.Size}>0) {
            /declare i int local
                /for i 1 to ${${varArray}.Size}
                    /if (!${Ini[${IniFileName},${sectionName},${varArray}${i}].Length}) /ini "${IniFileName}" "${sectionName}" "${varArray}${i}" "${varValue}"
                    /if (${Ini[${IniFileName},${sectionName},${varArray}${i}].Length}) /varset ${varArray}[${i}]  ${Ini[${IniFileName},${sectionName},${varArray}${i}]}
                /next i
            /return
        } else {
            /if (!${Defined[${varName}]} && ${Defined[varType]}) /declare ${varName} ${varType} outer
            /if (!${Ini[${IniFileName},${sectionName},${varName}].Length}) {
                /if (${varValue.Length}) {
                    /ini "${IniFileName}" "${sectionName}" "${varName}" "${varValue}"
                    /varset ${varName} ${varValue}
                }
            } else {
                /varset ${varName} ${Ini[${IniFileName},${sectionName},${varName}]} 
            }
        }
    /return

|----------------------------------------------------------------------------
| SUB: Set Ini
|----------------------------------------------------------------------------
    Sub SetIni(string sectionName, string varName, string varType, string varValue, string varArray)
        /if (${${varArray}.Size}>0) {
            /declare i int local
                /for i 1 to ${${varArray}.Size}
                    /if (${Ini[${IniFileName},${sectionName},${varArray}${i}].Length}) /ini "${IniFileName}" "${sectionName}" "${varArray}${i}" "${varValue}"
                /next i
            /return
        } else /if (${varValue.Length}) {
            /ini "${IniFileName}" "${sectionName}" "${varName}" "${varValue}"
        }
    /return
|----------------------------------------------------------------------------
| SUB: FollowToon
|---------------------------------------------------------------------------- 
Sub FollowToon
/if (${Spawn[pc ${FollowToonName}].ID}) {
	/if (${Me.Dead}) /return
	/if (${Spawn[pc ${FollowToonName}].Distance}>20 && !${Spawn[pc ${FollowToonName}].Dead} && ${Spawn[pc ${FollowToonName}].Distance}<400) {
		 /squelch /afollow spawn ${Spawn[pc ${FollowToonName}].ID}
		 /delay 2s ${Spawn[pc ${FollowToonName}].Distance}<20
		 /squelch /afollow off
	}
}
/return
|----------------------------------------------------------------------------
| SUB: AutoCampCheck
|----------------------------------------------------------------------------
Sub AutoCampCheck
|----Camp Hold
	/if (!${Bool[${FollowToonName}]}&&${Stick.Status.Equal[OFF]}&&!${AdvPath.Following}&&${AutoCampReturn}==0&&${AutoCampHold}==1) {
		/echo No Toon to follow, pitching my tent to pwn some fools
		/varset AutoCampReturn 1
		/docommand /varset AutoCampX ${Me.X}
		/docommand /varset AutoCampY ${Me.Y}

		}
|----Camp Return Logic
	/if (${Spawn[${FollowToonName}].ID}&&(!${Me.Casting.ID})&&!${MoveTo.Moving}) {
		/if (${AutoCampReturn}==1) {
			/varset AutoCampReturn 0
			/echo FollowToon Detected - Disabling Camp
		}
		/call FollowToon
	} else /if (${Stick.Status.Equal[ON]}&&${AutoCampReturn}==1) {
		/varset AutoCampReturn 0
		/echo Stick Detected - Disabling Camp
	} else /if (${AdvPath.Following}&&${AutoCampReturn}==1) {
		/varset AutoCampReturn 0
		/echo AdvPath Follow Detected - Disabling Camp
	} else /if (${Stick.Status.Equal[OFF]} && !${Me.Casting.ID} && ${Math.Distance[${Me.Y},${Me.X}:${AutoCampY},${AutoCampX}]}>${AutoCampRadius} && (${Math.Distance[${Me.Y},${Me.X}:${AutoCampY},${AutoCampX}]}<400) && !${MoveTo.Moving} && ${AutoCampReturn}==1) {
		/moveto loc ${AutoCampY} ${AutoCampX}|on
		:movinghome
		/delay 5
		/if ((${Math.Distance[${Me.Y},${Me.X}:${AutoCampY},${AutoCampX}]}>20)&&(${MoveTo.Moving})) /goto :movinghome
		/moveto off
	}
/return
|----------------------------------------------------------------------------
| SUB: BagModRods
|----------------------------------------------------------------------------
Sub BagModRods
	/if ((${Cursor.ID})&&${CursorTimer}==0&&${ShitOnCursor.Equal[NULL]}&&(${Me.FreeInventory}>1)) {
		/varset ShitOnCursor ${Cursor}
		/varset CursorTimer 1m
	} else /if (${Cursor.ID}&&${CursorTimer}==0&&${ShitOnCursor.NotEqual[NULL]}&&(${Me.FreeInventory}>1)) {
		/echo Something has been left on cursor for over a minute Bagging it
		/autoinventory
		/varset ShitOnCursor NULL
	} else /if (${Cursor.ID}&&${ShitOnCursor.Find[Transvergence]}&&(${Me.FreeInventory}>1)) {
		/echo Bagging Mod Rod
		/autoinventory
		/varset ShitOnCursor NULL
	} else /if (${Cursor.ID}&&${ShitOnCursor.Find[Modulation]}&&(${Me.FreeInventory}>1)) {
		/echo Bagging Mod Rod
		/autoinventory
		/varset ShitOnCursor NULL
	}
/return
|----------------------------------------------------------------------------
| SUB: Event Gain Something - EQBC message 
|----------------------------------------------------------------------------    
    Sub Event_GainSomething(string Line,string text)
        /if (!${EQBC.Connected}) /return
        /if (${Line.Find["ABILITY POINT!"]}) {
            /docommand /bc [+w+]${Me.Name} gained an AA, now has ${Me.AAPoints} unspent[+x+]
        } else /if (${Line.Find[LEVEL]}) {
            /docommand /bc [+w+]${Me.Name} gained a level, now is Level ${Me.Level}[+x+]
		}
    /return 
|----------------------------------------------------------------------------
| SUB: Event Task Update - EQBC message 
|----------------------------------------------------------------------------    
    Sub Event_TaskUpdate(string Line,string name) 
        /if (${EQBC.Connected}&&${acverbose}!=0) /docommand /bc [+t+]Task updated...(${name})[+x+]    
    /return
|----------------------------------------------------------------------------
| SUB: Check Plugin
|----------------------------------------------------------------------------
    Sub CheckPlugin(string pluginname)
        /if (!${Bool[${Plugin[${pluginname}]}]}) {
            /squelch /plugin ${pluginname}
            /echo ${pluginname} not detected! This macro requires it! Loading ...
        }
    /return	
|----------------------------------------------------------------------------
| SUB: Death Sub
|----------------------------------------------------------------------------
Sub DeathSub
	/echo You are sleeping with the fishes
	/if (${AutoCampReturn}!=0) /varset AutoCampReturn 0
    :DeathSubwait
	/if (${Window[RespawnWnd].Open} && ${UseFellowship}==1 && ${InstantRelease}==1) {
		/nomodkey /notify RespawnWnd RW_OptionsList listselect 1 
		/delay 1s
		/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup
		/delay 5s ${Me.Zoning}
	}
    /if (${Me.Hovering}) /goto :DeathSubwait
	/delay 1m !${Me.Zoning}
	/if (${UseFellowship}==1 && ${FindItem["Fellowship Registration Insignia"].Timer}==0) {
		/delay 30s ${Me.CombatState.Equal[ACTIVE]}
		/useitem "Fellowship Registration Insignia"
		/delay 2s ${FindItem["Fellowship Registration Insignia"].Timer}!=0
	} else /if (${UseFellowship}==1 && ${FindItem["Fellowship Registration Insignia"].Timer}!=0) {
		/echo Bummer, Insignia on cooldown, you must really suck at this game...
	} else {
		/echo Waiting on someone that knows how to heal to rez me
	}
/return
|----------------------------------------------------------------------------
| SUB: SitCheck
|---------------------------------------------------------------------------- 
Sub SitCheck
|---SIT Check
   /if ((${Me.Moving} || ${Me.XTarget[1].ID} || ${MoveTo.Moving} || ${Me.CombatState.Equal[COMBAT]} || ${Stick.Status.Equal[ON]} || ${AdvPath.Following}) && (${enablesit}==1)) /varset enablesit 0
|--Out of Combat MedTime - Sitting
	/if (${Me.Standing} && !${Me.Moving} && ${Stick.Status.Equal[OFF]} && !${Me.XTarget[1].ID} && !${MoveTo.Moving} && !${AdvPath.Following} && ${Me.CombatState.NotEqual[COMBAT]}&&${Me.CombatState.NotEqual[DEBUFFED]}&&${enablesit}==0&&(${Me.PctMana}<=80&&${Me.MaxMana}>1000 || ${Me.PctEndurance}<=80)) {
		/varset enablesit 1
	} else /if (${sittimer}==0&&${enablesit}==0&&${Me.Sitting}&&(${Me.Moving} || ${Me.XTarget[1].ID} || ${MoveTo.Moving} || ${Me.CombatState.Equal[COMBAT]} || ${Stick.Status.Equal[ON]} || ${AdvPath.Following})) {
		/stand
		/varset sittimer 10s
	} else /if (${sittimer}==0&&${enablesit}==1&&!${Me.Sitting}) {
		/sit
		/varset sittimer 10s
	}

/return
|----------------------------------------------------------------------------
| SUB: UnCheck Plugin
|----------------------------------------------------------------------------
    Sub UnCheckPlugin(string pluginname)
        /if (${Bool[${Plugin[${pluginname}]}]}) {
            /squelch /plugin ${pluginname} unload
            /echo ${pluginname} detected! Unloading it before it fucks shit up! UnLoading ...
        }
    /return	
|----------------------------------------------------------------------------
| SUB: DiscQueue
|---------------------------------------------------------------------------- 
Sub DiscQueue(string nextdisc)
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/if (${Me.ActiveDisc.ID}) /stopdisc
	/delay 20 !${Me.ActiveDisc.ID}
	/squelch /disc ${nextdisc}
	/delay 20 ${Me.ActiveDisc.ID}
	/delay 8 !${Me.CombatAbilityReady[${nextdisc}]}
	/echo Casting ${nextdisc}
/return
|----------------------------------------------------------------------------
| SUB: DiscNow
|---------------------------------------------------------------------------- 
Sub DiscNow(string nextdisc)
	/if (${Window[CastingWindow].Open}) /return
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/squelch /disc ${nextdisc}
	/delay 8 !${Me.CombatAbilityReady[${nextdisc}]}
	/echo Casting ${nextdisc}
/return
|----------------------------------------------------------------------------
| SUB: AANow
|---------------------------------------------------------------------------- 
Sub AANow(int nextaa,int aatargetid)
	/if (${Spawn[ID ${aatargetid}].FeetWet}!=${Me.FeetWet}&&${Spawn[id ${aatargetid} npc].ID}&&${Window[CastingWindow].Open}) /return
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/if (${Window[CastingWindow].Open}) /interrupt
	/delay 5 !${Window[CastingWindow].Open}
	/if (${Target.ID}!=${aatargetid}&&${Spawn[id ${aatargetid} npc].ID}) {
		/target id ${aatargetid}
		/delay 10 ${Target.ID}==${aatargetid}
	}
	/squelch /alt act ${nextaa}
	/delay 8 !${Me.AltAbilityReady[${nextaa}]}
	/echo Casting ${Me.AltAbility[${nextaa}].Name}
/return
|----------------------------------------------------------------------------
| SUB: AbilityNow
|---------------------------------------------------------------------------- 
Sub AbilityNow(string nextability)
	/squelch /doability ${nextability}
	/delay 8 !${Me.AbilityReady[${nextability}]}
	/echo Casting ${nextability}
/return
|----------------------------------------------------------------------------
| SUB: ItemNow
|---------------------------------------------------------------------------- 
Sub ItemNow(string nextitem)
	/if (${Window[CastingWindow].Open}) /return
	/if (!${Me.Standing}) /stand
	/delay 10 ${Me.Standing}
	/if (${Window[SpellBookWnd]}) /book
	/if (${Window[CastingWindow].Open}) /interrupt
	/delay 5 !${Window[CastingWindow].Open}
	/squelch /useitem ${nextitem}
	/delay 15 ${FindItem[${nextitem}].Timer}!=0
|--Checks if item is used and does it again if not
	/if (${FindItem[${nextitem}].Timer}==0&&${FindItem[${nextitem}].ID}) {
		/squelch /useitem ${nextitem}
		/delay 15 ${FindItem[${nextitem}].Timer}!=0
	}
	/echo Using Item ${nextitem}
/return
|----------------------------------------------------------------------------
| SUB: AutoVersionCheck
|---------------------------------------------------------------------------- 
Sub AutoVersionCheck(int autoversion)
	/if (${autoversion}!=3) {
		/echo AUTOSUBS VERSION MISMATCH
		/echo download new autosubs.inc - ending macro
	}
/return
|----------------------------------------------------------------------------
| SUB: FindTarget
|---------------------------------------------------------------------------- 
Sub FindTarget
/declare xt int local 1
/declare xtlow int local ${AutoAssistAt}
/declare xtid int local 0
	/if (!${Spawn[npc id ${autotargetid}].ID}&&${autotargetid}!=0) /varset autotargetid 0
|--Query XTarget to find hp percents
	/for xt 1 to ${Me.XTarget}
		/if (${Me.XTarget[${xt}].PctHPs}<${xtlow}&&${Spawn[id ${Me.XTarget[${xt}].ID} npc].ID}) {
			/varset xtlow ${Me.XTarget[${xt}].PctHPs}
			/varset xtid ${Me.XTarget[${xt}].ID}
		}
	/next xt
|--Check target matches MA if not based of lowest hp in XTarget
	/if (${Target.ID}!=${Me.GroupAssistTarget.ID} && ${Group.MainAssist.ID}!=${Me.ID} && ${autotargetid}!=${Me.GroupAssistTarget.ID} && ${Spawn[id ${Me.GroupAssistTarget.ID} npc].ID}) {
		/varset autotargetid ${Me.GroupAssistTarget.ID}
	} else /if ((!${Group.MainAssist.ID}||${Group.MainAssist.ID}==${Me.ID})&&${Me.XTarget[1].ID}&&${Spawn[NPC id ${xtid}].ID}) {
		/varset autotargetid ${xtid}
	}
|--Target the new target
	/if (${Target.ID}!=${autotargetid} && ${Spawn[npc id ${autotargetid}].ID}) {
		/squelch /target id ${autotargetid}
		/delay 10 ${Target.ID}==${autotargetid}
	}
|--Determine Engage	
	/if (!${Me.Combat}&&${Me.XTarget[1].ID}&&${UseMelee}==1&&${Spawn[npc id ${autotargetid}].PctHPs}<=${AutoAssistAt}&&${Spawn[npc id ${autotargetid}].PctHPs}>0&&${Target.ID}==${autotargetid}&& ${Spawn[id ${autotargetid} npc los radius ${AutoCampRadius}].ID}) {
		/squelch /attack on
		/delay 10 ${Me.Combat}
	}
/return
|----------------------------------------------------------------------------
| SUB: Engage Target
|---------------------------------------------------------------------------- 
Sub EngageTarget
	/if (${Me.State.Equal[FEIGN]}) /stand
	/if (${Me.Combat}&&${Me.XTarget[1].ID}&&${UseMelee}==1&&${Spawn[npc id ${autotargetid}].PctHPs}>0&&${Target.ID}==${autotargetid}&& ${Stick.Status.Equal[OFF]}) {
		/if (${Math.Calc[${Target.MaxRangeTo}*0.8]} > 29) {
			/stick 25 moveback loose uw
		} else {
			/stick ${Math.Calc[${Target.MaxRangeTo}*0.8]} moveback loose uw
		}
		/delay 10 ${Stick.Active}
	}
/return
|----------------------------------------------------------------------------
| SUB: FindNumTargets
|---------------------------------------------------------------------------- 
Sub FindNumTargets
/declare xt int local 1
|--Query XTarget to find npcs
	/for xt 1 to ${Me.XTarget}
		/if (${Spawn[id ${Me.XTarget[${xt}].ID} npc].ID}) /varcalc numoftargets ${numoftargets}+1
	/next xt
/return