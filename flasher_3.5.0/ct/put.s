/
/ put.s: function "void PUT(__u8 byte,__u32 addr,int dev)"
/
/ the quickest way is writing all in assembler.
/
/ Dirk Pfau 05/2001 dpfau@gmx.de
/
/ This program is under the terms of the BSD License.
/
/

.text
	.align 4
.globl flash_put
flash_put:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl window_size,%edi
	movl 16(%esp),%ebx
	movl 20(%esp),%esi

	bsfl %edi,%ecx 
	movl %esi,%eax 
	movw map_io,%dx 
	decl %edi 
	andl %edi,%esi 
	shrl %cl,%eax 
	outb %al,%dx 
	addl prom_io,%esi 
	orl $-1073741824,%esi 
	movb %bl,(%esi) 
	
	popl %ebx
	popl %esi
	popl %edi
	ret
