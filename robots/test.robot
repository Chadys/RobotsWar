main : 
{
	var i = 1;

	var butin_min = PORTE_MAX * 2;
	var i_min = PORTE_MAX
	var ii_min = PORTE_MAX
	while(i < PORTE_MAX)
	{
		var ii = -i;
		while(ii < i)
		{
			var case = regarder(i, ii);
			var dist = i + ii;
			if(case == butin)
			{
				if(butin_min > dist)
				{
					butin_min = dist;
					i_min = i;
					ii_min = ii;
				}
			}
			
		}
		
	}

	goto(i_min, ii_min);
}