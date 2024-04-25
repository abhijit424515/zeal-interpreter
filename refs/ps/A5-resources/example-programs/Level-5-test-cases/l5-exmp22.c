int a;
int b;
int c;
int q(int d)
{
  if(d<=0)
     return 0;
  a=q(d-1);
  a=a+1;
  return 1;
}
int p(int e)
{   
   int x;
   if(e<=0)
     return 1;
   if(x>0)
      x=q(e-1);
   else
   {
	   while(a > b ){
	      a = p(e-1);
	      x=c+1;
	   }
   }
   print b;
   a=c+b;
   return 1;
}
void main()
{
    a=b*c;
    b=p(2);
    b=a+c;
    print a;
}
