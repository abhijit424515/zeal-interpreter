void main()
{
    float marks;
    string grade;

    print "Enter your percentage ";
    read marks;

    grade = marks < 40.0 ? "F" : marks >= 40.0 && marks < 50.0 ? "D" : marks >= 50.0 && marks < 60.0 ? "C" : marks >= 60.0 && marks < 70.0 ? "B" : "A";
    print "The grade obtained is: ";
    print grade;
}