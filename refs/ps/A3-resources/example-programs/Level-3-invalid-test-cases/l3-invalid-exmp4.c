void main()
{
	int a,b,c,d,e;
    a = 10;
    b = 20;

    c = (a+b) * (a-b);
    d = c > 0 ? a - b : a + b;
    e = d / (a < b && b > c);

    print e;
}


