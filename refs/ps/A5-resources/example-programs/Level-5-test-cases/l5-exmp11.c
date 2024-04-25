float convert_centigrade_to_fahrenheit(float ct);

void main()
{

	float in, out;

	print "Input temperature in centigrade";

	read in;

	out = convert_centigrade_to_fahrenheit(in);
	
	print "Temperature in fahrenheit is ";

	print out;
}


float convert_centigrade_to_fahrenheit(float ct)
{
	float ft;

	ft = (ct == 0.0)? 0.0 : (ct * 9.0)/5.0 + 32.0;

	return ft;
}
