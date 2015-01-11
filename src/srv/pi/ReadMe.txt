Note: Code is not according to coding guidelines and comments are not added everywhere.
      Updated SendToUart.c file. Main.c is the reference c file for testing.


Master Id	: '4321' (Values in ASCII)4 bytes
Node Id   	: '9876' 4 bytes
Switch Id	: 'er', 'ty' (Two Switch Id for two leds) 2 bytes
Command Id 	: For Switching ON  'as' 2 bytes
			: For Switching OFF 'ad' 2 bytes
			: For Status update	'af' 2 bytes

For Testing Through Arduino Serial Monitor:
LED 1:
SW ON	qwaaser67891234wq
SW OFF	qwaader67891234wq
SW Sts	qwaafer67891234wq

LED 2:
		Start Pattern	NodeRFID	CommandId	SwitchId	NodeId	MasterId	End Pattern
SW ON	qw 				a 			as 			ty 			6789 	1234 		wq	// Controller is lil endian
SW OFF	qwaadty67891234wq
SW Sts	qwaafty67891234wq

For changing Master ID: 
qwasder67891234wq
			
For Testing Through C code:
Note: header file not ready yet. For Reference check main.c source code.
#include "SendToUart.c" and Compile your c file, No external libraries are required. But include SendToUart.c as in your c file.
Function to be called is integrate(b,c), where b and c char arrays of length 13 and 8 respectively.

