#include <stdio.h>
#include "SendToUart.c"

int main()
{
  char ch=0;
  char b1[13]={'a','a','s','e','r','6','7','8','9','1','2','3','4'}; //Switch 1 On
  char b2[13]={'a','a','d','e','r','6','7','8','9','1','2','3','4'}; //Switch 1 off
  char b3[13]={'a','a','s','t','y','6','7','8','9','1','2','3','4'}; //Switch 2 On
  char b4[13]={'a','a','d','t','y','6','7','8','9','1','2','3','4'}; //Switch 2 Off
  char b5[13]={'a','a','f','e','r','6','7','8','9','1','2','3','4'}; //Switch 1 status
  char b6[13]={'a','s','d','e','r','6','7','8','9','1','2','3','4'}; //Node Master Id change
  char c[8]={""};
  int i;
  printf("Enter Number from 1 to 6 to select the input\n");
  scanf("%c",&ch);
  if (ch == '1')
    integrate(b1,c);
  if (ch == '2')
    integrate(b2,c);
  if (ch == '3')
    integrate(b3,c);
  if (ch == '4')
    integrate(b4,c);
  if (ch == '5')
    integrate(b5,c);
  if (ch == '6')
    integrate(b6,c);
    
    for (i=0;i<8;i++)
    {
      printf("%d\n",c[i]);
    }
	printf("Process done\n");
return 0; 
}
