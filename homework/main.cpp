#include <iostream>
using namespace std;

#include "homework.h"

//extern char __hwkmains_start;
//extern char __bss_start;
HWmain_t *xxxx;	// = (HWmain_t *)0x8050000;	//__segment_begin("hwkmains");

extern long __start___hwkmains[];
extern long __stop___hwkmains[];

#define SECTION_SIZE(sect) ((size_t)((__stop_##sect - __start_##sect)))

static Arith4 hw_ofall[100];
static unsigned cnt = 0 ;
static int save_hw(Arith4* itt)
{
	//cout << "save this ONE" << endl;
	if(cnt==100) {
		cout << "At most teacher have 100-Stu" << endl;
		return -1;
	}

	hw_ofall[cnt++] = *itt;
	return 0;
};

int Commit_hw(Arith4* one4)
{
	cout << one4->getname() << ">Commit home_work!" << endl;
	//cout << "ADD:" << one4->hw_add(23, 45) << endl;
	cout << "***CHECK ADD: 23+45=" << (one4->hw_add(23,45)==(23+45)?"OK":"error") << endl;
	return save_hw(one4);
}
 
int main(void)
{
	unsigned i;
	//cout << (unsigned long)&__start___hwkmains;
	xxxx = (HWmain_t *)__start___hwkmains;
	//cout << hex << static_cast<const void *>(xxxx) << endl;

	size_t sz = SECTION_SIZE(__hwkmains);
    for (i=0; i < sz; i++) {
        xxxx[i]();
    }

	cout << "=================================" << endl;
	cout << "Now, Teacher Cheching HomeWork..." << endl;
	for(i=0;i<cnt;i++) {
		cout << hw_ofall[i].getname() << endl;
		cout << "***CHECK ADD: 23+45=" << hw_ofall[i].hw_add(23,45) << endl;
	}

	return 0;
}
