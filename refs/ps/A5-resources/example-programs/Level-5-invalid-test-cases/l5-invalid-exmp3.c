int a;
int b;
int c;
void p()
{   
   int x;
   p();
   while(a > b ){
   print a;
   x=c+1;
   }
   print b;
   a=c+b;
   return a;
}
void main()
{
    a=b*c;
    a=p();
    b=a+c;
    print a;
}
