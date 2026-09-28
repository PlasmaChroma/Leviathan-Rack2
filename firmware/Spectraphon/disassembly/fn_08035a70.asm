; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,vector
08020384  034a      ldr	r2, [pc, #12] ; [0x08020394] = 0x2000204c
08020386  044b      ldr	r3, [pc, #16] ; [0x08020398] = 0x20000000
08020388  1168      ldr	r1, [r2]
0802038a  1b78      ldrb	r3, [r3]
0802038c  0b44      add	r3, r1
0802038e  1360      str	r3, [r2]
08020390  7047      bx	lr
08035a70  eaf788bc  b.w	#-87792 ; -> 0x08020384 ; branch_target=0x08020384
