; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080327a8  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
080327ac  0122      movs	r2, #1
080327ae  83b0      sub	sp, #12
080327b0  0646      mov	r6, r0
080327b2  4ff40041  mov.w	r1, #32768
080327b6  bc48      ldr	r0, [pc, #752] ; [0x08032aa8] = 0x58020800
080327b8  40f29e37  movw	r7, #926
080327bc  bb4c      ldr	r4, [pc, #748] ; [0x08032aac] = 0x2000000c
080327be  f0f725fe  bl	#-62390 ; -> 0x0802340c ; branch_target=0x0802340c
080327c2  bb4d      ldr	r5, [pc, #748] ; [0x08032ab0] = 0x20000008
080327c4  4ff00003  mov.w	r3, #0
080327c8  2146      mov	r1, r4
080327ca  0322      movs	r2, #3
080327cc  a370      strb	r3, [r4, #2]
080327ce  3046      mov	r0, r6
080327d0  2b68      ldr	r3, [r5]
080327d2  4ff09f0a  mov.w	r10, #159
080327d6  2780      strh	r7, [r4]
080327d8  f4f766fb  bl	#-47412 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080327dc  2780      strh	r7, [r4]
080327de  2146      mov	r1, r4
080327e0  b44f      ldr	r7, [pc, #720] ; [0x08032ab4] = 0x20002f78
080327e2  0222      movs	r2, #2
080327e4  2b68      ldr	r3, [r5]
080327e6  40f29e4b  movw	r11, #1182
080327ea  3870      strb	r0, [r7]
080327ec  3046      mov	r0, r6
080327ee  dff8d082  ldr.w	r8, [pc, #720] ; [0x08032ac0] = 0x20002f88
080327f2  f4f759fb  bl	#-47438 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080327f6  2b68      ldr	r3, [r5]
080327f8  2146      mov	r1, r4
080327fa  3870      strb	r0, [r7]
080327fc  4246      mov	r2, r8
080327fe  0093      str	r3, [sp]
08032800  3046      mov	r0, r6
08032802  0223      movs	r3, #2
08032804  84f800a0  strb.w	r10, [r4]
08032808  dff8b892  ldr.w	r9, [pc, #696] ; [0x08032ac4] = 0x20002f7c
0803280c  f4f794fc  bl	#-46808 ; -> 0x08027138 ; branch_target=0x08027138
08032810  98f80130  ldrb.w	r3, [r8, #1]
08032814  2146      mov	r1, r4
08032816  3870      strb	r0, [r7]
08032818  0322      movs	r2, #3
0803281a  89f80030  strb.w	r3, [r9]
0803281e  4023      movs	r3, #64
08032820  3046      mov	r0, r6
08032822  a4f800b0  strh.w	r11, [r4]
08032826  a370      strb	r3, [r4, #2]
08032828  2b68      ldr	r3, [r5]
0803282a  f4f73dfb  bl	#-47494 ; -> 0x08026ea8 ; branch_target=0x08026ea8
0803282e  2b68      ldr	r3, [r5]
08032830  2146      mov	r1, r4
08032832  3870      strb	r0, [r7]
08032834  0222      movs	r2, #2
08032836  3046      mov	r0, r6
08032838  a4f800b0  strh.w	r11, [r4]
0803283c  f4f734fb  bl	#-47512 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032840  2b68      ldr	r3, [r5]
08032842  4246      mov	r2, r8
08032844  3870      strb	r0, [r7]
08032846  2146      mov	r1, r4
08032848  84f800a0  strb.w	r10, [r4]
0803284c  3046      mov	r0, r6
0803284e  0093      str	r3, [sp]
08032850  0223      movs	r3, #2
08032852  f4f771fc  bl	#-46878 ; -> 0x08027138 ; branch_target=0x08027138
08032856  98f80130  ldrb.w	r3, [r8, #1]
0803285a  40f29e5b  movw	r11, #1438
0803285e  2146      mov	r1, r4
08032860  89f80130  strb.w	r3, [r9, #1]
08032864  4623      movs	r3, #70
08032866  3870      strb	r0, [r7]
08032868  0322      movs	r2, #3
0803286a  a370      strb	r3, [r4, #2]
0803286c  3046      mov	r0, r6
0803286e  2b68      ldr	r3, [r5]
08032870  a4f800b0  strh.w	r11, [r4]
08032874  f4f718fb  bl	#-47568 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032878  2b68      ldr	r3, [r5]
0803287a  2146      mov	r1, r4
0803287c  3870      strb	r0, [r7]
0803287e  0222      movs	r2, #2
08032880  3046      mov	r0, r6
08032882  a4f800b0  strh.w	r11, [r4]
08032886  f4f70ffb  bl	#-47586 ; -> 0x08026ea8 ; branch_target=0x08026ea8
0803288a  2b68      ldr	r3, [r5]
0803288c  3870      strb	r0, [r7]
0803288e  4246      mov	r2, r8
08032890  2146      mov	r1, r4
08032892  84f800a0  strb.w	r10, [r4]
08032896  3046      mov	r0, r6
08032898  0093      str	r3, [sp]
0803289a  0223      movs	r3, #2
0803289c  40f29e6b  movw	r11, #1694
080328a0  f4f74afc  bl	#-46956 ; -> 0x08027138 ; branch_target=0x08027138
080328a4  98f80130  ldrb.w	r3, [r8, #1]
080328a8  2146      mov	r1, r4
080328aa  3870      strb	r0, [r7]
080328ac  89f80230  strb.w	r3, [r9, #2]
080328b0  2223      movs	r3, #34
080328b2  0322      movs	r2, #3
080328b4  3046      mov	r0, r6
080328b6  a370      strb	r3, [r4, #2]
080328b8  2b68      ldr	r3, [r5]
080328ba  a4f800b0  strh.w	r11, [r4]
080328be  f4f7f3fa  bl	#-47642 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080328c2  2b68      ldr	r3, [r5]
080328c4  2146      mov	r1, r4
080328c6  3870      strb	r0, [r7]
080328c8  0222      movs	r2, #2
080328ca  3046      mov	r0, r6
080328cc  a4f800b0  strh.w	r11, [r4]
080328d0  f4f7eafa  bl	#-47660 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080328d4  2b68      ldr	r3, [r5]
080328d6  3870      strb	r0, [r7]
080328d8  4246      mov	r2, r8
080328da  2146      mov	r1, r4
080328dc  84f800a0  strb.w	r10, [r4]
080328e0  3046      mov	r0, r6
080328e2  0093      str	r3, [sp]
080328e4  0223      movs	r3, #2
080328e6  40f69e5b  movw	r11, #3486
080328ea  f4f725fc  bl	#-47030 ; -> 0x08027138 ; branch_target=0x08027138
080328ee  98f80130  ldrb.w	r3, [r8, #1]
080328f2  2146      mov	r1, r4
080328f4  3870      strb	r0, [r7]
080328f6  89f80330  strb.w	r3, [r9, #3]
080328fa  40f69e03  movw	r3, #2206
080328fe  0222      movs	r2, #2
08032900  3046      mov	r0, r6
08032902  2380      strh	r3, [r4]
08032904  2b68      ldr	r3, [r5]
08032906  f4f7cffa  bl	#-47714 ; -> 0x08026ea8 ; branch_target=0x08026ea8
0803290a  2b68      ldr	r3, [r5]
0803290c  3870      strb	r0, [r7]
0803290e  4246      mov	r2, r8
08032910  2146      mov	r1, r4
08032912  84f800a0  strb.w	r10, [r4]
08032916  3046      mov	r0, r6
08032918  0093      str	r3, [sp]
0803291a  0223      movs	r3, #2
0803291c  f4f70cfc  bl	#-47080 ; -> 0x08027138 ; branch_target=0x08027138
08032920  98f80130  ldrb.w	r3, [r8, #1]
08032924  2146      mov	r1, r4
08032926  3870      strb	r0, [r7]
08032928  89f80630  strb.w	r3, [r9, #6]
0803292c  2823      movs	r3, #40
0803292e  0322      movs	r2, #3
08032930  3046      mov	r0, r6
08032932  a370      strb	r3, [r4, #2]
08032934  2b68      ldr	r3, [r5]
08032936  a4f800b0  strh.w	r11, [r4]
0803293a  f4f7b5fa  bl	#-47766 ; -> 0x08026ea8 ; branch_target=0x08026ea8
0803293e  2b68      ldr	r3, [r5]
08032940  2146      mov	r1, r4
08032942  3870      strb	r0, [r7]
08032944  0222      movs	r2, #2
08032946  3046      mov	r0, r6
08032948  a4f800b0  strh.w	r11, [r4]
0803294c  f4f7acfa  bl	#-47784 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032950  2b68      ldr	r3, [r5]
08032952  3870      strb	r0, [r7]
08032954  4246      mov	r2, r8
08032956  2146      mov	r1, r4
08032958  84f800a0  strb.w	r10, [r4]
0803295c  3046      mov	r0, r6
0803295e  0093      str	r3, [sp]
08032960  0223      movs	r3, #2
08032962  41f69e6b  movw	r11, #7838
08032966  f4f7e7fb  bl	#-47154 ; -> 0x08027138 ; branch_target=0x08027138
0803296a  98f80130  ldrb.w	r3, [r8, #1]
0803296e  2146      mov	r1, r4
08032970  3870      strb	r0, [r7]
08032972  89f80330  strb.w	r3, [r9, #3]
08032976  8023      movs	r3, #128
08032978  0322      movs	r2, #3
0803297a  3046      mov	r0, r6
0803297c  a370      strb	r3, [r4, #2]
0803297e  2b68      ldr	r3, [r5]
08032980  a4f800b0  strh.w	r11, [r4]
08032984  f4f790fa  bl	#-47840 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032988  2b68      ldr	r3, [r5]
0803298a  2146      mov	r1, r4
0803298c  3870      strb	r0, [r7]
0803298e  0222      movs	r2, #2
08032990  3046      mov	r0, r6
08032992  a4f800b0  strh.w	r11, [r4]
08032996  f4f787fa  bl	#-47858 ; -> 0x08026ea8 ; branch_target=0x08026ea8
0803299a  2b68      ldr	r3, [r5]
0803299c  4246      mov	r2, r8
0803299e  2146      mov	r1, r4
080329a0  3870      strb	r0, [r7]
080329a2  3046      mov	r0, r6
080329a4  84f800a0  strb.w	r10, [r4]
080329a8  40f29e2b  movw	r11, #670
080329ac  0093      str	r3, [sp]
080329ae  0223      movs	r3, #2
080329b0  f4f7c2fb  bl	#-47228 ; -> 0x08027138 ; branch_target=0x08027138
080329b4  98f80130  ldrb.w	r3, [r8, #1]
080329b8  3870      strb	r0, [r7]
080329ba  0220      movs	r0, #2
080329bc  89f80430  strb.w	r3, [r9, #4]
080329c0  edf7f2fc  bl	#-75292 ; -> 0x080203a8 ; branch_target=0x080203a8
080329c4  4ff00003  mov.w	r3, #0
080329c8  2146      mov	r1, r4
080329ca  0322      movs	r2, #3
080329cc  3046      mov	r0, r6
080329ce  a370      strb	r3, [r4, #2]
080329d0  2b68      ldr	r3, [r5]
080329d2  a4f800b0  strh.w	r11, [r4]
080329d6  f4f767fa  bl	#-47922 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080329da  2b68      ldr	r3, [r5]
080329dc  2146      mov	r1, r4
080329de  3870      strb	r0, [r7]
080329e0  0222      movs	r2, #2
080329e2  3046      mov	r0, r6
080329e4  a4f800b0  strh.w	r11, [r4]
080329e8  f4f75efa  bl	#-47940 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080329ec  2b68      ldr	r3, [r5]
080329ee  3870      strb	r0, [r7]
080329f0  4246      mov	r2, r8
080329f2  2146      mov	r1, r4
080329f4  84f800a0  strb.w	r10, [r4]
080329f8  3046      mov	r0, r6
080329fa  0093      str	r3, [sp]
080329fc  0223      movs	r3, #2
080329fe  2e4e      ldr	r6, [pc, #184] ; [0x08032ab8] = 0x20014bb8
08032a00  f4f79afb  bl	#-47308 ; -> 0x08027138 ; branch_target=0x08027138
08032a04  98f80130  ldrb.w	r3, [r8, #1]
08032a08  41f69e4b  movw	r11, #7326
08032a0c  3870      strb	r0, [r7]
08032a0e  0227      movs	r7, #2
08032a10  2146      mov	r1, r4
08032a12  89f80530  strb.w	r3, [r9, #5]
08032a16  0322      movs	r2, #3
08032a18  2b68      ldr	r3, [r5]
08032a1a  3046      mov	r0, r6
08032a1c  a4f800b0  strh.w	r11, [r4]
08032a20  a770      strb	r7, [r4, #2]
08032a22  f4f741fa  bl	#-47998 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032a26  2b68      ldr	r3, [r5]
08032a28  3a46      mov	r2, r7
08032a2a  2146      mov	r1, r4
08032a2c  3046      mov	r0, r6
08032a2e  a4f800b0  strh.w	r11, [r4]
08032a32  f4f739fa  bl	#-48014 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032a36  2b68      ldr	r3, [r5]
08032a38  4246      mov	r2, r8
08032a3a  2146      mov	r1, r4
08032a3c  84f800a0  strb.w	r10, [r4]
08032a40  3046      mov	r0, r6
08032a42  0093      str	r3, [sp]
08032a44  3b46      mov	r3, r7
08032a46  41f69e5b  movw	r11, #7582
08032a4a  f4f775fb  bl	#-47382 ; -> 0x08027138 ; branch_target=0x08027138
08032a4e  98f80130  ldrb.w	r3, [r8, #1]
08032a52  2146      mov	r1, r4
08032a54  0322      movs	r2, #3
08032a56  89f80230  strb.w	r3, [r9, #2]
08032a5a  3046      mov	r0, r6
08032a5c  2b68      ldr	r3, [r5]
08032a5e  a770      strb	r7, [r4, #2]
08032a60  a4f800b0  strh.w	r11, [r4]
08032a64  f4f720fa  bl	#-48064 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032a68  2b68      ldr	r3, [r5]
08032a6a  3a46      mov	r2, r7
08032a6c  2146      mov	r1, r4
08032a6e  3046      mov	r0, r6
08032a70  a4f800b0  strh.w	r11, [r4]
08032a74  f4f718fa  bl	#-48080 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032a78  2b68      ldr	r3, [r5]
08032a7a  4246      mov	r2, r8
08032a7c  2146      mov	r1, r4
08032a7e  84f800a0  strb.w	r10, [r4]
08032a82  3046      mov	r0, r6
08032a84  0093      str	r3, [sp]
08032a86  3b46      mov	r3, r7
08032a88  f4f756fb  bl	#-47444 ; -> 0x08027138 ; branch_target=0x08027138
08032a8c  98f80130  ldrb.w	r3, [r8, #1]
08032a90  0a20      movs	r0, #10
08032a92  89f80230  strb.w	r3, [r9, #2]
08032a96  edf787fc  bl	#-75506 ; -> 0x080203a8 ; branch_target=0x080203a8
08032a9a  084b      ldr	r3, [pc, #32] ; [0x08032abc] = 0x20002f66
08032a9c  0122      movs	r2, #1
08032a9e  1a70      strb	r2, [r3]
08032aa0  03b0      add	sp, #12
08032aa2  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
