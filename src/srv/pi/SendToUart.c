#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * serialOpen:
 *	Open and initialise the serial port, setting all the right
 *	port parameters - or as many as are required - hopefully!
 *********************************************************************************
 */

int serialOpen (char *device, int baud)
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case     50:	myBaud =     B50 ; break ;
    case     75:	myBaud =     B75 ; break ;
    case    110:	myBaud =    B110 ; break ;
    case    134:	myBaud =    B134 ; break ;
    case    150:	myBaud =    B150 ; break ;
    case    200:	myBaud =    B200 ; break ;
    case    300:	myBaud =    B300 ; break ;
    case    600:	myBaud =    B600 ; break ;
    case   1200:	myBaud =   B1200 ; break ;
    case   1800:	myBaud =   B1800 ; break ;
    case   2400:	myBaud =   B2400 ; break ;
    case   9600:	myBaud =   B9600 ; break ;
    case  19200:	myBaud =  B19200 ; break ;
    case  38400:	myBaud =  B38400 ; break ;
    case  57600:	myBaud =  B57600 ; break ;
    case 115200:	myBaud = B115200 ; break ;
    case 230400:	myBaud = B230400 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;	// 10mS

  return fd ;
}

/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */

void serialClose (int fd)
{
  close (fd) ;
}
/*
 * serialPutchar:
 *	Send a single character to the serial port
 *********************************************************************************
 */
void serialPutchar (int fd, unsigned char c)
{
  write (fd, &c, 1) ;
}
int fd=0;

/*
 * serialDataAvail:
 *	Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

int serialDataAvail (int fd)
{
  int result ;

  if (ioctl (fd, FIONREAD, &result) == -1)
    return -1 ;

  return result ;
}


/*
 * serialGetchar:
 *	Get a single character from the serial device.
 *	Note: Zero is a valid character and this function will time-out after
 *	10 seconds.
 *********************************************************************************
 */

int serialGetchar (int fd)
{
  uint8_t x ;

  if (read (fd, &x, 1) != 1)
    return -1 ;

  return ((int)x) & 0xFF ;
}

/*
 * serialUART:
 *	Sends the data to serial port
 *********************************************************************************
 */
int sendUART(char strTosendUART[])
{
int i=0;
printf("Sending : %s \n", strTosendUART);
for (i = 0; i < 17; i++)
serialPutchar (fd, strTosendUART[i]);
serialClose(fd);
return 1;
// TODO: Add code to send via UART and based on success/failure of sending, return TRUE/FALSE
}

/*
 Main Fucntion *********************************************************************************
 */

int integrate(char *recv, char *send)
{
 int i=0;
 char strTosend[17];
 int  RecByt_u16[8];
 fd= serialOpen ("/dev/ttyUSB0", 115200);
 if(fd<0)
 {
  printf("Error opening serial port!");
  return 0;
 }
 //printf(" Total Count = %d\n",argc);

 for (i = 0; i < 13; i++)
 {
  strTosend[i+2] = recv[i];
//  printf(" strTosend[%d] = %x\n",i+2, strTosend[i+2]);
 }

 
 /*union
 {
  char Rec_Byte[4];
  unsigned int Rec_u16;
 }Rec_Data;

 Rec_Data.Rec_u16=atoi(argv[1]);
 strTosend[2]=Rec_Data.Rec_Byte[0];

 Rec_Data.Rec_u16=atoi(argv[2]);
 strTosend[3]=Rec_Data.Rec_Byte[0];
 strTosend[4]=Rec_Data.Rec_Byte[1];

 Rec_Data.Rec_u16=atoi(argv[3]);
 strTosend[5]=Rec_Data.Rec_Byte[0];
 strTosend[6]=Rec_Data.Rec_Byte[1];

 Rec_Data.Rec_u16=atoi(argv[4]);
 strTosend[7]=Rec_Data.Rec_Byte[0];
 strTosend[8]=Rec_Data.Rec_Byte[1];
 strTosend[9]=Rec_Data.Rec_Byte[2];
 strTosend[10]=Rec_Data.Rec_Byte[3];

 Rec_Data.Rec_u16=atoi(argv[5]);
 strTosend[11]=Rec_Data.Rec_Byte[0];
 strTosend[12]=Rec_Data.Rec_Byte[1];
 strTosend[13]=Rec_Data.Rec_Byte[2];
 strTosend[14]=Rec_Data.Rec_Byte[3];*/

 strTosend[0]='q';
 strTosend[1]='w';
 strTosend[15]='w';
 strTosend[16]='q';

 int sendSuccess=sendUART(strTosend);

 if(!sendSuccess)
 {
  printf("ERROR: Sending Failed!!!\n");
  return 0;
 }

/*
 Fucntion to Receive data from Arduino *********************************************************************************

 */
 fd= serialOpen ("/dev/ttyUSB0", 115200);

 for (;;)
 { 
   while (serialDataAvail(fd) > 1)
   {
     for (i=0; i<8; i++)
     {
       send[i]= serialGetchar(fd);
       //printf("Sending Byte [%d]= %d\n",i,send[i]);
       if (i == 7)
       {
	fflush (stdout);
	 serialClose(fd);
         return 1;
       }
     }
   }
 }

}
