int fun(int n){
	return fun(n-1);
}

void main(){
	print fun(10);
}
