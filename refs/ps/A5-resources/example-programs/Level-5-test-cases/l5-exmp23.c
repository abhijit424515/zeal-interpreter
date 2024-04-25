int a,b,c;
int p(int k)
{
if(k<0)
 return 1;
if (1<2)
    {
        b = p(k-1);
        a = a*b;
    }
else if (1<4)
    {
        c = a * b;
        c = p(k-1);
    }
return 0;
}
void main()
{
    int a,b;
    c = a*b;
    a=p(a);
    c=p(c);
    a = a*b;
}
