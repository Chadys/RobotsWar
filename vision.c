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
	int i, j, ii, count, l, val;
	coord r;
	float pente;

	if(a.dir == 1)
	{
		r.j = a.j + larg;
		r.i = a.i - prof;
	}
	else if(a.dir == 2)
	{
		r.j = a.j + prof;
		r.i = a.i + larg;
	}
	else if(a.dir == 3)
	{
		r.j = a.j - larg;
		r.i = a.i + prof;
	}
	else if(a.dir == 4)
	{
		r.j = a.j - prof;
		r.i = a.i - larg;
	}
	//printf("xb : %d - yb : %d\n", xb, yb);
	
	if(r.j >= nlvl || r.i >= nlvl){
		r.dir=-2;
		return r;
	}
	
	if((abs(prof) < abs(larg)) || prof < 0){
		r.dir=-1;
		return r;
	}
	

	if(larg == 0)
		pente = prof;
	else if(abs(larg) == 1)
		pente = (((float)prof / (float)2) + 1) * larg;
	else
		pente = (float)prof / (float)larg;
	//printf("Pente : %f\n", pente);
	// on monte de pente et on décale à gauche ou à droite de 1

	i = abs((int)pente);
	j = i;
	if(fabs(pente - (int)pente) >= 0.5)
		j++;

	//printf("i : %d - j : %d\n", i, j);
	
	count = 0;
	
	while(1)
	{
		//printf("%d\n", count);
		// on monte de i ou j
		if(count % 2)
			l = j;
		else
			l=i;
	
		for(ii = 0; ii < l; ii++)
		{
			switch(a.dir)
			{
				case 1 : if (a.i!=r.i) a.i--;break;
				case 2 : if (a.j!=r.j) a.j++;break;
				case 3 : if (a.i!=r.i) a.i++;break;
				case 4 : if (a.j!=r.j) a.j--;break;
			}
	
			val = check(a.i, a.j);
			if(a.i == r.i && a.j == r.j){
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
				case 1 : if (a.j!=r.j) a.j--;break;
				case 2 : if (a.i!=r.i) a.i--;break;
				case 3 : if (a.j!=r.j) a.j++;break;
				case 4 : if (a.i!=r.i) a.i++;break;
			}
		}
		else if(pente > 0)
		{
			switch(a.dir)
			{
				case 1 : if (a.j!=r.j) a.j++;break;
				case 2 : if (a.i!=r.i) a.i++;break;
				case 3 : if (a.j!=r.j) a.j--;break;
				case 4 : if (a.i!=r.i) a.i--;break;
			}
		}

		val = check(a.i, a.j);
		if(a.i == r.i && a.j == r.j){
			r.dir = val;
			return r;
		}
		else if(val == 1 || val == 4){
			r.dir = -1;
			return r;
		}
	
		// on décale à gauche/droite
		count++;
	}
}
