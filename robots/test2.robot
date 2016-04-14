var x
var y
var found
var turn

x = 1
y = -1
turn = 0

while x < VISIONMAX+1
	while y < x+1
		found = Look x y

		if found == ROBOT
			if Energy > 4
				Shoot x y
				Go x y
			endif
		endif
		if found == BASE
			if Score == 10
				Go x y
			endif
			if Life < 3
				Go x y
			endif
		endif
		if found == TREASURE
			Go x y
		endif
		if found == WALL
			if turn == 3
				turn = 0
				Go 2 -2
				x = 0
				y = -1
			endif
			if turn != 3
				TurnAround RIGHT
				x = 0
				y = -1
				turn = turn+3
			endif
		endif

		y = y+1
	endwhile
	x = x+1
	y = -x
endwhile
