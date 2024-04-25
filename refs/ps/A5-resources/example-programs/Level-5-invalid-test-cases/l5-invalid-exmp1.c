int a,b,c,d,e;
void q();
int p()
{
   int x;
   b= 2;
   if(b<d)
      c=a+b;
   else
      q();
   x = c+d;
return;
}
void q()
{
    a=1;
    p();
    a=a*b;
}
void main()
{
   int x;
   a=5;b=3;c=7;d=2;
   p();
   q();
   x=a+c+e;
}
