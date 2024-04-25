int global1;
int global2;
void main(){	
	int local;
	int ans;
	read local;	
	global2 = local;	
	global1 = 5;	
	ans = ((global2 > global1) && ((local-2)!=0)) ? 1:0; 

	if(ans==1)
	{		
		while(global2>0)
		{	
			print global2; 
			global2=global2-1; 
		}
	}

	else
	{
		do
		{		
			print global2;
			global2=global2+1;
		}
		while(global2<10);
	}

}
