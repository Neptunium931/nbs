.intel_syntax noprefix
.global _start

_start:
	mov rax, 1
	mov rdi, 1
	mov rsi, offset hello_string
	mov rdx, 14
	syscall

quit:
	mov rax, 60
	mov rdi, 0
	syscall

hello_string:
	.string "Hello, World!\n"
