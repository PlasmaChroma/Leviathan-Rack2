; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,manually_confirmed_callback
08032688  4028      cmp	r0, #64
0803268a  09d0      beq	#18 ; -> 0x080326a0 ; branch_target=0x080326a0
0803268c  8028      cmp	r0, #128
0803268e  06d1      bne	#12 ; -> 0x0803269e ; branch_target=0x0803269e
08032690  054a      ldr	r2, [pc, #20] ; [0x080326a8] = 0x20002ee0
08032692  0649      ldr	r1, [pc, #24] ; [0x080326ac] = 0x20002edc
08032694  1368      ldr	r3, [r2]
08032696  0120      movs	r0, #1
08032698  0344      add	r3, r0
0803269a  0860      str	r0, [r1]
0803269c  1360      str	r3, [r2]
0803269e  7047      bx	lr
080326a0  034a      ldr	r2, [pc, #12] ; [0x080326b0] = 0x20002ee8
080326a2  0449      ldr	r1, [pc, #16] ; [0x080326b4] = 0x20002ee4
080326a4  f6e7      b	#-20 ; -> 0x08032694 ; branch_target=0x08032694
