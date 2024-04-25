int a;
int b;
int c;
int p(int k)
{   
   if(k<=0)
    return 1;
   b = p(k-1);
   print b;
   a=c+b;
}
void main()
{
    a=1;
    b=2;
    c=3;
    a=b*c;
    a=p(c);
    b=a+c;
    c=p(b);
}
