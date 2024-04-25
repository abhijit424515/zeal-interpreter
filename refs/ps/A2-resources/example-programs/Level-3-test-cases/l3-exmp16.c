int a,b,c;

void main()
{
    bool d,e;
    string z;

    print "Enter value of a: ";
    read a;
    print "Enter value of b: ";
    read b;

    d = a*a > b*b;
    e = b*b > a*a;

    z = d ? "a" : e ? "b" : "none";
    print z;
}
