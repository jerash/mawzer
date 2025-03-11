INCLUDE "..\..\Common_Ez_Z8\identity_inc.asm"

define 	IDENTITY_ADDRESS, space=rom
segment IDENTITY_ADDRESS
ORG    	%0E00
.def 	_identity
_identity:
		db	FW_VERSION,FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0

 

 			
