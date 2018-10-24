#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <poll.h>

#define LCPLAYER "mplayer"
static const char *usrname = "nico";
static int player_stat = 0;	//0: stop, 1: playing, 2: pausing
static int vtfd;

static char *film[128];
static int nobo = 0;
static char plcmd[256];

static char conbuf[512];
static int con_len;

static FILE *ferr;

__attribute__((constructor)) void before_main() {
	printf(" ***DDD*** before main\n");
	film[0] = "/SDD/0419_150157.3gp";
	film[1] = "/SDD/TEST/test1.mp4";
	film[2] = "/SDD/TEST/test2.mp4";
	film[3] = "/SDD/FEN_FANG/78871_25.mp4";
	film[4] = "/SDD/FEN_FANG/78871_22.mp4";
	film[5] = "/SDD/FEN_FANG/78871_26.mp4";
	
	ferr = fopen("/run/shm/Ch.log", "a");
}

__attribute__((destructor)) void after_main() {
	printf(" ***DDD*** after main\n");
}

static void out_msg(int fd) {
	char buf[4096];
	int nread;
	int i;
	if ((nread = read(fd, buf, 4096)) > 0) {
		for (i = 0; i < nread; i++) {
			putchar(buf[i]);
		}
	}
}

int start_play() {
	int fd;
	printf("do play a one\n");

	int k;
	
	pid_t pid = forkpty(&fd, NULL, NULL, NULL);
	if (pid == -1) {
		perror("forkpty");
		return -1;
	} else if (pid == 0) {
		snprintf(plcmd, 256, LCPLAYER " %s", film[nobo]);
		fprintf(ferr, "Cplay %s\n", plcmd);fflush(ferr);
		if (execlp("/bin/su", "--login", usrname, "-c", plcmd, (void*) 0)
				== -1) {
			perror("execlp");
		}
		fprintf(stderr, "program exited.\n");
		return -1;
	}

	printf("I am father, play pid=%d\n", pid);
	do {
		con_len = read(fd, conbuf, 512);
		if (con_len > 0) {
			for (k = 0; k < con_len; k++) {
				fprintf(ferr, "%c", conbuf[k]);
			}
		} else {
			fprintf(ferr, "-----OVER\n");
			break;
		}
	} while (1);
	fflush(ferr);

	vtfd = fd;
	player_stat = 1;
	printf("Return to Listern\n");
	return fd;
}

int process_cmd(char *cmd, int len) {
	int nw;

	if (0 == strncmp(cmd, "^play^", 6)) {
		printf("<PLAY>\n");
		if (0 == player_stat) {
			vtfd = start_play();
			if (vtfd > 0) {
				player_stat = 1;
			}
		}
	} else if (0 == strncmp(cmd, "^stop^", 6)) {
		printf("<STOP>%d\n", vtfd);
		if (vtfd > 0) {
			nw = write(vtfd, "q", 1);
			printf("nw=%d\n", nw);
			player_stat = 0;
		}
	} else if (0 == strncmp(cmd, "^paus^", 6)) {
		printf("<PAUS>\n");
		if (vtfd > 0) {
			do {
				nw = write(vtfd, " ", 1);
				player_stat = 2;
			} while (nw != 1);
		}
	} else if (0 == strncmp(cmd, "^next^", 6)) {
		printf("<NEXT>\n");
		if (vtfd > 0) {
			do {
				nw = write(vtfd, "q", 1);
			} while (nw != 1);
			close(vtfd);
			nobo++;
			vtfd = start_play();
		}
	} else if (0 == strncmp(cmd, "^stat^", 6)) {
		printf("<STAT>\n");
	}

	return 0;
}
