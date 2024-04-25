int a,b,c,d,e;
void q();
void p()
{
   int x;
   b= 2;
   if(b<d)
      c=a+b;
   else
      q();
   x = c+d;
}
void q()
{
    a=1;
    a=a*b;
}
void main()
{
   int x;
   a=5;b=3;c=7;d=2;
   p();
   a=a+2;
   e=c+d;
   d=a*b;
   x=a+c+e;
}
