
int global1;	
int global2;

void main(){
	int local;
	read local;	//read statement
	global1 = 5;	//RHS of assignment statement can be a constant
	global2 = local;	//assignment statement. RHS can be a variable
	local = global1;	//assignment statement. RHS can be a variable
	print global1;	//print statement
	print global2;
	print local;
}
