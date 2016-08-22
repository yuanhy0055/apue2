#define SETUPSECS 4
#define SYSSEG 		0x1000		/* system loaded at 0x10000(64K) */
#define PREFIXSIZE ((SETUPSECS+1)*512)
#boot sect 512

.text
.code16
/*.arch i386 */
.org	0
/*.section	".prefix", "ax", @progbits */

.org	497
.byte	SETUPSECS

.org	510
.word	0x55AA

#setup part
/*
	We're now at the beginning of the second sector of the image -
	where the setup code goes.

	This code gets loaded at SETUPSEG:0.  It wants to start
	executing the Etherboot image that's loaded at SYSSEG:0 and
	whose entry point is SYSSEG:0.
*/
setup_code:

	mov		$0, %si
	mov		$0xb800, %bx
	mov		%bx, %fs
	mov		$0, %ax
	mov		$80, %cx
loop1:
	movb	$'v', %fs:(%si)
	inc		%si
	loop	loop1

	ljmp	$(SYSSEG-(PREFIXSIZE/16)), $_start

/*	.previous */

	.org	(PREFIXSIZE-4)
	.byte	'Y', 'U', 'A', 'N'
