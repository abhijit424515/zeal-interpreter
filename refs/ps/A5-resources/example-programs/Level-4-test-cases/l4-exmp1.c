int test;

void main()
{
 int loop;
 test=0;
 loop=10;
    while(loop > 1)
    {   
        loop=loop-1;
        if(loop==0)
        {
            print loop;
        }
        else
        {
            print test;
        }
        
    }
    do
    {
        loop=loop + 1;
        
    } while(loop<10);
    print loop;
}
