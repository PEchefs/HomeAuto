#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

char action,Node_ID_received,Node_State_received;

char command_string[100]="java -jar /home/pi/HomeAuto/updatedb1.jar ";
char tmp_str[100];
char state_parsed[4];

clear()
{
	strcpy(command_string,"java -jar /home/pi/HomeAuto/updatedb1.jar ");
	strcpy(tmp_str,"");
	strcpy(state_parsed,"");
	action=0;
	Node_ID_received=0;
	Node_State_received=0;
}

updateDB(char NDID, char STATE)
{	
	switch (STATE)
	{
			case 0x00: strcpy(state_parsed,"0000");break;
			case 0x01: strcpy(state_parsed,"0001");break;
			case 0x02: strcpy(state_parsed,"0010");break;
			case 0x03: strcpy(state_parsed,"0011");break;
			case 0x04: strcpy(state_parsed,"0100");break;
			case 0x05: strcpy(state_parsed,"0101");break;
			case 0x06: strcpy(state_parsed,"0110");break;
			case 0x07: strcpy(state_parsed,"0111");break;
			case 0x08: strcpy(state_parsed,"1000");break;
			case 0x09: strcpy(state_parsed,"1001");break;
			case 0x0A: strcpy(state_parsed,"1010");break;
			case 0x0B: strcpy(state_parsed,"1011");break;
			case 0x0C: strcpy(state_parsed,"1100");break;
			case 0x0D: strcpy(state_parsed,"1101");break;
			case 0x0E: strcpy(state_parsed,"1110");break;
			case 0x0F: strcpy(state_parsed,"1111");break;
			default: return 1;
	}
	printf("state parsed=%s\n",state_parsed);
	sprintf(tmp_str,"0x%X ",NDID);
	printf("Node_ID=%s\n",tmp_str);
	strcat(command_string,tmp_str);
	strcat(command_string,state_parsed);
	printf("%s\n",command_string);
	system(command_string);
}

int main ()
{
	int fd ;
	int count ;
	if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		return 1 ;
	}
	
	if (wiringPiSetup () == -1)
	{
		fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
		return 1 ;
	}

	for(;;)
	{
		while (serialDataAvail(fd)==2)
		{
		// int action=serialGetchar(fd);
		action=1;
		if (action==1)
		{
			int Node_ID_received=serialGetchar(fd);
			printf("Node ID received=%d\n",Node_ID_received);
			int Node_state_received=serialGetchar(fd);
			printf("Node State received=%d",Node_state_received);
				fflush (stdout);
			updateDB(Node_ID_received,Node_state_received);
		}
		clear();
	
		}
	
	}
  return 0 ;
}