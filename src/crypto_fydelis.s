; crypto_fydelis.s - Rotinas nativas x86_64
; Compativel: NASM / MinGW / GCC
; Sintaxe: Intel

section .text
bits 64

global fydelis_xor_block
global fydelis_hash_bloco

;---------------------------------------------------
; void fydelis_xor_block(unsigned char* dados,
;                        long tamanho,
;                        const unsigned char* chave)
; rdi = dados, rsi = tamanho, rdx = chave
;---------------------------------------------------
fydelis_xor_block:
    cmp     rsi, 0
    jz      .fim

    xor     rcx, rcx
.loop:
    cmp     rcx, rsi
    jge     .fim

    movzx   r8b, byte [rdx + rcx]
    xor     [rdi + rcx], r8b

    inc     rcx
    jmp     .loop

.fim:
    ret

;---------------------------------------------------
; uint64_t fydelis_hash_bloco(const unsigned char* entrada,
;                             unsigned char* saida,
;                             long tamanho)
; rdi = entrada, rsi = saida, rdx = tamanho
; Retorna hash em rax, grava em *saida se nao nulo
;---------------------------------------------------
fydelis_hash_bloco:
    push    rbx
    xor     rax, rax
    mov     rcx, rdx

.hash_loop:
    cmp     rcx, 0
    jle     .hash_fim

    rol     rax, 5
    movzx   rbx, byte [rdi + rcx - 1]
    xor     rax, rbx
    add     rax, 0x5F2D3C4E
    dec     rcx
    jmp     .hash_loop

.hash_fim:
    pop     rbx
    cmp     rsi, 0
    jz      .nao_gravar
    mov     [rsi], rax

.nao_gravar:
    ret