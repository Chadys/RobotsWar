var found
var case 
var i
var j

i = 1
found = 0

if Look 1 0 == WALL
	TurnAround RIGHT
endif

if Look 1 0 == GAMELIMIT
	TurnAround RIGHT
endif

while i < VISIONMAX/2
	if found == 1
		break
	endif

	j = -i
	while j < i+1

		case = Look i j

		if case == ROBOT
			Shoot i j
		endif

		if case == TREASURE
			found = 1
			break
		endif

		if case == BASE
			found = 1
			break
		endif
		j = j+1
	endwhile
	i = i+1
endwhile

if i+j > 7
	TurnAround RIGHT
	Snooze
endif
	
if Look 2 0 == NOTHING
	Go SPRINT
endif

if Look 1 0 == NOTHING
	Go FORWARD
endif

if Look 1 -1 == NOTHING
	Go LEFT
endif

if Look 1 1 == NOTHING
	Go RIGHT
endif