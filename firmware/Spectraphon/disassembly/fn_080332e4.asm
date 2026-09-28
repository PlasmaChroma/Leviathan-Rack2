; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080332e4  f8b5      push	{r3, r4, r5, r6, r7, lr}
080332e6  304a      ldr	r2, [pc, #192] ; [0x080333a8] = 0xe000ed00
080332e8  304b      ldr	r3, [pc, #192] ; [0x080333ac] = 0x081e0000
080332ea  314d      ldr	r5, [pc, #196] ; [0x080333b0] = 0x20002ebc
080332ec  9268      ldr	r2, [r2, #8]
080332ee  1c46      mov	r4, r3
080332f0  304e      ldr	r6, [pc, #192] ; [0x080333b4] = 0x2001346c
080332f2  314f      ldr	r7, [pc, #196] ; [0x080333b8] = 0x081f0000
080332f4  2b60      str	r3, [r5]
080332f6  04e0      b	#8 ; -> 0x08033302 ; branch_target=0x08033302
080332f8  03f0f2f8  bl	#12772 ; -> 0x080364e0 ; branch_target=0x080364e0
080332fc  bc42      cmp	r4, r7
080332fe  2c60      str	r4, [r5]
08033300  30d0      beq	#96 ; -> 0x08033364 ; branch_target=0x08033364
08033302  2368      ldr	r3, [r4]
08033304  2146      mov	r1, r4
08033306  4ff48072  mov.w	r2, #256
0803330a  2034      adds	r4, #32
0803330c  0133      adds	r3, #1
0803330e  3046      mov	r0, r6
08033310  f2d1      bne	#-28 ; -> 0x080332f8 ; branch_target=0x080332f8
08033312  284a      ldr	r2, [pc, #160] ; [0x080333b4] = 0x2001346c
08033314  294b      ldr	r3, [pc, #164] ; [0x080333bc] = 0x2001348c
08033316  d2e90014  ldrd	r1, r4, [r2]
0803331a  c1f30340  ubfx	r0, r1, #16, #4
0803331e  a5b2      uxth	r5, r4
08033320  2414      asrs	r4, r4, #16
08033322  1880      strh	r0, [r3]
08033324  01f00f00  and	r0, r1, #15
08033328  1c61      str	r4, [r3, #16]
0803332a  5880      strh	r0, [r3, #2]
0803332c  0815      asrs	r0, r1, #20
0803332e  c1f30311  ubfx	r1, r1, #4, #4
08033332  9d60      str	r5, [r3, #8]
08033334  9880      strh	r0, [r3, #4]
08033336  9068      ldr	r0, [r2, #8]
08033338  d980      strh	r1, [r3, #6]
0803333a  0414      asrs	r4, r0, #16
0803333c  80b2      uxth	r0, r0
0803333e  5c61      str	r4, [r3, #20]
08033340  d860      str	r0, [r3, #12]
08033342  d2e90301  ldrd	r0, r1, [r2, #12]
08033346  84b2      uxth	r4, r0
08033348  0014      asrs	r0, r0, #16
0803334a  9c61      str	r4, [r3, #24]
0803334c  8cb2      uxth	r4, r1
0803334e  0914      asrs	r1, r1, #16
08033350  1862      str	r0, [r3, #32]
08033352  9069      ldr	r0, [r2, #24]
08033354  5962      str	r1, [r3, #36]
08033356  d169      ldr	r1, [r2, #28]
08033358  5269      ldr	r2, [r2, #20]
0803335a  d862      str	r0, [r3, #44]
0803335c  dc61      str	r4, [r3, #28]
0803335e  1963      str	r1, [r3, #48]
08033360  9a62      str	r2, [r3, #40]
08033362  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08033364  eff790fd  bl	#-66784 ; -> 0x08022e88 ; branch_target=0x08022e88
08033368  18b9      cbnz	r0, #6 ; -> 0x08033372 ; branch_target=0x08033372
0803336a  154b      ldr	r3, [pc, #84] ; [0x080333c0] = 0x081effff
0803336c  2a68      ldr	r2, [r5]
0803336e  9a42      cmp	r2, r3
08033370  f7dd      ble	#-18 ; -> 0x08033362 ; branch_target=0x08033362
08033372  124b      ldr	r3, [pc, #72] ; [0x080333bc] = 0x2001348c
08033374  0022      movs	r2, #0
08033376  4ff4fa71  mov.w	r1, #500
0803337a  4ff00110  mov.w	r0, #65537
0803337e  1a63      str	r2, [r3, #48]
08033380  1860      str	r0, [r3]
08033382  c3e90222  strd	r2, r2, [r3, #8]
08033386  c3e90422  strd	r2, r2, [r3, #16]
0803338a  c3e90611  strd	r1, r1, [r3, #24]
0803338e  c3e90822  strd	r2, r2, [r3, #32]
08033392  c3e90a22  strd	r2, r2, [r3, #40]
08033396  f9f757ff  bl	#-24914 ; -> 0x0802d248 ; branch_target=0x0802d248
0803339a  044b      ldr	r3, [pc, #16] ; [0x080333ac] = 0x081e0000
0803339c  2b60      str	r3, [r5]
0803339e  bde8f840  pop.w	{r3, r4, r5, r6, r7, lr}
080333a2  f9f76dbf  b.w	#-24870 ; -> 0x0802d280 ; branch_target=0x0802d280
