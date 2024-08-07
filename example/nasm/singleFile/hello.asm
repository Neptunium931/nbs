.text
global _start

_start:
	mov eax, 4
	mov ebx, 1
	mov ecx, hello
	mov edx, helloLen
	int 0x80

quit:
	mov eax, 1
	xor ebx, ebx
	int 0x80

	.data

hello:
	db "Hello, World!", 10

helloLen:
	equ $ - hello
