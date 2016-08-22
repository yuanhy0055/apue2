/
/ get.s: function "__u8 GET(__u32 addr,int dev)"
/
/ the quickest way is writing all in assembler.
/
/ Dirk Pfau 05/2001 dpfau@gmx.de
/
/ This program is under the terms of the BSD License.
/

.text
	.align 4
.globl flash_get
flash_get:
	pushl %esi
	pushl %edi
	movl window_size,%edi
	movl 12(%esp),%esi

	bsfl %edi,%ecx 
	movl %esi,%eax 
	movw map_io,%dx 
	decl %edi 
	andl %edi,%esi 
	shrl %cl,%eax 
	outb %al,%dx 
	addl prom_io,%esi
	orl $-1073741824,%esi
	xorl %eax,%eax
	movb (%esi),%al

	popl %edi
	popl %esi
	ret
