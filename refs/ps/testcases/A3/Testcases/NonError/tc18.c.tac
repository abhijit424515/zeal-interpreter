**PROCEDURE: main
**BEGIN: Three Address Code Statements
	read  __
	read  ___
	temp0 = __ == ___
	temp6 = ! temp0
	if(temp6) goto Label0
	temp1 = 4 + 2
	temp2 = temp1 + __
	temp3 = temp2 + ___
	stemp0 = temp3
	goto Label1
Label0:
	temp4 = __ * 11
	temp5 = temp4 * ___
	stemp0 = temp5
Label1:
	write  stemp0
**END: Three Address Code Statements
