BITS 64

global _start

section .text

_start:
	push rax
	push rdx
	push rdi
	push rsi

	mov rax, 1
	mov rdi, rax
	lea rsi, [rel mystr]
	mov rdx, 14
	syscall

	pop rsi
	pop rdi
	pop rdx
	pop rax

	ret

mystr: db	"....WOODY....",10,0
