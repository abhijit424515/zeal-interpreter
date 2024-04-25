int test;

void main()
{
    int n;
    int next;
    int m;
    int t;
    int b;
    n = 10;
    m = 0;
    t = 1;
    b = 1;
    
    while (1)
    {
        next = m + t;
        print next;
        m = t;
        t = next;
        n = n -1 ;
        if (n > 0) {
            b = 0;
        }
        b ? break : continue;
    }
}
