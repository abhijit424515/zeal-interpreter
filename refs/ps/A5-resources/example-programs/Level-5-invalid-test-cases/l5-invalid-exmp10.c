int a,b,c,d;
float x,y;
void p()
{
    a=1;       
}
int q()
{
    p();
    print a;
    return 3;
}
float r()
{
    p();
    print b;
    return 7.00;
}
void main()
{
    int c;
    y=r();
    a=1;
    c=2;
    p();
    c=q();
    print a;
    print c;
    x=q();
}
