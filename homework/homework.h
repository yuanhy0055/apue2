#ifndef _HOMEWORK_INIT_H
#define _HOMEWORK_INIT_H

#include <string.h>

//#define __used		__attribute__((used))
#define __used		__attribute__((__used__))

typedef int (*HWmain_t)(void);

#define submit_hw(fn,id) \
    static HWmain_t __hw_##fn##id __used \
    __attribute__((__section__("__hwkmains"))) = fn

//$$$ Day 1
class Arith4 {
private:
	char name[64];
public:
	Arith4(){ /*cout << "F class [C]" << endl;*/ };
	Arith4(const char *me){ strncpy(name, me, 64); };
	~Arith4(){ /*cout << "F class [X]" << endl;*/ };

	char* getname(void) { return name; };

	virtual int hw_add(int, int){ return 0; };
	virtual int hw_sub(int, int){ return 0; };
	virtual int hw_mul(int, int){ return 0; };
	virtual int hw_div(int, int){ return 0; };

	//virtual int hw_add(int, int) = 0;
	//virtual int hw_sub(int, int) = 0;
	//virtual int hw_mul(int, int) = 0;
	//virtual int hw_div(int, int) = 0;
};

int Commit_hw(Arith4*);

#endif
