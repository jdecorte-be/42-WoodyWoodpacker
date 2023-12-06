BITS 32

global _rc4_elf32

section .text

; void rc4(char *bytes, uint32_t length, const char *key, uint32_t keysize)
_rc4_elf32:
	push ebp
	mov ebp, esp

	cmp DWORD [ebp + 8], 0		; bytes
	je .endfunc
	cmp DWORD [ebp + 12], 0		; length
	je .endfunc
	cmp DWORD [ebp + 16], 0		; key
	je .endfunc
	cmp DWORD [ebp + 20], 0		; keysize
	je .endfunc

	sub esp, 4 + 4 + 256		; j + k + state

	mov ecx, -1					; ecx = i = -1
	lea edi, [esp + 8]			; edi = state

.state_init_loop:
	inc ecx						; ++i
	cmp ecx, 256
	je .ready_state_mix

	mov BYTE [edi + ecx], cl	; state[i] = i

	jmp .state_init_loop

.ready_state_mix:
	mov ecx, -1					; i = -1
	mov esi, [ebp + 16]			; esi = key
	mov DWORD [esp], 0			; j = 0

.state_mixing_loop:
	inc ecx						; ++i
	cmp ecx, 256
	je .ready_encrypt

	; i % keysize
	push ecx
	mov eax, ecx				; eax = i
	mov ecx, [ebp + 20]			; ecx = keysize
	xor edx, edx
	div ecx						; edx = i % keysize
	pop ecx						; ecx = i

	; j = (j + state[i] + key[i % keysize]) & 255
	mov eax, [esp]				; eax = j
	add al, BYTE [edi + ecx]	; j += state[i]
	add al, BYTE [esi + edx]	; j += key[i % keysize]
	and eax, 255				; j &= 255
	mov DWORD [esp], eax		; save j

	; ft_swap(&state[i], &state[j])
	xor edx, edx
	mov dl, BYTE [edi + ecx]	; dl = state[i]
	mov dh, BYTE [edi + eax]	; dh = state[j]
	mov BYTE [edi + ecx], dh	; state[i] = dh
	mov BYTE [edi + eax], dl	; state[j] = dl

	jmp .state_mixing_loop

.ready_encrypt:
	mov ecx, -1					; i = -1
	mov esi, [ebp + 8]			; esi = bytes
	mov DWORD [esp], 0			; j = 0
	mov DWORD [esp + 4], 0		; k = 0

.encrypt:
	inc ecx						; ++i
	cmp ecx, [ebp + 12]			; while i < length
	je .restore_stack

	; k = (k + 1) & 255
	mov eax, [esp + 4]			; eax = k
	inc eax						; ++k
	and eax, 255				; k &= 255
	mov DWORD [esp + 4], eax	; save k

	; j = (j + state[k]) & 255
	mov edx, [esp]				; edx = j
	add dl, BYTE [edi + eax]	; j += state[k]
	and edx, 255				; j &= 255
	mov DWORD [esp], edx		; save j

	; ft_swap((char *)&state[k], (char *)&state[j])
	push ecx

	xor ecx, ecx
	mov cl, BYTE [edi + edx]	; cl = state[k]
	mov ch, BYTE [edi + eax]	; ch = state[j]
	mov BYTE [edi + edx], ch	; state[k] = ch
	mov BYTE [edi + eax], cl	; state[j] = cl

	pop ecx

	; int t = (state[k] + state[j]) & 255
	movzx eax, BYTE [edi + eax]	; eax = t = state[k]
	add al, BYTE [edi + edx]	; t += state[j]
	and eax, 255				; t &= 255

	; bytes[i] ^= state[t]
	mov dl, BYTE [edi + eax]	; dl = state[t]
	xor dl, BYTE [esi + ecx]	; dl ^= bytes[i]
	mov BYTE [esi + ecx], dl	; bytes[i] = dl

	jmp .encrypt

.restore_stack:
	add esp, 4 + 4 + 256

.endfunc:
	pop ebp
	ret
