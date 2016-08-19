#include <iostream>
using namespace std;
#include "homework.h"

class h_Arith4 : public Arith4 {
public:
h_Arith4(const char *me):Arith4(me) {};
~h_Arith4() {};

virtual int hw_add(int, int);
virtual int hw_sub(int, int);
virtual int hw_mul(int, int);
virtual int hw_div(int, int);
};

int h_Arith4::hw_add(int a, int b)
{
    return a+b;
}

int h_Arith4::hw_sub(int a, int b)
{
    return a-b;
}

int h_Arith4::hw_mul(int a, int b)
{
    return a*b;
}

int h_Arith4::hw_div(int a, int b)
{
    return a/b;
}

static int xsj_hw(void)
{
	cout << "I am HU Q S\n";
	h_Arith4 *my = new h_Arith4("胡全胜");
	Commit_hw(my);
	delete my;
	return 0;
}

submit_hw(xsj_hw, 0);
