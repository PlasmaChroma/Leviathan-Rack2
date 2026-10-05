000061c8 <checkForCalibrationModeOnStartup>:
    61c8: e92d4010     	push	{r4, lr}
    61cc: e1a04000     	mov	r4, r0
    61d0: e3a0002a     	mov	r0, #42
    61d4: ebfff56b     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x2a54  // CALL bcm2835_gpio_lev
    61d8: e3500000     	cmp	r0, #0
    61dc: 0a000001     	beq	0x61e8 <checkForCalibrationModeOnStartup+0x20> @ imm = #0x4
    61e0: e3a00000     	mov	r0, #0
    61e4: e8bd8010     	pop	{r4, pc}
    61e8: e3a00026     	mov	r0, #38
    61ec: ebfff565     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x2a6c  // CALL bcm2835_gpio_lev
    61f0: e3500000     	cmp	r0, #0
    61f4: 1afffff9     	bne	0x61e0 <checkForCalibrationModeOnStartup+0x18> @ imm = #-0x1c
    61f8: e3a00021     	mov	r0, #33
    61fc: ebfff561     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x2a7c  // CALL bcm2835_gpio_lev
    6200: e3500000     	cmp	r0, #0
    6204: 1afffff5     	bne	0x61e0 <checkForCalibrationModeOnStartup+0x18> @ imm = #-0x2c
    6208: e3a03063     	mov	r3, #99
    620c: e3a00001     	mov	r0, #1
    6210: e5c43030     	strb	r3, [r4, #0x30]
    6214: e8bd8010     	pop	{r4, pc}

