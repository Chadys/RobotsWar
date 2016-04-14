var x
var y
var found

x = 1
y = 0

while y <= VISIONMAX
	while x <= VISIONMAX
		found = Look x y

		if found == ROBOT
			if Energy > 3
				Shoot x y
			endif
		endif
		if found == BASE
			if Score == 10
				if y == 0
					Go FORWARD
				endif
				if y < 0
					Go LEFT
				endif
				Go RIGHT
			endif
		endif
		if found == TREASURE
			if y == 0
				Go FORWARD
			endif
			if y < 0
				Go LEFT
			endif
			Go RIGHT
		endif
		if found == GAMELIMIT
			found = WALL
		endif
		if found == WALL
			TurnAround RIGHT
		endif
		x = x+1
	endwhile
	x = 1
	y = y+1
	if y == VISIONMAX
		y = -(VISIONMAX-1)
	endif
	if y == 0
		break
	endif
endwhile