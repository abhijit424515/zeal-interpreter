int f1(int x,int y);
int f1(int b,int c)
{
	int d;
	d=b+c;
	return d+2;

}

int g()
{
    int b;
	return b; 
}
void main()
{
	int a;
	 a=3;
	a=f1(a,a);
    print a;
}
