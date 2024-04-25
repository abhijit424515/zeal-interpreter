
int global; 

void main()
{
    int local, temp; //local variable
    int test;
    local=2;
    global=1;
    test=1;
    temp=1;
    print local;
    print global;
    read temp;
    test= global + local * temp;
    print test;
    read temp;
    test= global * local /temp;
    print test;
 
}
