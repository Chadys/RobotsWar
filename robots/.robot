#include "../header.h"

void proceed(){
unsigned int timer = 0;
	int x;
YCHECKTIMER(1);
	int y;
YCHECKTIMER(1);
	int found;
YCHECKTIMER(1);
	x = 1;
YCHECKTIMER(3);
	y = 0;
YCHECKTIMER(3);
	if (current_p->energy < 3){
YCHECKTIMER(1);
		return create_action(SNOOZE, 0, 0);
	}
	while (y <= 10){
YCHECKTIMER(1);
		while (x <= 10){
YCHECKTIMER(1);
			found = ligne(current_p->loc, x, y).dir;
YCHECKTIMER(1);
			if (found == 4){
YCHECKTIMER(1);
				if (current_p->energy > 3){
YCHECKTIMER(1);
					return create_action(SHOOT, x, y);
				}
			}
			if (found == 3){
YCHECKTIMER(1);
				if (current_p->treasure == 10){
YCHECKTIMER(1);
					if (y == 0){
YCHECKTIMER(1);
						return create_action(GO, 1, 0);
					}
					if (y < 0){
YCHECKTIMER(1);
						return create_action(GO, 4, 0);
					}
					return create_action(GO, 2, 0);
				}
			}
			if (found == 2){
YCHECKTIMER(1);
				if (y == 0){
YCHECKTIMER(1);
					return create_action(GO, 1, 0);
				}
				if (y < 0){
YCHECKTIMER(1);
					return create_action(GO, 4, 0);
				}
				return create_action(GO, 2, 0);
			}
			if (found == -2){
YCHECKTIMER(1);
				found = 1;
YCHECKTIMER(1);
			}
			if (found == 1){
YCHECKTIMER(1);
				x = x + 1;
YCHECKTIMER(1);
				continue;
			}
			x = x + 1;
YCHECKTIMER(1);
		}
		x = 1;
YCHECKTIMER(1);
		y = y + 1;
YCHECKTIMER(1);
		if (y == 10){
YCHECKTIMER(1);
			y =  -( 10 - 1 );
YCHECKTIMER(1);
		}
		if (y == 0){
YCHECKTIMER(1);
			break;
		}
	}
	return create_action(TURNAROUND, 2, 0);
}