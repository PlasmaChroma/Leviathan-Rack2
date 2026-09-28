; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032d24  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
08032d28  a24b      ldr	r3, [pc, #648] ; [0x08032fb4] = 0x58024400
08032d2a  8fb0      sub	sp, #60
08032d2c  0024      movs	r4, #0
08032d2e  dff89892  ldr.w	r9, [pc, #664] ; [0x08032fc8] = 0x58020800
08032d32  dff898b2  ldr.w	r11, [pc, #664] ; [0x08032fcc] = 0x58021400
08032d36  48f2c001  movw	r1, #32960
08032d3a  0c94      str	r4, [sp, #48]
08032d3c  4846      mov	r0, r9
08032d3e  9e4e      ldr	r6, [pc, #632] ; [0x08032fb8] = 0x58020400
08032d40  0125      movs	r5, #1
08032d42  dff88c82  ldr.w	r8, [pc, #652] ; [0x08032fd0] = 0x58020c00
08032d46  dff88ca2  ldr.w	r10, [pc, #652] ; [0x08032fd4] = 0x58020000
08032d4a  9c4f      ldr	r7, [pc, #624] ; [0x08032fbc] = 0x58021800
08032d4c  cde90844  strd	r4, r4, [sp, #32]
08032d50  cde90a44  strd	r4, r4, [sp, #40]
08032d54  d3f8e020  ldr.w	r2, [r3, #224]
08032d58  42f01002  orr	r2, r2, #16
08032d5c  c3f8e020  str.w	r2, [r3, #224]
08032d60  d3f8e020  ldr.w	r2, [r3, #224]
08032d64  02f01002  and	r2, r2, #16
08032d68  0092      str	r2, [sp]
08032d6a  009a      ldr	r2, [sp]
08032d6c  d3f8e020  ldr.w	r2, [r3, #224]
08032d70  42f00402  orr	r2, r2, #4
08032d74  c3f8e020  str.w	r2, [r3, #224]
08032d78  d3f8e020  ldr.w	r2, [r3, #224]
08032d7c  02f00402  and	r2, r2, #4
08032d80  0192      str	r2, [sp, #4]
08032d82  019a      ldr	r2, [sp, #4]
08032d84  d3f8e020  ldr.w	r2, [r3, #224]
08032d88  42f02002  orr	r2, r2, #32
08032d8c  c3f8e020  str.w	r2, [r3, #224]
08032d90  d3f8e020  ldr.w	r2, [r3, #224]
08032d94  02f02002  and	r2, r2, #32
08032d98  0292      str	r2, [sp, #8]
08032d9a  029a      ldr	r2, [sp, #8]
08032d9c  d3f8e020  ldr.w	r2, [r3, #224]
08032da0  42f08002  orr	r2, r2, #128
08032da4  c3f8e020  str.w	r2, [r3, #224]
08032da8  d3f8e020  ldr.w	r2, [r3, #224]
08032dac  02f08002  and	r2, r2, #128
08032db0  0392      str	r2, [sp, #12]
08032db2  039a      ldr	r2, [sp, #12]
08032db4  d3f8e020  ldr.w	r2, [r3, #224]
08032db8  42f00102  orr	r2, r2, #1
08032dbc  c3f8e020  str.w	r2, [r3, #224]
08032dc0  d3f8e020  ldr.w	r2, [r3, #224]
08032dc4  02f00102  and	r2, r2, #1
08032dc8  0492      str	r2, [sp, #16]
08032dca  049a      ldr	r2, [sp, #16]
08032dcc  d3f8e020  ldr.w	r2, [r3, #224]
08032dd0  42f00202  orr	r2, r2, #2
08032dd4  c3f8e020  str.w	r2, [r3, #224]
08032dd8  d3f8e020  ldr.w	r2, [r3, #224]
08032ddc  02f00202  and	r2, r2, #2
08032de0  0592      str	r2, [sp, #20]
08032de2  059a      ldr	r2, [sp, #20]
08032de4  d3f8e020  ldr.w	r2, [r3, #224]
08032de8  42f04002  orr	r2, r2, #64
08032dec  c3f8e020  str.w	r2, [r3, #224]
08032df0  d3f8e020  ldr.w	r2, [r3, #224]
08032df4  02f04002  and	r2, r2, #64
08032df8  0692      str	r2, [sp, #24]
08032dfa  069a      ldr	r2, [sp, #24]
08032dfc  d3f8e020  ldr.w	r2, [r3, #224]
08032e00  42f00802  orr	r2, r2, #8
08032e04  c3f8e020  str.w	r2, [r3, #224]
08032e08  2246      mov	r2, r4
08032e0a  d3f8e030  ldr.w	r3, [r3, #224]
08032e0e  03f00803  and	r3, r3, #8
08032e12  0793      str	r3, [sp, #28]
08032e14  079b      ldr	r3, [sp, #28]
08032e16  f0f7f9fa  bl	#-64014 ; -> 0x0802340c ; branch_target=0x0802340c
08032e1a  2246      mov	r2, r4
08032e1c  5846      mov	r0, r11
08032e1e  4ff48061  mov.w	r1, #1024
08032e22  f0f7f3fa  bl	#-64026 ; -> 0x0802340c ; branch_target=0x0802340c
08032e26  2246      mov	r2, r4
08032e28  3046      mov	r0, r6
08032e2a  48f21021  movw	r1, #33296
08032e2e  f0f7edfa  bl	#-64038 ; -> 0x0802340c ; branch_target=0x0802340c
08032e32  2246      mov	r2, r4
08032e34  4046      mov	r0, r8
08032e36  4ff44051  mov.w	r1, #12288
08032e3a  f0f7e7fa  bl	#-64050 ; -> 0x0802340c ; branch_target=0x0802340c
08032e3e  0422      movs	r2, #4
08032e40  0023      movs	r3, #0
08032e42  08a9      add	r1, sp, #32
08032e44  5e48      ldr	r0, [pc, #376] ; [0x08032fc0] = 0x58021000
08032e46  0a94      str	r4, [sp, #40]
08032e48  cde90823  strd	r2, r3, [sp, #32]
08032e4c  f0f7a4f9  bl	#-64696 ; -> 0x08023198 ; branch_target=0x08023198
08032e50  4ff40052  mov.w	r2, #8192
08032e54  5b4b      ldr	r3, [pc, #364] ; [0x08032fc4] = 0x11110000
08032e56  08a9      add	r1, sp, #32
08032e58  4846      mov	r0, r9
08032e5a  0a94      str	r4, [sp, #40]
08032e5c  cde90823  strd	r2, r3, [sp, #32]
08032e60  f0f79af9  bl	#-64716 ; -> 0x08023198 ; branch_target=0x08023198
08032e64  4ff48042  mov.w	r2, #16384
08032e68  564b      ldr	r3, [pc, #344] ; [0x08032fc4] = 0x11110000
08032e6a  08a9      add	r1, sp, #32
08032e6c  4846      mov	r0, r9
08032e6e  0a94      str	r4, [sp, #40]
08032e70  cde90823  strd	r2, r3, [sp, #32]
08032e74  f0f790f9  bl	#-64736 ; -> 0x08023198 ; branch_target=0x08023198
08032e78  48f2c003  movw	r3, #32960
08032e7c  4846      mov	r0, r9
08032e7e  08a9      add	r1, sp, #32
08032e80  0893      str	r3, [sp, #32]
08032e82  0b94      str	r4, [sp, #44]
08032e84  cde90954  strd	r5, r4, [sp, #36]
08032e88  f0f786f9  bl	#-64756 ; -> 0x08023198 ; branch_target=0x08023198
08032e8c  4ff48063  mov.w	r3, #1024
08032e90  5846      mov	r0, r11
08032e92  08a9      add	r1, sp, #32
08032e94  0893      str	r3, [sp, #32]
08032e96  0b94      str	r4, [sp, #44]
08032e98  cde90954  strd	r5, r4, [sp, #36]
08032e9c  f0f77cf9  bl	#-64776 ; -> 0x08023198 ; branch_target=0x08023198
08032ea0  43f28402  movw	r2, #12420
08032ea4  0323      movs	r3, #3
08032ea6  08a9      add	r1, sp, #32
08032ea8  3046      mov	r0, r6
08032eaa  0a94      str	r4, [sp, #40]
08032eac  cde90823  strd	r2, r3, [sp, #32]
08032eb0  f0f772f9  bl	#-64796 ; -> 0x08023198 ; branch_target=0x08023198
08032eb4  4cf61373  movw	r3, #53011
08032eb8  08a9      add	r1, sp, #32
08032eba  3046      mov	r0, r6
08032ebc  0893      str	r3, [sp, #32]
08032ebe  0b94      str	r4, [sp, #44]
08032ec0  cde90954  strd	r5, r4, [sp, #36]
08032ec4  f0f768f9  bl	#-64816 ; -> 0x08023198 ; branch_target=0x08023198
08032ec8  4ff4a063  mov.w	r3, #1280
08032ecc  08a9      add	r1, sp, #32
08032ece  5046      mov	r0, r10
08032ed0  0893      str	r3, [sp, #32]
08032ed2  0223      movs	r3, #2
08032ed4  cde90954  strd	r5, r4, [sp, #36]
08032ed8  0b93      str	r3, [sp, #44]
08032eda  f0f75df9  bl	#-64838 ; -> 0x08023198 ; branch_target=0x08023198
08032ede  4ff44053  mov.w	r3, #12288
08032ee2  08a9      add	r1, sp, #32
08032ee4  4046      mov	r0, r8
08032ee6  0b94      str	r4, [sp, #44]
08032ee8  0893      str	r3, [sp, #32]
08032eea  cde90954  strd	r5, r4, [sp, #36]
08032eee  f0f753f9  bl	#-64858 ; -> 0x08023198 ; branch_target=0x08023198
08032ef2  4ff40272  mov.w	r2, #520
08032ef6  0323      movs	r3, #3
08032ef8  08a9      add	r1, sp, #32
08032efa  3846      mov	r0, r7
08032efc  0a94      str	r4, [sp, #40]
08032efe  cde90823  strd	r2, r3, [sp, #32]
08032f02  f0f749f9  bl	#-64878 ; -> 0x08023198 ; branch_target=0x08023198
08032f06  4ff4a042  mov.w	r2, #20480
08032f0a  0023      movs	r3, #0
08032f0c  08a9      add	r1, sp, #32
08032f0e  3846      mov	r0, r7
08032f10  0a94      str	r4, [sp, #40]
08032f12  cde90823  strd	r2, r3, [sp, #32]
08032f16  f0f73ff9  bl	#-64898 ; -> 0x08023198 ; branch_target=0x08023198
08032f1a  c022      movs	r2, #192
08032f1c  294b      ldr	r3, [pc, #164] ; [0x08032fc4] = 0x11110000
08032f1e  08a9      add	r1, sp, #32
08032f20  3846      mov	r0, r7
08032f22  0a94      str	r4, [sp, #40]
08032f24  cde90823  strd	r2, r3, [sp, #32]
08032f28  f0f736f9  bl	#-64916 ; -> 0x08023198 ; branch_target=0x08023198
08032f2c  4ff4d052  mov.w	r2, #6656
08032f30  0323      movs	r3, #3
08032f32  08a9      add	r1, sp, #32
08032f34  5046      mov	r0, r10
08032f36  0a94      str	r4, [sp, #40]
08032f38  cde90823  strd	r2, r3, [sp, #32]
08032f3c  f0f72cf9  bl	#-64936 ; -> 0x08023198 ; branch_target=0x08023198
08032f40  4ff40042  mov.w	r2, #32768
08032f44  0023      movs	r3, #0
08032f46  5046      mov	r0, r10
08032f48  08a9      add	r1, sp, #32
08032f4a  0a94      str	r4, [sp, #40]
08032f4c  cde90823  strd	r2, r3, [sp, #32]
08032f50  f0f722f9  bl	#-64956 ; -> 0x08023198 ; branch_target=0x08023198
08032f54  c822      movs	r2, #200
08032f56  0023      movs	r3, #0
08032f58  08a9      add	r1, sp, #32
08032f5a  4046      mov	r0, r8
08032f5c  0a94      str	r4, [sp, #40]
08032f5e  cde90823  strd	r2, r3, [sp, #32]
08032f62  f0f719f9  bl	#-64974 ; -> 0x08023198 ; branch_target=0x08023198
08032f66  3022      movs	r2, #48
08032f68  0323      movs	r3, #3
08032f6a  4046      mov	r0, r8
08032f6c  08a9      add	r1, sp, #32
08032f6e  0a94      str	r4, [sp, #40]
08032f70  cde90823  strd	r2, r3, [sp, #32]
08032f74  f0f710f9  bl	#-64992 ; -> 0x08023198 ; branch_target=0x08023198
08032f78  4ff40062  mov.w	r2, #2048
08032f7c  114b      ldr	r3, [pc, #68] ; [0x08032fc4] = 0x11110000
08032f7e  3846      mov	r0, r7
08032f80  08a9      add	r1, sp, #32
08032f82  0a94      str	r4, [sp, #40]
08032f84  cde90823  strd	r2, r3, [sp, #32]
08032f88  f0f706f9  bl	#-65012 ; -> 0x08023198 ; branch_target=0x08023198
08032f8c  0023      movs	r3, #0
08032f8e  4022      movs	r2, #64
08032f90  3046      mov	r0, r6
08032f92  08a9      add	r1, sp, #32
08032f94  0a94      str	r4, [sp, #40]
08032f96  cde90823  strd	r2, r3, [sp, #32]
08032f9a  f0f7fdf8  bl	#-65030 ; -> 0x08023198 ; branch_target=0x08023198
08032f9e  2246      mov	r2, r4
08032fa0  2146      mov	r1, r4
08032fa2  1720      movs	r0, #23
08032fa4  eef79cfa  bl	#-72392 ; -> 0x080214e0 ; branch_target=0x080214e0
08032fa8  1720      movs	r0, #23
08032faa  eef7d5fa  bl	#-72278 ; -> 0x08021558 ; branch_target=0x08021558
08032fae  0fb0      add	sp, #60
08032fb0  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
