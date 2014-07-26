#include <stdio.h>
#include <string.h>
#include<stdlib.h>

void displayUsage()
{
	printf("ERROR: Expected inputs: MSID=<>, NDID=<>, CMID=<>, STST=<>\n");
	printf("NOTE: CMID and STST values must be in Hex\n");

	// TODO: Change all values to be read in Hex and on Node side interpret as Hex
}

int sendUART(char strTosendUART[])
{
	printf("Sending : %s \n", strTosendUART);
	return 1;
	// TODO: Add code to send via UART and based on success/failure of sending, return TRUE/FALSE
}

int translate(char toTranslate)
{
	char ref[]="0123456789ABCDEF";
	char i;
	for (i=0;i<16;i++)
	{
		if(toTranslate==ref[i])
		{
			return i;
		}
	}
}

char convert(char ToConvert[])
{
	printf("Argument to Convert: %s\n", ToConvert);
	char HexText[2]="";
	strncpy(HexText,ToConvert+2,2);
	printf("Hex text in input = %s\n", HexText);
	char HexVal=0;
	HexVal=(16*translate(HexText[0]))+translate(HexText[1]);
	printf("ASCII character corresponding to Hex Value = %c\n", HexVal);
	return HexVal;
}


char *checkForHexAndConvert(char chkArg[])
{
	int l=0;
	char attr_val[10]="";
	char *attr_name = malloc (sizeof (char) * 10); 
	printf("Checking the argument %s for Hex values\n", chkArg);
	for(l=0;l<10;l++)
	{
		if(chkArg[l]!='\0')
		{
			if(chkArg[l]=='=')
			{
				int m=0,i=0;
				while(m<l)
				{
					attr_name[i]=chkArg[m];
					i++;
					m++;
				}
				m++;
				int j=0;
				while(chkArg[m]!='\0')             {
					attr_val[j]=chkArg[m];
					j++;
					m++;

				}
				printf("Name = %s\n", attr_name);
				printf("Value = %s\n", attr_val);
				printf("First two characters = %.2s\n", attr_val);	
				char tmp[]="";
				strncpy(tmp,attr_val,2);
				tmp[2]='\0';
				printf("First two characters are: tmp = %s\n", tmp);
				if(!strcmp(tmp,"0X") || !strcmp(tmp,"0x"))
				{
					printf("Argument %s has Hex value\n", chkArg);
					char attr_val_new;
					attr_val_new=convert(attr_val);
					strcat(attr_name,"=");
					int attr_name_str_len=strlen(attr_name);
					attr_name[attr_name_str_len]=attr_val_new;
					attr_name[attr_name_str_len+1]='\0';
					return attr_name;
				}           
			}
		}

	}
	return chkArg;
} 







int main(int argc, char *argv[])
{
	if(argc!=5)
	{
		displayUsage();
		return 1;
	}
	printf("The values received are\n");
	printf("ARGC = %d \n",argc);
	printf("ARGV[0] = %s \n", argv[0]);
	printf("ARGV[1] = %s \n", argv[1]);
	printf("ARGV[2] = %s \n", argv[2]);
	printf("ARGV[3] = %s \n", argv[3]);
	printf("ARGV[4] = %s \n", argv[4]);

	// Assuming argv[1] is MSID=<>, argv[2] is NDID=<>, arg[3] is STST=<>, arg[4] is CMID=<>
	int i;
	for (i=1;i<argc;i++)
	{
		argv[i]=checkForHexAndConvert(argv[i]);
		printf("Arg[%d]: %s\n", i,argv[i]);
	}

	char strTosend[32]="";
	strcpy(strTosend,argv[1]);
	strcat(strTosend,",");
	strcat(strTosend,argv[2]);
	strcat(strTosend,",");
	strcat(strTosend,argv[3]);
	strcat(strTosend,",");
	strcat(strTosend,argv[4]);
	strcat(strTosend,",");
	printf("STRTOSEND=%s\n", strTosend);


	printf("Sending via UART...\n");
	int sendSuccess=sendUART(strTosend);

	if(!sendSuccess)
	{
		printf("ERROR: Sending Failed!!!\n");
		return 1;
	}

	return 0;
}