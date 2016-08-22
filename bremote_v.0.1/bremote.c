/*
 *  SonyEricsson T610 Bluetooth remote control for Xine
 *  (c) by Niko Mauno 2004
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 **********************************************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h> /* usleep(useconds) */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>

#include <sys/wait.h>
#include <signal.h> 
#include <sys/types.h>  /* data types */
#include <sys/socket.h> /* Internet Protocol family */
#include <sys/time.h>   /* file access and modification times structure */
#include <netinet/in.h> /* Internet address family */
#include <errno.h>      /* system error numbers */
#include <ctype.h>      /* character types */
#include <netdb.h>      /* definitions for network database operations */
#include <stdio.h>      /* standard buffered input/output */
#include <string.h>     /* string ops */

short verbose=0, success=0;

extern void cfmakeraw (struct termios *__termios_p) __THROW;

/*
 * at_command (modified by Niko Mauno)
 * Original version of at_command by Marcel Holtmann <marcel@holtmann.org>
 * taken from bluez-utils-2.10/test/attest.c (http://www.bluez.org/)
*/

static int at_command(int fd, char *cmd, int to)
{
	fd_set rfds;
	struct timeval timeout;
	char buf[1024];
	int sel, len, i;

	write(fd, cmd, strlen(cmd));

	for (i = 0; i < 100; i++) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds); 
		timeout.tv_sec = 0;
		timeout.tv_usec = to;
		if ((sel = select(fd + 1, &rfds, NULL, NULL, &timeout)) > 0) {
			if (FD_ISSET(fd, &rfds)) {
				memset(buf, 0, sizeof(buf)); 
				len = read(fd, buf, sizeof(buf)); 
				/* for (n = 0; n < len; n++) printf("%c", buf[n]);  */
				if (strstr(buf,  "\r\nOK") != NULL) 		return 0;
				if (strstr(buf, "\r\nERROR") != NULL) 		return -1;
			}
		}
	}
	return -1;
}

/*
 * open_device by Marcel Holtmann <marcel@holtmann.org>
 * taken from bluez-utils-2.10/test/attest.c (http://www.bluez.org/)
*/

static int open_device(char *device)
{
	int fd;
	struct termios ti;
	if ((fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) { 
		printf("Can't open serial port. %s (%d)\n", strerror(errno), errno);
		return -1;
	}
	tcflush(fd, TCIOFLUSH);
	cfmakeraw(&ti); 
	tcsetattr(fd, TCSANOW, &ti);
	return fd;
}

/*
 * open_socket by Marcel Holtmann <marcel@holtmann.org>
 * taken from bluez-utils-2.10/test/attest.c (http://www.bluez.org/)
*/
static int open_socket(bdaddr_t *bdaddr, uint8_t channel)
{
	struct sockaddr_rc remote_addr, local_addr;
	int s;

	if ((s = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
		printf("Can't create socket. %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.rc_family = AF_BLUETOOTH;
	bacpy(&local_addr.rc_bdaddr, BDADDR_ANY); 
	if (bind(s, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) { 
		printf("Can't bind socket. %s (%d)\n", strerror(errno), errno);
		close(s);
		return -1;
	}

	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.rc_family = AF_BLUETOOTH;
	bacpy(&remote_addr.rc_bdaddr, bdaddr);
	remote_addr.rc_channel = channel;

	if (connect(s, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0) {
		printf("Can't connect. %s (%d)\n", strerror(errno), errno);
		close(s);
		return -1;
	}

	return s;
}

/*
 * at_parser (by Niko Mauno)
 * 
 * contains modified implementation of "client.c" file listed in the 
 * document "BSD Socket Reference: Berkeley UNIX System Calls and 
 * Interprocess Communication" by Lawrence Besaw.
 *
 * contains modified implementation of at_command by Marcel Holtmann <marcel@holtmann.org>
 * taken from bluez-utils-2.10/test/attest.c (http://www.bluez.org/)
 *
*/
static int at_parser(int fd, int to)
{

	fd_set rfds;
	struct timeval timeout;
	unsigned char buf[1024], pre[9]="\r\n+CKEV: ";
	int sel, len;
	
	int replay_speed=4; /* Normal */	
	int audio_mute=0; /* Unmuted */
	int shift_key=0;	/* Unpressed */
	int paused=0; /* Unpaused */

	struct hostent *hostp;
	struct servent *servp;
	struct sockaddr_in server;
	int sock;
	static struct timeval inet_long_timeout = { 5, 0 }; 
	static struct timeval inet_short_timeout = { 0, 100000 }; 

	fd_set rmask, mask; /* xmask; */
	/* fd_set contains information about a set of files */

	char inet_buf_in[BUFSIZ], inet_buf_out[100];
	int nfound, bytesread;

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Socket");
		exit(1);
	}
	
		static struct servent s;
		servp = &s;
		s.s_port = htons((__u_short)6789); 
		/* s.s_port = htons((unsigned short)atoi(argv[1])); */
	
	if ((hostp = gethostbyname("localhost")) == 0) {
		fprintf(stderr,"Unknown host\n");
		exit(1);
	}

/*	 fnctl (sock, SETFL, fcntl(sock, GETFL) | O_NONBLOCK);  */

	memset((void *) &server, 0, sizeof server);
	server.sin_family = AF_INET;
	memcpy((void *) &server.sin_addr, hostp->h_addr, hostp->h_length);
	server.sin_port = servp->s_port;
	
	if (connect(sock, (struct sockaddr *)&server, sizeof server) < 0) {
		(void) close(sock);
		perror("Connect");
		exit(1);
	}
	
	FD_ZERO(&mask);
	FD_SET(sock, &mask);
	FD_SET(fileno(stdin), &mask);

	rmask = mask;
	nfound = select(FD_SETSIZE, &rmask, (fd_set *)0, (fd_set *)0, &inet_long_timeout);

	if (!(FD_ISSET(sock,&rmask))) {
			/* no data from network */
			printf("Didn't receive any data from the host! Exiting...\n");
			(void) close(sock);			
			return -1;
	}

	strcpy(inet_buf_out, "identify root:root\n");

	while(1) {

	FD_ZERO(&mask);
	FD_SET(sock, &mask);
	FD_SET(fileno(stdin), &mask);

	rmask = mask;
	nfound = select(FD_SETSIZE, &rmask, (fd_set *)0, (fd_set *)0, &inet_short_timeout);

		if (nfound < 0) {
			if (errno == EINTR) {
				printf("Interrupted system call\n");
				continue;
			}
			/* something is very wrong! */
			perror("Select");
			exit(1);
		}

		if (strcmp(inet_buf_out,"") != 0) {
			if (verbose!=0) printf("Sending>%s",inet_buf_out);
			if (write(sock, inet_buf_out, strlen(inet_buf_out)) < 0) {
				perror("write");
				exit(1);
			} else {
				if (strcmp(inet_buf_out,"halt\n")==0) {
					if (verbose!=0) printf("Quitting, bye!\n");
					fflush(stdin);
					(void) close(sock);
					return 0;
				} else strcpy(inet_buf_out,"");
			}
		} 
		
		if (FD_ISSET(sock,&rmask)) {
			/* data from network */
			bytesread = read(sock, inet_buf_in, sizeof inet_buf_in);
			inet_buf_in[bytesread] = '\0';
			if (verbose!=0) printf("%s", inet_buf_in);
		}
	
	memset(buf, 0, sizeof(buf)); 	

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds); 
	timeout.tv_sec = 0;
	timeout.tv_usec = to;

	if ((sel = select(fd + 1, &rfds, NULL, NULL, &timeout)) > 0) {
		if (FD_ISSET(fd, &rfds)) {
			len = read(fd, buf, sizeof(buf)); 
		}
	}

	if (strncmp(buf,pre,9)==0) {
		if ((isdigit(buf[9])) && (buf[11]=='1')) {	
				paused = 0;
				replay_speed = 4;
			if (shift_key == 1) {
				if (buf[9]=='1')  strcpy(inet_buf_out,"seek %10\n"); /* 1 */
				if (buf[9]=='2')  strcpy(inet_buf_out,"seek %20\n"); /* 2 */
				if (buf[9]=='3')  strcpy(inet_buf_out,"seek %30\n"); /* 3 */
				if (buf[9]=='4')  strcpy(inet_buf_out,"seek %40\n"); /* 4 */
				if (buf[9]=='5')  strcpy(inet_buf_out,"seek %50\n"); /* 5 */
				if (buf[9]=='6')  strcpy(inet_buf_out,"seek %60\n"); /* 6 */
				if (buf[9]=='7')  strcpy(inet_buf_out,"seek %70\n"); /* 7 */
				if (buf[9]=='8')  strcpy(inet_buf_out,"seek %80\n"); /* 8 */
				if (buf[9]=='9')  strcpy(inet_buf_out,"seek %90\n"); /* 9 */				
				if (buf[9]=='0')  strcpy(inet_buf_out,"seek %0\n"); /* 0 */
			} else {
				if (buf[9]=='1')  strcpy(inet_buf_out,"seek -15\n"); /* 1 */
			/*	if (buf[9]=='2')  strcpy(inet_buf_out,"\n"); */
				if (buf[9]=='3')  strcpy(inet_buf_out,"seek +15\n"); /* 3 */
			
				if (buf[9]=='4')  strcpy(inet_buf_out,"seek -30\n"); /* 4 */
			/*	if (buf[9]=='5')  strcpy(inet_buf_out,"\n");  */
				if (buf[9]=='6')  strcpy(inet_buf_out,"seek +30\n"); /* 6 */
			
				if (buf[9]=='7')  strcpy(inet_buf_out,"seek -60\n"); /* 7 */
			/*	if (buf[9]=='8')  strcpy(inet_buf_out,"\n");  */
				if (buf[9]=='9')  strcpy(inet_buf_out,"seek +60\n"); /* 9 */				
			/*	if (buf[9]=='0')  strcpy(inet_buf_out,"\n");  */
				
			}
		} else if ((buf[9]==':') && (buf[12]=='1')) {
			if (buf[10]=='C')  strcpy(inet_buf_out,"fullscreen\n"); /* Camera button */
			if (buf[10]=='J') { /* Joystick Press */
				if (audio_mute++ == 0) 
					strcpy(inet_buf_out,"set audio mute 1\n");
				else 
					strcpy(inet_buf_out,"set audio mute 0\n"), audio_mute=0;
			};
			if (buf[10]=='O') strcpy(inet_buf_out,"xineramafull\n");/* Internet button */
			if (buf[10]=='R') strcpy(inet_buf_out,"halt\n"); /* Back button */ 
			/* could use stop too so wouldn't kill xine */

		} else if ((buf[9]==0x1b) && (buf[12]=='1')) {
			if (buf[10]==0x14) { /* Joystick up */
				pid_t pid;
				pid = vfork();
				if (pid == -1) {
					printf("Fork error\n");
				} else if (pid == 0) {
					if (execlp("aumix","aumix","-v","+5",(char *)0,"3",(char *)0) == -1) 
						printf("Error: aumix application not installed!\n");
					_exit(1); /* Exit child process in case exec fails */
				} else {
					wait(NULL);	/* Wait for child to exit, don't care about exit status */
					if (verbose!=0) printf("(aumix): Volume up\n");
				}
			}

			if (buf[10]==0x3e) {	/* Right menu button */
				pid_t pid;
				pid = vfork();
				if (pid == -1) {
					printf("Fork error\n");
				} else if (pid == 0) {
					if (execlp("eject","eject","-r",(char *)0,"2",(char *)0) == -1) 
						printf("Error: eject application not installed!\n");
					_exit(1); /* Exit child process in case exec fails */
				} else {
					wait(NULL);	/* Wait for child to exit, don't care about exit status */
					if (verbose!=0) printf("(eject): Eject CD tray\n");
				}
			}

			if (buf[10]=='<') { /* Left menu button */
				pid_t pid;
				pid = vfork();
				if (pid == -1) {
					printf("Fork error\n");
				} else if (pid == 0) {
					if (execlp("eject","eject","-t",(char *)0,"2",(char *)0) == -1) 
						printf("Error: eject application not installed!\n");
					_exit(1); /* Exit child process in case exec fails */
				} else {
					wait(NULL);	/* Wait for child to exit, don't care about exit status */
					if (verbose!=0) printf("(eject): Close CD tray\n");
				}
			}



		} else if (buf[11] == '1') {
			if (buf[9]=='u') { /* Volume up */
				strcpy(inet_buf_out,"stop\n");
			}

			if (buf[9]=='d') { /* Volume down */		
				strcpy(inet_buf_out,"play\n");
			}

			if ((buf[9]=='>')  && (replay_speed != 16)) { /* Joystick right */
				if (replay_speed == 8) 
					strcpy(inet_buf_out,"set speed 16\n"), replay_speed = 16; 
				else {
					strcpy(inet_buf_out,"set speed  \n"), 
					inet_buf_out[10]=48+(replay_speed*=2); 				
				}
				paused=0;
			} else if ((buf[9]=='>')  && (replay_speed == 16) && (paused==1) ) { 
				paused = 0;
				strcpy(inet_buf_out,"set speed 16\n"), replay_speed = 16; 
			}

			if (buf[9]=='v') { /* Joystick down */
				pid_t pid;
				pid = vfork();
				if (pid == -1) {
					printf("Fork error\n");
				} else if (pid == 0) {
					if (execlp("aumix","aumix","-v","-5",(char *)0,"3",(char *)0) == -1) 
						printf("Error: aumix application not installed!\n");
					_exit(1); /* Exit child process in case exec fails */
				} else {
					wait(NULL);	/* Wait for child to exit */
					if (verbose!=0) printf("(aumix): Volume down\n");
				}
			}

			if ((buf[9]=='<') && (replay_speed != 1)) {  /* Joystick left */
				strcpy(inet_buf_out,"set speed  \n");
				inet_buf_out[10] = 48+(replay_speed /= 2);
				paused=0;
			} else if ((buf[9]=='<')  && (replay_speed == 1) && (paused==1) ) { 
				paused = 0;
				strcpy(inet_buf_out,"set speed 1\n"), replay_speed = 1; 
			}
 
			if (buf[9]=='c') { /* Clear button */
				if (paused++ == 0) {
					strcpy(inet_buf_out,"set speed 0\n");
					if (verbose!=0) printf("Pause\n");
				} else {
					paused=0;
					if (replay_speed<10) {				
						strcpy(inet_buf_out,"set speed  \n");
						inet_buf_out[10] = 48+replay_speed;	
					} else {
						strcpy(inet_buf_out,"set speed 16\n");
					}
					if (verbose!=0) printf("Unpause\n");
				}					
			}

			if ((buf[9]=='*') || (buf[9]=='#'))  {  /* *||# */
					shift_key = 1; 
					if (verbose!=0) printf("Shift pressed\n"); 
			}
		} else {
			if ((buf[9]=='*') || (buf[9]=='#'))  {  /* *||# */
				shift_key = 0;
				if (verbose!=0) printf("Shift released\n");
			}
		} 
		memset(buf, 0, sizeof(buf)); 	
	
	}
	
   }
	return -1;
}

static void usage(char* appname)
{
	printf("Xine Bluetooth Remote control for (at least) Sony-Ericsson T610\nby Niko Mauno, 2004\n");
	printf("\nUsage: %s [-c channel] [-v verbose mode] <bdaddr>\n",appname);
	printf("\n Required:\n <bdaddr>  bluetooth address in colon-separated format\n");	
}


void error(void) {
	printf("\nError occurred, exiting...\n");
}


/*
 * main function by Niko Mauno
 */
int main(int argc, char *argv[])
{

	int fd, tmp=0;

	bdaddr_t bdaddr;
	uint8_t channel=1;

	if (argc< 2) usage(argv[0]), exit(-1);
	
	/* begin options parser */
	while ( tmp++ < (argc-2) ) {
		if (strcmp(argv[tmp],"-v") == 0) verbose++;
		else if (strcmp(argv[tmp],"-c") == 0) channel = atoi(argv[(tmp+1)]), tmp++;
		else printf("Unknown option \"%s\"\n",argv[tmp]), exit(-1);
	}
	/* end options parser */

	str2ba(argv[argc-1], &bdaddr);

	if (bacmp(BDADDR_ANY, &bdaddr)) {
		if (verbose!=0) printf("Connecting to %s on channel %d\n", argv[argc-1], channel);
		fd = open_socket(&bdaddr, channel);
	} else {
		if (verbose!=0) printf("Opening device %s\n", argv[argc-1]);
		fd = open_device(argv[1]);
	}


	if (fd < 0) exit(-2);

	if (at_command(fd, "AT\r\n", 10000) != 0) error(), exit(-1);

	/* enable unsolicited result code forwarding and keypad event reporting */
	if (at_command(fd,"AT+CMER=3,2,0,0,0\r\n",10000) != 0) error(), exit(-1);   

	if (at_command(fd,"AT*EAM=\"BRemote\"\r\n",10000) != 0) error(), exit(-1);
	if (at_command(fd,"AT*EASM=\"BRemote\",8,1,1,\"Running\",1\r\n",10000) != 0) error(), exit(-1);



	printf("\nBluetooth Remote control is up & running!\n");

	at_parser(fd,10000);
	
	/* Clean up Bluetooth phone connection*/
	at_command(fd,"AT+CMER=0,0,0,0,0\r\n",10000); 
	close(fd);

  return 0;
}


/* 
Just a list of some SonyEricsson Bluetooth phones...

T68i : BT, IR, SyncML 
T610 : BT, IR, SyncML
Z600 : BT, IR, SyncML
T630 : BT, IR, SyncML 
Z1010 : BT, IR, SyncML
K700 series : Bluetooth, IR, SyncML
S700 series : Bluetooth, IR, SyncML
*/

