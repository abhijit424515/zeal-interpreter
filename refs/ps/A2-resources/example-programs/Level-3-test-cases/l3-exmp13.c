void main()
{
    int a,b,c,d;
    print "Enter first number: ";
    read a;
    print "Enter second number: ";
    read b;
    print "Enter third number: ";
    read c;

    // Nested Ternary Operator check
    d = a > b ? (a > c ? a : c) : (b > c ? b : c);
    print "Enter largest number is: ";
    print d;
}