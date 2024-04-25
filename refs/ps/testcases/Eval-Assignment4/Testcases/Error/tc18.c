void main()
{
    int a,b;
    int c;
    a = 200;
    b = 300;
    read c;
    c = a/b;
    do
    {
        do
        {
            print a;
            if(c>105)
            {
                a = a - (c/a);
            }
            else
            {
                b = b - (c/b);
                do
                {
                    print c;
                } while (c<105);
                // break;
            }
            else
            {
                b = b - (c/b);
                do
                {
                    print c;
                } while (c<105);
            }
        } while (0<0);
        
    } while ((c>105 && a == 1) || (b <= 2 && c >= 3));

    print c;
}