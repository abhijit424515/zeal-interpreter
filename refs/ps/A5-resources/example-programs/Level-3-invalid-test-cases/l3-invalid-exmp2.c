void main()
{
    float a,b;
    string s;

    read a;
    read b;

    // Wrong use of boolean operator
    s = a + b > 0 || b ? "works" : "doesnt work";
    print s;
}