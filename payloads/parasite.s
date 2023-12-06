BITS 64

global _start

section .text

_start:

	jmp parasite
	mystr: db	"....WOODY....",10

parasite:

	push rax
	push rcx
	push rdx
	push rsi
	push rdi
	push r11

	mov rax, 1
	mov rdi, rax
	lea rsi, [rel mystr]
	mov rdx, 14
	syscall

	pop r11
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rax

	ret
