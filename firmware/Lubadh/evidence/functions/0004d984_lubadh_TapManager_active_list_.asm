; lubadh::TapManager::active_list()
; VA 0x4d984 size 384

   4d984: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   4d988: e2805eab     	add	r5, r0, #2736
   4d98c: e1a04000     	mov	r4, r0
   4d990: ed2d8b04     	vpush	{d8, d9}
   4d994: e2807eaa     	add	r7, r0, #2720
   4d998: f2808050     	vmov.i32	q4, #0x0
   4d99c: e1a06005     	mov	r6, r5
   4d9a0: e4973004     	ldr	r3, [r7], #4
   4d9a4: e3a02014     	mov	r2, #20
   4d9a8: e1a00006     	mov	r0, r6
   4d9ac: e2831fa5     	add	r1, r3, #660
   4d9b0: e3530000     	cmp	r3, #0
   4d9b4: 0a00002f     	beq	0x4da78
   4d9b8: ebff2009     	bl	0x159e4    @ imm = #-0x37fdc ; memmove
   4d9bc: e2866014     	add	r6, r6, #20
   4d9c0: e1570005     	cmp	r7, r5
   4d9c4: 1afffff5     	bne	0x4d9a0
   4d9c8: e2841c0b     	add	r1, r4, #2816
   4d9cc: e0413005     	sub	r3, r1, r5
   4d9d0: e1a02243     	asr	r2, r3, #4
   4d9d4: e1a03143     	asr	r3, r3, #2
   4d9d8: e3520000     	cmp	r2, #0
   4d9dc: da000046     	ble	0x4dafc
   4d9e0: e28220ab     	add	r2, r2, #171
   4d9e4: e1a00005     	mov	r0, r5
   4d9e8: e0842202     	add	r2, r4, r2, lsl #4
   4d9ec: ea00000b     	b	0x4da20
   4d9f0: e5903004     	ldr	r3, [r0, #0x4]
   4d9f4: e3530000     	cmp	r3, #0
   4d9f8: 0a000022     	beq	0x4da88
   4d9fc: e5903008     	ldr	r3, [r0, #0x8]
   4da00: e3530000     	cmp	r3, #0
   4da04: 0a000021     	beq	0x4da90
   4da08: e590300c     	ldr	r3, [r0, #0xc]
   4da0c: e3530000     	cmp	r3, #0
   4da10: 0a000020     	beq	0x4da98
   4da14: e2800010     	add	r0, r0, #16
   4da18: e1520000     	cmp	r2, r0
   4da1c: 0a00001f     	beq	0x4daa0
   4da20: e5903000     	ldr	r3, [r0]
   4da24: e3530000     	cmp	r3, #0
   4da28: 1afffff0     	bne	0x4d9f0
   4da2c: e1510000     	cmp	r1, r0
   4da30: 0a00000d     	beq	0x4da6c
   4da34: e2803004     	add	r3, r0, #4
   4da38: e1510003     	cmp	r1, r3
   4da3c: 0a000006     	beq	0x4da5c
   4da40: e4932004     	ldr	r2, [r3], #4
   4da44: e3520000     	cmp	r2, #0
   4da48: 14802004     	strne	r2, [r0], #4
   4da4c: e1510003     	cmp	r1, r3
   4da50: 1afffffa     	bne	0x4da40
   4da54: e1510000     	cmp	r1, r0
   4da58: 0a000003     	beq	0x4da6c
   4da5c: e0442000     	sub	r2, r4, r0
   4da60: e3a01000     	mov	r1, #0
   4da64: e2822c0b     	add	r2, r2, #2816
   4da68: ebff20c1     	bl	0x15d74    @ imm = #-0x37cfc ; memset
   4da6c: ecbd8b04     	vpop	{d8, d9}
   4da70: e1a00005     	mov	r0, r5
   4da74: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4da78: e286300c     	add	r3, r6, #12
   4da7c: f4068a0f     	vst1.8	{d8, d9}, [r6]
   4da80: f403870f     	vst1.8	{d8}, [r3]
   4da84: eaffffcc     	b	0x4d9bc
   4da88: e2800004     	add	r0, r0, #4
   4da8c: eaffffe6     	b	0x4da2c
   4da90: e2800008     	add	r0, r0, #8
   4da94: eaffffe4     	b	0x4da2c
   4da98: e280000c     	add	r0, r0, #12
   4da9c: eaffffe2     	b	0x4da2c
   4daa0: e0413000     	sub	r3, r1, r0
   4daa4: e1a03143     	asr	r3, r3, #2
   4daa8: e3530002     	cmp	r3, #2
   4daac: 0a00000d     	beq	0x4dae8
   4dab0: e3530003     	cmp	r3, #3
   4dab4: 0a000007     	beq	0x4dad8
   4dab8: e3530001     	cmp	r3, #1
   4dabc: 1affffea     	bne	0x4da6c
   4dac0: e5903000     	ldr	r3, [r0]
   4dac4: e3530000     	cmp	r3, #0
   4dac8: 0affffd7     	beq	0x4da2c
   4dacc: ecbd8b04     	vpop	{d8, d9}
   4dad0: e1a00005     	mov	r0, r5
   4dad4: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4dad8: e5903000     	ldr	r3, [r0]
   4dadc: e3530000     	cmp	r3, #0
   4dae0: 0affffd1     	beq	0x4da2c
   4dae4: e2800004     	add	r0, r0, #4
   4dae8: e5903000     	ldr	r3, [r0]
   4daec: e3530000     	cmp	r3, #0
   4daf0: 0affffcd     	beq	0x4da2c
   4daf4: e2800004     	add	r0, r0, #4
   4daf8: eafffff0     	b	0x4dac0
   4dafc: e1a00005     	mov	r0, r5
   4db00: eaffffe8     	b	0x4daa8
