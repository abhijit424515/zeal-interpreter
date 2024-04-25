void main();
void main()
{
    int a,b,c;
    read a;
    read b;
    c = (a+b+"c") / (a-b-"d");
    c = (c<a*b + 2.0) ? c:a*b + 10.0;
    print c;
}