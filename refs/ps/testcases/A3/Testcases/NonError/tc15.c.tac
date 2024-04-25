**PROCEDURE: main
**BEGIN: Three Address Code Statements
	def_ = 1
	temp0 = - 2
	abc_ = temp0
	temp1 = abc_ < def_
	temp3 = ! temp1
	if(temp3) goto Label0
	stemp0 = abc_
	goto Label1
Label0:
	temp2 = - def_
	stemp0 = temp2
Label1:
	ghi_ = stemp0
	write  ghi_
**END: Three Address Code Statements
