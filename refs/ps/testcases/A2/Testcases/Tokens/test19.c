int global;
void main()
{
    int a,b;
    global = !((a-b)>(a+b))?a/4:b*6;
    print "Result is:";
    print global;
}