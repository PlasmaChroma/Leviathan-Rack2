; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,vector
08036430  dff834d0  ldr.w	sp, [pc, #52] ; [0x08036468] = 0x20020000
08036434  0021      movs	r1, #0
08036436  03e0      b	#6 ; -> 0x08036440 ; branch_target=0x08036440
08036438  0c4b      ldr	r3, [pc, #48] ; [0x0803646c] = 0x08049ac8
0803643a  5b58      ldr	r3, [r3, r1]
0803643c  4350      str	r3, [r0, r1]
0803643e  0431      adds	r1, #4
08036440  0b48      ldr	r0, [pc, #44] ; [0x08036470] = 0x20000000
08036442  0c4b      ldr	r3, [pc, #48] ; [0x08036474] = 0x2000001c
08036444  4218      adds	r2, r0, r1
08036446  9a42      cmp	r2, r3
08036448  f6d3      blo	#-20 ; -> 0x08036438 ; branch_target=0x08036438
0803644a  0b4a      ldr	r2, [pc, #44] ; [0x08036478] = 0x20002030
0803644c  02e0      b	#4 ; -> 0x08036454 ; branch_target=0x08036454
0803644e  0023      movs	r3, #0
08036450  42f8043b  str	r3, [r2], #4
08036454  094b      ldr	r3, [pc, #36] ; [0x0803647c] = 0x20014fa4
08036456  9a42      cmp	r2, r3
08036458  f9d3      blo	#-14 ; -> 0x0803644e ; branch_target=0x0803644e
0803645a  fff771fb  bl	#-2334 ; -> 0x08035b40 ; branch_target=0x08035b40
0803645e  00f019f8  bl	#50 ; -> 0x08036494 ; branch_target=0x08036494
08036462  fdf7fdfd  bl	#-9222 ; -> 0x08034060 ; branch_target=0x08034060
08036466  7047      bx	lr
