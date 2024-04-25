int a,b,c;
void p()
{
if (1<2)
    {
        a = a*b;
    }
else if (1<4)
    {
        c = a * b;
        c = a;
    }
}
void main()
{
    int a,b;
    c = a*b;
    p();
    p();
    a = a*b;
}
