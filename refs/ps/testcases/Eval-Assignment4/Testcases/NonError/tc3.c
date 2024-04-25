int date,time; 
string a;
void main()
{   string b,c;
    read date;
    //checks nestability of ?: and precendence of ?,+,== and associativity of ?:
    //and nestability of while.
    time = 2;
    while(date<100){
        date = date -1;
        while(date<100){
            date = date + date>2?date+1:date+2<3?date+4:date+3==3?1:2;
            while(date<100){
                        date = date * time;
                        c = b;
            }
        }
    }
}