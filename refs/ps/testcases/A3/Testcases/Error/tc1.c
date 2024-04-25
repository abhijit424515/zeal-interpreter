void main()
{
    int a,b,c;
    read a;
    read b;
    c = (a+b+1)/(a-b-1);
    c = (c < a*b + 2)? c:a*b + 2;
    print c;
}

void main();