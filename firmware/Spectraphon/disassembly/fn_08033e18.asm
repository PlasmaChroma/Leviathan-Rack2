; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033e18  10b5      push	{r4, lr}
08033e1a  9eb0      sub	sp, #120
08033e1c  4c22      movs	r2, #76
08033e1e  0021      movs	r1, #0
08033e20  0aa8      add	r0, sp, #40
08033e22  02f02efb  bl	#9820 ; -> 0x08036482 ; branch_target=0x08036482
08033e26  2022      movs	r2, #32
08033e28  0021      movs	r1, #0
08033e2a  02a8      add	r0, sp, #8
08033e2c  02f029fb  bl	#9810 ; -> 0x08036482 ; branch_target=0x08036482
08033e30  0220      movs	r0, #2
08033e32  eff7bdfb  bl	#-67718 ; -> 0x080235b0 ; branch_target=0x080235b0
08033e36  2a4a      ldr	r2, [pc, #168] ; [0x08033ee0] = 0x58024800
08033e38  0021      movs	r1, #0
08033e3a  2a4b      ldr	r3, [pc, #168] ; [0x08033ee4] = 0x58000400
08033e3c  0191      str	r1, [sp, #4]
08033e3e  9169      ldr	r1, [r2, #24]
08033e40  41f44041  orr	r1, r1, #49152
08033e44  9161      str	r1, [r2, #24]
08033e46  9169      ldr	r1, [r2, #24]
08033e48  01f44041  and	r1, r1, #49152
08033e4c  0191      str	r1, [sp, #4]
08033e4e  d96a      ldr	r1, [r3, #44]
08033e50  41f00101  orr	r1, r1, #1
08033e54  d962      str	r1, [r3, #44]
08033e56  db6a      ldr	r3, [r3, #44]
08033e58  03f00103  and	r3, r3, #1
08033e5c  0193      str	r3, [sp, #4]
08033e5e  019b      ldr	r3, [sp, #4]
08033e60  9369      ldr	r3, [r2, #24]
08033e62  9b04      lsls	r3, r3, #18
08033e64  fcd5      bpl	#-8 ; -> 0x08033e60 ; branch_target=0x08033e60
08033e66  204a      ldr	r2, [pc, #128] ; [0x08033ee8] = 0x58024400
08033e68  0121      movs	r1, #1
08033e6a  0824      movs	r4, #8
08033e6c  0aa8      add	r0, sp, #40
08033e6e  936a      ldr	r3, [r2, #40]
08033e70  23f00303  bic	r3, r3, #3
08033e74  43f00203  orr	r3, r3, #2
08033e78  9362      str	r3, [r2, #40]
08033e7a  0223      movs	r3, #2
08033e7c  1091      str	r1, [sp, #64]
08033e7e  0521      movs	r1, #5
08033e80  0022      movs	r2, #0
08033e82  1993      str	r3, [sp, #100]
08033e84  1591      str	r1, [sp, #84]
08033e86  c021      movs	r1, #192
08033e88  1c92      str	r2, [sp, #112]
08033e8a  1691      str	r1, [sp, #88]
08033e8c  9fed127b  vldr	d7, [pc, #72] ; [0x08033ed8] = 0x00000021 / f64_bits_interpretation=1.3906711615671639e-309
08033e90  cde91333  strd	r3, r3, [sp, #76]
08033e94  cde91733  strd	r3, r3, [sp, #92]
08033e98  8ded0a7b  vstr	d7, [sp, #40]
08033e9c  cde91a42  strd	r4, r2, [sp, #104]
08033ea0  eff71cfc  bl	#-67528 ; -> 0x080236dc ; branch_target=0x080236dc
08033ea4  0346      mov	r3, r0
08033ea6  00b1      cbz	r0, #0 ; -> 0x08033eaa ; branch_target=0x08033eaa
08033ea8  fee7      b	#-4 ; -> 0x08033ea8 ; branch_target=0x08033ea8
08033eaa  3f21      movs	r1, #63
08033eac  4022      movs	r2, #64
08033eae  0493      str	r3, [sp, #16]
08033eb0  4ff48063  mov.w	r3, #1024
08033eb4  0291      str	r1, [sp, #8]
08033eb6  0321      movs	r1, #3
08033eb8  02a8      add	r0, sp, #8
08033eba  0792      str	r2, [sp, #28]
08033ebc  0391      str	r1, [sp, #12]
08033ebe  0421      movs	r1, #4
08033ec0  cde90542  strd	r4, r2, [sp, #20]
08033ec4  cde90832  strd	r3, r2, [sp, #32]
08033ec8  eff776ff  bl	#-65812 ; -> 0x08023db8 ; branch_target=0x08023db8
08033ecc  00b1      cbz	r0, #0 ; -> 0x08033ed0 ; branch_target=0x08033ed0
08033ece  fee7      b	#-4 ; -> 0x08033ece ; branch_target=0x08033ece
08033ed0  1eb0      add	sp, #120
08033ed2  10bd      pop	{r4, pc}
