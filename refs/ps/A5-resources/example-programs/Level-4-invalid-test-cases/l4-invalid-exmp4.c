void main()
{
	int n;
	read n;
	if(n >= 5 && n <= 10)
	{
		while(n > 0)
		{
			print n; 
			print " ";
			n = n - 1;
		}
	}
	else
	{
		else(n <= 1024 && n >= -1024)
		{
			do
			{
				print n; 
				print " ";
				n = n / 2;
			}while(n != 0);
		}
		else
		{
			print "Your number is: ";
			print n;
		}
	}
}
