; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080357cc  d0b5      push	{r4, r6, r7, lr}
080357ce  2e4a      ldr	r2, [pc, #184] ; [0x08035888] = 0x52007000
080357d0  8ab0      sub	sp, #40
080357d2  0168      ldr	r1, [r0]
080357d4  0023      movs	r3, #0
080357d6  9142      cmp	r1, r2
080357d8  0893      str	r3, [sp, #32]
080357da  cde90433  strd	r3, r3, [sp, #16]
080357de  cde90633  strd	r3, r3, [sp, #24]
080357e2  01d0      beq	#2 ; -> 0x080357e8 ; branch_target=0x080357e8
080357e4  0ab0      add	sp, #40
080357e6  d0bd      pop	{r4, r6, r7, pc}
080357e8  284b      ldr	r3, [pc, #160] ; [0x0803588c] = 0x58024400
080357ea  4ff47060  mov.w	r0, #3840
080357ee  0221      movs	r1, #2
080357f0  0c24      movs	r4, #12
080357f2  d3f8d420  ldr.w	r2, [r3, #212]
080357f6  0126      movs	r6, #1
080357f8  0327      movs	r7, #3
080357fa  42f48032  orr	r2, r2, #65536
080357fe  c3f8d420  str.w	r2, [r3, #212]
08035802  d3f8d420  ldr.w	r2, [r3, #212]
08035806  02f48032  and	r2, r2, #65536
0803580a  0192      str	r2, [sp, #4]
0803580c  019a      ldr	r2, [sp, #4]
0803580e  d3f8e020  ldr.w	r2, [r3, #224]
08035812  42f00402  orr	r2, r2, #4
08035816  c3f8e020  str.w	r2, [r3, #224]
0803581a  d3f8e020  ldr.w	r2, [r3, #224]
0803581e  02f00402  and	r2, r2, #4
08035822  0292      str	r2, [sp, #8]
08035824  029a      ldr	r2, [sp, #8]
08035826  d3f8e020  ldr.w	r2, [r3, #224]
0803582a  42f00802  orr	r2, r2, #8
0803582e  c3f8e020  str.w	r2, [r3, #224]
08035832  d3f8e030  ldr.w	r3, [r3, #224]
08035836  0894      str	r4, [sp, #32]
08035838  03f00803  and	r3, r3, #8
0803583c  cde90401  strd	r0, r1, [sp, #16]
08035840  0393      str	r3, [sp, #12]
08035842  04a9      add	r1, sp, #16
08035844  1248      ldr	r0, [pc, #72] ; [0x08035890] = 0x58020800
08035846  039b      ldr	r3, [sp, #12]
08035848  cde90667  strd	r6, r7, [sp, #24]
0803584c  edf7a4fc  bl	#-75448 ; -> 0x08023198 ; branch_target=0x08023198
08035850  4ff48052  mov.w	r2, #4096
08035854  0223      movs	r3, #2
08035856  04a9      add	r1, sp, #16
08035858  0d48      ldr	r0, [pc, #52] ; [0x08035890] = 0x58020800
0803585a  0894      str	r4, [sp, #32]
0803585c  cde90423  strd	r2, r3, [sp, #16]
08035860  0022      movs	r2, #0
08035862  0323      movs	r3, #3
08035864  cde90623  strd	r2, r3, [sp, #24]
08035868  edf796fc  bl	#-75476 ; -> 0x08023198 ; branch_target=0x08023198
0803586c  0422      movs	r2, #4
0803586e  0223      movs	r3, #2
08035870  04a9      add	r1, sp, #16
08035872  0848      ldr	r0, [pc, #32] ; [0x08035894] = 0x58020c00
08035874  0894      str	r4, [sp, #32]
08035876  cde90667  strd	r6, r7, [sp, #24]
0803587a  cde90423  strd	r2, r3, [sp, #16]
0803587e  edf78bfc  bl	#-75498 ; -> 0x08023198 ; branch_target=0x08023198
08035882  0ab0      add	sp, #40
08035884  d0bd      pop	{r4, r6, r7, pc}
