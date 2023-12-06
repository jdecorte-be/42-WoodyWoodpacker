BITS 64

global _rc4_elf64

section .text

; void rc4(char *bytes, uint64_t length, const char *key, uint32_t keysize)
_rc4_elf64:
	; check args
	cmp rdi, 0 
	je .endfunc
	cmp rsi, 0 
	jng .endfunc
	cmp rdx, 0 
	je .endfunc
	cmp ecx, 0 
	jng .endfunc

	push rbp
	mov rbp, rsp

	; alloc space/save params onto stack
	sub rsp, 8 + 8 + 8 + 4 + 4 + 4 + 256
	mov [rsp], rdi					; bytes
	mov [rsp + 8], rsi				; length
	mov [rsp + 16], rdx				; key
	mov DWORD [rsp + 24], ecx		; keysize

	mov rcx, -1						; rcx = i = -1
	lea r8, [rsp + 36]				; r8 = state

.state_init_loop:
	inc rcx							; ++i
	cmp rcx, 256
	jnl .ready_state_mix

	mov BYTE [r8 + rcx], cl			; state[i] = i

	jmp .state_init_loop

.ready_state_mix:
	mov rcx, -1						; i = -1
	mov r9, [rsp + 16]				; r9 = key
	mov DWORD [rsp + 28], 0			; j = 0

.state_mixing_loop:
	inc rcx							; ++i
	cmp rcx, 256
	jnl .ready_encrypt

	; i % keysize
	push rcx
	mov rax, rcx					; rax = i
	mov ecx, DWORD [rsp + 24 + 8]	; rcx = keysize
	xor rdx, rdx
	div rcx							; rdx = i % keysize
	pop rcx

	; j = (j + state[i] + key[i % keysize]) & 255
	mov eax, DWORD [rsp + 28]		; eax = j
	add al, BYTE [r8 + rcx]			; j += state[i]
	add al, BYTE [r9 + rdx]			; j += key[i % keysize]
	and eax, 255					; j &= 255
	mov DWORD [rsp + 28], eax		; save j

	; ft_swap(&state[i], &state[j])
	xor rdx, rdx
	xor rdi, rdi
	mov dl, BYTE [r8 + rcx]			; dl = state[i]
	mov dil, BYTE [r8 + rax]		; dil = state[j]
	mov BYTE [r8 + rcx], dil		; state[i] = dil
	mov BYTE [r8 + rax], dl			; state[j] = dl

	jmp .state_mixing_loop

.ready_encrypt:
	mov rcx, -1						; i = -1
	mov r9, [rsp]					; r9 = bytes
	mov DWORD [rsp + 28], 0			; j = 0
	mov DWORD [rsp + 32], 0			; k = 0

.encrypt:
	inc rcx							; ++i
	cmp rcx, [rsp + 8]				; while i < length
	jnl .restore_stack

	; k = (k + 1) & 255
	mov eax, DWORD [rsp + 32]		; eax = k
	inc eax							; ++k
	and eax, 255					; k &= 255
	mov DWORD [rsp + 32], eax		; save k

	; j = (j + state[k]) & 255
	mov edx, DWORD [rsp + 28]		; edx = j
	add dl, BYTE [r8 + rax]			; j += state[k]
	and edx, 255					; j &= 255
	mov DWORD [rsp + 28], edx		; save j

	; ft_swap((char *)&state[k], (char *)&state[j])
	xor rdi, rdi
	xor rsi, rsi
	mov dil, BYTE [r8 + rdx]		; dil = state[k]
	mov sil, BYTE [r8 + rax]		; sil = state[j]
	mov BYTE [r8 + rdx], sil		; state[k] = sil
	mov BYTE [r8 + rax], dil		; state[j] = dil

	; int t = (state[k] + state[j]) & 255
	movzx eax, BYTE [r8 + rax]		; eax = t = state[k]
	add al, BYTE [r8 + rdx]			; t += state[j]
	and eax, 255					; t &= 255

	; bytes[i] ^= state[t]
	mov dl, BYTE [r8 + rax]			; dl = state[t]
	xor dl, BYTE [r9 + rcx]			; dl ^= bytes[i]
	mov BYTE [r9 + rcx], dl			; bytes[i] = dl

	jmp .encrypt

.restore_stack:
	add rsp, 8 + 8 + 8 + 4 + 4 + 4 + 256
	pop rbp

.endfunc:
	ret
