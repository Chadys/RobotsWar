var x
var y
var found
var turn

x = 1
y = 0
turn = 0

if Energy < 3
	Snooze
endif

while y <= VISIONMAX
	while x <= VISIONMAX
		found = Look x y

		if found == ROBOT
			if Energy > 3
				Shoot x y
			endif
			continue
		endif
		if found == TREASURE
			if y == 0
				if x > 1 && Energy > 3
					Go SPRINT
				endif
				Go FORWARD
			endif
			if y < 0
				Go LEFT
			endif
			Go RIGHT
		endif
		if found == BASE
			if Score >= 5
				if y == 0
					if x > 1 && Energy > 3
						Go SPRINT
					endif
					Go FORWARD
				endif
				if y < 0
					Go LEFT
				endif
				Go RIGHT
			endif
		endif
		x = x+1
	endwhile
	x = 1
	turn = turn+1
	if y > 0
		y = -y
	endif
	else
		if y < 0
			y = -y+1
		endif
		else
			y = 1
		endelse
	endelse
	if y > turn
		break
	endif
endwhile
TurnAround RIGHT