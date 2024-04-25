int a,b;
// Cannot assign values here
a = 10;
b = 20;

void main()
{
    int c;
    c = a > b ? a : b;
    print c;
}