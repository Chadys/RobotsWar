var x
var y
var found

x = 1
y = -1

while x < VISIONMAX+1
	while y < x+1
		found = Look x, y

		if found == ROBOT
			if Energy > 3
				Shoot x, y
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
		y = y+1
	endwhile
	x = x+1
	y = -x
endwhile
