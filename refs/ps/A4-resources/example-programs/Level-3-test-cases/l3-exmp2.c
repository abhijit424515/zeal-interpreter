float averageWorkingHoursPerDay;
void main() {
	float salary,bonus,employeeDailyWorkingHours;
	averageWorkingHoursPerDay=8.5;
	salary=10000.0;	
	read employeeDailyWorkingHours;
	bonus = employeeDailyWorkingHours > averageWorkingHoursPerDay?salary*0.2 : salary*0.1;
	salary = salary + bonus;
	print salary;
}

