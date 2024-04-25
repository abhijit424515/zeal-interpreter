int gv_lv; 

void main()
{
    int gv_lv; //local variable with the same name 
    bool test;
//  print test; won't work because boolean variables cannot be printed

    gv_lv=1;// This will only update the local variable
    
    print gv_lv;//prints the local variable
}
