; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020324  10b5      push	{r4, lr}
08020326  0320      movs	r0, #3
08020328  124c      ldr	r4, [pc, #72] ; [0x08020374] = 0x20000010
0802032a  01f0c7f8  bl	#4494 ; -> 0x080214bc ; branch_target=0x080214bc
0802032e  03f0b5fc  bl	#14698 ; -> 0x08023c9c ; branch_target=0x08023c9c
08020332  114b      ldr	r3, [pc, #68] ; [0x08020378] = 0x58024400
08020334  1149      ldr	r1, [pc, #68] ; [0x0802037c] = 0x08049aac
08020336  9a69      ldr	r2, [r3, #24]
08020338  9b69      ldr	r3, [r3, #24]
0802033a  c2f30322  ubfx	r2, r2, #8, #4
0802033e  03f00f03  and	r3, r3, #15
08020342  8a5c      ldrb	r2, [r1, r2]
08020344  cb5c      ldrb	r3, [r1, r3]
08020346  02f01f02  and	r2, r2, #31
0802034a  0d49      ldr	r1, [pc, #52] ; [0x08020380] = 0x20000014
0802034c  03f01f03  and	r3, r3, #31
08020350  d040      lsrs	r0, r2
08020352  20fa03f3  lsr.w	r3, r0, r3
08020356  0860      str	r0, [r1]
08020358  0020      movs	r0, #0
0802035a  2360      str	r3, [r4]
0802035c  fff7bcff  bl	#-136 ; -> 0x080202d8 ; branch_target=0x080202d8
08020360  10b1      cbz	r0, #4 ; -> 0x08020368 ; branch_target=0x08020368
08020362  0124      movs	r4, #1
08020364  2046      mov	r0, r4
08020366  10bd      pop	{r4, pc}
08020368  0446      mov	r4, r0
0802036a  15f05ffb  bl	#87742 ; -> 0x08035a2c ; branch_target=0x08035a2c
0802036e  2046      mov	r0, r4
08020370  10bd      pop	{r4, pc}
