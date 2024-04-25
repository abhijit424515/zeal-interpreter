void main()
{
    int a,b;
    int c;
    a = 200;
    b = 300;
    read c;
    c = a/b;
    while((c>105 && a == 1) || (b <= 2 && c >= 3))
    {
        a = a - (c/a);
        print a;
        c = c - 1;
    }
    print c;
}