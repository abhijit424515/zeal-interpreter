void main()
{
	int age, count;
	int iterations;
	iterations = 10;
	do
	{
		count = 3;
		while(count>0)
		{
			if(age>10)
				print "yes";
			else
				print "no";
			count = count - 1;
		}
		iterations = iterations - 1;
	}while(iterations>0);
}