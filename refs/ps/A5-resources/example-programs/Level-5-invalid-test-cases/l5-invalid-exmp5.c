int a,b,d;
int p(){
    if(b<10){
        p();
        a=d;
    }
    return 5;
}
int main(){
    return p();
}
