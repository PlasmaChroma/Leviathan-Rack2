; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08023c9c  3f4a      ldr	r2, [pc, #252] ; [0x08023d9c] = 0x58024400
08023c9e  1369      ldr	r3, [r2, #16]
08023ca0  03f03803  and	r3, r3, #56
08023ca4  102b      cmp	r3, #16
08023ca6  04d0      beq	#8 ; -> 0x08023cb2 ; branch_target=0x08023cb2
08023ca8  182b      cmp	r3, #24
08023caa  0dd0      beq	#26 ; -> 0x08023cc8 ; branch_target=0x08023cc8
08023cac  1bb1      cbz	r3, #6 ; -> 0x08023cb6 ; branch_target=0x08023cb6
08023cae  3c48      ldr	r0, [pc, #240] ; [0x08023da0] = 0x003d0900
08023cb0  7047      bx	lr
08023cb2  3c48      ldr	r0, [pc, #240] ; [0x08023da4] = 0x017d7840
08023cb4  7047      bx	lr
08023cb6  1368      ldr	r3, [r2]
08023cb8  9906      lsls	r1, r3, #26
08023cba  48d5      bpl	#144 ; -> 0x08023d4e ; branch_target=0x08023d4e
08023cbc  1368      ldr	r3, [r2]
08023cbe  3a48      ldr	r0, [pc, #232] ; [0x08023da8] = 0x03d09000
08023cc0  c3f3c103  ubfx	r3, r3, #3, #2
08023cc4  d840      lsrs	r0, r3
08023cc6  7047      bx	lr
08023cc8  30b4      push	{r4, r5}
08023cca  936a      ldr	r3, [r2, #40]
08023ccc  946a      ldr	r4, [r2, #40]
08023cce  d56a      ldr	r5, [r2, #44]
08023cd0  14f47c7f  tst.w	r4, #1008
08023cd4  516b      ldr	r1, [r2, #52]
08023cd6  c4f30510  ubfx	r0, r4, #4, #6
08023cda  36d0      beq	#108 ; -> 0x08023d4a ; branch_target=0x08023d4a
08023cdc  c1f3cc01  ubfx	r1, r1, #3, #13
08023ce0  05f00105  and	r5, r5, #1
08023ce4  03f00303  and	r3, r3, #3
08023ce8  07ee900a  vmov	s15, r0
08023cec  05fb01f1  mul	r1, r5, r1
08023cf0  012b      cmp	r3, #1
08023cf2  f8eee77a  vcvt.f32.s32	s15, s15
08023cf6  06ee901a  vmov	s13, r1
08023cfa  faeee96a  vcvt.f32.s32	s13, s13, #13
08023cfe  02d0      beq	#4 ; -> 0x08023d06 ; branch_target=0x08023d06
08023d00  022b      cmp	r3, #2
08023d02  26d0      beq	#76 ; -> 0x08023d52 ; branch_target=0x08023d52
08023d04  43b3      cbz	r3, #80 ; -> 0x08023d58 ; branch_target=0x08023d58
08023d06  9fed297a  vldr	s14, [pc, #164] ; [0x08023dac] = 0x4a742400 / f32_bits_interpretation=4000000
08023d0a  87ee276a  vdiv.f32	s12, s14, s15
08023d0e  136b      ldr	r3, [r2, #48]
08023d10  c3f30803  ubfx	r3, r3, #0, #9
08023d14  07ee103a  vmov	s14, r3
08023d18  f7ee005a  vmov.f32	s11, #1.000000e+00
08023d1c  b8eec77a  vcvt.f32.s32	s14, s14
08023d20  37ee257a  vadd.f32	s14, s14, s11
08023d24  37ee267a  vadd.f32	s14, s14, s13
08023d28  27ee067a  vmul.f32	s14, s14, s12
08023d2c  1b4b      ldr	r3, [pc, #108] ; [0x08023d9c] = 0x58024400
08023d2e  1b6b      ldr	r3, [r3, #48]
08023d30  c3f34623  ubfx	r3, r3, #9, #7
08023d34  0133      adds	r3, #1
08023d36  07ee903a  vmov	s15, r3
08023d3a  f8eee76a  vcvt.f32.s32	s13, s15
08023d3e  c7ee267a  vdiv.f32	s15, s14, s13
08023d42  fceee77a  vcvt.u32.f32	s15, s15
08023d46  17ee900a  vmov	r0, s15
08023d4a  30bc      pop	{r4, r5}
08023d4c  7047      bx	lr
08023d4e  1648      ldr	r0, [pc, #88] ; [0x08023da8] = 0x03d09000
08023d50  7047      bx	lr
08023d52  9fed177a  vldr	s14, [pc, #92] ; [0x08023db0] = 0x4bbebc20 / f32_bits_interpretation=25000000
08023d56  d8e7      b	#-80 ; -> 0x08023d0a ; branch_target=0x08023d0a
08023d58  1368      ldr	r3, [r2]
08023d5a  9b06      lsls	r3, r3, #26
08023d5c  1ad5      bpl	#52 ; -> 0x08023d94 ; branch_target=0x08023d94
08023d5e  1068      ldr	r0, [r2]
08023d60  b7ee007a  vmov.f32	s14, #1.000000e+00
08023d64  136b      ldr	r3, [r2, #48]
08023d66  1049      ldr	r1, [pc, #64] ; [0x08023da8] = 0x03d09000
08023d68  c0f3c102  ubfx	r2, r0, #3, #2
08023d6c  c3f30803  ubfx	r3, r3, #0, #9
08023d70  d140      lsrs	r1, r2
08023d72  06ee103a  vmov	s12, r3
08023d76  05ee901a  vmov	s11, r1
08023d7a  b8eec66a  vcvt.f32.s32	s12, s12
08023d7e  f8eee55a  vcvt.f32.s32	s11, s11
08023d82  36ee076a  vadd.f32	s12, s12, s14
08023d86  85eea77a  vdiv.f32	s14, s11, s15
08023d8a  76ee267a  vadd.f32	s15, s12, s13
08023d8e  27ee277a  vmul.f32	s14, s14, s15
08023d92  cbe7      b	#-106 ; -> 0x08023d2c ; branch_target=0x08023d2c
08023d94  9fed077a  vldr	s14, [pc, #28] ; [0x08023db4] = 0x4c742400 / f32_bits_interpretation=64000000
08023d98  b7e7      b	#-146 ; -> 0x08023d0a ; branch_target=0x08023d0a
