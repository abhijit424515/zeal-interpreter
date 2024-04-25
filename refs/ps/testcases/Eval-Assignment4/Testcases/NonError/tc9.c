int test;

void main()
{
    int n;
    int next;
    int m;
    int t;
    n = 10;
    m = 0;
    t = 1;
    
    while (n>0)
    {
        next = m + t;
        print next;
        m = t;
        t = next;
        n = n -1 ;
    }
}
