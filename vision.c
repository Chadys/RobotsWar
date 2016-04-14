#include "header.h"

//transform a case content to a value
int check(int ya, int xa)
{
	switch(level[ya][xa])
	{
		case ' ' : return 0;break;
		case '=' : return 1;break;
		case '$' : return 2;break;
		case '@' : return 3;break;
		default : return 4;
	}
}

//return the value of a case content, seen from a specified location 
coord ligne(coord a, int prof, int larg)
{
	int xb, yb;
	coord r;

	if(a.dir == 1)
	{
		xb = a.j + larg;
		yb = a.i - prof;
	}
	else if(a.dir == 2)
	{
		xb = a.j + prof;
		yb = a.i + larg;
	}
	else if(a.dir == 3)
	{
		xb = a.j - larg;
		yb = a.i + prof;
	}
	else if(a.dir == 4)
	{
		xb = a.j - prof;
		yb = a.i - larg;
	}
	r.i=yb;
	r.j=xb;
	//printf("xb : %d - yb : %d\n", xb, yb);
	
	if(xb < 0 || xb >= nlvl || yb < 0 || yb >= nlvl){
		r.dir=-2;
		return r;
	}
	
	if((abs(prof) < abs(larg)) || prof < 0){
		r.dir=-1;
		return r;
	}
	

	float pente;
	if(larg == 0)
		pente = prof;
	else
		pente = (float)prof / (float)larg;
	if(abs(larg) == 1)
		pente = (((float)prof / (float)2) + 1) * larg;
	//printf("Pente : %f\n", pente);
	// on monte de pente et on décale à gauche ou à droite de 1
	
	int i, j, ii;
	i = abs((int)pente);
	j = i;
	if(fabs(pente - (int)pente) > 0.5)
		j = i+1;

	//printf("i : %d - j : %d\n", i, j);
	
	int count = 0;
	
	int l;
	int val;
	while(1)
	{
		//printf("%d\n", count);
		// on monte de i ou j
		l = i;
		if(count % 2)
			l = j;
	
		for(ii = 0; ii < l; ii++)
		{
			switch(a.dir)
			{
				case 1 : a.i--;break;
				case 2 : a.j++;break;
				case 3 : a.i++;break;
				case 4 : a.j--;break;
			}
	
			val = check(a.i, a.j);
			if(a.i == yb && a.j == xb){
				r.dir = val;
				return r;
			}
			if(val == 1 || val == 4){
				r.dir = -1;
				return r;
			}
		}
	
		if(pente < 0)
		{
			switch(a.dir)
			{
				case 1 : a.j--;break;
				case 2 : a.i--;break;
				case 3 : a.j++;break;
				case 4 : a.i++;break;
			}
		}
		else if(pente > 0)
		{
			switch(a.dir)
			{
				case 1 : a.j++;break;
				case 2 : a.i++;break;
				case 3 : a.j--;break;
				case 4 : a.i--;break;
			}
		}

		val = check(a.i, a.j);
		if(a.i == yb && a.j == xb){
			r.dir = val;
			return r;
		}
		else if(val == 1 || val == 4){
			r.dir = -1;
			return r;
		}
	
		// on décale à gauche/drointe
		count++;
	}
}