int a;
int b;
int c;
void p()
{   
   print a;
   print b;
   b=c+1;
   print b;
   a=c+b;
}
void main()
{
    a=1;
    b=2;
    c=3;
    a=b*c;
    p();
    b=a+c;
    print a;
}
