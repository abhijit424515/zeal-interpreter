void main(){
    int target, a, b, n;
    while(n != 0) {
        if(a + b == target) {
            print a+b;
        } 
        else{ 
            n = n-1;
            a = a+1;
            b = b-1; 
        }
    } 
    print "Not Find";
}