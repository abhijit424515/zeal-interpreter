int global1;	
int global2;

void main()
{	
	int local;
	global1 = 5; 	
	read local;
	global2 = local+1;	
	local = -local + global2 * -global1; 
	global2 = local/2;
	print global1;	
	print global2;
	print local;
}
