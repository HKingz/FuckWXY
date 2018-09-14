EXTERN g_dwJmp:DQ;
EXTERN ShowProtobuf:PROC;
EXTERN g_strProtobuf:DQ;
EXTERN g_len:DQ;
.CODE


PUSHAQ MACRO
	push	rcx
    push    rax
    push    rdx
    push    rbx
    push    rbp
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
ENDM


POPAQ MACRO
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    pop    rdi
    pop    rsi
    pop    rbp
    pop    rbx
    pop    rdx
    pop    rax
	pop    rcx
ENDM





hookSend PROC

	


 PUSHAQ
 pushfq
 mov r8d, [r9+10000h]
 sub r8d,eax
 mov rcx,r8
 lea rdx,[r9+rax]
 call ShowProtobuf
 test al, al
 je  exit
 popfq
 POPAQ
mov r8,g_len
lea rdx, g_strProtobuf
jmp g_dwJmp 
 exit:
 popfq
 POPAQ
mov r8d, [r9+10000h]
sub r8d, eax
lea rdx, [r9+rax]
jmp g_dwJmp 

hookSend ENDP

END
