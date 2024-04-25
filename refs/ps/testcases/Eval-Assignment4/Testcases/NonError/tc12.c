void main();
void main()
{ 
  int n,i;
  int sum;
  sum=0;
  i=1;
  read n;
  do
  {
    sum=sum+i;
    i=i+1;
  }while(i<n);
  print sum;
}
