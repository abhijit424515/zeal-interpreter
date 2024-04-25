void main() {
    int a, b, c, d;
    read a;  
    read b; 
    read c;  

    // Using nested ternary operators with logical operations
    d = a > b ? (b > c ? b : c) : (a < c ? c : a);

    print d;  
}
