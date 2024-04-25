**PROCEDURE: main
**BEGIN: Three Address Code Statements
	def_ = 1
	temp0 = - 2
	abc_ = temp0
	temp1 = abc_ + ghi_
	temp2 = temp1 + def_
	ghi_ = temp2
	temp3 = abc_ < def_
	temp5 = ! temp3
	if(temp5) goto Label0
	stemp0 = abc_
	goto Label1
Label0:
	temp4 = - def_
	stemp0 = temp4
Label1:
	write  stemp0
	temp6 = ghi_ + def_
	temp7 = temp6 + 8
	write  temp7
**END: Three Address Code Statements
