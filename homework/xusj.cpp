#include <iostream>
using namespace std;
#include "homework.h"

class x_Arith4 : public Arith4 {
public:
x_Arith4() { cout << "XSJ [C]" << endl; };
x_Arith4(const char *me):Arith4(me) { cout << "XSJ [C]+name=" << me << endl; };
~x_Arith4() { cout << "XSJ [X]" << endl; };

virtual int hw_add(int, int);
virtual int hw_sub(int, int);
virtual int hw_mul(int, int);
virtual int hw_div(int, int);
};

int x_Arith4::hw_add(int a, int b)
{
	return a+b+1;
}

int x_Arith4::hw_sub(int a, int b)
{
	return a-b;
}

int x_Arith4::hw_mul(int a, int b)
{
	return a*b;
}

int x_Arith4::hw_div(int a, int b)
{
	return a/b;
}

static int xsj_hw(void)
{
	cout << "I am XU_SHAOJUN, ";
	cout << "I Commit work\n";

	x_Arith4 *my = new x_Arith4("许少军");
	Commit_hw(my);
	delete my;

	return 0;
}

submit_hw(xsj_hw, 0);
