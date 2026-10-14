00007b74 <arbhar_play_tilde_setup>:
    7b74: e59f05c0     	ldr	r0, [pc, #0x5c0]        @ 0x813c <arbhar_play_tilde_setup+0x5c8>
    7b78: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    7b7c: e08f0000     	add	r0, pc, r0
    7b80: e24dd014     	sub	sp, sp, #20
    7b84: e59f75b4     	ldr	r7, [pc, #0x5b4]        @ 0x8140 <arbhar_play_tilde_setup+0x5cc>
    7b88: ebffea19     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x579c
    7b8c: e59f25b0     	ldr	r2, [pc, #0x5b0]        @ 0x8144 <arbhar_play_tilde_setup+0x5d0>
    7b90: e59f15b0     	ldr	r1, [pc, #0x5b0]        @ 0x8148 <arbhar_play_tilde_setup+0x5d4>
    7b94: e08f7007     	add	r7, pc, r7
    7b98: e3a04000     	mov	r4, #0
    7b9c: e3a0800a     	mov	r8, #10
    7ba0: e7972002     	ldr	r2, [r7, r2]
    7ba4: e30238fc     	movw	r3, #0x28fc
    7ba8: e7971001     	ldr	r1, [r7, r1]
    7bac: e3a06001     	mov	r6, #1
    7bb0: e58d4008     	str	r4, [sp, #0x8]
    7bb4: e88d0110     	stm	sp, {r4, r8}
    7bb8: ebffeab8     	bl	0x26a0 <.plt+0x2c0>     @ imm = #-0x5520
    7bbc: e59f5588     	ldr	r5, [pc, #0x588]        @ 0x814c <arbhar_play_tilde_setup+0x5d8>
    7bc0: e59f3588     	ldr	r3, [pc, #0x588]        @ 0x8150 <arbhar_play_tilde_setup+0x5dc>
    7bc4: e08f5005     	add	r5, pc, r5
    7bc8: e1a09000     	mov	r9, r0
    7bcc: e08f0003     	add	r0, pc, r3
    7bd0: e5859020     	str	r9, [r5, #0x20]
    7bd4: ebffea06     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x57e8
    7bd8: e59fc574     	ldr	r12, [pc, #0x574]       @ 0x8154 <arbhar_play_tilde_setup+0x5e0>
    7bdc: e3a0300b     	mov	r3, #11
    7be0: e797100c     	ldr	r1, [r7, r12]
    7be4: e58d4000     	str	r4, [sp]
    7be8: e1a02000     	mov	r2, r0
    7bec: e1a00009     	mov	r0, r9
    7bf0: ebffeab6     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5528
    7bf4: e59f055c     	ldr	r0, [pc, #0x55c]        @ 0x8158 <arbhar_play_tilde_setup+0x5e4>
    7bf8: e5959020     	ldr	r9, [r5, #0x20]
    7bfc: e08f0000     	add	r0, pc, r0
    7c00: ebffe9fb     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5814
    7c04: e59f2550     	ldr	r2, [pc, #0x550]        @ 0x815c <arbhar_play_tilde_setup+0x5e8>
    7c08: e1a03006     	mov	r3, r6
    7c0c: e58d4000     	str	r4, [sp]
    7c10: e08f1002     	add	r1, pc, r2
    7c14: e1a02000     	mov	r2, r0
    7c18: e1a00009     	mov	r0, r9
    7c1c: ebffeaab     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5554
    7c20: e59f1538     	ldr	r1, [pc, #0x538]        @ 0x8160 <arbhar_play_tilde_setup+0x5ec>
    7c24: e5959020     	ldr	r9, [r5, #0x20]
    7c28: e08f0001     	add	r0, pc, r1
    7c2c: ebffe9f0     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5840
    7c30: e59fc52c     	ldr	r12, [pc, #0x52c]       @ 0x8164 <arbhar_play_tilde_setup+0x5f0>
    7c34: e58d4000     	str	r4, [sp]
    7c38: e1a03006     	mov	r3, r6
    7c3c: e08f100c     	add	r1, pc, r12
    7c40: e1a02000     	mov	r2, r0
    7c44: e1a00009     	mov	r0, r9
    7c48: ebffeaa0     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5580
    7c4c: e59f3514     	ldr	r3, [pc, #0x514]        @ 0x8168 <arbhar_play_tilde_setup+0x5f4>
    7c50: e5959020     	ldr	r9, [r5, #0x20]
    7c54: e08f0003     	add	r0, pc, r3
    7c58: ebffe9e5     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x586c
    7c5c: e59f2508     	ldr	r2, [pc, #0x508]        @ 0x816c <arbhar_play_tilde_setup+0x5f8>
    7c60: e1a03006     	mov	r3, r6
    7c64: e58d4000     	str	r4, [sp]
    7c68: e08f1002     	add	r1, pc, r2
    7c6c: e1a02000     	mov	r2, r0
    7c70: e1a00009     	mov	r0, r9
    7c74: ebffea95     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x55ac
    7c78: e59f04f0     	ldr	r0, [pc, #0x4f0]        @ 0x8170 <arbhar_play_tilde_setup+0x5fc>
    7c7c: e5959020     	ldr	r9, [r5, #0x20]
    7c80: e08f0000     	add	r0, pc, r0
    7c84: ebffe9da     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5898
    7c88: e59f14e4     	ldr	r1, [pc, #0x4e4]        @ 0x8174 <arbhar_play_tilde_setup+0x600>
    7c8c: e1a03006     	mov	r3, r6
    7c90: e58d4000     	str	r4, [sp]
    7c94: e08f1001     	add	r1, pc, r1
    7c98: e1a02000     	mov	r2, r0
    7c9c: e1a00009     	mov	r0, r9
    7ca0: ebffea8a     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x55d8
    7ca4: e59fc4cc     	ldr	r12, [pc, #0x4cc]       @ 0x8178 <arbhar_play_tilde_setup+0x604>
    7ca8: e5959020     	ldr	r9, [r5, #0x20]
    7cac: e08f000c     	add	r0, pc, r12
    7cb0: ebffe9cf     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x58c4
    7cb4: e59f24c0     	ldr	r2, [pc, #0x4c0]        @ 0x817c <arbhar_play_tilde_setup+0x608>
    7cb8: e58d4000     	str	r4, [sp]
    7cbc: e1a03006     	mov	r3, r6
    7cc0: e08f1002     	add	r1, pc, r2
    7cc4: e1a02000     	mov	r2, r0
    7cc8: e1a00009     	mov	r0, r9
    7ccc: ebffea7f     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5604
    7cd0: e59f34a8     	ldr	r3, [pc, #0x4a8]        @ 0x8180 <arbhar_play_tilde_setup+0x60c>
    7cd4: e5959020     	ldr	r9, [r5, #0x20]
    7cd8: e08f0003     	add	r0, pc, r3
    7cdc: ebffe9c4     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x58f0
    7ce0: e59f149c     	ldr	r1, [pc, #0x49c]        @ 0x8184 <arbhar_play_tilde_setup+0x610>
    7ce4: e1a03006     	mov	r3, r6
    7ce8: e58d4000     	str	r4, [sp]
    7cec: e08f1001     	add	r1, pc, r1
    7cf0: e1a02000     	mov	r2, r0
    7cf4: e1a00009     	mov	r0, r9
    7cf8: ebffea74     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5630
    7cfc: e59f0484     	ldr	r0, [pc, #0x484]        @ 0x8188 <arbhar_play_tilde_setup+0x614>
    7d00: e5959020     	ldr	r9, [r5, #0x20]
    7d04: e08f0000     	add	r0, pc, r0
    7d08: ebffe9b9     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x591c
    7d0c: e59fc478     	ldr	r12, [pc, #0x478]       @ 0x818c <arbhar_play_tilde_setup+0x618>
    7d10: e1a03006     	mov	r3, r6
    7d14: e58d4000     	str	r4, [sp]
    7d18: e08f100c     	add	r1, pc, r12
    7d1c: e1a02000     	mov	r2, r0
    7d20: e1a00009     	mov	r0, r9
    7d24: ebffea69     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x565c
    7d28: e59f2460     	ldr	r2, [pc, #0x460]        @ 0x8190 <arbhar_play_tilde_setup+0x61c>
    7d2c: e5959020     	ldr	r9, [r5, #0x20]
    7d30: e08f0002     	add	r0, pc, r2
    7d34: ebffe9ae     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5948
    7d38: e59f1454     	ldr	r1, [pc, #0x454]        @ 0x8194 <arbhar_play_tilde_setup+0x620>
    7d3c: e1a03004     	mov	r3, r4
    7d40: e08f1001     	add	r1, pc, r1
    7d44: e1a02000     	mov	r2, r0
    7d48: e1a00009     	mov	r0, r9
    7d4c: ebffea5f     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5684
    7d50: e59f3440     	ldr	r3, [pc, #0x440]        @ 0x8198 <arbhar_play_tilde_setup+0x624>
    7d54: e5959020     	ldr	r9, [r5, #0x20]
    7d58: e08f0003     	add	r0, pc, r3
    7d5c: ebffe9a4     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5970
    7d60: e59fc434     	ldr	r12, [pc, #0x434]       @ 0x819c <arbhar_play_tilde_setup+0x628>
    7d64: e1a03006     	mov	r3, r6
    7d68: e58d4004     	str	r4, [sp, #0x4]
    7d6c: e08f100c     	add	r1, pc, r12
    7d70: e58d6000     	str	r6, [sp]
    7d74: e1a02000     	mov	r2, r0
    7d78: e1a00009     	mov	r0, r9
    7d7c: ebffea53     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x56b4
    7d80: e59f0418     	ldr	r0, [pc, #0x418]        @ 0x81a0 <arbhar_play_tilde_setup+0x62c>
    7d84: e5959020     	ldr	r9, [r5, #0x20]
    7d88: e08f0000     	add	r0, pc, r0
    7d8c: ebffe998     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x59a0
    7d90: e59f140c     	ldr	r1, [pc, #0x40c]        @ 0x81a4 <arbhar_play_tilde_setup+0x630>
    7d94: e1a03004     	mov	r3, r4
    7d98: e08f1001     	add	r1, pc, r1
    7d9c: e1a02000     	mov	r2, r0
    7da0: e1a00009     	mov	r0, r9
    7da4: ebffea49     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x56dc
    7da8: e59f23f8     	ldr	r2, [pc, #0x3f8]        @ 0x81a8 <arbhar_play_tilde_setup+0x634>
    7dac: e5959020     	ldr	r9, [r5, #0x20]
    7db0: e08f0002     	add	r0, pc, r2
    7db4: ebffe98e     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x59c8
    7db8: e59fc3ec     	ldr	r12, [pc, #0x3ec]       @ 0x81ac <arbhar_play_tilde_setup+0x638>
    7dbc: e58d4000     	str	r4, [sp]
    7dc0: e1a03006     	mov	r3, r6
    7dc4: e08f100c     	add	r1, pc, r12
    7dc8: e1a02000     	mov	r2, r0
    7dcc: e1a00009     	mov	r0, r9
    7dd0: ebffea3e     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5708
    7dd4: e59f33d4     	ldr	r3, [pc, #0x3d4]        @ 0x81b0 <arbhar_play_tilde_setup+0x63c>
    7dd8: e5959020     	ldr	r9, [r5, #0x20]
    7ddc: e08f0003     	add	r0, pc, r3
    7de0: ebffe983     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x59f4
    7de4: e59f13c8     	ldr	r1, [pc, #0x3c8]        @ 0x81b4 <arbhar_play_tilde_setup+0x640>
    7de8: e1a03006     	mov	r3, r6
    7dec: e58d4000     	str	r4, [sp]
    7df0: e08f1001     	add	r1, pc, r1
    7df4: e1a02000     	mov	r2, r0
    7df8: e1a00009     	mov	r0, r9
    7dfc: ebffea33     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5734
    7e00: e59f03b0     	ldr	r0, [pc, #0x3b0]        @ 0x81b8 <arbhar_play_tilde_setup+0x644>
    7e04: e5959020     	ldr	r9, [r5, #0x20]
    7e08: e08f0000     	add	r0, pc, r0
    7e0c: ebffe978     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5a20
    7e10: e59f23a4     	ldr	r2, [pc, #0x3a4]        @ 0x81bc <arbhar_play_tilde_setup+0x648>
    7e14: e1a03006     	mov	r3, r6
    7e18: e58d4000     	str	r4, [sp]
    7e1c: e08f1002     	add	r1, pc, r2
    7e20: e1a02000     	mov	r2, r0
    7e24: e1a00009     	mov	r0, r9
    7e28: ebffea28     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5760
    7e2c: e59fc38c     	ldr	r12, [pc, #0x38c]       @ 0x81c0 <arbhar_play_tilde_setup+0x64c>
    7e30: e5959020     	ldr	r9, [r5, #0x20]
    7e34: e08f000c     	add	r0, pc, r12
    7e38: ebffe96d     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5a4c
    7e3c: e59f1380     	ldr	r1, [pc, #0x380]        @ 0x81c4 <arbhar_play_tilde_setup+0x650>
    7e40: e58d4000     	str	r4, [sp]
    7e44: e1a03006     	mov	r3, r6
    7e48: e08f1001     	add	r1, pc, r1
    7e4c: e1a02000     	mov	r2, r0
    7e50: e1a00009     	mov	r0, r9
    7e54: ebffea1d     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x578c
    7e58: e59f3368     	ldr	r3, [pc, #0x368]        @ 0x81c8 <arbhar_play_tilde_setup+0x654>
    7e5c: e5959020     	ldr	r9, [r5, #0x20]
    7e60: e08f0003     	add	r0, pc, r3
    7e64: ebffe962     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5a78
    7e68: e59f235c     	ldr	r2, [pc, #0x35c]        @ 0x81cc <arbhar_play_tilde_setup+0x658>
    7e6c: e1a03006     	mov	r3, r6
    7e70: e58d4000     	str	r4, [sp]
    7e74: e08f1002     	add	r1, pc, r2
    7e78: e1a02000     	mov	r2, r0
    7e7c: e1a00009     	mov	r0, r9
    7e80: ebffea12     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x57b8
    7e84: e59f0344     	ldr	r0, [pc, #0x344]        @ 0x81d0 <arbhar_play_tilde_setup+0x65c>
    7e88: e5959020     	ldr	r9, [r5, #0x20]
    7e8c: e08f0000     	add	r0, pc, r0
    7e90: ebffe957     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5aa4
    7e94: e59fc338     	ldr	r12, [pc, #0x338]       @ 0x81d4 <arbhar_play_tilde_setup+0x660>
    7e98: e1a03006     	mov	r3, r6
    7e9c: e58d4000     	str	r4, [sp]
    7ea0: e08f100c     	add	r1, pc, r12
    7ea4: e1a02000     	mov	r2, r0
    7ea8: e1a00009     	mov	r0, r9
    7eac: ebffea07     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x57e4
    7eb0: e59f1320     	ldr	r1, [pc, #0x320]        @ 0x81d8 <arbhar_play_tilde_setup+0x664>
    7eb4: e5959020     	ldr	r9, [r5, #0x20]
    7eb8: e08f0001     	add	r0, pc, r1
    7ebc: ebffe94c     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5ad0
    7ec0: e59f2314     	ldr	r2, [pc, #0x314]        @ 0x81dc <arbhar_play_tilde_setup+0x668>
    7ec4: e58d4004     	str	r4, [sp, #0x4]
    7ec8: e1a03006     	mov	r3, r6
    7ecc: e08f1002     	add	r1, pc, r2
    7ed0: e58d6000     	str	r6, [sp]
    7ed4: e1a02000     	mov	r2, r0
    7ed8: e1a00009     	mov	r0, r9
    7edc: ebffe9fb     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5814
    7ee0: e59f32f8     	ldr	r3, [pc, #0x2f8]        @ 0x81e0 <arbhar_play_tilde_setup+0x66c>
    7ee4: e5959020     	ldr	r9, [r5, #0x20]
    7ee8: e08f0003     	add	r0, pc, r3
    7eec: ebffe940     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5b00
    7ef0: e59fc2ec     	ldr	r12, [pc, #0x2ec]       @ 0x81e4 <arbhar_play_tilde_setup+0x670>
    7ef4: e1a03006     	mov	r3, r6
    7ef8: e58d4000     	str	r4, [sp]
    7efc: e08f100c     	add	r1, pc, r12
    7f00: e1a02000     	mov	r2, r0
    7f04: e1a00009     	mov	r0, r9
    7f08: ebffe9f0     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5840
    7f0c: e59f02d4     	ldr	r0, [pc, #0x2d4]        @ 0x81e8 <arbhar_play_tilde_setup+0x674>
    7f10: e5959020     	ldr	r9, [r5, #0x20]
    7f14: e08f0000     	add	r0, pc, r0
    7f18: ebffe935     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5b2c
    7f1c: e59f12c8     	ldr	r1, [pc, #0x2c8]        @ 0x81ec <arbhar_play_tilde_setup+0x678>
    7f20: e1a03006     	mov	r3, r6
    7f24: e58d4000     	str	r4, [sp]
    7f28: e08f1001     	add	r1, pc, r1
    7f2c: e1a02000     	mov	r2, r0
    7f30: e1a00009     	mov	r0, r9
    7f34: ebffe9e5     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x586c
    7f38: e59f22b0     	ldr	r2, [pc, #0x2b0]        @ 0x81f0 <arbhar_play_tilde_setup+0x67c>
    7f3c: e5959020     	ldr	r9, [r5, #0x20]
    7f40: e08f0002     	add	r0, pc, r2
    7f44: ebffe92a     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5b58
    7f48: e59fc2a4     	ldr	r12, [pc, #0x2a4]       @ 0x81f4 <arbhar_play_tilde_setup+0x680>
    7f4c: e58d4000     	str	r4, [sp]
    7f50: e1a03006     	mov	r3, r6
    7f54: e08f100c     	add	r1, pc, r12
    7f58: e1a02000     	mov	r2, r0
    7f5c: e1a00009     	mov	r0, r9
    7f60: ebffe9da     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5898
    7f64: e59f328c     	ldr	r3, [pc, #0x28c]        @ 0x81f8 <arbhar_play_tilde_setup+0x684>
    7f68: e5959020     	ldr	r9, [r5, #0x20]
    7f6c: e08f0003     	add	r0, pc, r3
    7f70: ebffe91f     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5b84
    7f74: e59f1280     	ldr	r1, [pc, #0x280]        @ 0x81fc <arbhar_play_tilde_setup+0x688>
    7f78: e1a03006     	mov	r3, r6
    7f7c: e58d4000     	str	r4, [sp]
    7f80: e08f1001     	add	r1, pc, r1
    7f84: e1a02000     	mov	r2, r0
    7f88: e1a00009     	mov	r0, r9
    7f8c: ebffe9cf     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x58c4
    7f90: e59f0268     	ldr	r0, [pc, #0x268]        @ 0x8200 <arbhar_play_tilde_setup+0x68c>
    7f94: e5959020     	ldr	r9, [r5, #0x20]
    7f98: e08f0000     	add	r0, pc, r0
    7f9c: ebffe914     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5bb0
    7fa0: e59f225c     	ldr	r2, [pc, #0x25c]        @ 0x8204 <arbhar_play_tilde_setup+0x690>
    7fa4: e1a03006     	mov	r3, r6
    7fa8: e58d4000     	str	r4, [sp]
    7fac: e08f1002     	add	r1, pc, r2
    7fb0: e1a02000     	mov	r2, r0
    7fb4: e1a00009     	mov	r0, r9
    7fb8: ebffe9c4     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x58f0
    7fbc: e59fc244     	ldr	r12, [pc, #0x244]       @ 0x8208 <arbhar_play_tilde_setup+0x694>
    7fc0: e5959020     	ldr	r9, [r5, #0x20]
    7fc4: e08f000c     	add	r0, pc, r12
    7fc8: ebffe909     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5bdc
    7fcc: e59f1238     	ldr	r1, [pc, #0x238]        @ 0x820c <arbhar_play_tilde_setup+0x698>
    7fd0: e58d4000     	str	r4, [sp]
    7fd4: e1a03008     	mov	r3, r8
    7fd8: e08f1001     	add	r1, pc, r1
    7fdc: e1a02000     	mov	r2, r0
    7fe0: e1a00009     	mov	r0, r9
    7fe4: ebffe9b9     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x591c
    7fe8: e59f3220     	ldr	r3, [pc, #0x220]        @ 0x8210 <arbhar_play_tilde_setup+0x69c>
    7fec: e5959020     	ldr	r9, [r5, #0x20]
    7ff0: e08f0003     	add	r0, pc, r3
    7ff4: ebffe8fe     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5c08
    7ff8: e59f2214     	ldr	r2, [pc, #0x214]        @ 0x8214 <arbhar_play_tilde_setup+0x6a0>
    7ffc: e1a03006     	mov	r3, r6
    8000: e58d4008     	str	r4, [sp, #0x8]
    8004: e08f1002     	add	r1, pc, r2
    8008: e58d6004     	str	r6, [sp, #0x4]
    800c: e58d6000     	str	r6, [sp]
    8010: e1a02000     	mov	r2, r0
    8014: e1a00009     	mov	r0, r9
    8018: ebffe9ac     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5950
    801c: e59f01f4     	ldr	r0, [pc, #0x1f4]        @ 0x8218 <arbhar_play_tilde_setup+0x6a4>
    8020: e5959020     	ldr	r9, [r5, #0x20]
    8024: e08f0000     	add	r0, pc, r0
    8028: ebffe8f1     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5c3c
    802c: e59fc1e8     	ldr	r12, [pc, #0x1e8]       @ 0x821c <arbhar_play_tilde_setup+0x6a8>
    8030: e1a03006     	mov	r3, r6
    8034: e58d4008     	str	r4, [sp, #0x8]
    8038: e08f100c     	add	r1, pc, r12
    803c: e58d6004     	str	r6, [sp, #0x4]
    8040: e58d6000     	str	r6, [sp]
    8044: e1a02000     	mov	r2, r0
    8048: e1a00009     	mov	r0, r9
    804c: ebffe99f     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5984
    8050: e59f11c8     	ldr	r1, [pc, #0x1c8]        @ 0x8220 <arbhar_play_tilde_setup+0x6ac>
    8054: e5959020     	ldr	r9, [r5, #0x20]
    8058: e08f0001     	add	r0, pc, r1
    805c: ebffe8e4     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5c70
    8060: e59f21bc     	ldr	r2, [pc, #0x1bc]        @ 0x8224 <arbhar_play_tilde_setup+0x6b0>
    8064: e58d4000     	str	r4, [sp]
    8068: e1a03006     	mov	r3, r6
    806c: e08f1002     	add	r1, pc, r2
    8070: e1a02000     	mov	r2, r0
    8074: e1a00009     	mov	r0, r9
    8078: ebffe994     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x59b0
    807c: e59f31a4     	ldr	r3, [pc, #0x1a4]        @ 0x8228 <arbhar_play_tilde_setup+0x6b4>
    8080: e5959020     	ldr	r9, [r5, #0x20]
    8084: e08f0003     	add	r0, pc, r3
    8088: ebffe8d9     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5c9c
    808c: e59fc198     	ldr	r12, [pc, #0x198]       @ 0x822c <arbhar_play_tilde_setup+0x6b8>
    8090: e1a03006     	mov	r3, r6
    8094: e58d4000     	str	r4, [sp]
    8098: e08f100c     	add	r1, pc, r12
    809c: e1a02000     	mov	r2, r0
    80a0: e1a00009     	mov	r0, r9
    80a4: ebffe989     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x59dc
    80a8: e59f0180     	ldr	r0, [pc, #0x180]        @ 0x8230 <arbhar_play_tilde_setup+0x6bc>
    80ac: e5959020     	ldr	r9, [r5, #0x20]
    80b0: e08f0000     	add	r0, pc, r0
    80b4: ebffe8ce     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5cc8
    80b8: e59f1174     	ldr	r1, [pc, #0x174]        @ 0x8234 <arbhar_play_tilde_setup+0x6c0>
    80bc: e1a03006     	mov	r3, r6
    80c0: e7971001     	ldr	r1, [r7, r1]
    80c4: e59f716c     	ldr	r7, [pc, #0x16c]        @ 0x8238 <arbhar_play_tilde_setup+0x6c4>
    80c8: e58d4008     	str	r4, [sp, #0x8]
    80cc: e58d6004     	str	r6, [sp, #0x4]
    80d0: e58d6000     	str	r6, [sp]
    80d4: e1a02000     	mov	r2, r0
    80d8: e1a00009     	mov	r0, r9
    80dc: ebffe97b     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5a14
    80e0: e08f0007     	add	r0, pc, r7
    80e4: e5959020     	ldr	r9, [r5, #0x20]
    80e8: ebffe8c1     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5cfc
    80ec: e1a03006     	mov	r3, r6
    80f0: e59f6144     	ldr	r6, [pc, #0x144]        @ 0x823c <arbhar_play_tilde_setup+0x6c8>
    80f4: e58d4000     	str	r4, [sp]
    80f8: e08f1006     	add	r1, pc, r6
    80fc: e1a02000     	mov	r2, r0
    8100: e1a00009     	mov	r0, r9
    8104: ebffe971     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5a3c
    8108: e59f2130     	ldr	r2, [pc, #0x130]        @ 0x8240 <arbhar_play_tilde_setup+0x6cc>
    810c: e5955020     	ldr	r5, [r5, #0x20]
    8110: e08f0002     	add	r0, pc, r2
    8114: ebffe8b6     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x5d28
    8118: e58d4000     	str	r4, [sp]
    811c: e59f4120     	ldr	r4, [pc, #0x120]        @ 0x8244 <arbhar_play_tilde_setup+0x6d0>
    8120: e1a03008     	mov	r3, r8
    8124: e08f1004     	add	r1, pc, r4
    8128: e1a02000     	mov	r2, r0
    812c: e1a00005     	mov	r0, r5
    8130: ebffe966     	bl	0x26d0 <.plt+0x2f0>     @ imm = #-0x5a68
    8134: e28dd014     	add	sp, sp, #20
    8138: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    813c: c0 51 00 00  	.word	0x000051c0
    8140: 64 64 01 00  	.word	0x00016464
    8144: 28 01 00 00  	.word	0x00000128
    8148: 4c 01 00 00  	.word	0x0000014c
    814c: d0 65 01 00  	.word	0x000165d0
    8150: 80 51 00 00  	.word	0x00005180
    8154: 44 01 00 00  	.word	0x00000144
    8158: 54 51 00 00  	.word	0x00005154
    815c: b8 b2 ff ff  	.word	0xffffb2b8
    8160: 34 51 00 00  	.word	0x00005134
    8164: 1c b1 ff ff  	.word	0xffffb11c
    8168: 14 51 00 00  	.word	0x00005114
    816c: 40 bc ff ff  	.word	0xffffbc40
    8170: f8 50 00 00  	.word	0x000050f8
    8174: e0 b0 ff ff  	.word	0xffffb0e0
    8178: d4 50 00 00  	.word	0x000050d4
    817c: d4 b0 ff ff  	.word	0xffffb0d4
    8180: b4 50 00 00  	.word	0x000050b4
    8184: b0 b0 ff ff  	.word	0xffffb0b0
    8188: 90 50 00 00  	.word	0x00005090
    818c: 8c b1 ff ff  	.word	0xffffb18c
    8190: 70 50 00 00  	.word	0x00005070
    8194: ec ee ff ff  	.word	0xffffeeec
    8198: 50 50 00 00  	.word	0x00005050
    819c: 34 ac ff ff  	.word	0xffffac34
    81a0: 30 50 00 00  	.word	0x00005030
    81a4: ac ab ff ff  	.word	0xffffabac
    81a8: 10 50 00 00  	.word	0x00005010
    81ac: ac b0 ff ff  	.word	0xffffb0ac
    81b0: ec 4f 00 00  	.word	0x00004fec
    81b4: 28 b0 ff ff  	.word	0xffffb028
    81b8: d0 4f 00 00  	.word	0x00004fd0
    81bc: 88 af ff ff  	.word	0xffffaf88
    81c0: ac 4f 00 00  	.word	0x00004fac
    81c4: 68 af ff ff  	.word	0xffffaf68
    81c8: 8c 4f 00 00  	.word	0x00004f8c
    81cc: 74 af ff ff  	.word	0xffffaf74
    81d0: 68 4f 00 00  	.word	0x00004f68
    81d4: a4 a9 ff ff  	.word	0xffffa9a4
    81d8: 48 4f 00 00  	.word	0x00004f48
    81dc: 94 be ff ff  	.word	0xffffbe94
    81e0: 28 4f 00 00  	.word	0x00004f28
    81e4: 50 a9 ff ff  	.word	0xffffa950
    81e8: 0c 4f 00 00  	.word	0x00004f0c
    81ec: 58 a9 ff ff  	.word	0xffffa958
    81f0: e8 4e 00 00  	.word	0x00004ee8
    81f4: e4 a9 ff ff  	.word	0xffffa9e4
    81f8: c4 4e 00 00  	.word	0x00004ec4
    81fc: c4 c0 ff ff  	.word	0xffffc0c4
    8200: a8 4e 00 00  	.word	0x00004ea8
    8204: 84 ad ff ff  	.word	0xffffad84
    8208: 88 4e 00 00  	.word	0x00004e88
    820c: a0 b7 ff ff  	.word	0xffffb7a0
    8210: 64 4e 00 00  	.word	0x00004e64
    8214: 1c b3 ff ff  	.word	0xffffb31c
    8218: 3c 4e 00 00  	.word	0x00004e3c
    821c: 80 bd ff ff  	.word	0xffffbd80
    8220: 18 4e 00 00  	.word	0x00004e18
    8224: e4 f9 ff ff  	.word	0xfffff9e4
    8228: f8 4d 00 00  	.word	0x00004df8
    822c: b0 ac ff ff  	.word	0xffffacb0
    8230: d4 4d 00 00  	.word	0x00004dd4
    8234: 20 01 00 00  	.word	0x00000120
    8238: b4 4d 00 00  	.word	0x00004db4
    823c: a8 c0 ff ff  	.word	0xffffc0a8
    8240: 8c 4d 00 00  	.word	0x00004d8c
    8244: 10 b8 ff ff  	.word	0xffffb810

