BITS 32

global _start

section .text

_start:
	push eax
	push ebx
	push ecx
	push edx

	;
	; write(1, mystr, 14);
	;

	mov eax, 4	; SYS_WRITE
	mov ebx, 1	; fd = 1
	mov edx, 14	; mystr len

	call $ + 5	; get mystr rel addr
	pop ecx		;
	add ecx, 11	;

	int 0x80	; exec syscall

	pop edx
	pop ecx
	pop ebx
	pop eax

	ret

mystr: db	"....WOODY....",10,0
