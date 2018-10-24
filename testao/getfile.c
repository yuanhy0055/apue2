#include <stdio.h>
#include <dirent.h>

//#define TEST_SELF

int getf(const char *wdir) {
	DIR *dir;
	struct dirent *ptr;
	dir = opendir(wdir);
	while ((ptr = readdir(dir)) != NULL) {
		printf("D_name: %s\n", ptr->d_name);
	}

	return 0;
}

#ifdef TEST_SELF
int main(void)
{
	static char *film[128];
	return getf("/home/nico/SDD");
}
#endif
