int global1;	
int global2;

void main(){	
	int local;
	int ans;
	read local;	
	global1 = 5;	
	global2 = local;	
	ans = ((global2 > global1) && (global1/2!=0)) ? 1:0; 
	print ans;	
}
