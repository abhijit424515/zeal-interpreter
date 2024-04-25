int a,b;
void p()
{
    a=1;       
}
void q()
{
    p();
    print a;
}
void r()
{
    p();
    print b;
}
void main()
{
    int c;
    r();
    a=1;
    c=2;
    p();
    print a;
    print c;
    q();
}