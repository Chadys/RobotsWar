if Look 1, 0 != WALL && Look 1, 0 != GAMELIMIT
	if Look 2, 0 != WALL && Look 2, 0 != GAMELIMIT && Energy > 3
		Go SPRINT
	endif
	if Energy > 2
		Go FORWARD
	endif
	Snooze
endif
TurnAround BACK