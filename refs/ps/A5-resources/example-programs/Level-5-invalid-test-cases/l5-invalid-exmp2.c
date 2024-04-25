int a;
int b;
int c;
void p()
{   
   p();
   print a;
   print b;
   b=c+1;
   p();
   print b;
   a=c+b;
   return a;
}
void main()
{
    a=b*c;
    p();
    b=a+c;
    print a;
}
