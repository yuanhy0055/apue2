#include <stdio.h>
#include <stdlib.h>

static char code[8] = {
	0x55, 			/* push   %ebp */
	0x89, 0xe5, 	/* mov    %esp, %ebp */
	0xc9, 			/* leave */
	0xc3			/* ret */
};

typedef int (*intf)();

int main()
{
	intf ret;

	ret =code;
 
	printf("* (funcall (curry #'+ 1) 1)\n"
	       "%d\n", (*ret)());

	return 0;
}

