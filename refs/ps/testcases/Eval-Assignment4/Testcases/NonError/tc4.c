int          date           ,     time;

void main(){
    time = 9*2/3*1+5*9/3*2;
    //checks precendence of ?:,||,!,+ and +
    //and associativity of ||, !
    //checks nesting of do-while.
    do{
        date = !time*time>2||(time<10)?date:date+2;
        time = time + 1*time*time;
        print date;
        do{
            if(time*2==1){
                date = 10;
            }
            else{
                date = time;
            }
        }while(date!=10);

    }while(date!=100);

}