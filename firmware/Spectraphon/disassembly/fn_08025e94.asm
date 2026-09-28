; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
08025e94  10b5      push	{r4, lr}
08025e96  846b      ldr	r4, [r0, #56]
08025e98  fcf792fe  bl	#-13020 ; -> 0x08022bc0 ; branch_target=0x08022bc0
08025e9c  0228      cmp	r0, #2
08025e9e  25d0      beq	#74 ; -> 0x08025eec ; branch_target=0x08025eec
08025ea0  d4f89430  ldr.w	r3, [r4, #148]
08025ea4  2168      ldr	r1, [r4]
08025ea6  43f08003  orr	r3, r3, #128
08025eaa  1448      ldr	r0, [pc, #80] ; [0x08025efc] = 0x20000014
08025eac  144a      ldr	r2, [pc, #80] ; [0x08025f00] = 0x95cbec1b
08025eae  c4f89430  str.w	r3, [r4, #148]
08025eb2  0b68      ldr	r3, [r1]
08025eb4  23f40033  bic	r3, r3, #131072
08025eb8  0b60      str	r3, [r1]
08025eba  0368      ldr	r3, [r0]
08025ebc  2168      ldr	r1, [r4]
08025ebe  a2fb0323  umull	r2, r3, r2, r3
08025ec2  0a68      ldr	r2, [r1]
08025ec4  1b0b      lsrs	r3, r3, #12
08025ec6  22f48032  bic	r2, r2, #65536
08025eca  9b00      lsls	r3, r3, #2
08025ecc  0a60      str	r2, [r1]
08025ece  73b1      cbz	r3, #28 ; -> 0x08025eee ; branch_target=0x08025eee
08025ed0  2268      ldr	r2, [r4]
08025ed2  013b      subs	r3, #1
08025ed4  1268      ldr	r2, [r2]
08025ed6  d203      lsls	r2, r2, #15
08025ed8  f9d4      bmi	#-14 ; -> 0x08025ece ; branch_target=0x08025ece
08025eda  0122      movs	r2, #1
08025edc  0023      movs	r3, #0
08025ede  2046      mov	r0, r4
08025ee0  84f89120  strb.w	r2, [r4, #145]
08025ee4  a4f87e30  strh.w	r3, [r4, #126]
08025ee8  0cf0c0fb  bl	#51072 ; -> 0x0803266c ; branch_target=0x0803266c
08025eec  10bd      pop	{r4, pc}
08025eee  d4f89430  ldr.w	r3, [r4, #148]
08025ef2  43f04003  orr	r3, r3, #64
08025ef6  c4f89430  str.w	r3, [r4, #148]
08025efa  eee7      b	#-36 ; -> 0x08025eda ; branch_target=0x08025eda
