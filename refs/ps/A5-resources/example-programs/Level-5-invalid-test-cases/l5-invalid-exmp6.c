int a,b,c;
void p()
{
	if (1<2)
	{
		a = a*b;
		p();
	}
	else if (1<4)
	{
		c = a * b;
		p();
		c = a;
	}
}

int p(int a, int b)
{
	return a+b;
}

void main()
	{
		int a,b;
		a=2;
		b=3;
		c = p(a,b);
		p();
		a = a*b;
	}
//Function overloading not-supported.
