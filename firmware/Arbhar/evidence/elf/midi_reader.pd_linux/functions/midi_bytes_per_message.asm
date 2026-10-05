00000bc0 <midi_bytes_per_message>:
     bc0: e20000f0     	and	r0, r0, #240
     bc4: e35000b0     	cmp	r0, #176
     bc8: 0a000008     	beq	0xbf0 <midi_bytes_per_message+0x30> @ imm = #0x20
     bcc: da000009     	ble	0xbf8 <midi_bytes_per_message+0x38> @ imm = #0x24
     bd0: e35000d0     	cmp	r0, #208
     bd4: 0a00000f     	beq	0xc18 <midi_bytes_per_message+0x58> @ imm = #0x3c
     bd8: e35000e0     	cmp	r0, #224
     bdc: 0a000003     	beq	0xbf0 <midi_bytes_per_message+0x30> @ imm = #0xc
     be0: e35000c0     	cmp	r0, #192
     be4: 03a00002     	moveq	r0, #2
     be8: 13a00000     	movne	r0, #0
     bec: e12fff1e     	bx	lr
     bf0: e3a00003     	mov	r0, #3
     bf4: e12fff1e     	bx	lr
     bf8: e3500090     	cmp	r0, #144
     bfc: 0afffffb     	beq	0xbf0 <midi_bytes_per_message+0x30> @ imm = #-0x14
     c00: e35000a0     	cmp	r0, #160
     c04: 0afffff9     	beq	0xbf0 <midi_bytes_per_message+0x30> @ imm = #-0x1c
     c08: e3500080     	cmp	r0, #128
     c0c: 03a00003     	moveq	r0, #3
     c10: 13a00000     	movne	r0, #0
     c14: e12fff1e     	bx	lr
     c18: e3a00002     	mov	r0, #2
     c1c: e12fff1e     	bx	lr

