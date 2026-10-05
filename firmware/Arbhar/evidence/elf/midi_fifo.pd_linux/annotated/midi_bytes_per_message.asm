00000a24 <midi_bytes_per_message>:
     a24: e20000f0     	and	r0, r0, #240
     a28: e35000b0     	cmp	r0, #176
     a2c: 0a000008     	beq	0xa54 <midi_bytes_per_message+0x30> @ imm = #0x20
     a30: da000009     	ble	0xa5c <midi_bytes_per_message+0x38> @ imm = #0x24
     a34: e35000d0     	cmp	r0, #208
     a38: 0a00000f     	beq	0xa7c <midi_bytes_per_message+0x58> @ imm = #0x3c
     a3c: e35000e0     	cmp	r0, #224
     a40: 0a000003     	beq	0xa54 <midi_bytes_per_message+0x30> @ imm = #0xc
     a44: e35000c0     	cmp	r0, #192
     a48: 03a00002     	moveq	r0, #2
     a4c: 13a00000     	movne	r0, #0
     a50: e12fff1e     	bx	lr
     a54: e3a00003     	mov	r0, #3
     a58: e12fff1e     	bx	lr
     a5c: e3500090     	cmp	r0, #144
     a60: 0afffffb     	beq	0xa54 <midi_bytes_per_message+0x30> @ imm = #-0x14
     a64: e35000a0     	cmp	r0, #160
     a68: 0afffff9     	beq	0xa54 <midi_bytes_per_message+0x30> @ imm = #-0x1c
     a6c: e3500080     	cmp	r0, #128
     a70: 03a00003     	moveq	r0, #3
     a74: 13a00000     	movne	r0, #0
     a78: e12fff1e     	bx	lr
     a7c: e3a00002     	mov	r0, #2
     a80: e12fff1e     	bx	lr

