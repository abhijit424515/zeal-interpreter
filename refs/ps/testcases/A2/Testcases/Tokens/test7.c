void main() {
    int x, y, result;
    read x;    
    read y;    

    // Using arithmetic and comparison operators together with logical operators
    result = (x < y) && (x + 10 == y) ? x * 2 : y / 2;

    print result;  
}
