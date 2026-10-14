; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080215dc  4378      ldrb	r3, [r0, #1]
080215de  164a      ldr	r2, [pc, #88] ; [0x08021638] = 0xe000ed00
080215e0  c2f89830  str.w	r3, [r2, #152]
080215e4  0378      ldrb	r3, [r0]
080215e6  13b3      cbz	r3, #68 ; -> 0x0802162e ; branch_target=0x0802162e
080215e8  4368      ldr	r3, [r0, #4]
080215ea  c2f89c30  str.w	r3, [r2, #156]
080215ee  c37a      ldrb	r3, [r0, #11]
080215f0  90f80cc0  ldrb.w	r12, [r0, #12]
080215f4  1b06      lsls	r3, r3, #24
080215f6  0178      ldrb	r1, [r0]
080215f8  43ea0c73  orr.w	r3, r3, r12, lsl #28
080215fc  90f80ac0  ldrb.w	r12, [r0, #10]
08021600  0b43      orrs	r3, r1
08021602  417b      ldrb	r1, [r0, #13]
08021604  43eacc43  orr.w	r3, r3, r12, lsl #19
08021608  90f80ec0  ldrb.w	r12, [r0, #14]
0802160c  43ea8143  orr.w	r3, r3, r1, lsl #18
08021610  c17b      ldrb	r1, [r0, #15]
08021612  43ea4c43  orr.w	r3, r3, r12, lsl #17
08021616  90f809c0  ldrb.w	r12, [r0, #9]
0802161a  43ea0143  orr.w	r3, r3, r1, lsl #16
0802161e  017a      ldrb	r1, [r0, #8]
08021620  43ea0c23  orr.w	r3, r3, r12, lsl #8
08021624  43ea4103  orr.w	r3, r3, r1, lsl #1
08021628  c2f8a030  str.w	r3, [r2, #160]
0802162c  7047      bx	lr
0802162e  c2f89c30  str.w	r3, [r2, #156]
08021632  c2f8a030  str.w	r3, [r2, #160]
08021636  7047      bx	lr
