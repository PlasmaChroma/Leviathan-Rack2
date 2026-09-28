; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08032bc0  10b5      push	{r4, lr}
08032bc2  4360      str	r3, [r0, #4]
08032bc4  88b0      sub	sp, #32
08032bc6  c361      str	r3, [r0, #28]
08032bc8  8362      str	r3, [r0, #40]
08032bca  0423      movs	r3, #4
08032bcc  0392      str	r2, [sp, #12]
08032bce  174c      ldr	r4, [pc, #92] ; [0x08032c2c] = 0x52004140
08032bd0  0091      str	r1, [sp]
08032bd2  0691      str	r1, [sp, #24]
08032bd4  6946      mov	r1, sp
08032bd6  0460      str	r4, [r0]
08032bd8  cde90123  strd	r2, r3, [sp, #4]
08032bdc  0222      movs	r2, #2
08032bde  0223      movs	r3, #2
08032be0  cde90423  strd	r2, r3, [sp, #16]
08032be4  0122      movs	r2, #1
08032be6  0823      movs	r3, #8
08032be8  c0e90223  strd	r2, r3, [r0, #8]
08032bec  1022      movs	r2, #16
08032bee  4023      movs	r3, #64
08032bf0  c0e90423  strd	r2, r3, [r0, #16]
08032bf4  4ff44063  mov.w	r3, #3072
08032bf8  4ff48072  mov.w	r2, #256
08032bfc  0362      str	r3, [r0, #32]
08032bfe  4ff48053  mov.w	r3, #4096
08032c02  8261      str	r2, [r0, #24]
08032c04  4362      str	r3, [r0, #36]
08032c06  f3f7dbff  bl	#-49226 ; -> 0x08026bc0 ; branch_target=0x08026bc0
08032c0a  28b9      cbnz	r0, #10 ; -> 0x08032c18 ; branch_target=0x08032c18
08032c0c  4ff08070  mov.w	r0, #16777216
08032c10  edf7e2fb  bl	#-75836 ; -> 0x080203d8 ; branch_target=0x080203d8
08032c14  08b0      add	sp, #32
08032c16  10bd      pop	{r4, pc}
08032c18  02f072fa  bl	#9444 ; -> 0x08035100 ; branch_target=0x08035100
08032c1c  4ff08070  mov.w	r0, #16777216
08032c20  edf7dafb  bl	#-75852 ; -> 0x080203d8 ; branch_target=0x080203d8
08032c24  08b0      add	sp, #32
08032c26  10bd      pop	{r4, pc}
