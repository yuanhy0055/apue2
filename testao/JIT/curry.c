#include <stdio.h>
#include <stdlib.h>

static char code[18] = {
	0x55, 			/* push   %ebp */
	0x89, 0xe5, 		/* mov    %esp, %ebp */
	0xff, 0x75, 0x08, 	/* pushl  0x8(%ebp) */
	0x6a, 0,		/* push   $0x0 */
	0xe8, 0, 0, 0, 0, 	/* call <code+13> */
	0x83, 0xc4, 0x08, 	/* add $0x8, %esp */
	0xc9, 			/* leave */
	0xc3			/* ret */
};

typedef int (*intf)();

int (*curry(intf Func, int arg))()
{
	*(char*)(code + 7) = (char) arg;
	*(int*)(code + 9) = (int) Func - ((int) code + 9 + 4);
	return ((intf) code);
}

static int add(int x, int y) { return (x + y); }
static int sub(int x, int y) { return (x - y); }

int main()
{
	intf ret;

	ret = curry(add, 1);
	printf("* (funcall (curry #'+ 1) 1)\n"
	       "%d\n", (*ret)(1));

	ret = curry(sub, 2);
	printf("* (funcall (curry #'- 2) 3)\n"
	       "%d\n", (*ret)(3));
	return 0;
}

