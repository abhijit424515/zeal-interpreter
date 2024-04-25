int f1(int x,int y);
void h();
int f1(int b,int c)
{
	int d;
	d=b+c;
        h();
	return d+2;

}
void h()
{
   int c;
   c=1;	
}
int g()
{
    int b;
    h();
    b=1;
    b = f1(b,b);
    return b; 
}
void main()
{
	int a,b;
	 a=3;
	h();
	a=f1(a,a);
	b=g();
	h();
	a=f1(a,a);
	b=g();
    print a;
}
