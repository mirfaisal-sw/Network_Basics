
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <linux/serial.h>
#include <errno.h>
#include <sys/file.h>

/*
 * Define modem line bits
 */
/* modem lines */
#define TIOCM_LE        0x001
#define TIOCM_DTR       0x002
#define TIOCM_RTS       0x004
#define TIOCM_ST        0x008
#define TIOCM_SR        0x010
#define TIOCM_CTS       0x020
#define TIOCM_CAR       0x040
#define TIOCM_RNG       0x080
#define TIOCM_DSR       0x100
#define TIOCM_CD        TIOCM_CAR
#define TIOCM_RI        TIOCM_RNG
#define TIOCM_OUT1      0x2000
#define TIOCM_OUT2      0x4000
#define TIOCM_LOOP      0x8000

/*
 * Define Line discipline macros
 */
#define N_TTY		0
#define N_HCI           15
#define N_NULL          27

/*
 *
 * Macro to enable/disable debug
 */
//#define DEBUG

static int _fd = -1;

/*Command line args*/
static char *_cl_port = NULL;
static int _cl_baud = 0;
static unsigned char _cl_dynamic_config = 0;

static struct termios my_termios;
static struct termios my_termios_read;
static tcflag_t c_cflag;
static speed_t my_baud_rate;
static int ldisc_num;
static int modem_bits;
static int bytes_to_read;
static struct serial_icounter_struct icount = { 0 };
static char usr_write_buf[] = "HELLO WORLD";
static char usr_read_buf[128];

static void dump_data(unsigned char * b, int count)
{
	printf("%i bytes: ", count);
	int i;
	for (i=0; i < count; i++) {
		printf("%02x ", b[i]);
	}

	printf("\n");
}

void set_modem_lines(int fd, int bits, int mask)
{
	int status, ret;

	if (ioctl(fd, TIOCMGET, &status) < 0) {
		ret = -errno;
		perror("TIOCMGET failed");
		exit(ret);
	}

	status = (status & ~mask) | (bits & mask);

	if (ioctl(fd, TIOCMSET, &status) < 0) {
		ret = -errno;
		perror("TIOCMSET failed");
		exit(ret);
	}
}

static void display_help(void)
{
	printf("Usage: ./uart_std_api_app -p </dev/tty*> -b <baud_rate> \n"
			"\n"
			"  -h, --help\n"
			"  -b, --baud rate in bps\n"
			"  -p, --port Port (Ex - /dev/ttySAC4)\n"
			"  -d, --dynamic_config\n"
			"\n"
	      );
}

static void process_options(int argc, char * argv[])
{
	int opt;
	static const struct option longopts[] = {
		 {"help", no_argument, 0, 'h'},
                 {"baud", required_argument, 0, 'b'},
                 {"port", required_argument, 0, 'p'},
		 {"dynamic_config", no_argument, 0, 'd'},
	};

	while ((opt = getopt_long(argc, argv, "hdp:b:?", longopts,
                                    NULL)) != -1)
	{
        	switch(opt)
       		{

			case 'p':
	                        _cl_port = strdup(optarg);
	                        break;

			case 'b':
	                        _cl_baud = atoi(optarg);
        	                break;

			case 'd':
				_cl_dynamic_config = 1;
				break;

			case 'h':
                        	display_help();
                        	exit(0);
                        	break;

			case '?':
		        	display_help();
				exit(0);
				break;
		}

	}
}

/*converts integer baud to Linux define*/
static int get_baud(int baud)
{
        switch (baud) {
        case 4800:
                return B4800;
        case 9600:
                return B9600;
        case 19200:
                return B19200;
        case 38400:
                return B38400;
        case 57600:
                return B57600;
        case 115200:
                return B115200;
        case 230400:
                return B230400;
        case 460800:
                return B460800;
        case 500000:
                return B500000;
        case 576000:
                return B576000;
        case 921600:
                return B921600;
#ifdef B1000000
        case 1000000:
                return B1000000;
#endif
#ifdef B1152000
        case 1152000:
                return B1152000;
#endif
#ifdef B1500000
        case 1500000:
                return B1500000;
#endif
#ifdef B2000000
        case 2000000:
                return B2000000;
#endif
#ifdef B2500000
        case 2500000:
                return B2500000;
#endif
#ifdef B3000000
        case 3000000:
                return B3000000;
#endif
#ifdef B3500000
        case 3500000:
                return B3500000;
#endif
#ifdef B4000000
        case 4000000:
                return B4000000;
#endif
        default:
                return -1;
        }
}

int decode_baud_rate(struct termios mtermios)
{
        tcflag_t c_cflag = mtermios.c_cflag;

		/*Macros with prefix B are octal numbers*/
        if ((c_cflag & B4000000) == B4000000)
                return 4000000;
	else if ((c_cflag & B3500000) == B3500000)
                return 3500000;
	else if ((c_cflag & B3000000) == B3000000)
                return 3000000;
	else if ((c_cflag & B2500000) == B2500000)
                return 2500000;
	else if ((c_cflag & B2000000) == B2000000)
                return 2000000;
	else if ((c_cflag & B1500000) == B1500000)
                return 1500000;
	else if ((c_cflag & B1152000) == B1152000)
                return 1152000;
	else if ((c_cflag & B1000000) == B1000000)
                return 1000000;
	else if ((c_cflag & B921600) == B921600)
                return 921600;
	else if ((c_cflag & B576000) == B576000)
                return 576000;
	else if ((c_cflag & B500000) == B500000)
                return 500000;
	else if ((c_cflag & B460800) == B460800)
                return 460800;
	else if ((c_cflag & B230400) == B230400)
                return 230400;
	else if ((c_cflag & B115200) == B115200)
                return 115200;
	else if ((c_cflag & B57600) == B57600)
                return 57600;
	else if ((c_cflag & B38400) == B38400)
                return 38400;
	else if ((c_cflag & B19200) == B19200)
                return 19200;
	else if ((c_cflag & B9600) == B9600)
                return 9600;
	else if ((c_cflag & B4800) == B4800)
                return 4800;
	else if ((c_cflag & B2400) == B2400)
                return 2400;
	else if ((c_cflag & B1800) == B1800)
                return 1800;
	else if ((c_cflag & B1200) == B1200)
                return 1200;
	else if ((c_cflag & B600) == B600)
                return 600;
	else if ((c_cflag & B300) ==B300)
                return 300;
	else if ((c_cflag & B200) == B200)
                return 200;
	else if ((c_cflag & B150) == B150)
                return 150;
	else if ((c_cflag & B134) ==B134)
                return 134;
	else if ((c_cflag & B110) == B110)
                return 110;
	else if ((c_cflag & B75) ==B75)
                return 75;
        else if ((c_cflag & B50) == B50)
		return 50;
        else if ((c_cflag & B0) == B0)
                return 0;
        else
                return -1;
}

static void setup_serial_port(int baud)
{
	struct termios newtio;
	int ret;

	_fd = open(_cl_port, O_RDWR | O_NONBLOCK);

	if (_fd < 0) {
		ret = -errno;
		perror("Error opening serial port");
		exit(ret);
	}

	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

	/* man termios get more info on below settings */
	/*Default settings*/
	newtio.c_cflag = baud | CS8 | CLOCAL | CREAD;
	newtio.c_cflag |= CSTOPB; /*2 stop bits, 1 if cleared*/
        newtio.c_cflag |= PARENB;/*Enable parity*/
        newtio.c_cflag |= PARODD;/*Odd parity, Even parity if cleared*/
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;

	/*block for up till 128 characters*/
	newtio.c_cc[VMIN] = 128;

	/*0.5 seconds read timeout*/
	newtio.c_cc[VTIME] = 5;

	/* now clean the modem line and activate the settings for the port */
	tcflush(_fd, TCIOFLUSH);
	tcsetattr(_fd,TCSANOW,&newtio);
}

void perform_dynamic_config()
{
	struct termios m_termios, m_termios_read;
	tcflag_t c_cflag;
	int ret = -1;
	int baud_rate, baud_macro;
	
	bzero(&m_termios, sizeof(m_termios)); 
	bzero(&m_termios_read, sizeof(m_termios_read));

	printf("Initial UART Configuration: \n");
	
	printf("IOCTL command - TCGETS\n");
	ret = ioctl(_fd, TCGETS, &m_termios);
	if (ret == 0) {
		
		baud_rate = decode_baud_rate(m_termios);
		if(baud_rate != -1)
			printf(" Baud rate = %d bps\n", baud_rate);

                c_cflag = m_termios.c_cflag;

                if (c_cflag & CS8)
                        printf(" Data bits = 8 bits\n");
		else if (c_cflag & CS7)
                        printf(" Data bits = 7 bits\n");
		else if (c_cflag & CS6)
                        printf(" Data bits = 6 bits\n");
		else if (c_cflag & CS5)
                        printf(" Data bits = 5 bits\n");

                if (c_cflag & CSTOPB)
                        printf(" No of stop bits = 2\n");
                else
                        printf(" No of stop bits = 1\n");

                if (c_cflag & CRTSCTS)
                        printf(" RTS/CTS is enabled\n");
                else
                        printf(" RTS/CTS is disabled\n");
                
        } else {

                printf("IOCTL Command: TCGETS failed.\n");
                perror("Error getting TCGETS");
        }

	printf("\nChanging Uart baud rate to : 2400bps\n");
	printf("IOCTL command - TCSETS\n");

	baud_macro = get_baud(baud_rate);
	m_termios.c_cflag = ((m_termios.c_cflag & ~baud_macro) | B2400);
        ret = ioctl(_fd, TCSETS, &m_termios);
        if (ret == 0) {
        
		printf("Changed UART Configuration: \n");
		ioctl(_fd, TCGETS, &m_termios_read);
                c_cflag = m_termios_read.c_cflag;
                if(c_cflag & B2400)
                        printf(" Baud rate = 2400 bps \n");

		printf("\n\nUart dynamic config passed !!\n");

        } else {
                printf("IOCTL Command: TCSETS failed. \n");
                perror("Error getting TCSETS");
        }
	
}

int main(int argc, char * argv[])
{
	int ret = 0;
	int baud = 0;

	printf("Linux Uart STD API test app\n");
	process_options(argc, argv);

	if (!_cl_port) {
		fprintf(stderr, "ERROR: Port argument required\n");
		display_help();
		exit(-EINVAL);
	}

	if (_cl_baud <= 0){
		fprintf(stderr, "ERROR: Invalid baud rate\n");
		display_help();
		exit(-EINVAL);
	}else{
		baud = get_baud(_cl_baud);
		setup_serial_port(baud); /*use defualt baud rate of 115200bps*/
	}

	/*Perform dynamic configuration of Uart port using IOCTL command*/
	if(_cl_dynamic_config) {
		
		perform_dynamic_config();
		return 0;
	}

	/* Validate UART STD APIs*/

	/****************************
	 * Validate:  Termios APIs  *
	 * **************************/

	/*API 1: tcgetattr()*/
	tcgetattr(_fd, &my_termios);
	c_cflag = my_termios.c_cflag;
#ifdef DEBUG
    	if (c_cflag & CS8)
		printf(" Data bits = 8 bits\n");

    	if (c_cflag & CSTOPB)
		printf(" No of stop bits = 2\n");
    	else
		printf(" No of stop bits = 1\n");

    	if (c_cflag & PARENB) {
        	if (c_cflag & PARODD)
                	printf(" parity = odd\n");
        	else
            		printf(" parity = even\n");
	} else {
        	printf(" parity = none\n");
	}
#endif

    	if (c_cflag & baud) {
#ifdef DEBUG
        	printf(" Baud rate = %d bps\n", _cl_baud);
#endif
        	printf("API 1: tcgetattr() is working\n");
    	} else {
		perror("API 1: Error invoking tcgetattr()\n");
	}

	/*API 2: tcsetattr() */
	/*Change baud rate to 115200bps*/
	my_termios.c_cflag = ((my_termios.c_cflag & ~B9600) | B115200);
	tcsetattr(_fd, TCSANOW, &my_termios);
	my_baud_rate = cfgetispeed(&my_termios);
	(my_baud_rate == B115200) ? printf("API 2: tcsetattr() is working\n") :
		perror("API 2: Error invoking tcsetattr()\n");

	/*API 3: cfsetispeed() */
	cfsetispeed(&my_termios, B57600);
	my_baud_rate = cfgetispeed(&my_termios);
	(my_baud_rate == B57600) ? printf("API 3: cfsetispeed() is working\n") :
		perror("API 3: Error invoking cfsetispeed()");

	/*API 4: cfsetospeed()*/
        cfsetospeed(&my_termios, B57600);
        my_baud_rate = cfgetospeed(&my_termios);
	(my_baud_rate == B57600) ? printf("API 4: cfsetospeed() is working\n") :
		perror("API 4: Error invoking cfsetospeed()");

	/*API 5: cfgetispeed()*/
	my_baud_rate = cfgetispeed(&my_termios);
	(my_baud_rate == B57600) ? printf("API 5: cfgetispeed() is working\n") :
		perror("API 5: Error invoking cfgetispeed()");

	/*API 6: cfgetospeed()*/
	my_baud_rate = cfgetospeed(&my_termios);
	(my_baud_rate == B57600) ? printf("API 6: cfgetospeed() is working\n") :
		perror("API 6: Errorr invoking cfgetospeed()");

	/*API 7: tcsendbreak()*/
	ret = tcsendbreak(_fd, 0);
	(ret == 0) ? printf("API 7: tcsendbreak() is working\n") :
		perror("API 7: Error invoking tcsendbreak()");

	printf("\n\n");

	/*********************************
	 * Validate ioctl_tty commands   *
	 * *******************************/

	/*ioctl command 1: TCGETS*/
	ret = ioctl(_fd, TCGETS, &my_termios);

	if (ret == 0) {
		c_cflag = my_termios.c_cflag;
#ifdef DEBUG
        	if (c_cflag & B57600)
                	printf(" Baud rate = 57600bps\n");

        	if (c_cflag & CS8)
                	printf(" Data bits = 8 bits\n");

        	if (c_cflag & CSTOPB)
                	printf(" No of stop bits = 2\n");
        	else
                	printf(" No of stop bits = 1\n");

		if (c_cflag & CRTSCTS)
    			printf(" RTS/CTS is enabled\n");
		else
    			printf(" RTS/CTS is disabled\n");
#endif
        	if (c_cflag & B57600)
        		printf("IOCTL Command 1: TCGETS is working\n");
        } else {

        	printf("IOCTL Command 1: TCGETS is not working\n");
		perror("Error getting TCGETS");
	}

	/*ioctl command 2: TCSETS*/
	my_termios.c_cflag = ((my_termios.c_cflag & ~B57600) | B115200);
	ret = ioctl(_fd, TCSETS, &my_termios);
        if (ret == 0) {
		ioctl(_fd, TCGETS, &my_termios_read);
        	c_cflag = my_termios_read.c_cflag;
        	if(c_cflag & B115200)
                	printf("IOCTL Command 2: TCSETS is working\n");
	} else {
                printf("IOCTL Command 2: TCSETS is not working\n");
		perror("Error getting TCSETS");
	}

	/*ioctl command 3: FIONREAD */
	if(write(_fd, usr_write_buf, sizeof(usr_write_buf)) == -1)
		perror("Write API failed");
	ret = ioctl(_fd, FIONREAD, &bytes_to_read);
	if (ret == 0) {

#ifdef DEBUG
		printf(" Bytes in input buffer to read = %d\n", bytes_to_read);
#endif
        	printf("IOCTL command 3: FIONREAD is working\n");
	} else {
        	printf("IOCTL command 3: FIONREAD is not working\n");
		perror("Error getting FIONREAD");
	}

	/*ioctl command 4: TIOCGICOUNT*/
	if(write(_fd, usr_write_buf, sizeof(usr_write_buf)) == -1)
		perror("Write API failed");
	if(read(_fd, usr_read_buf, sizeof(usr_write_buf)) == -1)
		perror("Read API failed");
	ret = ioctl(_fd, TIOCGICOUNT, &icount);
	if (ret ==  0) {
		#ifdef DEBUG
                printf(" %s: TIOCGICOUNT: ret=%i, rx=%i, tx=%i, frame = %i, overrun = %i, parity = %i, brk = %i, buf_overrun = %i\n",
                                _cl_port, ret, icount.rx, icount.tx, icount.frame, icount.overrun, icount.parity, icount.brk,
                                icount.buf_overrun);
		#endif
        	printf("IOCTL command 4: TIOCGICOUNT is working\n");
        } else {
        	printf("IOCTL command 4: TIOCGICOUNT is not working\n");
		perror("Error getting TIOCGICOUNT");
	}

	/*ioctl command 5: TIOCGWINSZ*/
	struct winsize *m_winsz = NULL;
	ret = ioctl(_fd, TIOCGWINSZ, m_winsz);
        if (ret ==  0) {
#ifdef DEBUG
                printf(" Window row = %d, window col = %d, window x pixel = %d, window y pixel = %d\n",
			m_winsz->ws_row, m_winsz->ws_col, m_winsz->ws_xpixel, m_winsz->ws_ypixel);
#endif
                printf("IOCTL command 5: TIOCGWINSZ is working\n");
        } else {
                printf("IOCTL command 5: TIOCGWINSZ is not working\n");
		perror("Error getting TIOCGWINSZ");
        }


	/*ioctl command 6: TIOCMGET */
	ret = ioctl(_fd, TIOCMGET, modem_bits);
        if (ret == 0) {
#ifdef DEBUG
                printf(" Debug modem bits = %x\n", modem_bits);

		if(modem_bits & TIOCM_LOOP)
                        printf(" Internal loop back is set\n ");

                if(modem_bits & TIOCM_OUT2)
                        printf(" OUT2 is not set \n");

                if(modem_bits & TIOCM_OUT1)
                        printf(" OUT1 is not set\n");

                if(modem_bits & TIOCM_RI)
                        printf(" RI is not set \n");

                if(modem_bits & TIOCM_CD)
                        printf(" Carrier Detect is not set \n");

                if(modem_bits & TIOCM_DTR)
                        printf(" DTR is not set\n");

                if(modem_bits & TIOCM_DSR)
                        printf(" DSR is not set \n");

                if(modem_bits & TIOCM_CTS)
                        printf(" CTS is not set \n");

                if(modem_bits & TIOCM_RTS)
                        printf(" RTS is not set \n");
#endif
		//TODO Add verification
        	printf("IOCTL Command 6: TIOCMGET is working\n");
        } else {
        	printf("IOCTL Command 6: TIOCMGET is not working\n");
		perror("Error getting TIOCMGET");
	}

	/*ioctl command 7: TICOMSET */
	modem_bits = 0;
	modem_bits |= TIOCM_LOOP;

	ret = ioctl(_fd, TIOCMSET, modem_bits);
	if (ret == 0) {

		if (ioctl(_fd, TIOCMGET, modem_bits) == 0){

			if(modem_bits & TIOCM_LOOP){
#ifdef DEBUG
				printf("Internal loop back is set\n ");
#endif
                		printf("IOCTL Command 7: TIOCMSET is working\n");
			}
		}

	} else {
                printf("IOCTL Command 7: TIOCMSET is not working\n");
		perror("Error getting TIOCMSET");
	}

	/*ioctl command 8: TIOCGETD*/
	ret = ioctl(_fd, TIOCGETD, &ldisc_num);
	if (ret == 0) {
		if(ldisc_num == N_TTY){
#ifdef DEBUG
			printf(" Line discipline be - N_TTY\n");
#endif
			printf("IOCTL Command 8: TIOCGETD is working\n");
		}
	} else {
		printf("IOCTL Command 8: TIOCGETD is not working\n");
		perror("Error getting TIOCGETD");
	}

	/*ioctl command 9: TIOCSETD*/
    	ldisc_num = N_TTY;//N_HCI;
    	int temp = 0;
	ret = ioctl(_fd, TIOCSETD, &ldisc_num);
        if (ret == 0) {

		if(ioctl(_fd, TIOCGETD, &temp) == 0){
                	if(temp == ldisc_num)
                   		 printf("IOCTL Command 9: TIOCSETD is working\n");
        	}
        } else {
                printf("IOCTL Command 9: TIOCSETD is not working\n");
		perror("Error getting TIOCSETD");
        }
	/*******End of adding UART STD API under test**********/

	printf("Uart STD API test completed.\n");

	tcflush(_fd, TCIOFLUSH);

	return 0;
}
