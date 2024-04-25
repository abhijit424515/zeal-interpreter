int          date           ,     time;

void main(){
    time = 9*2/3*1+5*9/3*2;
    //checks precendence of ?:,||,!,+ and +
    //and associativity of ||, !
    date = !time*time>2||(time<10)?date:date+2;
    print date;
}