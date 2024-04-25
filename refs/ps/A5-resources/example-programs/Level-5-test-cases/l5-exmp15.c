int a;
int b;
int c;
void q()
{
print a;
}
void p()
{   
   int x;
   if(x>0)
       q();
   else
   {
	   while(a > b ){
	      print a;
	      x=c+1;
	   }
   }
   print b;
   a=c+b;
}
void main()
{
    a=b*c;
    p();
    b=a+c;
    print a;
}
