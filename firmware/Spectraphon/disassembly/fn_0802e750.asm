; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: confirmed_audio_callback_tail_target,prologue_heuristic
0802e750  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802e754  354b      ldr	r3, [pc, #212] ; [0x0802e82c] = 0xe0001000
0802e756  8146      mov	r9, r0
0802e758  354c      ldr	r4, [pc, #212] ; [0x0802e830] = 0x20002f65
0802e75a  3649      ldr	r1, [pc, #216] ; [0x0802e834] = 0x2000227c
0802e75c  2ded108b  vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
0802e760  5a68      ldr	r2, [r3, #4]
0802e762  b1b0      sub	sp, #196
0802e764  2378      ldrb	r3, [r4]
0802e766  0a60      str	r2, [r1]
0802e768  0bb1      cbz	r3, #2 ; -> 0x0802e76e ; branch_target=0x0802e76e
0802e76a  0133      adds	r3, #1
0802e76c  2370      strb	r3, [r4]
0802e76e  fff7c7f8  bl	#-3698 ; -> 0x0802d900 ; branch_target=0x0802d900
0802e772  2378      ldrb	r3, [r4]
0802e774  022b      cmp	r3, #2
0802e776  03f25585  bhi.w	#15018 ; -> 0x08032224 ; branch_target=0x08032224
0802e77a  fef773fe  bl	#-4890 ; -> 0x0802d464 ; branch_target=0x0802d464
0802e77e  2e4b      ldr	r3, [pc, #184] ; [0x0802e838] = 0x20002eec
0802e780  2e4d      ldr	r5, [pc, #184] ; [0x0802e83c] = 0x20002f50
0802e782  1b68      ldr	r3, [r3]
0802e784  802b      cmp	r3, #128
0802e786  14d9      bls	#40 ; -> 0x0802e7b2 ; branch_target=0x0802e7b2
0802e788  2d4b      ldr	r3, [pc, #180] ; [0x0802e840] = 0x20002e90
0802e78a  2e4e      ldr	r6, [pc, #184] ; [0x0802e844] = 0x2001348c
0802e78c  1946      mov	r1, r3
0802e78e  eb78      ldrb	r3, [r5, #3]
0802e790  aa78      ldrb	r2, [r5, #2]
0802e792  2491      str	r1, [sp, #144]
0802e794  0b60      str	r3, [r1]
0802e796  b6f90010  ldrsh.w	r1, [r6]
0802e79a  dff8c880  ldr.w	r8, [pc, #200] ; [0x0802e864] = 0x20002e94
0802e79e  8a42      cmp	r2, r1
0802e7a0  c8f80020  str.w	r2, [r8]
0802e7a4  41f03b86  bne.w	#7286 ; -> 0x0803041e ; branch_target=0x0803041e
0802e7a8  b6f90220  ldrsh.w	r2, [r6, #2]
0802e7ac  9a42      cmp	r2, r3
0802e7ae  41f00586  bne.w	#7178 ; -> 0x080303bc ; branch_target=0x080303bc
0802e7b2  2b79      ldrb	r3, [r5, #4]
0802e7b4  244a      ldr	r2, [pc, #144] ; [0x0802e848] = 0x2000242c
0802e7b6  0393      str	r3, [sp, #12]
0802e7b8  1368      ldr	r3, [r2]
0802e7ba  002b      cmp	r3, #0
0802e7bc  01dd      ble	#2 ; -> 0x0802e7c2 ; branch_target=0x0802e7c2
0802e7be  013b      subs	r3, #1
0802e7c0  1360      str	r3, [r2]
0802e7c2  224a      ldr	r2, [pc, #136] ; [0x0802e84c] = 0x20002428
0802e7c4  1368      ldr	r3, [r2]
0802e7c6  002b      cmp	r3, #0
0802e7c8  01dd      ble	#2 ; -> 0x0802e7ce ; branch_target=0x0802e7ce
0802e7ca  013b      subs	r3, #1
0802e7cc  1360      str	r3, [r2]
0802e7ce  204b      ldr	r3, [pc, #128] ; [0x0802e850] = 0x20002f66
0802e7d0  1b78      ldrb	r3, [r3]
0802e7d2  002b      cmp	r3, #0
0802e7d4  4ad1      bne	#148 ; -> 0x0802e86c ; branch_target=0x0802e86c
0802e7d6  1f4b      ldr	r3, [pc, #124] ; [0x0802e854] = 0x20002ea4
0802e7d8  2593      str	r3, [sp, #148]
0802e7da  1f4b      ldr	r3, [pc, #124] ; [0x0802e858] = 0x20002ea0
0802e7dc  2f93      str	r3, [sp, #188]
0802e7de  1f4f      ldr	r7, [pc, #124] ; [0x0802e85c] = 0x20002e84
0802e7e0  3b68      ldr	r3, [r7]
0802e7e2  012b      cmp	r3, #1
0802e7e4  03f08084  beq.w	#14592 ; -> 0x080320e8 ; branch_target=0x080320e8
0802e7e8  0023      movs	r3, #0
0802e7ea  259a      ldr	r2, [sp, #148]
0802e7ec  dff878c0  ldr.w	r12, [pc, #120] ; [0x0802e868] = 0x20002e80
0802e7f0  1360      str	r3, [r2]
0802e7f2  dcf80030  ldr.w	r3, [r12]
0802e7f6  012b      cmp	r3, #1
0802e7f8  03f0cf84  beq.w	#14750 ; -> 0x0803219a ; branch_target=0x0803219a
0802e7fc  184b      ldr	r3, [pc, #96] ; [0x0802e860] = 0x20002eb8
0802e7fe  0022      movs	r2, #0
0802e800  2f99      ldr	r1, [sp, #188]
0802e802  1b68      ldr	r3, [r3]
0802e804  0a60      str	r2, [r1]
0802e806  012b      cmp	r3, #1
0802e808  03f0b984  beq.w	#14706 ; -> 0x0803217e ; branch_target=0x0803217e
0802e80c  0948      ldr	r0, [pc, #36] ; [0x0802e834] = 0x2000227c
0802e80e  074a      ldr	r2, [pc, #28] ; [0x0802e82c] = 0xe0001000
0802e810  0168      ldr	r1, [r0]
0802e812  5268      ldr	r2, [r2, #4]
0802e814  c36b      ldr	r3, [r0, #60]
0802e816  521a      subs	r2, r2, r1
0802e818  1344      add	r3, r2
0802e81a  8262      str	r2, [r0, #40]
0802e81c  5b08      lsrs	r3, r3, #1
0802e81e  c363      str	r3, [r0, #60]
0802e820  31b0      add	sp, #196
0802e822  bdec108b  vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
0802e826  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802e86c  ce4b      ldr	r3, [pc, #824] ; [0x0802eba8] = 0x20001200
0802e86e  cf4e      ldr	r6, [pc, #828] ; [0x0802ebac] = 0x30000020 / f32_bits_interpretation=4.656630637e-10
0802e870  cf4c      ldr	r4, [pc, #828] ; [0x0802ebb0] = 0x20001380
0802e872  93ed047a  vldr	s14, [r3, #16]
0802e876  d3ed055a  vldr	s11, [r3, #20]
0802e87a  93ed026a  vldr	s12, [r3, #8]
0802e87e  3189      ldrh	r1, [r6, #8]
0802e880  2369      ldr	r3, [r4, #16]
0802e882  7289      ldrh	r2, [r6, #10]
0802e884  cb1a      subs	r3, r1, r3
0802e886  dfedcb6a  vldr	s13, [pc, #812] ; [0x0802ebb4] = 0x42700000 / f32_bits_interpretation=60
0802e88a  b088      ldrh	r0, [r6, #4]
0802e88c  07ee903a  vmov	s15, r3
0802e890  6369      ldr	r3, [r4, #20]
0802e892  f8eee77a  vcvt.f32.s32	s15, s15
0802e896  d31a      subs	r3, r2, r3
0802e898  67ee877a  vmul.f32	s15, s15, s14
0802e89c  67eea77a  vmul.f32	s15, s15, s15
0802e8a0  67eea77a  vmul.f32	s15, s15, s15
0802e8a4  27eea67a  vmul.f32	s14, s15, s13
0802e8a8  07ee903a  vmov	s15, r3
0802e8ac  a368      ldr	r3, [r4, #8]
0802e8ae  f8eee77a  vcvt.f32.s32	s15, s15
0802e8b2  c31a      subs	r3, r0, r3
0802e8b4  00ee103a  vmov	s0, r3
0802e8b8  67eea57a  vmul.f32	s15, s15, s11
0802e8bc  be4b      ldr	r3, [pc, #760] ; [0x0802ebb8] = 0x20002000
0802e8be  b8eec00a  vcvt.f32.s32	s0, s0
0802e8c2  67eea77a  vmul.f32	s15, s15, s15
0802e8c6  9860      str	r0, [r3, #8]
0802e8c8  20ee060a  vmul.f32	s0, s0, s12
0802e8cc  67eea77a  vmul.f32	s15, s15, s15
0802e8d0  b5eec00a  vcmpe.f32	s0, #0
0802e8d4  67eea67a  vmul.f32	s15, s15, s13
0802e8d8  c3e90412  strd	r1, r2, [r3, #16]
0802e8dc  b74b      ldr	r3, [pc, #732] ; [0x0802ebbc] = 0x20001140
0802e8de  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802e8e2  83ed007a  vstr	s14, [r3]
0802e8e6  b64b      ldr	r3, [pc, #728] ; [0x0802ebc0] = 0x20002e50
0802e8e8  c3ed007a  vstr	s15, [r3]
0802e8ec  01f16085  bmi.w	#6848 ; -> 0x080303b0 ; branch_target=0x080303b0
0802e8f0  f7ee007a  vmov.f32	s15, #1.000000e+00
0802e8f4  b4eee70a  vcmpe.f32	s0, s15
0802e8f8  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802e8fc  43f3e385  ble.w	#15302 ; -> 0x080324c6 ; branch_target=0x080324c6
0802e900  b0ee670a  vmov.f32	s0, s15
0802e904  af4b      ldr	r3, [pc, #700] ; [0x0802ebc4] = 0x20002e68
0802e906  b049      ldr	r1, [pc, #704] ; [0x0802ebc8] = 0x20002434
0802e908  c3ed007a  vstr	s15, [r3]
0802e90c  0b68      ldr	r3, [r1]
0802e90e  002b      cmp	r3, #0
0802e910  03f0c984  beq.w	#14738 ; -> 0x080322a6 ; branch_target=0x080322a6
0802e914  ad4b      ldr	r3, [pc, #692] ; [0x0802ebcc] = 0x20002e40
0802e916  d3ed006a  vldr	s13, [r3]
0802e91a  ad4b      ldr	r3, [pc, #692] ; [0x0802ebd0] = 0x20002e64
0802e91c  2293      str	r3, [sp, #136]
0802e91e  a24b      ldr	r3, [pc, #648] ; [0x0802eba8] = 0x20001200
0802e920  a54a      ldr	r2, [pc, #660] ; [0x0802ebb8] = 0x20002000
0802e922  d3ed037a  vldr	s15, [r3, #12]
0802e926  f388      ldrh	r3, [r6, #6]
0802e928  d360      str	r3, [r2, #12]
0802e92a  a14a      ldr	r2, [pc, #644] ; [0x0802ebb0] = 0x20001380
0802e92c  d268      ldr	r2, [r2, #12]
0802e92e  9b1a      subs	r3, r3, r2
0802e930  00ee903a  vmov	s1, r3
0802e934  f8eee00a  vcvt.f32.s32	s1, s1
0802e938  60eea70a  vmul.f32	s1, s1, s15
0802e93c  f5eec00a  vcmpe.f32	s1, #0
0802e940  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802e944  03f1c784  bmi.w	#14734 ; -> 0x080322d6 ; branch_target=0x080322d6
0802e948  f7ee007a  vmov.f32	s15, #1.000000e+00
0802e94c  f4eee70a  vcmpe.f32	s1, s15
0802e950  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802e954  43f3bb85  ble.w	#15222 ; -> 0x080324ce ; branch_target=0x080324ce
0802e958  f0ee670a  vmov.f32	s1, s15
0802e95c  30eee67a  vsub.f32	s14, s1, s13
0802e960  229b      ldr	r3, [sp, #136]
0802e962  c3ed007a  vstr	s15, [r3]
0802e966  b5eec07a  vcmpe.f32	s14, #0
0802e96a  dfed9a7a  vldr	s15, [pc, #616] ; [0x0802ebd4] = 0x3ba3d70a / f32_bits_interpretation=0.004999999888
0802e96e  9a4b      ldr	r3, [pc, #616] ; [0x0802ebd8] = 0x20002e48
0802e970  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802e974  48bf      it	mi
0802e976  36eee07a  vsubmi.f32	s14, s13, s1
0802e97a  b4eee77a  vcmpe.f32	s14, s15
0802e97e  83ed007a  vstr	s14, [r3]
0802e982  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802e986  01dd      ble	#2 ; -> 0x0802e98c ; branch_target=0x0802e98c
0802e988  0023      movs	r3, #0
0802e98a  0b60      str	r3, [r1]
0802e98c  934b      ldr	r3, [pc, #588] ; [0x0802ebdc] = 0x20002e90
0802e98e  2493      str	r3, [sp, #144]
0802e990  1b68      ldr	r3, [r3]
0802e992  012b      cmp	r3, #1
0802e994  03f00c85  beq.w	#14872 ; -> 0x080323b0 ; branch_target=0x080323b0
0802e998  f7ee007a  vmov.f32	s15, #1.000000e+00
0802e99c  9fed907a  vldr	s14, [pc, #576] ; [0x0802ebe0] = 0x3f7ae148 / f32_bits_interpretation=0.9800000191
0802e9a0  dfed906a  vldr	s13, [pc, #576] ; [0x0802ebe4] = 0x45fff800 / f32_bits_interpretation=8191
0802e9a4  0122      movs	r2, #1
0802e9a6  9048      ldr	r0, [pc, #576] ; [0x0802ebe8] = 0x08043a74
0802e9a8  b7ee005b  vmov.f64	d5, #1.000000e+00
0802e9ac  e0ee477a  vfms.f32	s15, s0, s14
0802e9b0  9fed8e4a  vldr	s8, [pc, #568] ; [0x0802ebec] = 0x39da740e / f32_bits_interpretation=0.0004166666768
0802e9b4  60eea60a  vmul.f32	s1, s1, s13
0802e9b8  8d4c      ldr	r4, [pc, #564] ; [0x0802ebf0] = 0x200023f0
0802e9ba  dfed8e4a  vldr	s9, [pc, #568] ; [0x0802ebf4] = 0x3f7ffeb0 / f32_bits_interpretation=0.9999799728
0802e9be  0890      str	r0, [sp, #32]
0802e9c0  1594      str	r4, [sp, #84]
0802e9c2  b7eee77a  vcvt.f64.f32	d7, s15
0802e9c6  9fed766b  vldr	d6, [pc, #472] ; [0x0802eba0] = 0x51eb851f / f64_bits_interpretation=-3.1400000000000001
0802e9ca  27ee077b  vmul.f64	d7, d7, d7
0802e9ce  27ee067b  vmul.f64	d7, d7, d6
0802e9d2  fdeee06a  vcvt.s32.f32	s13, s1
0802e9d6  16ee903a  vmov	r3, s13
0802e9da  c3f30a01  ubfx	r1, r3, #0, #11
0802e9de  db12      asrs	r3, r3, #11
0802e9e0  00eb8101  add.w	r1, r0, r1, lsl #2
0802e9e4  02fa03f3  lsl.w	r3, r2, r3
0802e9e8  91ed006a  vldr	s12, [r1]
0802e9ec  06ee903a  vmov	s13, r3
0802e9f0  26ee046a  vmul.f32	s12, s12, s8
0802e9f4  f8ee666a  vcvt.f32.u32	s13, s13
0802e9f8  66ee866a  vmul.f32	s13, s13, s12
0802e9fc  b7eee63a  vcvt.f64.f32	d3, s13
0802ea00  c4ed006a  vstr	s13, [r4]
0802ea04  a3ee075b  vfma.f64	d5, d3, d7
0802ea08  f7eec5bb  vcvt.f32.f64	s23, d5
0802ea0c  cbfee4ba  vminnm.f32	s23, s23, s9
0802ea10  654b      ldr	r3, [pc, #404] ; [0x0802eba8] = 0x20001200
0802ea12  694a      ldr	r2, [pc, #420] ; [0x0802ebb8] = 0x20002000
0802ea14  d3ed017a  vldr	s15, [r3, #4]
0802ea18  7388      ldrh	r3, [r6, #2]
0802ea1a  5360      str	r3, [r2, #4]
0802ea1c  644a      ldr	r2, [pc, #400] ; [0x0802ebb0] = 0x20001380
0802ea1e  5268      ldr	r2, [r2, #4]
0802ea20  9b1a      subs	r3, r3, r2
0802ea22  00ee103a  vmov	s0, r3
0802ea26  b8eec00a  vcvt.f32.s32	s0, s0
0802ea2a  20ee270a  vmul.f32	s0, s0, s15
0802ea2e  b5eec00a  vcmpe.f32	s0, #0
0802ea32  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ea36  03f14884  bmi.w	#14480 ; -> 0x080322ca ; branch_target=0x080322ca
0802ea3a  f7ee007a  vmov.f32	s15, #1.000000e+00
0802ea3e  b4eee70a  vcmpe.f32	s0, s15
0802ea42  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ea46  43f33a85  ble.w	#14964 ; -> 0x080324be ; branch_target=0x080324be
0802ea4a  b0ee670a  vmov.f32	s0, s15
0802ea4e  6a4b      ldr	r3, [pc, #424] ; [0x0802ebf8] = 0x20002e60
0802ea50  6a49      ldr	r1, [pc, #424] ; [0x0802ebfc] = 0x20002438
0802ea52  c3ed007a  vstr	s15, [r3]
0802ea56  0b68      ldr	r3, [r1]
0802ea58  002b      cmp	r3, #0
0802ea5a  03f02d84  beq.w	#14426 ; -> 0x080322b8 ; branch_target=0x080322b8
0802ea5e  684b      ldr	r3, [pc, #416] ; [0x0802ec00] = 0x20002e44
0802ea60  93ed007a  vldr	s14, [r3]
0802ea64  674b      ldr	r3, [pc, #412] ; [0x0802ec04] = 0x20002e5c
0802ea66  2193      str	r3, [sp, #132]
0802ea68  4f4b      ldr	r3, [pc, #316] ; [0x0802eba8] = 0x20001200
0802ea6a  534a      ldr	r2, [pc, #332] ; [0x0802ebb8] = 0x20002000
0802ea6c  d3ed007a  vldr	s15, [r3]
0802ea70  3388      ldrh	r3, [r6]
0802ea72  1360      str	r3, [r2]
0802ea74  4e4a      ldr	r2, [pc, #312] ; [0x0802ebb0] = 0x20001380
0802ea76  1268      ldr	r2, [r2]
0802ea78  9b1a      subs	r3, r3, r2
0802ea7a  00ee903a  vmov	s1, r3
0802ea7e  219b      ldr	r3, [sp, #132]
0802ea80  f8eee00a  vcvt.f32.s32	s1, s1
0802ea84  60eea70a  vmul.f32	s1, s1, s15
0802ea88  70eec77a  vsub.f32	s15, s1, s14
0802ea8c  c3ed000a  vstr	s1, [r3]
0802ea90  5d4b      ldr	r3, [pc, #372] ; [0x0802ec08] = 0x20002e4c
0802ea92  f5eec07a  vcmpe.f32	s15, #0
0802ea96  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ea9a  48bf      it	mi
0802ea9c  77ee607a  vsubmi.f32	s15, s14, s1
0802eaa0  9fed4c7a  vldr	s14, [pc, #304] ; [0x0802ebd4] = 0x3ba3d70a / f32_bits_interpretation=0.004999999888
0802eaa4  f4eec77a  vcmpe.f32	s15, s14
0802eaa8  c3ed007a  vstr	s15, [r3]
0802eaac  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802eab0  01dd      ble	#2 ; -> 0x0802eab6 ; branch_target=0x0802eab6
0802eab2  0023      movs	r3, #0
0802eab4  0b60      str	r3, [r1]
0802eab6  dfed557a  vldr	s15, [pc, #340] ; [0x0802ec0c] = 0x00000000
0802eaba  f4eee70a  vcmpe.f32	s1, s15
0802eabe  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802eac2  06d4      bmi	#12 ; -> 0x0802ead2 ; branch_target=0x0802ead2
0802eac4  f7ee007a  vmov.f32	s15, #1.000000e+00
0802eac8  f4eee70a  vcmpe.f32	s1, s15
0802eacc  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ead0  04dd      ble	#8 ; -> 0x0802eadc ; branch_target=0x0802eadc
0802ead2  f0ee670a  vmov.f32	s1, s15
0802ead6  219b      ldr	r3, [sp, #132]
0802ead8  c3ed007a  vstr	s15, [r3]
0802eadc  4c4b      ldr	r3, [pc, #304] ; [0x0802ec10] = 0x20002e94
0802eade  1b68      ldr	r3, [r3]
0802eae0  012b      cmp	r3, #1
0802eae2  03f00f84  beq.w	#14366 ; -> 0x08032304 ; branch_target=0x08032304
0802eae6  f7ee007a  vmov.f32	s15, #1.000000e+00
0802eaea  9fed3d7a  vldr	s14, [pc, #244] ; [0x0802ebe0] = 0x3f7ae148 / f32_bits_interpretation=0.9800000191
0802eaee  dfed3d6a  vldr	s13, [pc, #244] ; [0x0802ebe4] = 0x45fff800 / f32_bits_interpretation=8191
0802eaf2  0122      movs	r2, #1
0802eaf4  9fed3d4a  vldr	s8, [pc, #244] ; [0x0802ebec] = 0x39da740e / f32_bits_interpretation=0.0004166666768
0802eaf8  b7ee005b  vmov.f64	d5, #1.000000e+00
0802eafc  e0ee477a  vfms.f32	s15, s0, s14
0802eb00  4448      ldr	r0, [pc, #272] ; [0x0802ec14] = 0x200023ec
0802eb02  60eea60a  vmul.f32	s1, s1, s13
0802eb06  dfed3b4a  vldr	s9, [pc, #236] ; [0x0802ebf4] = 0x3f7ffeb0 / f32_bits_interpretation=0.9999799728
0802eb0a  1490      str	r0, [sp, #80]
0802eb0c  9fed246b  vldr	d6, [pc, #144] ; [0x0802eba0] = 0x51eb851f / f64_bits_interpretation=-3.1400000000000001
0802eb10  b7eee77a  vcvt.f64.f32	d7, s15
0802eb14  27ee077b  vmul.f64	d7, d7, d7
0802eb18  27ee067b  vmul.f64	d7, d7, d6
0802eb1c  fdeee06a  vcvt.s32.f32	s13, s1
0802eb20  16ee903a  vmov	r3, s13
0802eb24  c3f30a01  ubfx	r1, r3, #0, #11
0802eb28  db12      asrs	r3, r3, #11
0802eb2a  02fa03f3  lsl.w	r3, r2, r3
0802eb2e  06ee903a  vmov	s13, r3
0802eb32  089b      ldr	r3, [sp, #32]
0802eb34  03eb8101  add.w	r1, r3, r1, lsl #2
0802eb38  f8ee666a  vcvt.f32.u32	s13, s13
0802eb3c  91ed006a  vldr	s12, [r1]
0802eb40  26ee046a  vmul.f32	s12, s12, s8
0802eb44  66ee866a  vmul.f32	s13, s13, s12
0802eb48  b7eee63a  vcvt.f64.f32	d3, s13
0802eb4c  c0ed006a  vstr	s13, [r0]
0802eb50  a3ee075b  vfma.f64	d5, d3, d7
0802eb54  b7eec5bb  vcvt.f32.f64	s22, d5
0802eb58  8bfe64ba  vminnm.f32	s22, s22, s9
0802eb5c  2e4b      ldr	r3, [pc, #184] ; [0x0802ec18] = 0x200144d4
0802eb5e  1b68      ldr	r3, [r3]
0802eb60  002b      cmp	r3, #0
0802eb62  0493      str	r3, [sp, #16]
0802eb64  40f39680  ble.w	#300 ; -> 0x0802ec94 ; branch_target=0x0802ec94
0802eb68  2c4f      ldr	r7, [pc, #176] ; [0x0802ec1c] = 0x20002f00
0802eb6a  2d4c      ldr	r4, [pc, #180] ; [0x0802ec20] = 0x200013c0
0802eb6c  2d49      ldr	r1, [pc, #180] ; [0x0802ec24] = 0x20001400
0802eb6e  3868      ldr	r0, [r7]
0802eb70  2368      ldr	r3, [r4]
0802eb72  51f82020  ldr.w	r2, [r1, r0, lsl #2]
0802eb76  00f1400c  add.w	r12, r0, #64
0802eb7a  3588      ldrh	r5, [r6]
0802eb7c  00f1800e  add.w	lr, r0, #128
0802eb80  9b1a      subs	r3, r3, r2
0802eb82  6268      ldr	r2, [r4, #4]
0802eb84  00f1c008  add.w	r8, r0, #192
0802eb88  00f5807a  add.w	r10, r0, #256
0802eb8c  2b44      add	r3, r5
0802eb8e  00f5a07b  add.w	r11, r0, #320
0802eb92  41f82050  str.w	r5, [r1, r0, lsl #2]
0802eb96  2360      str	r3, [r4]
0802eb98  431c      adds	r3, r0, #1
0802eb9a  03f03f03  and	r3, r3, #63
0802eb9e  43e0      b	#134 ; -> 0x0802ec28 ; branch_target=0x0802ec28
0802ec28  3b60      str	r3, [r7]
0802ec2a  51f82c30  ldr.w	r3, [r1, r12, lsl #2]
0802ec2e  d01a      subs	r0, r2, r3
0802ec30  51f82e30  ldr.w	r3, [r1, lr, lsl #2]
0802ec34  a268      ldr	r2, [r4, #8]
0802ec36  d51a      subs	r5, r2, r3
0802ec38  e268      ldr	r2, [r4, #12]
0802ec3a  51f82830  ldr.w	r3, [r1, r8, lsl #2]
0802ec3e  d71a      subs	r7, r2, r3
0802ec40  51f82a30  ldr.w	r3, [r1, r10, lsl #2]
0802ec44  2269      ldr	r2, [r4, #16]
0802ec46  d21a      subs	r2, r2, r3
0802ec48  51f82b30  ldr.w	r3, [r1, r11, lsl #2]
0802ec4c  0292      str	r2, [sp, #8]
0802ec4e  6269      ldr	r2, [r4, #20]
0802ec50  d21a      subs	r2, r2, r3
0802ec52  7388      ldrh	r3, [r6, #2]
0802ec54  41f82c30  str.w	r3, [r1, r12, lsl #2]
0802ec58  0344      add	r3, r0
0802ec5a  0298      ldr	r0, [sp, #8]
0802ec5c  6360      str	r3, [r4, #4]
0802ec5e  b388      ldrh	r3, [r6, #4]
0802ec60  41f82e30  str.w	r3, [r1, lr, lsl #2]
0802ec64  2b44      add	r3, r5
0802ec66  a360      str	r3, [r4, #8]
0802ec68  f388      ldrh	r3, [r6, #6]
0802ec6a  41f82830  str.w	r3, [r1, r8, lsl #2]
0802ec6e  3b44      add	r3, r7
0802ec70  e360      str	r3, [r4, #12]
0802ec72  3389      ldrh	r3, [r6, #8]
0802ec74  41f82a30  str.w	r3, [r1, r10, lsl #2]
0802ec78  0344      add	r3, r0
0802ec7a  2361      str	r3, [r4, #16]
0802ec7c  7389      ldrh	r3, [r6, #10]
0802ec7e  1a44      add	r2, r3
0802ec80  41f82b30  str.w	r3, [r1, r11, lsl #2]
0802ec84  049b      ldr	r3, [sp, #16]
0802ec86  6261      str	r2, [r4, #20]
0802ec88  012b      cmp	r3, #1
0802ec8a  03f09e83  beq.w	#14140 ; -> 0x080323ca ; branch_target=0x080323ca
0802ec8e  022b      cmp	r3, #2
0802ec90  03f02184  beq.w	#14402 ; -> 0x080324d6 ; branch_target=0x080324d6
0802ec94  c54b      ldr	r3, [pc, #788] ; [0x0802efac] = 0x30000440 / f32_bits_interpretation=4.657216834e-10
0802ec96  c64f      ldr	r7, [pc, #792] ; [0x0802efb0] = 0x2000227c
0802ec98  03eb8901  add.w	r1, r3, r9, lsl #2
0802ec9c  c54b      ldr	r3, [pc, #788] ; [0x0802efb4] = 0x30001040 / f32_bits_interpretation=4.658922137e-10
0802ec9e  dfedc6ea  vldr	s29, [pc, #792] ; [0x0802efb8] = 0x46000000 / f32_bits_interpretation=8192
0802eca2  03eb8900  add.w	r0, r3, r9, lsl #2
0802eca6  c54b      ldr	r3, [pc, #788] ; [0x0802efbc] = 0x30000c40 / f32_bits_interpretation=4.658353703e-10
0802eca8  2091      str	r1, [sp, #128]
0802ecaa  03eb8904  add.w	r4, r3, r9, lsl #2
0802ecae  c44b      ldr	r3, [pc, #784] ; [0x0802efc0] = 0x30000840 / f32_bits_interpretation=4.657785269e-10
0802ecb0  1790      str	r0, [sp, #92]
0802ecb2  03eb8905  add.w	r5, r3, r9, lsl #2
0802ecb6  c34b      ldr	r3, [pc, #780] ; [0x0802efc4] = 0x38000000 / f32_bits_interpretation=3.051757812e-05
0802ecb8  1894      str	r4, [sp, #96]
0802ecba  03eb8906  add.w	r6, r3, r9, lsl #2
0802ecbe  1995      str	r5, [sp, #100]
0802ecc0  4fea5903  lsr.w	r3, r9, #1
0802ecc4  dff89cb3  ldr.w	r11, [pc, #924] ; [0x0802f064] = 0x0803a674
0802ecc8  1a96      str	r6, [sp, #104]
0802ecca  1693      str	r3, [sp, #88]
0802eccc  be4b      ldr	r3, [pc, #760] ; [0x0802efc8] = 0xe0001000
0802ecce  5a68      ldr	r2, [r3, #4]
0802ecd0  3b68      ldr	r3, [r7]
0802ecd2  d21a      subs	r2, r2, r3
0802ecd4  bd4b      ldr	r3, [pc, #756] ; [0x0802efcc] = 0x20002640
0802ecd6  7a60      str	r2, [r7, #4]
0802ecd8  03f1f80a  add.w	r10, r3, #248
0802ecdc  bc4a      ldr	r2, [pc, #752] ; [0x0802efd0] = 0x20002e94
0802ecde  0d93      str	r3, [sp, #52]
0802ece0  1268      ldr	r2, [r2]
0802ece2  bc4b      ldr	r3, [pc, #752] ; [0x0802efd4] = 0x20002ea4
0802ece4  0692      str	r2, [sp, #24]
0802ece6  249a      ldr	r2, [sp, #144]
0802ece8  2593      str	r3, [sp, #148]
0802ecea  1268      ldr	r2, [r2]
0802ecec  ba4b      ldr	r3, [pc, #744] ; [0x0802efd8] = 0x20002ea0
0802ecee  0592      str	r2, [sp, #20]
0802ecf0  0a1d      adds	r2, r1, #4
0802ecf2  ba49      ldr	r1, [pc, #744] ; [0x0802efdc] = 0x20002540
0802ecf4  1d92      str	r2, [sp, #116]
0802ecf6  021d      adds	r2, r0, #4
0802ecf8  0b91      str	r1, [sp, #44]
0802ecfa  1c92      str	r2, [sp, #112]
0802ecfc  221d      adds	r2, r4, #4
0802ecfe  b849      ldr	r1, [pc, #736] ; [0x0802efe0] = 0x20002940
0802ed00  1f92      str	r2, [sp, #124]
0802ed02  2a1d      adds	r2, r5, #4
0802ed04  0e91      str	r1, [sp, #56]
0802ed06  1b92      str	r2, [sp, #108]
0802ed08  321d      adds	r2, r6, #4
0802ed0a  b649      ldr	r1, [pc, #728] ; [0x0802efe4] = 0x20002840
0802ed0c  1e92      str	r2, [sp, #120]
0802ed0e  0022      movs	r2, #0
0802ed10  0c91      str	r1, [sp, #48]
0802ed12  9146      mov	r9, r2
0802ed14  2f93      str	r3, [sp, #188]
0802ed16  b44b      ldr	r3, [pc, #720] ; [0x0802efe8] = 0x20002eac
0802ed18  9fedb47a  vldr	s14, [pc, #720] ; [0x0802efec] = 0x2ffffff6 / f32_bits_interpretation=4.656610098e-10
0802ed1c  d3ed007a  vldr	s15, [r3]
0802ed20  209b      ldr	r3, [sp, #128]
0802ed22  1d9a      ldr	r2, [sp, #116]
0802ed24  53f82930  ldr.w	r3, [r3, r9, lsl #2]
0802ed28  52f82920  ldr.w	r2, [r2, r9, lsl #2]
0802ed2c  06ee903a  vmov	s13, r3
0802ed30  a549      ldr	r1, [pc, #660] ; [0x0802efc8] = 0xe0001000
0802ed32  db13      asrs	r3, r3, #15
0802ed34  0292      str	r2, [sp, #8]
0802ed36  b8eee68a  vcvt.f32.s32	s16, s13
0802ed3a  06ee902a  vmov	s13, r2
0802ed3e  d213      asrs	r2, r2, #15
0802ed40  c3f1000c  rsb.w	r12, r3, #0
0802ed44  b8eee69a  vcvt.f32.s32	s18, s13
0802ed48  0793      str	r3, [sp, #28]
0802ed4a  28ee078a  vmul.f32	s16, s16, s14
0802ed4e  a84b      ldr	r3, [pc, #672] ; [0x0802eff0] = 0x20002000
0802ed50  9748      ldr	r0, [pc, #604] ; [0x0802efb0] = 0x2000227c
0802ed52  5742      rsbs	r7, r2, #0
0802ed54  29ee079a  vmul.f32	s18, s18, s14
0802ed58  0992      str	r2, [sp, #36]
0802ed5a  b0eec86a  vabs.f32	s12, s16
0802ed5e  4968      ldr	r1, [r1, #4]
0802ed60  c3f818c0  str.w	r12, [r3, #24]
0802ed64  df61      str	r7, [r3, #28]
0802ed66  f0eec96a  vabs.f32	s13, s18
0802ed6a  f4eec67a  vcmpe.f32	s15, s12
0802ed6e  4fea6903  asr.w	r3, r9, #1
0802ed72  0291      str	r1, [sp, #8]
0802ed74  c162      str	r1, [r0, #44]
0802ed76  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ed7a  41f19a84  bpl.w	#6452 ; -> 0x080306b2 ; branch_target=0x080306b2
0802ed7e  9fed9d7a  vldr	s14, [pc, #628] ; [0x0802eff4] = 0x3f666666 / f32_bits_interpretation=0.8999999762
0802ed82  dfed9d5a  vldr	s11, [pc, #628] ; [0x0802eff8] = 0x3dcccccd / f32_bits_interpretation=0.1000000015
0802ed86  27ee877a  vmul.f32	s14, s15, s14
0802ed8a  a6ee257a  vfma.f32	s14, s12, s11
0802ed8e  9b4a      ldr	r2, [pc, #620] ; [0x0802effc] = 0x20002ea8
0802ed90  d2ed007a  vldr	s15, [r2]
0802ed94  944a      ldr	r2, [pc, #592] ; [0x0802efe8] = 0x20002eac
0802ed96  f4eee67a  vcmpe.f32	s15, s13
0802ed9a  82ed007a  vstr	s14, [r2]
0802ed9e  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802eda2  41f19084  bpl.w	#6432 ; -> 0x080306c6 ; branch_target=0x080306c6
0802eda6  9fed936a  vldr	s12, [pc, #588] ; [0x0802eff4] = 0x3f666666 / f32_bits_interpretation=0.8999999762
0802edaa  9fed937a  vldr	s14, [pc, #588] ; [0x0802eff8] = 0x3dcccccd / f32_bits_interpretation=0.1000000015
0802edae  67ee867a  vmul.f32	s15, s15, s12
0802edb2  e6ee877a  vfma.f32	s15, s13, s14
0802edb6  169a      ldr	r2, [sp, #88]
0802edb8  9148      ldr	r0, [pc, #580] ; [0x0802f000] = 0x20002e60
0802edba  1344      add	r3, r2
0802edbc  914a      ldr	r2, [pc, #580] ; [0x0802f004] = 0x20001200
0802edbe  9249      ldr	r1, [pc, #584] ; [0x0802f008] = 0x20002e54
0802edc0  90ed005a  vldr	s10, [r0]
0802edc4  92ed093a  vldr	s6, [r2, #36]
0802edc8  d2ed0b3a  vldr	s7, [r2, #44]
0802edcc  8f48      ldr	r0, [pc, #572] ; [0x0802f00c] = 0x20002e68
0802edce  904a      ldr	r2, [pc, #576] ; [0x0802f010] = 0x20002e58
0802edd0  d1ed006a  vldr	s13, [r1]
0802edd4  92ed006a  vldr	s12, [r2]
0802edd8  d0ed005a  vldr	s11, [r0]
0802eddc  8748      ldr	r0, [pc, #540] ; [0x0802effc] = 0x20002ea8
0802edde  35ee465a  vsub.f32	s10, s10, s12
0802ede2  9fed8c7a  vldr	s14, [pc, #560] ; [0x0802f014] = 0x3c23d70a / f32_bits_interpretation=0.009999999776
0802ede6  c0ed007a  vstr	s15, [r0]
0802edea  75eee67a  vsub.f32	s15, s11, s13
0802edee  a5ee076a  vfma.f32	s12, s10, s14
0802edf2  894c      ldr	r4, [pc, #548] ; [0x0802f018] = 0x20001380
0802edf4  894d      ldr	r5, [pc, #548] ; [0x0802f01c] = 0x20000880
0802edf6  e7ee876a  vfma.f32	s13, s15, s14
0802edfa  9fed894a  vldr	s8, [pc, #548] ; [0x0802f020] = 0x00000000
0802edfe  d5ed008a  vldr	s17, [r5]
0802ee02  884e      ldr	r6, [pc, #544] ; [0x0802f024] = 0x200023fc
0802ee04  7a48      ldr	r0, [pc, #488] ; [0x0802eff0] = 0x20002000
0802ee06  82ed006a  vstr	s12, [r2]
0802ee0a  9a00      lsls	r2, r3, #2
0802ee0c  d6ed004a  vldr	s9, [r6]
0802ee10  c1ed006a  vstr	s13, [r1]
0802ee14  8449      ldr	r1, [pc, #528] ; [0x0802f028] = 0x30000040 / f32_bits_interpretation=4.6566484e-10
0802ee16  31f833e0  ldrh.w	lr, [r1, r3, lsl #3]
0802ee1a  931c      adds	r3, r2, #2
0802ee1c  31f81380  ldrh.w	r8, [r1, r3, lsl #1]
0802ee20  636a      ldr	r3, [r4, #36]
0802ee22  c0f824e0  str.w	lr, [r0, #36]
0802ee26  aeeb0303  sub.w	r3, lr, r3
0802ee2a  c0f82c80  str.w	r8, [r0, #44]
0802ee2e  07ee903a  vmov	s15, r3
0802ee32  e36a      ldr	r3, [r4, #44]
0802ee34  f8eee77a  vcvt.f32.s32	s15, s15
0802ee38  a8eb0303  sub.w	r3, r8, r3
0802ee3c  67ee837a  vmul.f32	s15, s15, s6
0802ee40  c7fe847a  vmaxnm.f32	s15, s15, s8
0802ee44  77eee87a  vsub.f32	s15, s15, s17
0802ee48  e7ee878a  vfma.f32	s17, s15, s14
0802ee4c  07ee903a  vmov	s15, r3
0802ee50  531c      adds	r3, r2, #1
0802ee52  0332      adds	r2, #3
0802ee54  f8eee77a  vcvt.f32.s32	s15, s15
0802ee58  67eea37a  vmul.f32	s15, s15, s7
0802ee5c  c5ed008a  vstr	s17, [r5]
0802ee60  31f81350  ldrh.w	r5, [r1, r3, lsl #1]
0802ee64  c7fe847a  vmaxnm.f32	s15, s15, s8
0802ee68  77eee47a  vsub.f32	s15, s15, s9
0802ee6c  236a      ldr	r3, [r4, #32]
0802ee6e  0562      str	r5, [r0, #32]
0802ee70  e7ee874a  vfma.f32	s9, s15, s14
0802ee74  c6ed004a  vstr	s9, [r6]
0802ee78  b0ee64aa  vmov.f32	s20, s9
0802ee7c  31f81260  ldrh.w	r6, [r1, r2, lsl #1]
0802ee80  ea1a      subs	r2, r5, r3
0802ee82  6a49      ldr	r1, [pc, #424] ; [0x0802f02c] = 0x20001300
0802ee84  8662      str	r6, [r0, #40]
0802ee86  4b68      ldr	r3, [r1, #4]
0802ee88  9342      cmp	r3, r2
0802ee8a  41f3fb83  ble.w	#6134 ; -> 0x08030684 ; branch_target=0x08030684
0802ee8e  684b      ldr	r3, [pc, #416] ; [0x0802f030] = 0x20001280
0802ee90  93ed007a  vldr	s14, [r3]
0802ee94  0b68      ldr	r3, [r1]
0802ee96  d31a      subs	r3, r2, r3
0802ee98  07ee903a  vmov	s15, r3
0802ee9c  f8eee77a  vcvt.f32.s32	s15, s15
0802eea0  67ee877a  vmul.f32	s15, s15, s14
0802eea4  fdeee77a  vcvt.s32.f32	s15, s15
0802eea8  17ee902a  vmov	r2, s15
0802eeac  22eae272  bic.w	r2, r2, r2, asr #31
0802eeb0  0121      movs	r1, #1
0802eeb2  dfed606a  vldr	s13, [pc, #384] ; [0x0802f034] = 0x3f82d013 / f32_bits_interpretation=1.021974921
0802eeb6  d012      asrs	r0, r2, #11
0802eeb8  c2f30a02  ubfx	r2, r2, #0, #11
0802eebc  5e4b      ldr	r3, [pc, #376] ; [0x0802f038] = 0x200023d0
0802eebe  0430      adds	r0, #4
0802eec0  1b68      ldr	r3, [r3]
0802eec2  8140      lsls	r1, r0
0802eec4  012b      cmp	r3, #1
0802eec6  07ee901a  vmov	s15, r1
0802eeca  0899      ldr	r1, [sp, #32]
0802eecc  01eb8202  add.w	r2, r1, r2, lsl #2
0802eed0  f8ee677a  vcvt.f32.u32	s15, s15
0802eed4  92ed007a  vldr	s14, [r2]
0802eed8  584a      ldr	r2, [pc, #352] ; [0x0802f03c] = 0x200023e0
0802eeda  27ee267a  vmul.f32	s14, s14, s13
0802eede  67ee877a  vmul.f32	s15, s15, s14
0802eee2  9fed577a  vldr	s14, [pc, #348] ; [0x0802f040] = 0x37aec33e / f32_bits_interpretation=2.083333311e-05
0802eee6  67ee872a  vmul.f32	s5, s15, s14
0802eeea  c2ed002a  vstr	s5, [r2]
0802eeee  554a      ldr	r2, [pc, #340] ; [0x0802f044] = 0x20000860
0802eef0  c2ed002a  vstr	s5, [r2]
0802eef4  08d1      bne	#16 ; -> 0x0802ef08 ; branch_target=0x0802ef08
0802eef6  9fed547a  vldr	s14, [pc, #336] ; [0x0802f048] = 0x3b800000 / f32_bits_interpretation=0.00390625
0802eefa  504b      ldr	r3, [pc, #320] ; [0x0802f03c] = 0x200023e0
0802eefc  62ee872a  vmul.f32	s5, s5, s14
0802ef00  c3ed002a  vstr	s5, [r3]
0802ef04  c2ed002a  vstr	s5, [r2]
0802ef08  069b      ldr	r3, [sp, #24]
0802ef0a  012b      cmp	r3, #1
0802ef0c  01f0e583  beq.w	#6090 ; -> 0x080306da ; branch_target=0x080306da
0802ef10  414b      ldr	r3, [pc, #260] ; [0x0802f018] = 0x20001380
0802ef12  4e49      ldr	r1, [pc, #312] ; [0x0802f04c] = 0x200012c0
0802ef14  9b6a      ldr	r3, [r3, #40]
0802ef16  f21a      subs	r2, r6, r3
0802ef18  4b68      ldr	r3, [r1, #4]
0802ef1a  9342      cmp	r3, r2
0802ef1c  41f39b83  ble.w	#5942 ; -> 0x08030656 ; branch_target=0x08030656
0802ef20  4b4b      ldr	r3, [pc, #300] ; [0x0802f050] = 0x20001240
0802ef22  d3ed006a  vldr	s13, [r3]
0802ef26  0b68      ldr	r3, [r1]
0802ef28  d31a      subs	r3, r2, r3
0802ef2a  07ee103a  vmov	s14, r3
0802ef2e  b8eec77a  vcvt.f32.s32	s14, s14
0802ef32  27ee267a  vmul.f32	s14, s14, s13
0802ef36  bdeec77a  vcvt.s32.f32	s14, s14
0802ef3a  17ee103a  vmov	r3, s14
0802ef3e  23eae373  bic.w	r3, r3, r3, asr #31
0802ef42  0899      ldr	r1, [sp, #32]
0802ef44  c3f30a02  ubfx	r2, r3, #0, #11
0802ef48  db12      asrs	r3, r3, #11
0802ef4a  01eb8202  add.w	r2, r1, r2, lsl #2
0802ef4e  d2ed006a  vldr	s13, [r2]
0802ef52  039a      ldr	r2, [sp, #12]
0802ef54  002a      cmp	r2, #0
0802ef56  41f03583  bne.w	#5738 ; -> 0x080305c4 ; branch_target=0x080305c4
0802ef5a  1a1d      adds	r2, r3, #4
0802ef5c  3d4b      ldr	r3, [pc, #244] ; [0x0802f054] = 0x20002404
0802ef5e  dfed357a  vldr	s15, [pc, #212] ; [0x0802f034] = 0x3f82d013 / f32_bits_interpretation=1.021974921
0802ef62  93ed007a  vldr	s14, [r3]
0802ef66  0123      movs	r3, #1
0802ef68  66eea76a  vmul.f32	s13, s13, s15
0802ef6c  9340      lsls	r3, r2
0802ef6e  07ee903a  vmov	s15, r3
0802ef72  f8ee677a  vcvt.f32.u32	s15, s15
0802ef76  67eea67a  vmul.f32	s15, s15, s13
0802ef7a  dfed316a  vldr	s13, [pc, #196] ; [0x0802f040] = 0x37aec33e / f32_bits_interpretation=2.083333311e-05
0802ef7e  364b      ldr	r3, [pc, #216] ; [0x0802f058] = 0x200023e8
0802ef80  67eea67a  vmul.f32	s15, s15, s13
0802ef84  c3ed007a  vstr	s15, [r3]
0802ef88  344b      ldr	r3, [pc, #208] ; [0x0802f05c] = 0x200023e4
0802ef8a  c3ed007a  vstr	s15, [r3]
0802ef8e  344b      ldr	r3, [pc, #208] ; [0x0802f060] = 0x20000820
0802ef90  1b68      ldr	r3, [r3]
0802ef92  012b      cmp	r3, #1
0802ef94  68d1      bne	#208 ; -> 0x0802f068 ; branch_target=0x0802f068
0802ef96  dfed2c6a  vldr	s13, [pc, #176] ; [0x0802f048] = 0x3b800000 / f32_bits_interpretation=0.00390625
0802ef9a  2f4b      ldr	r3, [pc, #188] ; [0x0802f058] = 0x200023e8
0802ef9c  67eea67a  vmul.f32	s15, s15, s13
0802efa0  c3ed007a  vstr	s15, [r3]
0802efa4  2d4b      ldr	r3, [pc, #180] ; [0x0802f05c] = 0x200023e4
0802efa6  c3ed007a  vstr	s15, [r3]
0802efaa  5de0      b	#186 ; -> 0x0802f068 ; branch_target=0x0802f068
0802f068  059b      ldr	r3, [sp, #20]
0802f06a  012b      cmp	r3, #1
0802f06c  01f05983  beq.w	#5810 ; -> 0x08030722 ; branch_target=0x08030722
0802f070  f0ee679a  vmov.f32	s19, s15
0802f074  cc4b      ldr	r3, [pc, #816] ; [0x0802f3a8] = 0x200008a0
0802f076  83ed007a  vstr	s14, [r3]
0802f07a  039b      ldr	r3, [sp, #12]
0802f07c  002b      cmp	r3, #0
0802f07e  01f09b82  beq.w	#5430 ; -> 0x080305b8 ; branch_target=0x080305b8
0802f082  ca4b      ldr	r3, [pc, #808] ; [0x0802f3ac] = 0x20000860
0802f084  87eea27a  vdiv.f32	s14, s15, s5
0802f088  93ed006a  vldr	s12, [r3]
0802f08c  c9ee866a  vdiv.f32	s13, s19, s12
0802f090  c74b      ldr	r3, [pc, #796] ; [0x0802f3b0] = 0x20002404
0802f092  83ed007a  vstr	s14, [r3]
0802f096  c74b      ldr	r3, [pc, #796] ; [0x0802f3b4] = 0x20002400
0802f098  c3ed006a  vstr	s13, [r3]
0802f09c  049b      ldr	r3, [sp, #16]
0802f09e  002b      cmp	r3, #0
0802f0a0  41f35782  ble.w	#5294 ; -> 0x08030552 ; branch_target=0x08030552
0802f0a4  c449      ldr	r1, [pc, #784] ; [0x0802f3b8] = 0x20002f00
0802f0a6  c54b      ldr	r3, [pc, #788] ; [0x0802f3bc] = 0x20001400
0802f0a8  0a68      ldr	r2, [r1]
0802f0aa  c54c      ldr	r4, [pc, #788] ; [0x0802f3c0] = 0x200013c0
0802f0ac  02f5c070  add.w	r0, r2, #384
0802f0b0  0a90      str	r0, [sp, #40]
0802f0b2  02f5e070  add.w	r0, r2, #448
0802f0b6  0f90      str	r0, [sp, #60]
0802f0b8  02f50070  add.w	r0, r2, #512
0802f0bc  1090      str	r0, [sp, #64]
0802f0be  02f51070  add.w	r0, r2, #576
0802f0c2  1190      str	r0, [sp, #68]
0802f0c4  02f52070  add.w	r0, r2, #640
0802f0c8  1290      str	r0, [sp, #72]
0802f0ca  02f53070  add.w	r0, r2, #704
0802f0ce  0132      adds	r2, #1
0802f0d0  1390      str	r0, [sp, #76]
0802f0d2  02f03f02  and	r2, r2, #63
0802f0d6  0a60      str	r2, [r1]
0802f0d8  0a99      ldr	r1, [sp, #40]
0802f0da  53f82120  ldr.w	r2, [r3, r1, lsl #2]
0802f0de  43f821c0  str.w	r12, [r3, r1, lsl #2]
0802f0e2  a169      ldr	r1, [r4, #24]
0802f0e4  8a1a      subs	r2, r1, r2
0802f0e6  0799      ldr	r1, [sp, #28]
0802f0e8  521a      subs	r2, r2, r1
0802f0ea  0f99      ldr	r1, [sp, #60]
0802f0ec  2392      str	r2, [sp, #140]
0802f0ee  a261      str	r2, [r4, #24]
0802f0f0  53f82120  ldr.w	r2, [r3, r1, lsl #2]
0802f0f4  43f82170  str.w	r7, [r3, r1, lsl #2]
0802f0f8  e769      ldr	r7, [r4, #28]
0802f0fa  1099      ldr	r1, [sp, #64]
0802f0fc  b81a      subs	r0, r7, r2
0802f0fe  099f      ldr	r7, [sp, #36]
0802f100  c21b      subs	r2, r0, r7
0802f102  1198      ldr	r0, [sp, #68]
0802f104  676a      ldr	r7, [r4, #36]
0802f106  0a92      str	r2, [sp, #40]
0802f108  e261      str	r2, [r4, #28]
0802f10a  53f82120  ldr.w	r2, [r3, r1, lsl #2]
0802f10e  43f82150  str.w	r5, [r3, r1, lsl #2]
0802f112  216a      ldr	r1, [r4, #32]
0802f114  891a      subs	r1, r1, r2
0802f116  53f82020  ldr.w	r2, [r3, r0, lsl #2]
0802f11a  43f820e0  str.w	lr, [r3, r0, lsl #2]
0802f11e  ba1a      subs	r2, r7, r2
0802f120  2944      add	r1, r5
0802f122  0eeb020c  add.w	r12, lr, r2
0802f126  2162      str	r1, [r4, #32]
0802f128  c4f824c0  str.w	r12, [r4, #36]
0802f12c  a76a      ldr	r7, [r4, #40]
0802f12e  1298      ldr	r0, [sp, #72]
0802f130  53f82020  ldr.w	r2, [r3, r0, lsl #2]
0802f134  43f82060  str.w	r6, [r3, r0, lsl #2]
0802f138  1398      ldr	r0, [sp, #76]
0802f13a  bf1a      subs	r7, r7, r2
0802f13c  53f82020  ldr.w	r2, [r3, r0, lsl #2]
0802f140  3744      add	r7, r6
0802f142  43f82080  str.w	r8, [r3, r0, lsl #2]
0802f146  e36a      ldr	r3, [r4, #44]
0802f148  a762      str	r7, [r4, #40]
0802f14a  9a1a      subs	r2, r3, r2
0802f14c  049b      ldr	r3, [sp, #16]
0802f14e  4244      add	r2, r8
0802f150  012b      cmp	r3, #1
0802f152  e262      str	r2, [r4, #44]
0802f154  01f09a81  beq.w	#4916 ; -> 0x0803048c ; branch_target=0x0803048c
0802f158  022b      cmp	r3, #2
0802f15a  02f0c982  beq.w	#9618 ; -> 0x080316f0 ; branch_target=0x080316f0
0802f15e  032b      cmp	r3, #3
0802f160  4ff00102  mov.w	r2, #1
0802f164  4ff04001  mov.w	r1, #64
0802f168  9648      ldr	r0, [pc, #600] ; [0x0802f3c4] = 0x58020800
0802f16a  42f0c583  bne.w	#10122 ; -> 0x080318f8 ; branch_target=0x080318f8
0802f16e  f4f74df9  bl	#-48486 ; -> 0x0802340c ; branch_target=0x0802340c
0802f172  0122      movs	r2, #1
0802f174  8021      movs	r1, #128
0802f176  9348      ldr	r0, [pc, #588] ; [0x0802f3c4] = 0x58020800
0802f178  f4f748f9  bl	#-48496 ; -> 0x0802340c ; branch_target=0x0802340c
0802f17c  9249      ldr	r1, [pc, #584] ; [0x0802f3c8] = 0x20001380
0802f17e  226a      ldr	r2, [r4, #32]
0802f180  0d6a      ldr	r5, [r1, #32]
0802f182  a36a      ldr	r3, [r4, #40]
0802f184  c5eba215  rsb	r5, r5, r2, asr #6
0802f188  8c6a      ldr	r4, [r1, #40]
0802f18a  a5f11a02  sub.w	r2, r5, #26
0802f18e  c4eba314  rsb	r4, r4, r3, asr #6
0802f192  40f2b573  movw	r3, #1973
0802f196  9a42      cmp	r2, r3
0802f198  42f21487  bls.w	#11816 ; -> 0x08031fc4 ; branch_target=0x08031fc4
0802f19c  a4f11a02  sub.w	r2, r4, #26
0802f1a0  40f2b573  movw	r3, #1973
0802f1a4  9a42      cmp	r2, r3
0802f1a6  42f21b87  bls.w	#11830 ; -> 0x08031fe0 ; branch_target=0x08031fe0
0802f1aa  884b      ldr	r3, [pc, #544] ; [0x0802f3cc] = 0x20002eec
0802f1ac  1b68      ldr	r3, [r3]
0802f1ae  1c04      lsls	r4, r3, #16
0802f1b0  0a93      str	r3, [sp, #40]
0802f1b2  01f1b281  bmi.w	#4964 ; -> 0x0803051a ; branch_target=0x0803051a
0802f1b6  864b      ldr	r3, [pc, #536] ; [0x0802f3d0] = 0x200023e0
0802f1b8  d3ed002a  vldr	s5, [r3]
0802f1bc  854b      ldr	r3, [pc, #532] ; [0x0802f3d4] = 0x200023e8
0802f1be  d3ed007a  vldr	s15, [r3]
0802f1c2  854b      ldr	r3, [pc, #532] ; [0x0802f3d8] = 0x200023e4
0802f1c4  d3ed009a  vldr	s19, [r3]
0802f1c8  844b      ldr	r3, [pc, #528] ; [0x0802f3dc] = 0x2000227c
0802f1ca  db6a      ldr	r3, [r3, #44]
0802f1cc  0293      str	r3, [sp, #8]
0802f1ce  844b      ldr	r3, [pc, #528] ; [0x0802f3e0] = 0x20002e94
0802f1d0  1b68      ldr	r3, [r3]
0802f1d2  0693      str	r3, [sp, #24]
0802f1d4  249b      ldr	r3, [sp, #144]
0802f1d6  1b68      ldr	r3, [r3]
0802f1d8  0593      str	r3, [sp, #20]
0802f1da  824b      ldr	r3, [pc, #520] ; [0x0802f3e4] = 0x200008c0
0802f1dc  8248      ldr	r0, [pc, #520] ; [0x0802f3e8] = 0x20002f6c
0802f1de  93ed007a  vldr	s14, [r3]
0802f1e2  824b      ldr	r3, [pc, #520] ; [0x0802f3ec] = 0x2000240c
0802f1e4  67ee2e6a  vmul.f32	s13, s14, s29
0802f1e8  8149      ldr	r1, [pc, #516] ; [0x0802f3f0] = 0xe0001000
0802f1ea  93ed006a  vldr	s12, [r3]
0802f1ee  814c      ldr	r4, [pc, #516] ; [0x0802f3f4] = 0x08039674
0802f1f0  f6eee65a  vrintz.f32	s11, s13
0802f1f4  76eee55a  vsub.f32	s11, s13, s11
0802f1f8  fdeee66a  vcvt.s32.f32	s13, s13
0802f1fc  8ded0f7a  vstr	s14, [sp, #60]
0802f200  8ded106a  vstr	s12, [sp, #64]
0802f204  26ee2e7a  vmul.f32	s14, s12, s29
0802f208  4968      ldr	r1, [r1, #4]
0802f20a  0460      str	r4, [r0]
0802f20c  16ee902a  vmov	r2, s13
0802f210  0298      ldr	r0, [sp, #8]
0802f212  b6eec76a  vrintz.f32	s12, s14
0802f216  37ee466a  vsub.f32	s12, s14, s12
0802f21a  091a      subs	r1, r1, r0
0802f21c  6f48      ldr	r0, [pc, #444] ; [0x0802f3dc] = 0x2000227c
0802f21e  bdeec77a  vcvt.s32.f32	s14, s14
0802f222  8160      str	r1, [r0, #8]
0802f224  511c      adds	r1, r2, #1
0802f226  c2f30c02  ubfx	r2, r2, #0, #13
0802f22a  c1f30c01  ubfx	r1, r1, #0, #13
0802f22e  17ee103a  vmov	r3, s14
0802f232  0beb8202  add.w	r2, r11, r2, lsl #2
0802f236  0beb8101  add.w	r1, r11, r1, lsl #2
0802f23a  d2ed006a  vldr	s13, [r2]
0802f23e  5a1c      adds	r2, r3, #1
0802f240  91ed007a  vldr	s14, [r1]
0802f244  c3f30c03  ubfx	r3, r3, #0, #13
0802f248  c2f30c02  ubfx	r2, r2, #0, #13
0802f24c  37ee667a  vsub.f32	s14, s14, s13
0802f250  0beb8303  add.w	r3, r11, r3, lsl #2
0802f254  0beb8202  add.w	r2, r11, r2, lsl #2
0802f258  e7ee256a  vfma.f32	s13, s14, s11
0802f25c  92ed007a  vldr	s14, [r2]
0802f260  cded076a  vstr	s13, [sp, #28]
0802f264  d3ed006a  vldr	s13, [r3]
0802f268  634b      ldr	r3, [pc, #396] ; [0x0802f3f8] = 0x20002ef8
0802f26a  37ee667a  vsub.f32	s14, s14, s13
0802f26e  1b68      ldr	r3, [r3]
0802f270  e7ee066a  vfma.f32	s13, s14, s12
0802f274  032b      cmp	r3, #3
0802f276  0493      str	r3, [sp, #16]
0802f278  f0ee66ca  vmov.f32	s25, s13
0802f27c  01f0d782  beq.w	#5550 ; -> 0x0803082e ; branch_target=0x0803082e
0802f280  042b      cmp	r3, #4
0802f282  02f09e81  beq.w	#9020 ; -> 0x080315c2 ; branch_target=0x080315c2
0802f286  5d4a      ldr	r2, [pc, #372] ; [0x0802f3fc] = 0x20002418
0802f288  012b      cmp	r3, #1
0802f28a  92ed007a  vldr	s14, [r2]
0802f28e  8ded097a  vstr	s14, [sp, #36]
0802f292  42f02986  bne.w	#11346 ; -> 0x08031ee8 ; branch_target=0x08031ee8
0802f296  5a4b      ldr	r3, [pc, #360] ; [0x0802f400] = 0x200023d8
0802f298  d3ed003a  vldr	s7, [r3]
0802f29c  594b      ldr	r3, [pc, #356] ; [0x0802f404] = 0x20002410
0802f29e  bfee005a  vmov.f32	s10, #-1.000000e+00
0802f2a2  0e9c      ldr	r4, [sp, #56]
0802f2a4  93ed007a  vldr	s14, [r3]
0802f2a8  574b      ldr	r3, [pc, #348] ; [0x0802f408] = 0x200008e0
0802f2aa  f0ee451a  vmov.f32	s3, s10
0802f2ae  27ee2e6a  vmul.f32	s12, s14, s29
0802f2b2  8ded117a  vstr	s14, [sp, #68]
0802f2b6  d3ed006a  vldr	s13, [r3]
0802f2ba  544d      ldr	r5, [pc, #336] ; [0x0802f40c] = 0x20002640
0802f2bc  26eeae4a  vmul.f32	s8, s13, s29
0802f2c0  cded126a  vstr	s13, [sp, #72]
0802f2c4  dfed526a  vldr	s13, [pc, #328] ; [0x0802f410] = 0x45000000 / f32_bits_interpretation=2048
0802f2c8  b6eec63a  vrintz.f32	s6, s12
0802f2cc  36ee433a  vsub.f32	s6, s12, s6
0802f2d0  504e      ldr	r6, [pc, #320] ; [0x0802f414] = 0x20002940
0802f2d2  36ee267a  vadd.f32	s14, s12, s13
0802f2d6  b6eec42a  vrintz.f32	s4, s8
0802f2da  74ee266a  vadd.f32	s13, s8, s13
0802f2de  0e96      str	r6, [sp, #56]
0802f2e0  34ee422a  vsub.f32	s4, s8, s4
0802f2e4  f6eec74a  vrintz.f32	s9, s14
0802f2e8  77ee644a  vsub.f32	s9, s14, s9
0802f2ec  bdeec77a  vcvt.s32.f32	s14, s14
0802f2f0  f6eee65a  vrintz.f32	s11, s13
0802f2f4  76eee55a  vsub.f32	s11, s13, s11
0802f2f8  bdeec66a  vcvt.s32.f32	s12, s12
0802f2fc  17ee101a  vmov	r1, s14
0802f300  bdeee67a  vcvt.s32.f32	s14, s13
0802f304  481c      adds	r0, r1, #1
0802f306  c1f30c01  ubfx	r1, r1, #0, #13
0802f30a  17ee102a  vmov	r2, s14
0802f30e  bdeec47a  vcvt.s32.f32	s14, s8
0802f312  c0f30c00  ubfx	r0, r0, #0, #13
0802f316  0beb8101  add.w	r1, r11, r1, lsl #2
0802f31a  0beb8000  add.w	r0, r11, r0, lsl #2
0802f31e  17ee103a  vmov	r3, s14
0802f322  d1ed006a  vldr	s13, [r1]
0802f326  511c      adds	r1, r2, #1
0802f328  90ed007a  vldr	s14, [r0]
0802f32c  c2f30c02  ubfx	r2, r2, #0, #13
0802f330  c1f30c01  ubfx	r1, r1, #0, #13
0802f334  0d98      ldr	r0, [sp, #52]
0802f336  37ee667a  vsub.f32	s14, s14, s13
0802f33a  0beb8202  add.w	r2, r11, r2, lsl #2
0802f33e  0beb8101  add.w	r1, r11, r1, lsl #2
0802f342  0d95      str	r5, [sp, #52]
0802f344  e7ee246a  vfma.f32	s13, s14, s9
0802f348  92ed007a  vldr	s14, [r2]
0802f34c  5a1c      adds	r2, r3, #1
0802f34e  91ed004a  vldr	s8, [r1]
0802f352  c3f30c03  ubfx	r3, r3, #0, #13
0802f356  16ee101a  vmov	r1, s12
0802f35a  c2f30c02  ubfx	r2, r2, #0, #13
0802f35e  34ee474a  vsub.f32	s8, s8, s14
0802f362  0beb8303  add.w	r3, r11, r3, lsl #2
0802f366  0beb8202  add.w	r2, r11, r2, lsl #2
0802f36a  a4ee257a  vfma.f32	s14, s8, s11
0802f36e  d3ed005a  vldr	s11, [r3]
0802f372  92ed006a  vldr	s12, [r2]
0802f376  4a1c      adds	r2, r1, #1
0802f378  0c9b      ldr	r3, [sp, #48]
0802f37a  76eea64a  vadd.f32	s9, s13, s13
0802f37e  36ee656a  vsub.f32	s12, s12, s11
0802f382  c2f30c02  ubfx	r2, r2, #0, #13
0802f386  c0ed006a  vstr	s13, [r0]
0802f38a  0beb8202  add.w	r2, r11, r2, lsl #2
0802f38e  e6eea41a  vfma.f32	s3, s13, s9
0802f392  e6ee025a  vfma.f32	s11, s12, s4
0802f396  84ed007a  vstr	s14, [r4]
0802f39a  37ee074a  vadd.f32	s8, s14, s14
0802f39e  92ed001a  vldr	s2, [r2]
0802f3a2  a7ee045a  vfma.f32	s10, s14, s8
0802f3a6  37e0      b	#110 ; -> 0x0802f418 ; branch_target=0x0802f418
0802f418  c0ed011a  vstr	s3, [r0, #4]
0802f41c  25ee842a  vmul.f32	s4, s11, s8
0802f420  c3ed005a  vstr	s11, [r3]
0802f424  83ed012a  vstr	s4, [r3, #4]
0802f428  c1f30c03  ubfx	r3, r1, #0, #13
0802f42c  84ed015a  vstr	s10, [r4, #4]
0802f430  3146      mov	r1, r6
0802f432  0beb8303  add.w	r3, r11, r3, lsl #2
0802f436  ce4c      ldr	r4, [pc, #824] ; [0x0802f770] = 0x20002540
0802f438  93ed006a  vldr	s12, [r3]
0802f43c  2246      mov	r2, r4
0802f43e  0b9b      ldr	r3, [sp, #44]
0802f440  31ee461a  vsub.f32	s2, s2, s12
0802f444  0b94      str	r4, [sp, #44]
0802f446  a1ee036a  vfma.f32	s12, s2, s6
0802f44a  26ee243a  vmul.f32	s6, s12, s9
0802f44e  83ed006a  vstr	s12, [r3]
0802f452  83ed013a  vstr	s6, [r3, #4]
0802f456  c74b      ldr	r3, [pc, #796] ; [0x0802f774] = 0x20002840
0802f458  1846      mov	r0, r3
0802f45a  1f46      mov	r7, r3
0802f45c  0c93      str	r3, [sp, #48]
0802f45e  2b46      mov	r3, r5
0802f460  d4eea16a  vfnms.f32	s13, s9, s3
0802f464  0833      adds	r3, #8
0802f466  94ee836a  vfnms.f32	s12, s9, s6
0802f46a  0832      adds	r2, #8
0802f46c  95ee047a  vfnms.f32	s14, s10, s8
0802f470  0831      adds	r1, #8
0802f472  d2ee045a  vfnms.f32	s11, s4, s8
0802f476  0830      adds	r0, #8
0802f478  d6eea41a  vfnms.f32	s3, s13, s9
0802f47c  c3ed006a  vstr	s13, [r3]
0802f480  96ee243a  vfnms.f32	s6, s12, s9
0802f484  82ed006a  vstr	s12, [r2]
0802f488  97ee045a  vfnms.f32	s10, s14, s8
0802f48c  81ed007a  vstr	s14, [r1]
0802f490  95ee842a  vfnms.f32	s4, s11, s8
0802f494  c0ed005a  vstr	s11, [r0]
0802f498  c3ed011a  vstr	s3, [r3, #4]
0802f49c  9a45      cmp	r10, r3
0802f49e  82ed013a  vstr	s6, [r2, #4]
0802f4a2  81ed015a  vstr	s10, [r1, #4]
0802f4a6  80ed012a  vstr	s4, [r0, #4]
0802f4aa  d9d1      bne	#-78 ; -> 0x0802f460 ; branch_target=0x0802f460
0802f4ac  b24a      ldr	r2, [pc, #712] ; [0x0802f778] = 0x20002e6c
0802f4ae  b2ee087a  vmov.f32	s14, #1.200000e+01
0802f4b2  b249      ldr	r1, [pc, #712] ; [0x0802f77c] = 0x20001160
0802f4b4  92ed006a  vldr	s12, [r2]
0802f4b8  d1ed006a  vldr	s13, [r1]
0802f4bc  29ee077a  vmul.f32	s14, s18, s14
0802f4c0  dfedaf5a  vldr	s11, [pc, #700] ; [0x0802f780] = 0x3f7eb852 / f32_bits_interpretation=0.9950000048
0802f4c4  159b      ldr	r3, [sp, #84]
0802f4c6  96eea56a  vfnms.f32	s12, s13, s11
0802f4ca  93ed005a  vldr	s10, [r3]
0802f4ce  ad4b      ldr	r3, [pc, #692] ; [0x0802f784] = 0xe0001000
0802f4d0  5b68      ldr	r3, [r3, #4]
0802f4d2  82ed007a  vstr	s14, [r2]
0802f4d6  37ee066a  vadd.f32	s12, s14, s12
0802f4da  9fedab7a  vldr	s14, [pc, #684] ; [0x0802f788] = 0x3de66666 / f32_bits_interpretation=0.112499997
0802f4de  029a      ldr	r2, [sp, #8]
0802f4e0  b4eec75a  vcmpe.f32	s10, s14
0802f4e4  9b1a      subs	r3, r3, r2
0802f4e6  a94a      ldr	r2, [pc, #676] ; [0x0802f78c] = 0x2000227c
0802f4e8  81ed006a  vstr	s12, [r1]
0802f4ec  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802f4f0  1361      str	r3, [r2, #16]
0802f4f2  40f11081  bpl.w	#544 ; -> 0x0802f716 ; branch_target=0x0802f716
0802f4f6  f1ee465a  vneg.f32	s11, s12
0802f4fa  9feda54a  vldr	s8, [pc, #660] ; [0x0802f790] = 0x3ee66666 / f32_bits_interpretation=0.4499999881
0802f4fe  a548      ldr	r0, [pc, #660] ; [0x0802f794] = 0x20000620
0802f500  4ff00808  mov.w	r8, #8
0802f504  a449      ldr	r1, [pc, #656] ; [0x0802f798] = 0x20000420
0802f506  dff8b0e2  ldr.w	lr, [pc, #688] ; [0x0802f7b8] = 0x20002c40
0802f50a  dff8b0c2  ldr.w	r12, [pc, #688] ; [0x0802f7bc] = 0x20000f40
0802f50e  dfeda34a  vldr	s9, [pc, #652] ; [0x0802f79c] = 0x00000000
0802f512  96ed017a  vldr	s14, [r6, #4]
0802f516  1037      adds	r7, #16
0802f518  90ed022a  vldr	s4, [r0, #8]
0802f51c  1036      adds	r6, #16
0802f51e  57ed040a  vldr	s1, [r7, #-16]
0802f522  0cf1100c  add.w	r12, r12, #16
0802f526  a7ee252a  vfma.f32	s4, s14, s11
0802f52a  d1ed006a  vldr	s13, [r1]
0802f52e  16ed043a  vldr	s6, [r6, #-16]
0802f532  2030      adds	r0, #32
0802f534  50ed081a  vldr	s3, [r0, #-32]
0802f538  e0eea56a  vfma.f32	s13, s1, s11
0802f53c  17ed030a  vldr	s0, [r7, #-12]
0802f540  2031      adds	r1, #32
0802f542  e3ee251a  vfma.f32	s3, s6, s11
0802f546  10ed071a  vldr	s2, [r0, #-28]
0802f54a  0ef1100e  add.w	lr, lr, #16
0802f54e  2bee822a  vmul.f32	s4, s23, s4
0802f552  6beea66a  vmul.f32	s13, s23, s13
0802f556  a6ee072a  vfma.f32	s4, s12, s14
0802f55a  11ed067a  vldr	s14, [r1, #-24]
0802f55e  6beea11a  vmul.f32	s3, s23, s3
0802f562  a0ee257a  vfma.f32	s14, s0, s11
0802f566  e6ee206a  vfma.f32	s13, s12, s1
0802f56a  51ed070a  vldr	s1, [r1, #-28]
0802f56e  e6ee031a  vfma.f32	s3, s12, s6
0802f572  10ed053a  vldr	s6, [r0, #-20]
0802f576  33ee423a  vsub.f32	s6, s6, s4
0802f57a  00ed062a  vstr	s4, [r0, #-24]
0802f57e  2bee877a  vmul.f32	s14, s23, s14
0802f582  abee832a  vfma.f32	s4, s23, s6
0802f586  11ed053a  vldr	s6, [r1, #-20]
0802f58a  70eee60a  vsub.f32	s1, s1, s13
0802f58e  41ed086a  vstr	s13, [r1, #-32]
0802f592  a6ee007a  vfma.f32	s14, s12, s0
0802f596  40ed081a  vstr	s3, [r0, #-32]
0802f59a  31ee611a  vsub.f32	s2, s2, s3
0802f59e  17ed020a  vldr	s0, [r7, #-8]
0802f5a2  ebeea06a  vfma.f32	s13, s23, s1
0802f5a6  56ed020a  vldr	s1, [r6, #-8]
0802f5aa  ebee811a  vfma.f32	s3, s23, s2
0802f5ae  10ed031a  vldr	s2, [r0, #-12]
0802f5b2  00ed052a  vstr	s4, [r0, #-20]
0802f5b6  33ee473a  vsub.f32	s6, s6, s14
0802f5ba  01ed067a  vstr	s14, [r1, #-24]
0802f5be  41ed076a  vstr	s13, [r1, #-28]
0802f5c2  66eea66a  vmul.f32	s13, s13, s13
0802f5c6  abee837a  vfma.f32	s14, s23, s6
0802f5ca  10ed043a  vldr	s6, [r0, #-16]
0802f5ce  40ed071a  vstr	s3, [r0, #-28]
0802f5d2  e1eea16a  vfma.f32	s13, s3, s3
0802f5d6  01ee908a  vmov	s3, r8
0802f5da  a0eea53a  vfma.f32	s6, s1, s11
0802f5de  f8eee11a  vcvt.f32.s32	s3, s3
0802f5e2  01ed057a  vstr	s14, [r1, #-20]
0802f5e6  27ee077a  vmul.f32	s14, s14, s14
0802f5ea  61ee851a  vmul.f32	s3, s3, s10
0802f5ee  16ee903a  vmov	r3, s13
0802f5f2  50ed026a  vldr	s13, [r0, #-8]
0802f5f6  a2ee027a  vfma.f32	s14, s4, s4
0802f5fa  2bee833a  vmul.f32	s6, s23, s6
0802f5fe  a3f50002  sub.w	r2, r3, #8388608
0802f602  f4eec41a  vcmpe.f32	s3, s8
0802f606  51ed031a  vldr	s3, [r1, #-12]
0802f60a  5208      lsrs	r2, r2, #1
0802f60c  a6ee203a  vfma.f32	s6, s12, s1
0802f610  56ed010a  vldr	s1, [r6, #-4]
0802f614  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802f618  02f10052  add.w	r2, r2, #536870912
0802f61c  17ee103a  vmov	r3, s14
0802f620  11ed047a  vldr	s14, [r1, #-16]
0802f624  e0eea56a  vfma.f32	s13, s1, s11
0802f628  a0ee257a  vfma.f32	s14, s0, s11
0802f62c  a3f50003  sub.w	r3, r3, #8388608
0802f630  31ee431a  vsub.f32	s2, s2, s6
0802f634  00ed043a  vstr	s6, [r0, #-16]
0802f638  4fea5303  lsr.w	r3, r3, #1
0802f63c  abee813a  vfma.f32	s6, s23, s2
0802f640  03f10053  add.w	r3, r3, #536870912
0802f644  2bee877a  vmul.f32	s14, s23, s14
0802f648  6beea66a  vmul.f32	s13, s23, s13
0802f64c  a6ee007a  vfma.f32	s14, s12, s0
0802f650  e6ee206a  vfma.f32	s13, s12, s1
0802f654  b0ee432a  vmov.f32	s4, s6
0802f658  00ed033a  vstr	s6, [r0, #-12]
0802f65c  31eec73a  vsub.f32	s6, s3, s14
0802f660  01ed047a  vstr	s14, [r1, #-16]
0802f664  abee837a  vfma.f32	s14, s23, s6
0802f668  01ed037a  vstr	s14, [r1, #-12]
0802f66c  27ee077a  vmul.f32	s14, s14, s14
0802f670  40ed026a  vstr	s13, [r0, #-8]
0802f674  57ed010a  vldr	s1, [r7, #-4]
0802f678  11ed023a  vldr	s6, [r1, #-8]
0802f67c  a2ee027a  vfma.f32	s14, s4, s4
0802f680  51ed011a  vldr	s3, [r1, #-4]
0802f684  a0eea53a  vfma.f32	s6, s1, s11
0802f688  cef8f430  str.w	r3, [lr, #244]
0802f68c  10ed011a  vldr	s2, [r0, #-4]
0802f690  cef8f020  str.w	r2, [lr, #240]
0802f694  48bf      it	mi
0802f696  0122      movmi	r2, #1
0802f698  31ee661a  vsub.f32	s2, s2, s13
0802f69c  58bf      it	pl
0802f69e  0022      movpl	r2, #0
0802f6a0  17ee103a  vmov	r3, s14
0802f6a4  cced3f4a  vstr	s9, [r12, #252]
0802f6a8  b8f1400f  cmp.w	r8, #64
0802f6ac  0cbf      ite	eq
0802f6ae  0022      moveq	r2, #0
0802f6b0  02f00102  andne	r2, r2, #1
0802f6b4  cced3e4a  vstr	s9, [r12, #248]
0802f6b8  2bee833a  vmul.f32	s6, s23, s6
0802f6bc  a3f50003  sub.w	r3, r3, #8388608
0802f6c0  ebee816a  vfma.f32	s13, s23, s2
0802f6c4  cced3d4a  vstr	s9, [r12, #244]
0802f6c8  5b08      lsrs	r3, r3, #1
0802f6ca  cced3c4a  vstr	s9, [r12, #240]
0802f6ce  a6ee203a  vfma.f32	s6, s12, s1
0802f6d2  08f10408  add.w	r8, r8, #4
0802f6d6  03f10053  add.w	r3, r3, #536870912
0802f6da  cef8f830  str.w	r3, [lr, #248]
0802f6de  40ed016a  vstr	s13, [r0, #-4]
0802f6e2  31eec32a  vsub.f32	s4, s3, s6
0802f6e6  01ed023a  vstr	s6, [r1, #-8]
0802f6ea  b0ee437a  vmov.f32	s14, s6
0802f6ee  abee827a  vfma.f32	s14, s23, s4
0802f6f2  27ee073a  vmul.f32	s6, s14, s14
0802f6f6  01ed017a  vstr	s14, [r1, #-4]
0802f6fa  a6eea63a  vfma.f32	s6, s13, s13
0802f6fe  13ee103a  vmov	r3, s6
0802f702  a3f50003  sub.w	r3, r3, #8388608
0802f706  5b08      lsrs	r3, r3, #1
0802f708  03f10053  add.w	r3, r3, #536870912
0802f70c  cef8fc30  str.w	r3, [lr, #252]
0802f710  002a      cmp	r2, #0
0802f712  7ff4feae  bne.w	#-516 ; -> 0x0802f512 ; branch_target=0x0802f512
0802f716  224b      ldr	r3, [pc, #136] ; [0x0802f7a0] = 0x20002e74
0802f718  b2ee086a  vmov.f32	s12, #1.200000e+01
0802f71c  214a      ldr	r2, [pc, #132] ; [0x0802f7a4] = 0x20002e70
0802f71e  93ed007a  vldr	s14, [r3]
0802f722  d2ed006a  vldr	s13, [r2]
0802f726  28ee066a  vmul.f32	s12, s16, s12
0802f72a  dfed154a  vldr	s9, [pc, #84] ; [0x0802f780] = 0x3f7eb852 / f32_bits_interpretation=0.9950000048
0802f72e  1499      ldr	r1, [sp, #80]
0802f730  96eea47a  vfnms.f32	s14, s13, s9
0802f734  83ed006a  vstr	s12, [r3]
0802f738  d1ed005a  vldr	s11, [r1]
0802f73c  36ee076a  vadd.f32	s12, s12, s14
0802f740  9fed117a  vldr	s14, [pc, #68] ; [0x0802f788] = 0x3de66666 / f32_bits_interpretation=0.112499997
0802f744  f4eec75a  vcmpe.f32	s11, s14
0802f748  82ed006a  vstr	s12, [r2]
0802f74c  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802f750  40f13481  bpl.w	#616 ; -> 0x0802f9bc ; branch_target=0x0802f9bc
0802f754  f1ee464a  vneg.f32	s9, s12
0802f758  9fed0d4a  vldr	s8, [pc, #52] ; [0x0802f790] = 0x3ee66666 / f32_bits_interpretation=0.4499999881
0802f75c  1248      ldr	r0, [pc, #72] ; [0x0802f7a8] = 0x20000220
0802f75e  4ff0080c  mov.w	r12, #8
0802f762  1249      ldr	r1, [pc, #72] ; [0x0802f7ac] = 0x20000020
0802f764  4ff0000e  mov.w	lr, #0
0802f768  114f      ldr	r7, [pc, #68] ; [0x0802f7b0] = 0x20002a40
0802f76a  124e      ldr	r6, [pc, #72] ; [0x0802f7b4] = 0x20000d40
0802f76c  28e0      b	#80 ; -> 0x0802f7c0 ; branch_target=0x0802f7c0
0802f7c0  95ed017a  vldr	s14, [r5, #4]
0802f7c4  1034      adds	r4, #16
0802f7c6  90ed023a  vldr	s6, [r0, #8]
0802f7ca  1035      adds	r5, #16
0802f7cc  54ed040a  vldr	s1, [r4, #-16]
0802f7d0  1036      adds	r6, #16
0802f7d2  a7ee243a  vfma.f32	s6, s14, s9
0802f7d6  d1ed006a  vldr	s13, [r1]
0802f7da  15ed042a  vldr	s4, [r5, #-16]
0802f7de  2030      adds	r0, #32
0802f7e0  50ed081a  vldr	s3, [r0, #-32]
0802f7e4  e0eea46a  vfma.f32	s13, s1, s9
0802f7e8  14ed030a  vldr	s0, [r4, #-12]
0802f7ec  2031      adds	r1, #32
0802f7ee  e2ee241a  vfma.f32	s3, s4, s9
0802f7f2  10ed071a  vldr	s2, [r0, #-28]
0802f7f6  1037      adds	r7, #16
0802f7f8  2bee033a  vmul.f32	s6, s22, s6
0802f7fc  6bee266a  vmul.f32	s13, s22, s13
0802f800  a6ee073a  vfma.f32	s6, s12, s14
0802f804  11ed067a  vldr	s14, [r1, #-24]
0802f808  6bee211a  vmul.f32	s3, s22, s3
0802f80c  a0ee247a  vfma.f32	s14, s0, s9
0802f810  e6ee206a  vfma.f32	s13, s12, s1
0802f814  51ed070a  vldr	s1, [r1, #-28]
0802f818  e6ee021a  vfma.f32	s3, s12, s4
0802f81c  10ed052a  vldr	s4, [r0, #-20]
0802f820  32ee432a  vsub.f32	s4, s4, s6
0802f824  00ed063a  vstr	s6, [r0, #-24]
0802f828  2bee077a  vmul.f32	s14, s22, s14
0802f82c  abee023a  vfma.f32	s6, s22, s4
0802f830  11ed052a  vldr	s4, [r1, #-20]
0802f834  70eee60a  vsub.f32	s1, s1, s13
0802f838  41ed086a  vstr	s13, [r1, #-32]
0802f83c  a6ee007a  vfma.f32	s14, s12, s0
0802f840  40ed081a  vstr	s3, [r0, #-32]
0802f844  31ee611a  vsub.f32	s2, s2, s3
0802f848  14ed020a  vldr	s0, [r4, #-8]
0802f84c  ebee206a  vfma.f32	s13, s22, s1
0802f850  55ed020a  vldr	s1, [r5, #-8]
0802f854  ebee011a  vfma.f32	s3, s22, s2
0802f858  00ed053a  vstr	s6, [r0, #-20]
0802f85c  10ed031a  vldr	s2, [r0, #-12]
0802f860  32ee472a  vsub.f32	s4, s4, s14
0802f864  01ed067a  vstr	s14, [r1, #-24]
0802f868  41ed076a  vstr	s13, [r1, #-28]
0802f86c  66eea66a  vmul.f32	s13, s13, s13
0802f870  abee027a  vfma.f32	s14, s22, s4
0802f874  10ed042a  vldr	s4, [r0, #-16]
0802f878  40ed071a  vstr	s3, [r0, #-28]
0802f87c  e1eea16a  vfma.f32	s13, s3, s3
0802f880  01ee90ca  vmov	s3, r12
0802f884  a0eea42a  vfma.f32	s4, s1, s9
0802f888  f8eee11a  vcvt.f32.s32	s3, s3
0802f88c  01ed057a  vstr	s14, [r1, #-20]
0802f890  27ee077a  vmul.f32	s14, s14, s14
0802f894  61eea51a  vmul.f32	s3, s3, s11
0802f898  16ee903a  vmov	r3, s13
0802f89c  50ed026a  vldr	s13, [r0, #-8]
0802f8a0  a3ee037a  vfma.f32	s14, s6, s6
0802f8a4  11ed033a  vldr	s6, [r1, #-12]
0802f8a8  a3f50002  sub.w	r2, r3, #8388608
0802f8ac  2bee022a  vmul.f32	s4, s22, s4
0802f8b0  f4eec41a  vcmpe.f32	s3, s8
0802f8b4  5208      lsrs	r2, r2, #1
0802f8b6  a6ee202a  vfma.f32	s4, s12, s1
0802f8ba  55ed010a  vldr	s1, [r5, #-4]
0802f8be  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802f8c2  02f10052  add.w	r2, r2, #536870912
0802f8c6  17ee103a  vmov	r3, s14
0802f8ca  11ed047a  vldr	s14, [r1, #-16]
0802f8ce  e0eea46a  vfma.f32	s13, s1, s9
0802f8d2  a0ee247a  vfma.f32	s14, s0, s9
0802f8d6  a3f50003  sub.w	r3, r3, #8388608
0802f8da  71ee421a  vsub.f32	s3, s2, s4
0802f8de  00ed042a  vstr	s4, [r0, #-16]
0802f8e2  4fea5303  lsr.w	r3, r3, #1
0802f8e6  6bee266a  vmul.f32	s13, s22, s13
0802f8ea  03f10053  add.w	r3, r3, #536870912
0802f8ee  2bee077a  vmul.f32	s14, s22, s14
0802f8f2  abee212a  vfma.f32	s4, s22, s3
0802f8f6  e6ee206a  vfma.f32	s13, s12, s1
0802f8fa  a6ee007a  vfma.f32	s14, s12, s0
0802f8fe  00ed032a  vstr	s4, [r0, #-12]
0802f902  33ee473a  vsub.f32	s6, s6, s14
0802f906  01ed047a  vstr	s14, [r1, #-16]
0802f90a  abee037a  vfma.f32	s14, s22, s6
0802f90e  01ed037a  vstr	s14, [r1, #-12]
0802f912  27ee077a  vmul.f32	s14, s14, s14
0802f916  40ed026a  vstr	s13, [r0, #-8]
0802f91a  54ed010a  vldr	s1, [r4, #-4]
0802f91e  11ed023a  vldr	s6, [r1, #-8]
0802f922  a2ee027a  vfma.f32	s14, s4, s4
0802f926  51ed011a  vldr	s3, [r1, #-4]
0802f92a  a0eea43a  vfma.f32	s6, s1, s9
0802f92e  c7f8f430  str.w	r3, [r7, #244]
0802f932  10ed011a  vldr	s2, [r0, #-4]
0802f936  c7f8f020  str.w	r2, [r7, #240]
0802f93a  48bf      it	mi
0802f93c  0122      movmi	r2, #1
0802f93e  31ee661a  vsub.f32	s2, s2, s13
0802f942  58bf      it	pl
0802f944  0022      movpl	r2, #0
0802f946  17ee103a  vmov	r3, s14
0802f94a  c6f8fce0  str.w	lr, [r6, #252]
0802f94e  bcf1400f  cmp.w	r12, #64
0802f952  0cbf      ite	eq
0802f954  0022      moveq	r2, #0
0802f956  02f00102  andne	r2, r2, #1
0802f95a  c6f8f8e0  str.w	lr, [r6, #248]
0802f95e  2bee033a  vmul.f32	s6, s22, s6
0802f962  a3f50003  sub.w	r3, r3, #8388608
0802f966  ebee016a  vfma.f32	s13, s22, s2
0802f96a  c6f8f4e0  str.w	lr, [r6, #244]
0802f96e  5b08      lsrs	r3, r3, #1
0802f970  c6f8f0e0  str.w	lr, [r6, #240]
0802f974  a6ee203a  vfma.f32	s6, s12, s1
0802f978  0cf1040c  add.w	r12, r12, #4
0802f97c  03f10053  add.w	r3, r3, #536870912
0802f980  c7f8f830  str.w	r3, [r7, #248]
0802f984  40ed016a  vstr	s13, [r0, #-4]
0802f988  31eec32a  vsub.f32	s4, s3, s6
0802f98c  01ed023a  vstr	s6, [r1, #-8]
0802f990  b0ee437a  vmov.f32	s14, s6
0802f994  abee027a  vfma.f32	s14, s22, s4
0802f998  27ee073a  vmul.f32	s6, s14, s14
0802f99c  01ed017a  vstr	s14, [r1, #-4]
0802f9a0  a6eea63a  vfma.f32	s6, s13, s13
0802f9a4  13ee103a  vmov	r3, s6
0802f9a8  a3f50003  sub.w	r3, r3, #8388608
0802f9ac  5b08      lsrs	r3, r3, #1
0802f9ae  03f10053  add.w	r3, r3, #536870912
0802f9b2  c7f8fc30  str.w	r3, [r7, #252]
0802f9b6  002a      cmp	r2, #0
0802f9b8  7ff402af  bne.w	#-508 ; -> 0x0802f7c0 ; branch_target=0x0802f7c0
0802f9bc  e84b      ldr	r3, [pc, #928] ; [0x0802fd60] = 0x2000241c
0802f9be  029a      ldr	r2, [sp, #8]
0802f9c0  93ed009a  vldr	s18, [r3]
0802f9c4  e74b      ldr	r3, [pc, #924] ; [0x0802fd64] = 0x20000900
0802f9c6  0699      ldr	r1, [sp, #24]
0802f9c8  93ed008a  vldr	s16, [r3]
0802f9cc  e64b      ldr	r3, [pc, #920] ; [0x0802fd68] = 0x20000860
0802f9ce  d3ed001a  vldr	s3, [r3]
0802f9d2  e64b      ldr	r3, [pc, #920] ; [0x0802fd6c] = 0xe0001000
0802f9d4  5b68      ldr	r3, [r3, #4]
0802f9d6  9b1a      subs	r3, r3, r2
0802f9d8  e54a      ldr	r2, [pc, #916] ; [0x0802fd70] = 0x2000227c
0802f9da  d360      str	r3, [r2, #12]
0802f9dc  e54a      ldr	r2, [pc, #916] ; [0x0802fd74] = 0x20002e8c
0802f9de  1368      ldr	r3, [r2]
0802f9e0  01fb03f3  mul	r3, r1, r3
0802f9e4  1360      str	r3, [r2]
0802f9e6  002b      cmp	r3, #0
0802f9e8  01f07481  beq.w	#4840 ; -> 0x08030cd4 ; branch_target=0x08030cd4
0802f9ec  e24a      ldr	r2, [pc, #904] ; [0x0802fd78] = 0x20000880
0802f9ee  012b      cmp	r3, #1
0802f9f0  d2ed000a  vldr	s1, [r2]
0802f9f4  219a      ldr	r2, [sp, #132]
0802f9f6  92ed002a  vldr	s4, [r2]
0802f9fa  e04a      ldr	r2, [pc, #896] ; [0x0802fd7c] = 0x20002e58
0802f9fc  92ed003a  vldr	s6, [r2]
0802fa00  01f01a83  beq.w	#5684 ; -> 0x08031038 ; branch_target=0x08031038
0802fa04  f7ee006a  vmov.f32	s13, #1.000000e+00
0802fa08  dfeddd4a  vldr	s9, [pc, #884] ; [0x0802fd80] = 0x411e6666 / f32_bits_interpretation=9.899999619
0802fa0c  b3ee054a  vmov.f32	s8, #2.100000e+01
0802fa10  9feddc6a  vldr	s12, [pc, #880] ; [0x0802fd84] = 0x3e4ccccd / f32_bits_interpretation=0.200000003
0802fa14  60eea40a  vmul.f32	s1, s1, s9
0802fa18  b0ee667a  vmov.f32	s14, s13
0802fa1c  22ee062a  vmul.f32	s4, s4, s12
0802fa20  b6ee006a  vmov.f32	s12, #5.000000e-01
0802fa24  a3ee047a  vfma.f32	s14, s6, s8
0802fa28  22ee202a  vmul.f32	s4, s4, s1
0802fa2c  f0ee474a  vmov.f32	s9, s14
0802fa30  bdeec77a  vcvt.s32.f32	s14, s14
0802fa34  feeeca4a  vcvt.s32.f32	s9, s9, #12
0802fa38  17ee102a  vmov	r2, s14
0802fa3c  14ee903a  vmov	r3, s9
0802fa40  d007      lsls	r0, r2, #31
0802fa42  02f10101  add.w	r1, r2, #1
0802fa46  03f50063  add.w	r3, r3, #2048
0802fa4a  c3f30c03  ubfx	r3, r3, #0, #13
0802fa4e  0beb8303  add.w	r3, r11, r3, lsl #2
0802fa52  93ed000a  vldr	s0, [r3]
0802fa56  30ee260a  vadd.f32	s0, s0, s13
0802fa5a  20ee060a  vmul.f32	s0, s0, s12
0802fa5e  02d5      bpl	#4 ; -> 0x0802fa66 ; branch_target=0x0802fa66
0802fa60  0b46      mov	r3, r1
0802fa62  1146      mov	r1, r2
0802fa64  1a46      mov	r2, r3
0802fa66  c84b      ldr	r3, [pc, #800] ; [0x0802fd88] = 0x20001140
0802fa68  06ee101a  vmov	s12, r1
0802fa6c  f0ee61fa  vmov.f32	s31, s3
0802fa70  9ded073a  vldr	s6, [sp, #28]
0802fa74  d3ed006a  vldr	s13, [r3]
0802fa78  f8eec64a  vcvt.f32.s32	s9, s12
0802fa7c  c34b      ldr	r3, [pc, #780] ; [0x0802fd8c] = 0x200022bc
0802fa7e  f0ee00da  vmov.f32	s27, #2.000000e+00
0802fa82  26eea26a  vmul.f32	s12, s13, s5
0802fa86  9fedc21a  vldr	s2, [pc, #776] ; [0x0802fd90] = 0x3dcccccd / f32_bits_interpretation=0.1000000015
0802fa8a  66eea16a  vmul.f32	s13, s13, s3
0802fa8e  d3ed00aa  vldr	s21, [r3]
0802fa92  b5ee004a  vmov.f32	s8, #2.500000e-01
0802fa96  07ee102a  vmov	s14, r2
0802fa9a  26ee03ca  vmul.f32	s24, s12, s6
0802fa9e  93ed0b6a  vldr	s12, [r3, #44]
0802faa2  e3ee26fa  vfma.f32	s31, s6, s13
0802faa6  d3ed076a  vldr	s13, [r3, #28]
0802faaa  b8eec77a  vcvt.f32.s32	s14, s14
0802faae  93ed0ffa  vldr	s30, [r3, #60]
0802fab2  66ee816a  vmul.f32	s13, s13, s2
0802fab6  3cee22da  vadd.f32	s26, s24, s5
0802faba  24eeae3a  vmul.f32	s6, s9, s29
0802fabe  e6ee2d6a  vfma.f32	s13, s12, s27
0802fac2  93ed086a  vldr	s12, [r3, #32]
0802fac6  6dee048a  vmul.f32	s17, s26, s8
0802faca  36ee0d6a  vadd.f32	s12, s12, s26
0802face  2fee844a  vmul.f32	s8, s31, s8
0802fad2  7aeea8aa  vadd.f32	s21, s21, s17
0802fad6  20ee8dda  vmul.f32	s26, s1, s26
0802fada  62ee266a  vmul.f32	s13, s4, s13
0802fade  60eeaf0a  vmul.f32	s1, s1, s31
0802fae2  22ee042a  vmul.f32	s4, s4, s8
0802fae6  e8eea6aa  vfma.f32	s21, s17, s13
0802faea  d3ed0c6a  vldr	s13, [r3, #48]
0802faee  d3ed048a  vldr	s17, [r3, #16]
0802faf2  21ee4fea  vnmul.f32	s28, s2, s30
0802faf6  76eeaf6a  vadd.f32	s13, s13, s31
0802fafa  78ee848a  vadd.f32	s17, s17, s8
0802fafe  27ee2e4a  vmul.f32	s8, s14, s29
0802fb02  f6eeeafa  vrintz.f32	s31, s21
0802fb06  7aeeeffa  vsub.f32	s31, s21, s31
0802fb0a  6feea44a  vmul.f32	s9, s31, s9
0802fb0e  c3ed00fa  vstr	s31, [r3]
0802fb12  2fee877a  vmul.f32	s14, s31, s14
0802fb16  feeee94a  vcvt.s32.f32	s9, s9, #13
0802fb1a  beeee97a  vcvt.s32.f32	s14, s14, #13
0802fb1e  14ee902a  vmov	r2, s9
0802fb22  17ee101a  vmov	r1, s14
0802fb26  c2f30c02  ubfx	r2, r2, #0, #13
0802fb2a  c1f30c01  ubfx	r1, r1, #0, #13
0802fb2e  0beb8202  add.w	r2, r11, r2, lsl #2
0802fb32  0beb8101  add.w	r1, r11, r1, lsl #2
0802fb36  92ed007a  vldr	s14, [r2]
0802fb3a  d1ed004a  vldr	s9, [r1]
0802fb3e  74eec74a  vsub.f32	s9, s9, s14
0802fb42  a4ee807a  vfma.f32	s14, s9, s0
0802fb46  dfed994a  vldr	s9, [pc, #612] ; [0x0802fdac] = 0x3e99999a / f32_bits_interpretation=0.3000000119
0802fb4a  a7ee2dea  vfma.f32	s28, s14, s27
0802fb4e  83ed037a  vstr	s14, [r3, #12]
0802fb52  61ee47aa  vnmul.f32	s21, s2, s14
0802fb56  27ee247a  vmul.f32	s14, s14, s9
0802fb5a  aeee0d6a  vfma.f32	s12, s28, s26
0802fb5e  9fed94da  vldr	s26, [pc, #592] ; [0x0802fdb0] = 0x3f19999a / f32_bits_interpretation=0.6000000238
0802fb62  b6eec6ea  vrintz.f32	s28, s12
0802fb66  36ee4e6a  vsub.f32	s12, s12, s28
0802fb6a  83ed086a  vstr	s12, [r3, #32]
0802fb6e  beeee96a  vcvt.s32.f32	s12, s12, #13
0802fb72  16ee102a  vmov	r2, s12
0802fb76  c2f30c02  ubfx	r2, r2, #0, #13
0802fb7a  0beb8202  add.w	r2, r11, r2, lsl #2
0802fb7e  92ed006a  vldr	s12, [r2]
0802fb82  26ee011a  vmul.f32	s2, s12, s2
0802fb86  83ed0b6a  vstr	s12, [r3, #44]
0802fb8a  a6ee0d7a  vfma.f32	s14, s12, s26
0802fb8e  afee2d1a  vfma.f32	s2, s30, s27
0802fb92  e1ee028a  vfma.f32	s17, s2, s4
0802fb96  b6eee86a  vrintz.f32	s12, s17
0802fb9a  38eec66a  vsub.f32	s12, s17, s12
0802fb9e  24ee064a  vmul.f32	s8, s8, s12
0802fba2  83ed046a  vstr	s12, [r3, #16]
0802fba6  23ee063a  vmul.f32	s6, s6, s12
0802fbaa  bdeec44a  vcvt.s32.f32	s8, s8
0802fbae  bdeec36a  vcvt.s32.f32	s12, s6
0802fbb2  14ee101a  vmov	r1, s8
0802fbb6  16ee102a  vmov	r2, s12
0802fbba  c1f30c01  ubfx	r1, r1, #0, #13
0802fbbe  c2f30c02  ubfx	r2, r2, #0, #13
0802fbc2  0beb8101  add.w	r1, r11, r1, lsl #2
0802fbc6  0beb8202  add.w	r2, r11, r2, lsl #2
0802fbca  91ed006a  vldr	s12, [r1]
0802fbce  92ed004a  vldr	s8, [r2]
0802fbd2  36ee446a  vsub.f32	s12, s12, s8
0802fbd6  a6ee004a  vfma.f32	s8, s12, s0
0802fbda  e4ee2daa  vfma.f32	s21, s8, s27
0802fbde  83ed074a  vstr	s8, [r3, #28]
0802fbe2  eaeea06a  vfma.f32	s13, s21, s1
0802fbe6  b0ee666a  vmov.f32	s12, s13
0802fbea  64ee246a  vmul.f32	s13, s8, s9
0802fbee  f6eec64a  vrintz.f32	s9, s12
0802fbf2  36ee646a  vsub.f32	s12, s12, s9
0802fbf6  83ed0c6a  vstr	s12, [r3, #48]
0802fbfa  beeee96a  vcvt.s32.f32	s12, s12, #13
0802fbfe  16ee102a  vmov	r2, s12
0802fc02  c2f30c02  ubfx	r2, r2, #0, #13
0802fc06  0beb8202  add.w	r2, r11, r2, lsl #2
0802fc0a  92ed006a  vldr	s12, [r2]
0802fc0e  e6ee0d6a  vfma.f32	s13, s12, s26
0802fc12  83ed0f6a  vstr	s12, [r3, #60]
0802fc16  b7ee084a  vmov.f32	s8, #1.500000e+00
0802fc1a  9fed5e3a  vldr	s6, [pc, #376] ; [0x0802fd94] = 0x3e17b426 / f32_bits_interpretation=0.1481481493
0802fc1e  bfee086a  vmov.f32	s12, #-1.500000e+00
0802fc22  5d4b      ldr	r3, [pc, #372] ; [0x0802fd98] = 0x2000225c
0802fc24  f7ee004a  vmov.f32	s9, #1.000000e+00
0802fc28  5c49      ldr	r1, [pc, #368] ; [0x0802fd9c] = 0x20002e88
0802fc2a  87fe447a  vminnm.f32	s14, s14, s8
0802fc2e  c6fec46a  vminnm.f32	s13, s13, s8
0802fc32  87fe067a  vmaxnm.f32	s14, s14, s12
0802fc36  c6fe866a  vmaxnm.f32	s13, s13, s12
0802fc3a  b0ee644a  vmov.f32	s8, s9
0802fc3e  059a      ldr	r2, [sp, #20]
0802fc40  27ee476a  vnmul.f32	s12, s14, s14
0802fc44  0298      ldr	r0, [sp, #8]
0802fc46  26eee62a  vnmul.f32	s4, s13, s13
0802fc4a  a6ee034a  vfma.f32	s8, s12, s6
0802fc4e  b0ee646a  vmov.f32	s12, s9
0802fc52  a2ee036a  vfma.f32	s12, s4, s6
0802fc56  67ee04fa  vmul.f32	s31, s14, s8
0802fc5a  c3ed00fa  vstr	s31, [r3]
0802fc5e  26ee86fa  vmul.f32	s30, s13, s12
0802fc62  83ed01fa  vstr	s30, [r3, #4]
0802fc66  0b68      ldr	r3, [r1]
0802fc68  02fb03f3  mul	r3, r2, r3
0802fc6c  3f4a      ldr	r2, [pc, #252] ; [0x0802fd6c] = 0xe0001000
0802fc6e  5268      ldr	r2, [r2, #4]
0802fc70  0b60      str	r3, [r1]
0802fc72  121a      subs	r2, r2, r0
0802fc74  3e48      ldr	r0, [pc, #248] ; [0x0802fd70] = 0x2000227c
0802fc76  4261      str	r2, [r0, #20]
0802fc78  002b      cmp	r3, #0
0802fc7a  00f05386  beq.w	#3238 ; -> 0x08030924 ; branch_target=0x08030924
0802fc7e  484a      ldr	r2, [pc, #288] ; [0x0802fda0] = 0x200023fc
0802fc80  012b      cmp	r3, #1
0802fc82  92ed001a  vldr	s2, [r2]
0802fc86  229a      ldr	r2, [sp, #136]
0802fc88  92ed006a  vldr	s12, [r2]
0802fc8c  454a      ldr	r2, [pc, #276] ; [0x0802fda4] = 0x20002e54
0802fc8e  92ed007a  vldr	s14, [r2]
0802fc92  01f03d83  beq.w	#5754 ; -> 0x08031310 ; branch_target=0x08031310
0802fc96  dfed3a6a  vldr	s13, [pc, #232] ; [0x0802fd80] = 0x411e6666 / f32_bits_interpretation=9.899999619
0802fc9a  21ee261a  vmul.f32	s2, s2, s13
0802fc9e  dfed396a  vldr	s13, [pc, #228] ; [0x0802fd84] = 0x3e4ccccd / f32_bits_interpretation=0.200000003
0802fca2  26ee263a  vmul.f32	s6, s12, s13
0802fca6  b3ee056a  vmov.f32	s12, #2.100000e+01
0802fcaa  f0ee646a  vmov.f32	s13, s9
0802fcae  23ee013a  vmul.f32	s6, s6, s2
0802fcb2  e7ee066a  vfma.f32	s13, s14, s12
0802fcb6  b6ee007a  vmov.f32	s14, #5.000000e-01
0802fcba  b0ee666a  vmov.f32	s12, s13
0802fcbe  beeeca6a  vcvt.s32.f32	s12, s12, #12
0802fcc2  16ee103a  vmov	r3, s12
0802fcc6  03f50063  add.w	r3, r3, #2048
0802fcca  c3f30c03  ubfx	r3, r3, #0, #13
0802fcce  0beb8303  add.w	r3, r11, r3, lsl #2
0802fcd2  d3ed000a  vldr	s1, [r3]
0802fcd6  70eea40a  vadd.f32	s1, s1, s9
0802fcda  60ee870a  vmul.f32	s1, s1, s14
0802fcde  bdeee67a  vcvt.s32.f32	s14, s13
0802fce2  17ee102a  vmov	r2, s14
0802fce6  d307      lsls	r3, r2, #31
0802fce8  02f10101  add.w	r1, r2, #1
0802fcec  02d5      bpl	#4 ; -> 0x0802fcf4 ; branch_target=0x0802fcf4
0802fcee  0b46      mov	r3, r1
0802fcf0  1146      mov	r1, r2
0802fcf2  1a46      mov	r2, r3
0802fcf4  07ee102a  vmov	s14, r2
0802fcf8  2b4b      ldr	r3, [pc, #172] ; [0x0802fda8] = 0x20002e50
0802fcfa  f0ee678a  vmov.f32	s17, s15
0802fcfe  9fed242a  vldr	s4, [pc, #144] ; [0x0802fd90] = 0x3dcccccd / f32_bits_interpretation=0.1000000015
0802fd02  f8eec76a  vcvt.f32.s32	s13, s14
0802fd06  93ed004a  vldr	s8, [r3]
0802fd0a  07ee101a  vmov	s14, r1
0802fd0e  1f4b      ldr	r3, [pc, #124] ; [0x0802fd8c] = 0x200022bc
0802fd10  b0ee00aa  vmov.f32	s20, #2.000000e+00
0802fd14  f8eec74a  vcvt.f32.s32	s9, s14
0802fd18  d3ed17aa  vldr	s21, [r3, #92]
0802fd1c  24ee277a  vmul.f32	s14, s8, s15
0802fd20  93ed1fea  vldr	s28, [r3, #124]
0802fd24  6aee82aa  vmul.f32	s21, s21, s4
0802fd28  93ed186a  vldr	s12, [r3, #96]
0802fd2c  b5ee000a  vmov.f32	s0, #2.500000e-01
0802fd30  ecee878a  vfma.f32	s17, s25, s14
0802fd34  93ed1b7a  vldr	s14, [r3, #108]
0802fd38  64ee29da  vmul.f32	s27, s8, s19
0802fd3c  e7ee0aaa  vfma.f32	s21, s14, s20
0802fd40  93ed107a  vldr	s14, [r3, #64]
0802fd44  28ee80da  vmul.f32	s26, s17, s0
0802fd48  36ee286a  vadd.f32	s12, s12, s17
0802fd4c  63ee2aaa  vmul.f32	s21, s6, s21
0802fd50  37ee0d7a  vadd.f32	s14, s14, s26
0802fd54  61ee288a  vmul.f32	s17, s2, s17
0802fd58  adee2a7a  vfma.f32	s14, s26, s21
0802fd5c  2ae0      b	#84 ; -> 0x0802fdb4 ; branch_target=0x0802fdb4
0802fdb4  f0ee69aa  vmov.f32	s21, s19
0802fdb8  22ee4eda  vnmul.f32	s26, s4, s28
0802fdbc  eceeadaa  vfma.f32	s21, s25, s27
0802fdc0  f6eec7da  vrintz.f32	s27, s14
0802fdc4  37ee6d7a  vsub.f32	s14, s14, s27
0802fdc8  67ee24da  vmul.f32	s27, s14, s9
0802fdcc  83ed107a  vstr	s14, [r3, #64]
0802fdd0  27ee267a  vmul.f32	s14, s14, s13
0802fdd4  2aee800a  vmul.f32	s0, s21, s0
0802fdd8  feeee9da  vcvt.s32.f32	s27, s27, #13
0802fddc  beeee97a  vcvt.s32.f32	s14, s14, #13
0802fde0  23ee003a  vmul.f32	s6, s6, s0
0802fde4  1dee902a  vmov	r2, s27
0802fde8  21ee2a1a  vmul.f32	s2, s2, s21
0802fdec  17ee101a  vmov	r1, s14
0802fdf0  64eeae4a  vmul.f32	s9, s9, s29
0802fdf4  c2f30c02  ubfx	r2, r2, #0, #13
0802fdf8  66eeae6a  vmul.f32	s13, s13, s29
0802fdfc  c1f30c01  ubfx	r1, r1, #0, #13
0802fe00  0beb8202  add.w	r2, r11, r2, lsl #2
0802fe04  0beb8101  add.w	r1, r11, r1, lsl #2
0802fe08  92ed007a  vldr	s14, [r2]
0802fe0c  d1ed00da  vldr	s27, [r1]
0802fe10  7deec7da  vsub.f32	s27, s27, s14
0802fe14  adeea07a  vfma.f32	s14, s27, s1
0802fe18  a7ee0ada  vfma.f32	s26, s14, s20
0802fe1c  83ed137a  vstr	s14, [r3, #76]
0802fe20  adee286a  vfma.f32	s12, s26, s17
0802fe24  d3ed148a  vldr	s17, [r3, #80]
0802fe28  22ee47da  vnmul.f32	s26, s4, s14
0802fe2c  78ee808a  vadd.f32	s17, s17, s0
0802fe30  93ed1c0a  vldr	s0, [r3, #112]
0802fe34  30ee2a0a  vadd.f32	s0, s0, s21
0802fe38  f6eec6aa  vrintz.f32	s21, s12
0802fe3c  36ee6a6a  vsub.f32	s12, s12, s21
0802fe40  83ed186a  vstr	s12, [r3, #96]
0802fe44  beeee96a  vcvt.s32.f32	s12, s12, #13
0802fe48  16ee102a  vmov	r2, s12
0802fe4c  c2f30c02  ubfx	r2, r2, #0, #13
0802fe50  0beb8202  add.w	r2, r11, r2, lsl #2
0802fe54  92ed006a  vldr	s12, [r2]
0802fe58  26ee022a  vmul.f32	s4, s12, s4
0802fe5c  83ed1b6a  vstr	s12, [r3, #108]
0802fe60  aeee0a2a  vfma.f32	s4, s28, s20
0802fe64  e2ee038a  vfma.f32	s17, s4, s6
0802fe68  1fed303a  vldr	s6, [pc, #-192] ; [0x0802fdac] = 0x3e99999a / f32_bits_interpretation=0.3000000119
0802fe6c  1fed302a  vldr	s4, [pc, #-192] ; [0x0802fdb0] = 0x3f19999a / f32_bits_interpretation=0.6000000238
0802fe70  27ee037a  vmul.f32	s14, s14, s6
0802fe74  a6ee027a  vfma.f32	s14, s12, s4
0802fe78  b6eee86a  vrintz.f32	s12, s17
0802fe7c  38eec66a  vsub.f32	s12, s17, s12
0802fe80  66ee866a  vmul.f32	s13, s13, s12
0802fe84  83ed146a  vstr	s12, [r3, #80]
0802fe88  64ee864a  vmul.f32	s9, s9, s12
0802fe8c  fdeee66a  vcvt.s32.f32	s13, s13
0802fe90  bdeee46a  vcvt.s32.f32	s12, s9
0802fe94  16ee902a  vmov	r2, s13
0802fe98  16ee101a  vmov	r1, s12
0802fe9c  c2f30c02  ubfx	r2, r2, #0, #13
0802fea0  c1f30c01  ubfx	r1, r1, #0, #13
0802fea4  0beb8202  add.w	r2, r11, r2, lsl #2
0802fea8  0beb8101  add.w	r1, r11, r1, lsl #2
0802feac  92ed006a  vldr	s12, [r2]
0802feb0  d1ed006a  vldr	s13, [r1]
0802feb4  ca4a      ldr	r2, [pc, #808] ; [0x080301e0] = 0x20002424
0802feb6  36ee666a  vsub.f32	s12, s12, s13
0802feba  92ed00ea  vldr	s28, [r2]
0802febe  c94a      ldr	r2, [pc, #804] ; [0x080301e4] = 0x20002420
0802fec0  e6ee206a  vfma.f32	s13, s12, s1
0802fec4  d2ed00aa  vldr	s21, [r2]
0802fec8  a6ee8ada  vfma.f32	s26, s13, s20
0802fecc  c3ed176a  vstr	s13, [r3, #92]
0802fed0  26ee833a  vmul.f32	s6, s13, s6
0802fed4  adee010a  vfma.f32	s0, s26, s2
0802fed8  f6eec06a  vrintz.f32	s13, s0
0802fedc  70ee666a  vsub.f32	s13, s0, s13
0802fee0  c3ed1c6a  vstr	s13, [r3, #112]
0802fee4  feeee96a  vcvt.s32.f32	s13, s13, #13
0802fee8  16ee902a  vmov	r2, s13
0802feec  c2f30c02  ubfx	r2, r2, #0, #13
0802fef0  0beb8202  add.w	r2, r11, r2, lsl #2
0802fef4  d2ed006a  vldr	s13, [r2]
0802fef8  a6ee823a  vfma.f32	s6, s13, s4
0802fefc  c3ed1f6a  vstr	s13, [r3, #124]
0802ff00  b6ee006a  vmov.f32	s12, #5.000000e-01
0802ff04  79ee096a  vadd.f32	s13, s18, s18
0802ff08  b4eec69a  vcmpe.f32	s18, s12
0802ff0c  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ff10  03dd      ble	#6 ; -> 0x0802ff1a ; branch_target=0x0802ff1a
0802ff12  b0ee006a  vmov.f32	s12, #2.000000e+00
0802ff16  76ee666a  vsub.f32	s13, s12, s13
0802ff1a  9fedb36a  vldr	s12, [pc, #716] ; [0x080301e8] = 0x3d4ccccd / f32_bits_interpretation=0.05000000075
0802ff1e  f4eec66a  vcmpe.f32	s13, s12
0802ff22  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ff26  00f36183  bgt.w	#1730 ; -> 0x080305ec ; branch_target=0x080305ec
0802ff2a  b3ee046a  vmov.f32	s12, #2.000000e+01
0802ff2e  66ee864a  vmul.f32	s9, s13, s12
0802ff32  b6ee006a  vmov.f32	s12, #5.000000e-01
0802ff36  78ee086a  vadd.f32	s13, s16, s16
0802ff3a  b4eec68a  vcmpe.f32	s16, s12
0802ff3e  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ff42  03dd      ble	#6 ; -> 0x0802ff4c ; branch_target=0x0802ff4c
0802ff44  b0ee006a  vmov.f32	s12, #2.000000e+00
0802ff48  76ee666a  vsub.f32	s13, s12, s13
0802ff4c  9feda66a  vldr	s12, [pc, #664] ; [0x080301e8] = 0x3d4ccccd / f32_bits_interpretation=0.05000000075
0802ff50  f4eec66a  vcmpe.f32	s13, s12
0802ff54  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ff58  00f34583  bgt.w	#1674 ; -> 0x080305e6 ; branch_target=0x080305e6
0802ff5c  b3ee046a  vmov.f32	s12, #2.000000e+01
0802ff60  66ee866a  vmul.f32	s13, s13, s12
0802ff64  039b      ldr	r3, [sp, #12]
0802ff66  022b      cmp	r3, #2
0802ff68  03d1      bne	#6 ; -> 0x0802ff72 ; branch_target=0x0802ff72
0802ff6a  27ee247a  vmul.f32	s14, s14, s9
0802ff6e  23ee263a  vmul.f32	s6, s6, s13
0802ff72  9e4a      ldr	r2, [pc, #632] ; [0x080301ec] = 0x20002f6c
0802ff74  9e4b      ldr	r3, [pc, #632] ; [0x080301f0] = 0xe0001000
0802ff76  9f49      ldr	r1, [pc, #636] ; [0x080301f4] = 0x08037674
0802ff78  5b68      ldr	r3, [r3, #4]
0802ff7a  1160      str	r1, [r2]
0802ff7c  029a      ldr	r2, [sp, #8]
0802ff7e  9b1a      subs	r3, r3, r2
0802ff80  9d4a      ldr	r2, [pc, #628] ; [0x080301f8] = 0x2000227c
0802ff82  9361      str	r3, [r2, #24]
0802ff84  9d4b      ldr	r3, [pc, #628] ; [0x080301fc] = 0x20002ef4
0802ff86  1a68      ldr	r2, [r3]
0802ff88  032a      cmp	r2, #3
0802ff8a  00f0fc83  beq.w	#2040 ; -> 0x08030786 ; branch_target=0x08030786
0802ff8e  042a      cmp	r2, #4
0802ff90  01f01383  beq.w	#5670 ; -> 0x080315ba ; branch_target=0x080315ba
0802ff94  9a4b      ldr	r3, [pc, #616] ; [0x08030200] = 0x20002414
0802ff96  012a      cmp	r2, #1
0802ff98  93ed006a  vldr	s12, [r3]
0802ff9c  41f0be87  bne.w	#8060 ; -> 0x08031f1c ; branch_target=0x08031f1c
0802ffa0  984b      ldr	r3, [pc, #608] ; [0x08030204] = 0x20000840
0802ffa2  d3ed006a  vldr	s13, [r3]
0802ffa6  924b      ldr	r3, [pc, #584] ; [0x080301f0] = 0xe0001000
0802ffa8  0299      ldr	r1, [sp, #8]
0802ffaa  5b68      ldr	r3, [r3, #4]
0802ffac  5b1a      subs	r3, r3, r1
0802ffae  0399      ldr	r1, [sp, #12]
0802ffb0  0229      cmp	r1, #2
0802ffb2  00f07884  beq.w	#2288 ; -> 0x080308a6 ; branch_target=0x080308a6
0802ffb6  f0ee694a  vmov.f32	s9, s19
0802ffba  b0ee671a  vmov.f32	s2, s15
0802ffbe  f4ee697a  vcmp.f32	s15, s19
0802ffc2  2cee844a  vmul.f32	s8, s25, s8
0802ffc6  74eeaa4a  vadd.f32	s9, s9, s21
0802ffca  31ee0e2a  vadd.f32	s4, s2, s28
0802ffce  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802ffd2  e1ee044a  vfma.f32	s9, s2, s8
0802ffd6  a1ee042a  vfma.f32	s4, s2, s8
0802ffda  05d1      bne	#10 ; -> 0x0802ffe8 ; branch_target=0x0802ffe8
0802ffdc  32ee641a  vsub.f32	s2, s4, s9
0802ffe0  9fed894a  vldr	s8, [pc, #548] ; [0x08030208] = 0x3a03126f / f32_bits_interpretation=0.0005000000237
0802ffe4  e1ee044a  vfma.f32	s9, s2, s8
0802ffe8  9ded121a  vldr	s2, [sp, #72]
0802ffec  002a      cmp	r2, #0
0802ffee  18bf      it	ne
0802fff0  052a      cmpne	r2, #5
0802fff2  9ded0f4a  vldr	s8, [sp, #60]
0802fff6  31ee055a  vadd.f32	s10, s2, s10
0802fffa  9ded111a  vldr	s2, [sp, #68]
0802fffe  7e49      ldr	r1, [pc, #504] ; [0x080301f8] = 0x2000227c
08030000  34ee274a  vadd.f32	s8, s8, s15
08030004  71ee255a  vadd.f32	s11, s2, s11
08030008  cb61      str	r3, [r1, #28]
0803000a  14bf      ite	ne
0803000c  0121      movne	r1, #1
0803000e  0021      moveq	r1, #0
08030010  052a      cmp	r2, #5
08030012  18bf      it	ne
08030014  002a      cmpne	r2, #0
08030016  b6eec41a  vrintz.f32	s2, s8
0803001a  f6eee50a  vrintz.f32	s1, s11
0803001e  34ee414a  vsub.f32	s8, s8, s2
08030022  b6eec51a  vrintz.f32	s2, s10
08030026  75eee05a  vsub.f32	s11, s11, s1
0803002a  35ee415a  vsub.f32	s10, s10, s2
0803002e  40f0f582  bne.w	#1514 ; -> 0x0803061c ; branch_target=0x0803061c
08030032  f6ee000a  vmov.f32	s1, #5.000000e-01
08030036  754b      ldr	r3, [pc, #468] ; [0x0803020c] = 0x200008e0
08030038  b7ee001a  vmov.f32	s2, #1.000000e+00
0803003c  83ed005a  vstr	s10, [r3]
08030040  a7eea06a  vfma.f32	s12, s15, s1
08030044  724b      ldr	r3, [pc, #456] ; [0x08030210] = 0x20002410
08030046  c3ed005a  vstr	s11, [r3]
0803004a  724b      ldr	r3, [pc, #456] ; [0x08030214] = 0x200008c0
0803004c  83ed004a  vstr	s8, [r3]
08030050  b4eec16a  vcmpe.f32	s12, s2
08030054  6a4b      ldr	r3, [pc, #424] ; [0x08030200] = 0x20002414
08030056  83ed006a  vstr	s12, [r3]
0803005a  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803005e  20db      blt	#64 ; -> 0x080300a2 ; branch_target=0x080300a2
08030060  0029      cmp	r1, #0
08030062  40f0f482  bne.w	#1512 ; -> 0x0803064e ; branch_target=0x0803064e
08030066  f7ee007a  vmov.f32	s15, #1.000000e+00
0803006a  6b48      ldr	r0, [pc, #428] ; [0x08030218] = 0x20002eb0
0803006c  644b      ldr	r3, [pc, #400] ; [0x08030200] = 0x20002414
0803006e  ffee005a  vmov.f32	s11, #-1.000000e+00
08030072  6a4c      ldr	r4, [pc, #424] ; [0x0803021c] = 0x0bb38435
08030074  36ee676a  vsub.f32	s12, s12, s15
08030078  694a      ldr	r2, [pc, #420] ; [0x08030220] = 0x3619636b / f32_bits_interpretation=2.28566455e-06
0803007a  9fed6a5a  vldr	s10, [pc, #424] ; [0x08030224] = 0x3000000d / f32_bits_interpretation=4.65662009e-10
0803007e  83ed006a  vstr	s12, [r3]
08030082  0368      ldr	r3, [r0]
08030084  04fb0322  mla	r2, r4, r3, r2
08030088  5e4b      ldr	r3, [pc, #376] ; [0x08030204] = 0x20000840
0803008a  07ee902a  vmov	s15, r2
0803008e  0260      str	r2, [r0]
08030090  1868      ldr	r0, [r3]
08030092  f8ee677a  vcvt.f32.u32	s15, s15
08030096  644a      ldr	r2, [pc, #400] ; [0x08030228] = 0x200023d4
08030098  1060      str	r0, [r2]
0803009a  e7ee855a  vfma.f32	s11, s15, s10
0803009e  c3ed005a  vstr	s11, [r3]
080300a2  b5eec06a  vcmpe.f32	s12, #0
080300a6  f1ee10fa  vmrs	APSR_nzcv, fpscr
080300aa  06d5      bpl	#12 ; -> 0x080300ba ; branch_target=0x080300ba
080300ac  f7ee007a  vmov.f32	s15, #1.000000e+00
080300b0  534b      ldr	r3, [pc, #332] ; [0x08030200] = 0x20002414
080300b2  36ee276a  vadd.f32	s12, s12, s15
080300b6  83ed006a  vstr	s12, [r3]
080300ba  f4ee612a  vcmp.f32	s5, s3
080300be  f6eec25a  vrintz.f32	s11, s4
080300c2  32ee894a  vadd.f32	s8, s5, s18
080300c6  b6eee46a  vrintz.f32	s12, s9
080300ca  78ee217a  vadd.f32	s15, s16, s3
080300ce  f1ee10fa  vmrs	APSR_nzcv, fpscr
080300d2  72ee655a  vsub.f32	s11, s4, s11
080300d6  34eec66a  vsub.f32	s12, s9, s12
080300da  34ee0c4a  vadd.f32	s8, s8, s24
080300de  77ee8c7a  vadd.f32	s15, s15, s24
080300e2  05d1      bne	#10 ; -> 0x080300f0 ; branch_target=0x080300f0
080300e4  74ee674a  vsub.f32	s9, s8, s15
080300e8  9fed475a  vldr	s10, [pc, #284] ; [0x08030208] = 0x3a03126f / f32_bits_interpretation=0.0005000000237
080300ec  e4ee857a  vfma.f32	s15, s9, s10
080300f0  3b4b      ldr	r3, [pc, #236] ; [0x080301e0] = 0x20002424
080300f2  9ded105a  vldr	s10, [sp, #64]
080300f6  c3ed005a  vstr	s11, [r3]
080300fa  3a4b      ldr	r3, [pc, #232] ; [0x080301e4] = 0x20002420
080300fc  75ee224a  vadd.f32	s9, s10, s5
08030100  83ed006a  vstr	s12, [r3]
08030104  049b      ldr	r3, [sp, #16]
08030106  002b      cmp	r3, #0
08030108  18bf      it	ne
0803010a  052b      cmpne	r3, #5
0803010c  14bf      ite	ne
0803010e  0120      movne	r0, #1
08030110  0020      moveq	r0, #0
08030112  002b      cmp	r3, #0
08030114  00f06d82  beq.w	#1242 ; -> 0x080305f2 ; branch_target=0x080305f2
08030118  052b      cmp	r3, #5
0803011a  00f06a82  beq.w	#1236 ; -> 0x080305f2 ; branch_target=0x080305f2
0803011e  434b      ldr	r3, [pc, #268] ; [0x0803022c] = 0x200023f8
08030120  f7ee005a  vmov.f32	s11, #1.000000e+00
08030124  9ded095a  vldr	s10, [sp, #36]
08030128  93ed006a  vldr	s12, [r3]
0803012c  404b      ldr	r3, [pc, #256] ; [0x08030230] = 0x20002418
0803012e  35ee066a  vadd.f32	s12, s10, s12
08030132  b4eee56a  vcmpe.f32	s12, s11
08030136  83ed006a  vstr	s12, [r3]
0803013a  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803013e  20db      blt	#64 ; -> 0x08030182 ; branch_target=0x08030182
08030140  0a23      movs	r3, #10
08030142  3c4a      ldr	r2, [pc, #240] ; [0x08030234] = 0x2000242c
08030144  1360      str	r3, [r2]
08030146  b7ee005a  vmov.f32	s10, #1.000000e+00
0803014a  334c      ldr	r4, [pc, #204] ; [0x08030218] = 0x20002eb0
0803014c  384b      ldr	r3, [pc, #224] ; [0x08030230] = 0x20002418
0803014e  ffee005a  vmov.f32	s11, #-1.000000e+00
08030152  2268      ldr	r2, [r4]
08030154  36ee456a  vsub.f32	s12, s12, s10
08030158  304d      ldr	r5, [pc, #192] ; [0x0803021c] = 0x0bb38435
0803015a  dfed322a  vldr	s5, [pc, #200] ; [0x08030224] = 0x3000000d / f32_bits_interpretation=4.65662009e-10
0803015e  83ed006a  vstr	s12, [r3]
08030162  2f4b      ldr	r3, [pc, #188] ; [0x08030220] = 0x3619636b / f32_bits_interpretation=2.28566455e-06
08030164  05fb0233  mla	r3, r5, r2, r3
08030168  334a      ldr	r2, [pc, #204] ; [0x08030238] = 0x200023dc
0803016a  05ee103a  vmov	s10, r3
0803016e  2360      str	r3, [r4]
08030170  324b      ldr	r3, [pc, #200] ; [0x0803023c] = 0x200023d8
08030172  b8ee455a  vcvt.f32.u32	s10, s10
08030176  1c68      ldr	r4, [r3]
08030178  e5ee225a  vfma.f32	s11, s10, s5
0803017c  1460      str	r4, [r2]
0803017e  c3ed005a  vstr	s11, [r3]
08030182  b5eec06a  vcmpe.f32	s12, #0
08030186  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803018a  06d5      bpl	#12 ; -> 0x0803019a ; branch_target=0x0803019a
0803018c  f7ee005a  vmov.f32	s11, #1.000000e+00
08030190  274b      ldr	r3, [pc, #156] ; [0x08030230] = 0x20002418
08030192  36ee256a  vadd.f32	s12, s12, s11
08030196  83ed006a  vstr	s12, [r3]
0803019a  f6eee45a  vrintz.f32	s11, s9
0803019e  74eee55a  vsub.f32	s11, s9, s11
080301a2  274b      ldr	r3, [pc, #156] ; [0x08030240] = 0x2000240c
080301a4  b6eec45a  vrintz.f32	s10, s8
080301a8  34ee456a  vsub.f32	s12, s8, s10
080301ac  b6eee75a  vrintz.f32	s10, s15
080301b0  c3ed005a  vstr	s11, [r3]
080301b4  77eec57a  vsub.f32	s15, s15, s10
080301b8  224b      ldr	r3, [pc, #136] ; [0x08030244] = 0x2000241c
080301ba  0a9a      ldr	r2, [sp, #40]
080301bc  83ed006a  vstr	s12, [r3]
080301c0  214b      ldr	r3, [pc, #132] ; [0x08030248] = 0x20000900
080301c2  0132      adds	r2, #1
080301c4  029c      ldr	r4, [sp, #8]
080301c6  c3ed007a  vstr	s15, [r3]
080301ca  204b      ldr	r3, [pc, #128] ; [0x0803024c] = 0x20002eec
080301cc  1a60      str	r2, [r3]
080301ce  084b      ldr	r3, [pc, #32] ; [0x080301f0] = 0xe0001000
080301d0  5b68      ldr	r3, [r3, #4]
080301d2  1b1b      subs	r3, r3, r4
080301d4  084c      ldr	r4, [pc, #32] ; [0x080301f8] = 0x2000227c
080301d6  2362      str	r3, [r4, #32]
080301d8  0028      cmp	r0, #0
080301da  3fd0      beq	#126 ; -> 0x0803025c ; branch_target=0x0803025c
080301dc  3ae0      b	#116 ; -> 0x08030254 ; branch_target=0x08030254
08030254  f7ee007a  vmov.f32	s15, #1.000000e+00
08030258  73eea73a  vadd.f32	s7, s7, s15
0803025c  63eea37a  vmul.f32	s15, s7, s7
08030260  1fed056a  vldr	s12, [pc, #-20] ; [0x08030250] = 0x42c80000 / f32_bits_interpretation=100
08030264  bd48      ldr	r0, [pc, #756] ; [0x0803055c] = 0x40000400 / f32_bits_interpretation=2.000244141
08030266  67ee867a  vmul.f32	s15, s15, s12
0803026a  67eea37a  vmul.f32	s15, s15, s7
0803026e  fdeee77a  vcvt.s32.f32	s15, s15
08030272  17ee903a  vmov	r3, s15
08030276  c3f17f03  rsb.w	r3, r3, #127
0803027a  23eae373  bic.w	r3, r3, r3, asr #31
0803027e  c363      str	r3, [r0, #60]
08030280  19b1      cbz	r1, #6 ; -> 0x0803028a ; branch_target=0x0803028a
08030282  f7ee007a  vmov.f32	s15, #1.000000e+00
08030286  76eea76a  vadd.f32	s13, s13, s15
0803028a  66eea67a  vmul.f32	s15, s13, s13
0803028e  9fedb46a  vldr	s12, [pc, #720] ; [0x08030560] = 0x42c80000 / f32_bits_interpretation=100
08030292  b249      ldr	r1, [pc, #712] ; [0x0803055c] = 0x40000400 / f32_bits_interpretation=2.000244141
08030294  67ee867a  vmul.f32	s15, s15, s12
08030298  67eea67a  vmul.f32	s15, s15, s13
0803029c  fdeee77a  vcvt.s32.f32	s15, s15
080302a0  17ee903a  vmov	r3, s15
080302a4  c3f17f03  rsb.w	r3, r3, #127
080302a8  23eae373  bic.w	r3, r3, r3, asr #31
080302ac  0b64      str	r3, [r1, #64]
080302ae  ad4b      ldr	r3, [pc, #692] ; [0x08030564] = 0x20002ec8
080302b0  1b68      ldr	r3, [r3]
080302b2  012b      cmp	r3, #1
080302b4  00f05882  beq.w	#1200 ; -> 0x08030768 ; branch_target=0x08030768
080302b8  f7ee087a  vmov.f32	s15, #1.500000e+00
080302bc  dfedaa4a  vldr	s9, [pc, #680] ; [0x08030568] = 0x3e17b426 / f32_bits_interpretation=0.1481481493
080302c0  bfee085a  vmov.f32	s10, #-1.500000e+00
080302c4  9feda94a  vldr	s8, [pc, #676] ; [0x0803056c] = 0x4e4ccccd / f32_bits_interpretation=858993472
080302c8  f7ee005a  vmov.f32	s11, #1.000000e+00
080302cc  83fe676a  vminnm.f32	s12, s6, s15
080302d0  87fe677a  vminnm.f32	s14, s14, s15
080302d4  86fe056a  vmaxnm.f32	s12, s12, s10
080302d8  87fe057a  vmaxnm.f32	s14, s14, s10
080302dc  66ee067a  vmul.f32	s15, s12, s12
080302e0  27ee075a  vmul.f32	s10, s14, s14
080302e4  63ee843a  vmul.f32	s7, s7, s8
080302e8  67eea47a  vmul.f32	s15, s15, s9
080302ec  25ee245a  vmul.f32	s10, s10, s9
080302f0  66ee846a  vmul.f32	s13, s13, s8
080302f4  75eee77a  vsub.f32	s15, s11, s15
080302f8  75eec55a  vsub.f32	s11, s11, s10
080302fc  67ee867a  vmul.f32	s15, s15, s12
08030300  65ee875a  vmul.f32	s11, s11, s14
08030304  b0ee6c7a  vmov.f32	s14, s25
08030308  beeec17a  vcvt.s32.f32	s14, s14, #30
0803030c  17ee107a  vmov	r7, s14
08030310  bdeee37a  vcvt.s32.f32	s14, s7
08030314  17ee104a  vmov	r4, s14
08030318  b0ee6f7a  vmov.f32	s14, s31
0803031c  beeec17a  vcvt.s32.f32	s14, s14, #30
08030320  17ee106a  vmov	r6, s14
08030324  b0ee4f7a  vmov.f32	s14, s30
08030328  beeec17a  vcvt.s32.f32	s14, s14, #30
0803032c  17ee105a  vmov	r5, s14
08030330  b0ee677a  vmov.f32	s14, s15
08030334  f0ee657a  vmov.f32	s15, s11
08030338  beeec17a  vcvt.s32.f32	s14, s14, #30
0803033c  feeec17a  vcvt.s32.f32	s15, s15, #30
08030340  17ee101a  vmov	r1, s14
08030344  17ee900a  vmov	r0, s15
08030348  fdeee67a  vcvt.s32.f32	s15, s13
0803034c  17ee90ca  vmov	r12, s15
08030350  dded077a  vldr	s15, [sp, #28]
08030354  feeec17a  vcvt.s32.f32	s15, s15, #30
08030358  17ee902a  vmov	r2, s15
0803035c  179b      ldr	r3, [sp, #92]
0803035e  43f82970  str.w	r7, [r3, r9, lsl #2]
08030362  189b      ldr	r3, [sp, #96]
08030364  43f82960  str.w	r6, [r3, r9, lsl #2]
08030368  199b      ldr	r3, [sp, #100]
0803036a  43f82910  str.w	r1, [r3, r9, lsl #2]
0803036e  1a9b      ldr	r3, [sp, #104]
08030370  43f829c0  str.w	r12, [r3, r9, lsl #2]
08030374  1c9b      ldr	r3, [sp, #112]
08030376  43f82940  str.w	r4, [r3, r9, lsl #2]
0803037a  1f9b      ldr	r3, [sp, #124]
0803037c  43f82950  str.w	r5, [r3, r9, lsl #2]
08030380  1b9b      ldr	r3, [sp, #108]
08030382  43f82900  str.w	r0, [r3, r9, lsl #2]
08030386  1e9b      ldr	r3, [sp, #120]
08030388  43f82920  str.w	r2, [r3, r9, lsl #2]
0803038c  09f10209  add.w	r9, r9, #2
08030390  774b      ldr	r3, [pc, #476] ; [0x08030570] = 0xe0001000
08030392  029a      ldr	r2, [sp, #8]
08030394  b9f1800f  cmp.w	r9, #128
08030398  5b68      ldr	r3, [r3, #4]
0803039a  a3eb0203  sub.w	r3, r3, r2
0803039e  754a      ldr	r2, [pc, #468] ; [0x08030574] = 0x2000227c
080303a0  5362      str	r3, [r2, #36]
080303a2  3ef41caa  beq.w	#-7112 ; -> 0x0802e7de ; branch_target=0x0802e7de
080303a6  744b      ldr	r3, [pc, #464] ; [0x08030578] = 0x200144d4
080303a8  1b68      ldr	r3, [r3]
080303aa  0493      str	r3, [sp, #16]
080303ac  fef7b3bc  b.w	#-5786 ; -> 0x0802ed16 ; branch_target=0x0802ed16
080303b0  dfed727a  vldr	s15, [pc, #456] ; [0x0803057c] = 0x00000000
080303b4  b0ee670a  vmov.f32	s0, s15
080303b8  fef7a4ba  b.w	#-6840 ; -> 0x0802e904 ; branch_target=0x0802e904
080303bc  dff8f081  ldr.w	r8, [pc, #496] ; [0x080305b0] = 0x2000000c
080303c0  0127      movs	r7, #1
080303c2  7380      strh	r3, [r6, #2]
080303c4  40f69e63  movw	r3, #3742
080303c8  6d4e      ldr	r6, [pc, #436] ; [0x08030580] = 0x20000008
080303ca  0322      movs	r2, #3
080303cc  a8f80030  strh.w	r3, [r8]
080303d0  ff23      movs	r3, #255
080303d2  4146      mov	r1, r8
080303d4  6b48      ldr	r0, [pc, #428] ; [0x08030584] = 0x20014bb8
080303d6  88f80230  strb.w	r3, [r8, #2]
080303da  3368      ldr	r3, [r6]
080303dc  2770      strb	r7, [r4]
080303de  f6f763fd  bl	#-38202 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080303e2  40f29e53  movw	r3, #1438
080303e6  0222      movs	r2, #2
080303e8  4146      mov	r1, r8
080303ea  6648      ldr	r0, [pc, #408] ; [0x08030584] = 0x20014bb8
080303ec  a8f80030  strh.w	r3, [r8]
080303f0  3368      ldr	r3, [r6]
080303f2  f6f759fd  bl	#-38222 ; -> 0x08026ea8 ; branch_target=0x08026ea8
080303f6  3368      ldr	r3, [r6]
080303f8  4146      mov	r1, r8
080303fa  634a      ldr	r2, [pc, #396] ; [0x08030588] = 0x20002f88
080303fc  0093      str	r3, [sp]
080303fe  9f23      movs	r3, #159
08030400  6048      ldr	r0, [pc, #384] ; [0x08030584] = 0x20014bb8
08030402  88f80030  strb.w	r3, [r8]
08030406  0223      movs	r3, #2
08030408  f6f796fe  bl	#-37588 ; -> 0x08027138 ; branch_target=0x08027138
0803040c  249b      ldr	r3, [sp, #144]
0803040e  1b68      ldr	r3, [r3]
08030410  002b      cmp	r3, #0
08030412  41f07287  bne.w	#7908 ; -> 0x080322fa ; branch_target=0x080322fa
08030416  fcf733ff  bl	#-12698 ; -> 0x0802d280 ; branch_target=0x0802d280
0803041a  fef7cab9  b.w	#-7276 ; -> 0x0802e7b2 ; branch_target=0x0802e7b2
0803041e  0123      movs	r3, #1
08030420  dff88ca1  ldr.w	r10, [pc, #396] ; [0x080305b0] = 0x2000000c
08030424  564f      ldr	r7, [pc, #344] ; [0x08030580] = 0x20000008
08030426  2370      strb	r3, [r4]
08030428  40f69e63  movw	r3, #3742
0803042c  5146      mov	r1, r10
0803042e  3280      strh	r2, [r6]
08030430  aaf80030  strh.w	r3, [r10]
08030434  ff23      movs	r3, #255
08030436  0322      movs	r2, #3
08030438  5248      ldr	r0, [pc, #328] ; [0x08030584] = 0x20014bb8
0803043a  8af80230  strb.w	r3, [r10, #2]
0803043e  3b68      ldr	r3, [r7]
08030440  f6f732fd  bl	#-38300 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08030444  40f29e53  movw	r3, #1438
08030448  0222      movs	r2, #2
0803044a  5146      mov	r1, r10
0803044c  aaf80030  strh.w	r3, [r10]
08030450  4c48      ldr	r0, [pc, #304] ; [0x08030584] = 0x20014bb8
08030452  3b68      ldr	r3, [r7]
08030454  f6f728fd  bl	#-38320 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08030458  3b68      ldr	r3, [r7]
0803045a  5146      mov	r1, r10
0803045c  4a4a      ldr	r2, [pc, #296] ; [0x08030588] = 0x20002f88
0803045e  0093      str	r3, [sp]
08030460  9f23      movs	r3, #159
08030462  4848      ldr	r0, [pc, #288] ; [0x08030584] = 0x20014bb8
08030464  8af80030  strb.w	r3, [r10]
08030468  0223      movs	r3, #2
0803046a  f6f765fe  bl	#-37686 ; -> 0x08027138 ; branch_target=0x08027138
0803046e  d8f80030  ldr.w	r3, [r8]
08030472  002b      cmp	r3, #0
08030474  41f03587  bne.w	#7786 ; -> 0x080322e2 ; branch_target=0x080322e2
08030478  fcf702ff  bl	#-12796 ; -> 0x0802d280 ; branch_target=0x0802d280
0803047c  249b      ldr	r3, [sp, #144]
0803047e  b6f90220  ldrsh.w	r2, [r6, #2]
08030482  1b68      ldr	r3, [r3]
08030484  9a42      cmp	r2, r3
08030486  3ef494a9  beq.w	#-7384 ; -> 0x0802e7b2 ; branch_target=0x0802e7b2
0803048a  97e7      b	#-210 ; -> 0x080303bc ; branch_target=0x080303bc
0803048c  3f4b      ldr	r3, [pc, #252] ; [0x0803058c] = 0xffff15a0
0803048e  0798      ldr	r0, [sp, #28]
08030490  9842      cmp	r0, r3
08030492  03da      bge	#6 ; -> 0x0803049c ; branch_target=0x0803049c
08030494  239b      ldr	r3, [sp, #140]
08030496  9c11      asrs	r4, r3, #6
08030498  3d4b      ldr	r3, [pc, #244] ; [0x08030590] = 0x20001340
0803049a  9c61      str	r4, [r3, #24]
0803049c  3b4b      ldr	r3, [pc, #236] ; [0x0803058c] = 0xffff15a0
0803049e  0998      ldr	r0, [sp, #36]
080304a0  9842      cmp	r0, r3
080304a2  03da      bge	#6 ; -> 0x080304ac ; branch_target=0x080304ac
080304a4  0a9b      ldr	r3, [sp, #40]
080304a6  9811      asrs	r0, r3, #6
080304a8  394b      ldr	r3, [pc, #228] ; [0x08030590] = 0x20001340
080304aa  d861      str	r0, [r3, #28]
080304ac  4ef66023  movw	r3, #60000
080304b0  9d42      cmp	r5, r3
080304b2  02dd      ble	#4 ; -> 0x080304ba ; branch_target=0x080304ba
080304b4  8911      asrs	r1, r1, #6
080304b6  364b      ldr	r3, [pc, #216] ; [0x08030590] = 0x20001340
080304b8  1962      str	r1, [r3, #32]
080304ba  4ef66023  movw	r3, #60000
080304be  9e45      cmp	lr, r3
080304c0  03dd      ble	#6 ; -> 0x080304ca ; branch_target=0x080304ca
080304c2  4feaac11  asr.w	r1, r12, #6
080304c6  324b      ldr	r3, [pc, #200] ; [0x08030590] = 0x20001340
080304c8  5962      str	r1, [r3, #36]
080304ca  4ef66023  movw	r3, #60000
080304ce  9e42      cmp	r6, r3
080304d0  02dd      ble	#4 ; -> 0x080304d8 ; branch_target=0x080304d8
080304d2  bf11      asrs	r7, r7, #6
080304d4  2e4b      ldr	r3, [pc, #184] ; [0x08030590] = 0x20001340
080304d6  9f62      str	r7, [r3, #40]
080304d8  4ef66023  movw	r3, #60000
080304dc  9845      cmp	r8, r3
080304de  02dd      ble	#4 ; -> 0x080304e6 ; branch_target=0x080304e6
080304e0  9211      asrs	r2, r2, #6
080304e2  2b4b      ldr	r3, [pc, #172] ; [0x08030590] = 0x20001340
080304e4  da62      str	r2, [r3, #44]
080304e6  2b4b      ldr	r3, [pc, #172] ; [0x08030594] = 0xffff15a1
080304e8  079a      ldr	r2, [sp, #28]
080304ea  0999      ldr	r1, [sp, #36]
080304ec  9942      cmp	r1, r3
080304ee  b8bf      it	lt
080304f0  9a42      cmplt	r2, r3
080304f2  4ef65f22  movw	r2, #59999
080304f6  acbf      ite	ge
080304f8  0123      movge	r3, #1
080304fa  0023      movlt	r3, #0
080304fc  9645      cmp	lr, r2
080304fe  d8bf      it	le
08030500  43f00103  orrle	r3, r3, #1
08030504  9542      cmp	r5, r2
08030506  d8bf      it	le
08030508  43f00103  orrle	r3, r3, #1
0803050c  9642      cmp	r6, r2
0803050e  d8bf      it	le
08030510  43f00103  orrle	r3, r3, #1
08030514  0bb9      cbnz	r3, #2 ; -> 0x0803051a ; branch_target=0x0803051a
08030516  9045      cmp	r8, r2
08030518  1bdc      bgt	#54 ; -> 0x08030552 ; branch_target=0x08030552
0803051a  0122      movs	r2, #1
0803051c  4021      movs	r1, #64
0803051e  1e48      ldr	r0, [pc, #120] ; [0x08030598] = 0x58020800
08030520  f2f774ff  bl	#-53528 ; -> 0x0802340c ; branch_target=0x0802340c
08030524  0122      movs	r2, #1
08030526  8021      movs	r1, #128
08030528  1b48      ldr	r0, [pc, #108] ; [0x08030598] = 0x58020800
0803052a  f2f76fff  bl	#-53538 ; -> 0x0802340c ; branch_target=0x0802340c
0803052e  1b4b      ldr	r3, [pc, #108] ; [0x0803059c] = 0x200023e0
08030530  d3ed002a  vldr	s5, [r3]
08030534  1a4b      ldr	r3, [pc, #104] ; [0x080305a0] = 0x200023e8
08030536  d3ed007a  vldr	s15, [r3]
0803053a  1a4b      ldr	r3, [pc, #104] ; [0x080305a4] = 0x200023e4
0803053c  d3ed009a  vldr	s19, [r3]
08030540  0c4b      ldr	r3, [pc, #48] ; [0x08030574] = 0x2000227c
08030542  db6a      ldr	r3, [r3, #44]
08030544  0293      str	r3, [sp, #8]
08030546  184b      ldr	r3, [pc, #96] ; [0x080305a8] = 0x20002e94
08030548  1b68      ldr	r3, [r3]
0803054a  0693      str	r3, [sp, #24]
0803054c  249b      ldr	r3, [sp, #144]
0803054e  1b68      ldr	r3, [r3]
08030550  0593      str	r3, [sp, #20]
08030552  164b      ldr	r3, [pc, #88] ; [0x080305ac] = 0x20002eec
08030554  1b68      ldr	r3, [r3]
08030556  0a93      str	r3, [sp, #40]
08030558  fef73fbe  b.w	#-4994 ; -> 0x0802f1da ; branch_target=0x0802f1da
080305b8  b7ee007a  vmov.f32	s14, #1.000000e+00
080305bc  f0ee476a  vmov.f32	s13, s14
080305c0  fef766bd  b.w	#-5428 ; -> 0x0802f090 ; branch_target=0x0802f090
080305c4  0122      movs	r2, #1
080305c6  1fed057a  vldr	s14, [pc, #-20] ; [0x080305b4] = 0x3d7ae148 / f32_bits_interpretation=0.06125000119
080305ca  02fa03f3  lsl.w	r3, r2, r3
080305ce  66ee876a  vmul.f32	s13, s13, s14
080305d2  07ee103a  vmov	s14, r3
080305d6  b8ee477a  vcvt.f32.u32	s14, s14
080305da  27ee267a  vmul.f32	s14, s14, s13
080305de  67ee277a  vmul.f32	s15, s14, s15
080305e2  fef7cabc  b.w	#-5740 ; -> 0x0802ef7a ; branch_target=0x0802ef7a
080305e6  f7ee006a  vmov.f32	s13, #1.000000e+00
080305ea  bbe4      b	#-1674 ; -> 0x0802ff64 ; branch_target=0x0802ff64
080305ec  f7ee004a  vmov.f32	s9, #1.000000e+00
080305f0  9fe4      b	#-1730 ; -> 0x0802ff32 ; branch_target=0x0802ff32
080305f2  b6ee005a  vmov.f32	s10, #5.000000e-01
080305f6  9ded096a  vldr	s12, [sp, #36]
080305fa  f7ee005a  vmov.f32	s11, #1.000000e+00
080305fe  ad4b      ldr	r3, [pc, #692] ; [0x080308b4] = 0x20002418
08030600  a2ee856a  vfma.f32	s12, s5, s10
08030604  b4eee56a  vcmpe.f32	s12, s11
08030608  83ed006a  vstr	s12, [r3]
0803060c  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030610  fff6b7ad  blt.w	#-1170 ; -> 0x08030182 ; branch_target=0x08030182
08030614  0028      cmp	r0, #0
08030616  3ff496ad  beq.w	#-1236 ; -> 0x08030146 ; branch_target=0x08030146
0803061a  91e5      b	#-1246 ; -> 0x08030140 ; branch_target=0x08030140
0803061c  a64b      ldr	r3, [pc, #664] ; [0x080308b8] = 0x200008e0
0803061e  f7ee007a  vmov.f32	s15, #1.000000e+00
08030622  83ed005a  vstr	s10, [r3]
08030626  a54b      ldr	r3, [pc, #660] ; [0x080308bc] = 0x20002410
08030628  c3ed005a  vstr	s11, [r3]
0803062c  a44b      ldr	r3, [pc, #656] ; [0x080308c0] = 0x200008c0
0803062e  83ed004a  vstr	s8, [r3]
08030632  a44b      ldr	r3, [pc, #656] ; [0x080308c4] = 0x200023f4
08030634  d3ed005a  vldr	s11, [r3]
08030638  a34b      ldr	r3, [pc, #652] ; [0x080308c8] = 0x20002414
0803063a  36ee256a  vadd.f32	s12, s12, s11
0803063e  b4eee76a  vcmpe.f32	s12, s15
08030642  83ed006a  vstr	s12, [r3]
08030646  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803064a  fff62aad  blt.w	#-1452 ; -> 0x080300a2 ; branch_target=0x080300a2
0803064e  1423      movs	r3, #20
08030650  9e4a      ldr	r2, [pc, #632] ; [0x080308cc] = 0x20002428
08030652  1360      str	r3, [r2]
08030654  07e5      b	#-1522 ; -> 0x08030066 ; branch_target=0x08030066
08030656  9e49      ldr	r1, [pc, #632] ; [0x080308d0] = 0x200012c0
08030658  8968      ldr	r1, [r1, #8]
0803065a  9142      cmp	r1, r2
0803065c  40f31786  ble.w	#3118 ; -> 0x0803128e ; branch_target=0x0803128e
08030660  d31a      subs	r3, r2, r3
08030662  07ee103a  vmov	s14, r3
08030666  9b4b      ldr	r3, [pc, #620] ; [0x080308d4] = 0x20001240
08030668  f8eec76a  vcvt.f32.s32	s13, s14
0803066c  93ed016a  vldr	s12, [r3, #4]
08030670  9fed997a  vldr	s14, [pc, #612] ; [0x080308d8] = 0x45000000 / f32_bits_interpretation=2048
08030674  a6ee867a  vfma.f32	s14, s13, s12
08030678  bdeec77a  vcvt.s32.f32	s14, s14
0803067c  17ee103a  vmov	r3, s14
08030680  fef75dbc  b.w	#-5958 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
08030684  9549      ldr	r1, [pc, #596] ; [0x080308dc] = 0x20001300
08030686  8968      ldr	r1, [r1, #8]
08030688  9142      cmp	r1, r2
0803068a  40f37f87  ble.w	#3838 ; -> 0x0803158c ; branch_target=0x0803158c
0803068e  d31a      subs	r3, r2, r3
08030690  07ee903a  vmov	s15, r3
08030694  924b      ldr	r3, [pc, #584] ; [0x080308e0] = 0x20001280
08030696  b8eee77a  vcvt.f32.s32	s14, s15
0803069a  d3ed016a  vldr	s13, [r3, #4]
0803069e  dfed8e7a  vldr	s15, [pc, #568] ; [0x080308d8] = 0x45000000 / f32_bits_interpretation=2048
080306a2  e7ee267a  vfma.f32	s15, s14, s13
080306a6  fdeee77a  vcvt.s32.f32	s15, s15
080306aa  17ee902a  vmov	r2, s15
080306ae  fef7fdbb  b.w	#-6150 ; -> 0x0802eeac ; branch_target=0x0802eeac
080306b2  9fed8c7a  vldr	s14, [pc, #560] ; [0x080308e4] = 0x3f7fbe77 / f32_bits_interpretation=0.9990000129
080306b6  dfed8c5a  vldr	s11, [pc, #560] ; [0x080308e8] = 0x3a83126f / f32_bits_interpretation=0.001000000047
080306ba  27ee877a  vmul.f32	s14, s15, s14
080306be  a6ee257a  vfma.f32	s14, s12, s11
080306c2  fef764bb  b.w	#-6456 ; -> 0x0802ed8e ; branch_target=0x0802ed8e
080306c6  9fed876a  vldr	s12, [pc, #540] ; [0x080308e4] = 0x3f7fbe77 / f32_bits_interpretation=0.9990000129
080306ca  9fed877a  vldr	s14, [pc, #540] ; [0x080308e8] = 0x3a83126f / f32_bits_interpretation=0.001000000047
080306ce  67ee867a  vmul.f32	s15, s15, s12
080306d2  e6ee877a  vfma.f32	s15, s13, s14
080306d6  fef76ebb  b.w	#-6436 ; -> 0x0802edb6 ; branch_target=0x0802edb6
080306da  844b      ldr	r3, [pc, #528] ; [0x080308ec] = 0x20001200
080306dc  dfed846a  vldr	s13, [pc, #528] ; [0x080308f0] = 0x3e99999a / f32_bits_interpretation=0.3000000119
080306e0  93ed066a  vldr	s12, [r3, #24]
080306e4  834b      ldr	r3, [pc, #524] ; [0x080308f4] = 0x20001380
080306e6  9b69      ldr	r3, [r3, #24]
080306e8  aceb0303  sub.w	r3, r12, r3
080306ec  07ee103a  vmov	s14, r3
080306f0  b8eec77a  vcvt.f32.s32	s14, s14
080306f4  27ee067a  vmul.f32	s14, s14, s12
080306f8  b4eee67a  vcmpe.f32	s14, s13
080306fc  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030700  40f17987  bpl.w	#3826 ; -> 0x080315f6 ; branch_target=0x080315f6
08030704  dfed7c6a  vldr	s13, [pc, #496] ; [0x080308f8] = 0x38d1b717 / f32_bits_interpretation=9.999999747e-05
08030708  67ee266a  vmul.f32	s13, s14, s13
0803070c  36eea27a  vadd.f32	s14, s13, s5
08030710  7a4b      ldr	r3, [pc, #488] ; [0x080308fc] = 0x20000860
08030712  f6ee006a  vmov.f32	s13, #5.000000e-01
08030716  27ee267a  vmul.f32	s14, s14, s13
0803071a  83ed007a  vstr	s14, [r3]
0803071e  fef7f7bb  b.w	#-6162 ; -> 0x0802ef10 ; branch_target=0x0802ef10
08030722  724b      ldr	r3, [pc, #456] ; [0x080308ec] = 0x20001200
08030724  9fed726a  vldr	s12, [pc, #456] ; [0x080308f0] = 0x3e99999a / f32_bits_interpretation=0.3000000119
08030728  d3ed075a  vldr	s11, [r3, #28]
0803072c  714b      ldr	r3, [pc, #452] ; [0x080308f4] = 0x20001380
0803072e  db69      ldr	r3, [r3, #28]
08030730  fb1a      subs	r3, r7, r3
08030732  06ee903a  vmov	s13, r3
08030736  f8eee66a  vcvt.f32.s32	s13, s13
0803073a  66eea56a  vmul.f32	s13, s13, s11
0803073e  f4eec66a  vcmpe.f32	s13, s12
08030742  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030746  40f14887  bpl.w	#3728 ; -> 0x080315da ; branch_target=0x080315da
0803074a  9fed6b6a  vldr	s12, [pc, #428] ; [0x080308f8] = 0x38d1b717 / f32_bits_interpretation=9.999999747e-05
0803074e  26ee866a  vmul.f32	s12, s13, s12
08030752  76ee276a  vadd.f32	s13, s12, s15
08030756  6a4b      ldr	r3, [pc, #424] ; [0x08030900] = 0x200023e4
08030758  b6ee006a  vmov.f32	s12, #5.000000e-01
0803075c  66ee869a  vmul.f32	s19, s13, s12
08030760  c3ed009a  vstr	s19, [r3]
08030764  fef786bc  b.w	#-5876 ; -> 0x0802f074 ; branch_target=0x0802f074
08030768  b2f5005f  cmp.w	r2, #8192
0803076c  02d9      bls	#4 ; -> 0x08030774 ; branch_target=0x08030774
0803076e  0023      movs	r3, #0
08030770  644a      ldr	r2, [pc, #400] ; [0x08030904] = 0x20002ec8
08030772  1360      str	r3, [r2]
08030774  0022      movs	r2, #0
08030776  9446      mov	r12, r2
08030778  1046      mov	r0, r2
0803077a  1146      mov	r1, r2
0803077c  1546      mov	r5, r2
0803077e  1646      mov	r6, r2
08030780  1446      mov	r4, r2
08030782  1746      mov	r7, r2
08030784  eae5      b	#-1068 ; -> 0x0803035c ; branch_target=0x0803035c
08030786  604b      ldr	r3, [pc, #384] ; [0x08030908] = 0x08038674
08030788  1946      mov	r1, r3
0803078a  6048      ldr	r0, [pc, #384] ; [0x0803090c] = 0x20002f6c
0803078c  dfed604a  vldr	s9, [pc, #384] ; [0x08030910] = 0x44800000 / f32_bits_interpretation=1024
08030790  0360      str	r3, [r0]
08030792  4d4b      ldr	r3, [pc, #308] ; [0x080308c8] = 0x20002414
08030794  93ed006a  vldr	s12, [r3]
08030798  66ee244a  vmul.f32	s9, s12, s9
0803079c  fdeee46a  vcvt.s32.f32	s13, s9
080307a0  16ee903a  vmov	r3, s13
080307a4  581c      adds	r0, r3, #1
080307a6  c3f30903  ubfx	r3, r3, #0, #10
080307aa  c0f30900  ubfx	r0, r0, #0, #10
080307ae  01eb8303  add.w	r3, r1, r3, lsl #2
080307b2  01eb8001  add.w	r1, r1, r0, lsl #2
080307b6  d3ed006a  vldr	s13, [r3]
080307ba  91ed002a  vldr	s4, [r1]
080307be  32ee662a  vsub.f32	s4, s4, s13
080307c2  b6eee41a  vrintz.f32	s2, s9
080307c6  74eec14a  vsub.f32	s9, s9, s2
080307ca  042a      cmp	r2, #4
080307cc  e2ee246a  vfma.f32	s13, s4, s9
080307d0  00f0fb86  beq.w	#3574 ; -> 0x080315ca ; branch_target=0x080315ca
080307d4  022a      cmp	r2, #2
080307d6  00f01c87  beq.w	#3640 ; -> 0x08031612 ; branch_target=0x08031612
080307da  002a      cmp	r2, #0
080307dc  00f03387  beq.w	#3686 ; -> 0x08031646 ; branch_target=0x08031646
080307e0  052a      cmp	r2, #5
080307e2  7ff4e0ab  bne.w	#-2112 ; -> 0x0802ffa6 ; branch_target=0x0802ffa6
080307e6  4b4b      ldr	r3, [pc, #300] ; [0x08030914] = 0x20002000
080307e8  b7ee082a  vmov.f32	s4, #1.500000e+00
080307ec  dfed4a0a  vldr	s1, [pc, #296] ; [0x08030918] = 0x38000000 / f32_bits_interpretation=3.051757812e-05
080307f0  9b6a      ldr	r3, [r3, #40]
080307f2  b0ee421a  vmov.f32	s2, s4
080307f6  c3f58033  rsb.w	r3, r3, #65536
080307fa  04ee903a  vmov	s9, r3
080307fe  f8eee44a  vcvt.f32.s32	s9, s9
08030802  a4eea01a  vfma.f32	s2, s9, s1
08030806  ffee084a  vmov.f32	s9, #-1.500000e+00
0803080a  61ee266a  vmul.f32	s13, s2, s13
0803080e  c6fec26a  vminnm.f32	s13, s13, s4
08030812  c6fea46a  vmaxnm.f32	s13, s13, s9
08030816  f7ee004a  vmov.f32	s9, #1.000000e+00
0803081a  9fed402a  vldr	s4, [pc, #256] ; [0x0803091c] = 0x3e17b426 / f32_bits_interpretation=0.1481481493
0803081e  26eee61a  vnmul.f32	s2, s13, s13
08030822  e1ee024a  vfma.f32	s9, s2, s4
08030826  66eea46a  vmul.f32	s13, s13, s9
0803082a  fff7bcbb  b.w	#-2184 ; -> 0x0802ffa6 ; branch_target=0x0802ffa6
0803082e  364b      ldr	r3, [pc, #216] ; [0x08030908] = 0x08038674
08030830  1a46      mov	r2, r3
08030832  3649      ldr	r1, [pc, #216] ; [0x0803090c] = 0x20002f6c
08030834  0b60      str	r3, [r1]
08030836  1f4b      ldr	r3, [pc, #124] ; [0x080308b4] = 0x20002418
08030838  93ed007a  vldr	s14, [r3]
0803083c  f0ee476a  vmov.f32	s13, s14
08030840  8ded097a  vstr	s14, [sp, #36]
08030844  9fed327a  vldr	s14, [pc, #200] ; [0x08030910] = 0x44800000 / f32_bits_interpretation=1024
08030848  26ee877a  vmul.f32	s14, s13, s14
0803084c  fdeec76a  vcvt.s32.f32	s13, s14
08030850  16ee903a  vmov	r3, s13
08030854  591c      adds	r1, r3, #1
08030856  c3f30903  ubfx	r3, r3, #0, #10
0803085a  c1f30901  ubfx	r1, r1, #0, #10
0803085e  02eb8303  add.w	r3, r2, r3, lsl #2
08030862  02eb8102  add.w	r2, r2, r1, lsl #2
08030866  d3ed003a  vldr	s7, [r3]
0803086a  d2ed006a  vldr	s13, [r2]
0803086e  049b      ldr	r3, [sp, #16]
08030870  76eee36a  vsub.f32	s13, s13, s7
08030874  b6eec76a  vrintz.f32	s12, s14
08030878  37ee467a  vsub.f32	s14, s14, s12
0803087c  042b      cmp	r3, #4
0803087e  e6ee873a  vfma.f32	s7, s13, s14
08030882  00f0a686  beq.w	#3404 ; -> 0x080315d2 ; branch_target=0x080315d2
08030886  022b      cmp	r3, #2
08030888  00f0cf86  beq.w	#3486 ; -> 0x0803162a ; branch_target=0x0803162a
0803088c  069a      ldr	r2, [sp, #24]
0803088e  1343      orrs	r3, r2
08030890  7ef404ad  bne.w	#-5624 ; -> 0x0802f29c ; branch_target=0x0802f29c
08030894  224b      ldr	r3, [pc, #136] ; [0x08030920] = 0x20002eac
08030896  b1ee007a  vmov.f32	s14, #4.000000e+00
0803089a  d3ed003a  vldr	s7, [r3]
0803089e  63ee873a  vmul.f32	s7, s7, s14
080308a2  fef7fbbc  b.w	#-5642 ; -> 0x0802f29c ; branch_target=0x0802f29c
080308a6  f0ee614a  vmov.f32	s9, s3
080308aa  b0ee621a  vmov.f32	s2, s5
080308ae  fff786bb  b.w	#-2292 ; -> 0x0802ffbe ; branch_target=0x0802ffbe
08030924  039b      ldr	r3, [sp, #12]
08030926  022b      cmp	r3, #2
08030928  01f01083  beq.w	#5664 ; -> 0x08031f4c ; branch_target=0x08031f4c
0803092c  d64b      ldr	r3, [pc, #856] ; [0x08030c88] = 0x20002400
0803092e  b0ee644a  vmov.f32	s8, s9
08030932  b0ee647a  vmov.f32	s14, s9
08030936  c3ed004a  vstr	s9, [r3]
0803093a  d44b      ldr	r3, [pc, #848] ; [0x08030c8c] = 0x20002404
0803093c  c3ed004a  vstr	s9, [r3]
08030940  d34b      ldr	r3, [pc, #844] ; [0x08030c90] = 0x20002424
08030942  b7eeca0a  vcvt.f64.f32	d0, s20
08030946  dfede26a  vldr	s13, [pc, #904] ; [0x08030cd0] = 0x45000000 / f32_bits_interpretation=2048
0803094a  b7ee001a  vmov.f32	s2, #1.000000e+00
0803094e  93ed00ea  vldr	s28, [r3]
08030952  b0ee002a  vmov.f32	s4, #2.000000e+00
08030956  9feddd6a  vldr	s12, [pc, #884] ; [0x08030ccc] = 0x3f266666 / f32_bits_interpretation=0.6499999762
0803095a  20ee000b  vmul.f64	d0, d0, d0
0803095e  cd4b      ldr	r3, [pc, #820] ; [0x08030c94] = 0x20002420
08030960  cd49      ldr	r1, [pc, #820] ; [0x08030c98] = 0x20002740
08030962  6eee2e4a  vmul.f32	s9, s28, s29
08030966  d3ed00aa  vldr	s21, [r3]
0803096a  2aee066a  vmul.f32	s12, s20, s12
0803096e  cb4a      ldr	r2, [pc, #812] ; [0x08030c9c] = 0x20000c40
08030970  e4ee876a  vfma.f32	s13, s9, s14
08030974  b7eec6da  vcvt.f64.f32	d13, s12
08030978  2aeeae7a  vmul.f32	s14, s21, s29
0803097c  27ee047a  vmul.f32	s14, s14, s8
08030980  bdeee66a  vcvt.s32.f32	s12, s13
08030984  fdeec74a  vcvt.s32.f32	s9, s14
08030988  16ee103a  vmov	r3, s12
0803098c  9fedd06a  vldr	s12, [pc, #832] ; [0x08030cd0] = 0x45000000 / f32_bits_interpretation=2048
08030990  5c1c      adds	r4, r3, #1
08030992  c3f30c03  ubfx	r3, r3, #0, #13
08030996  37ee066a  vadd.f32	s12, s14, s12
0803099a  14ee900a  vmov	r0, s9
0803099e  c4f30c04  ubfx	r4, r4, #0, #13
080309a2  0beb8303  add.w	r3, r11, r3, lsl #2
080309a6  f6eee64a  vrintz.f32	s9, s13
080309aa  76eee46a  vsub.f32	s13, s13, s9
080309ae  0beb8404  add.w	r4, r11, r4, lsl #2
080309b2  93ed004a  vldr	s8, [r3]
080309b6  f6eec74a  vrintz.f32	s9, s14
080309ba  37ee647a  vsub.f32	s14, s14, s9
080309be  d4ed004a  vldr	s9, [r4]
080309c2  441c      adds	r4, r0, #1
080309c4  c0f30c00  ubfx	r0, r0, #0, #13
080309c8  74eec44a  vsub.f32	s9, s9, s8
080309cc  c4f30c04  ubfx	r4, r4, #0, #13
080309d0  0beb8000  add.w	r0, r11, r0, lsl #2
080309d4  0beb8404  add.w	r4, r11, r4, lsl #2
080309d8  a4eea64a  vfma.f32	s8, s9, s13
080309dc  fdeec66a  vcvt.s32.f32	s13, s12
080309e0  d4ed004a  vldr	s9, [r4]
080309e4  16ee903a  vmov	r3, s13
080309e8  d0ed006a  vldr	s13, [r0]
080309ec  74eee64a  vsub.f32	s9, s9, s13
080309f0  581c      adds	r0, r3, #1
080309f2  c3f30c03  ubfx	r3, r3, #0, #13
080309f6  f0ee448a  vmov.f32	s17, s8
080309fa  c0f30c00  ubfx	r0, r0, #0, #13
080309fe  e4ee876a  vfma.f32	s13, s9, s14
08030a02  0beb8303  add.w	r3, r11, r3, lsl #2
08030a06  0beb8000  add.w	r0, r11, r0, lsl #2
08030a0a  93ed007a  vldr	s14, [r3]
08030a0e  d0ed004a  vldr	s9, [r0]
08030a12  059b      ldr	r3, [sp, #20]
08030a14  74eec74a  vsub.f32	s9, s9, s14
08030a18  b0ee663a  vmov.f32	s6, s13
08030a1c  f6eec66a  vrintz.f32	s13, s12
08030a20  76ee666a  vsub.f32	s13, s12, s13
08030a24  c3f10100  rsb.w	r0, r3, #1
08030a28  a4eea67a  vfma.f32	s14, s9, s13
08030a2c  9fed944b  vldr	d4, [pc, #592] ; [0x08030c80] = 0x66666666 / f64_bits_interpretation=0.34999999999999998
08030a30  a0ee04db  vfma.f64	d13, d0, d4
08030a34  f7eecd0b  vcvt.f32.f64	s1, d13
08030a38  c0fec10a  vminnm.f32	s1, s1, s2
08030a3c  20eea84a  vmul.f32	s8, s1, s17
08030a40  60ee836a  vmul.f32	s13, s1, s6
08030a44  20eea01a  vmul.f32	s2, s1, s1
08030a48  30eea03a  vadd.f32	s6, s1, s1
08030a4c  81ed004a  vstr	s8, [r1]
08030a50  24ee046a  vmul.f32	s12, s8, s8
08030a54  c2ed006a  vstr	s13, [r2]
08030a58  b1ee410a  vneg.f32	s0, s2
08030a5c  23ee073a  vmul.f32	s6, s6, s14
08030a60  b0ee417a  vmov.f32	s14, s2
08030a64  66ee834a  vmul.f32	s9, s13, s6
08030a68  96ee027a  vfnms.f32	s14, s12, s4
08030a6c  9fed8c6a  vldr	s12, [pc, #560] ; [0x08030ca0] = 0x3de66666 / f32_bits_interpretation=0.112499997
08030a70  34ee042a  vadd.f32	s4, s8, s8
08030a74  f4eec67a  vcmpe.f32	s15, s12
08030a78  c2ed014a  vstr	s9, [r2, #4]
08030a7c  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030a80  81ed017a  vstr	s14, [r1, #4]
08030a84  41f10083  bpl.w	#5632 ; -> 0x08032088 ; branch_target=0x08032088
08030a88  b1ee42aa  vneg.f32	s20, s4
08030a8c  854b      ldr	r3, [pc, #532] ; [0x08030ca4] = 0x20002c40
08030a8e  9fed866a  vldr	s12, [pc, #536] ; [0x08030ca8] = 0x00000000
08030a92  f1ee43da  vneg.f32	s27, s6
08030a96  cded2cfa  vstr	s31, [sp, #176]
08030a9a  03eb0020  add.w	r0, r3, r0, lsl #8
08030a9e  f0ee468a  vmov.f32	s17, s12
08030aa2  824c      ldr	r4, [pc, #520] ; [0x08030cac] = 0x20000f40
08030aa4  f0ee4afa  vmov.f32	s31, s20
08030aa8  0425      movs	r5, #4
08030aaa  9fed81da  vldr	s26, [pc, #516] ; [0x08030cb0] = 0x3ee66666 / f32_bits_interpretation=0.4499999881
08030aae  cded132a  vstr	s5, [sp, #76]
08030ab2  cded230a  vstr	s1, [sp, #140]
08030ab6  8ded26ca  vstr	s24, [sp, #152]
08030aba  cded273a  vstr	s7, [sp, #156]
08030abe  8ded28ba  vstr	s22, [sp, #160]
08030ac2  cded29ba  vstr	s23, [sp, #164]
08030ac6  8ded2a1a  vstr	s2, [sp, #168]
08030aca  cded2bca  vstr	s25, [sp, #172]
08030ace  8ded2dfa  vstr	s30, [sp, #180]
08030ad2  cded2e5a  vstr	s11, [sp, #184]
08030ad6  02e0      b	#4 ; -> 0x08030ade ; branch_target=0x08030ade
08030ad8  3a2e      cmp	r6, #58
08030ada  00f38380  bgt.w	#262 ; -> 0x08030be4 ; branch_target=0x08030be4
08030ade  93ed001a  vldr	s2, [r3]
08030ae2  f0ee445a  vmov.f32	s11, s8
08030ae6  94ed00aa  vldr	s20, [r4]
08030aea  f0ee662a  vmov.f32	s5, s13
08030aee  d3ed02ca  vldr	s25, [r3, #8]
08030af2  b0ee44ca  vmov.f32	s24, s8
08030af6  31ee0a1a  vadd.f32	s2, s2, s20
08030afa  94ed02aa  vldr	s20, [r4, #8]
08030afe  d2ee075a  vfnms.f32	s11, s4, s14
08030b02  93ed01ba  vldr	s22, [r3, #4]
08030b06  d3ee242a  vfnms.f32	s5, s6, s9
08030b0a  94ed01fa  vldr	s30, [r4, #4]
08030b0e  7cee8aca  vadd.f32	s25, s25, s20
08030b12  d4ed03ba  vldr	s23, [r4, #12]
08030b16  93ed03aa  vldr	s20, [r3, #12]
08030b1a  3bee0ffa  vadd.f32	s30, s22, s30
08030b1e  afee87ca  vfma.f32	s24, s31, s14
08030b22  83ed001a  vstr	s2, [r3]
08030b26  7aee2bba  vadd.f32	s23, s20, s23
08030b2a  c3ed02ca  vstr	s25, [r3, #8]
08030b2e  f0ee643a  vmov.f32	s7, s9
08030b32  83ed01fa  vstr	s30, [r3, #4]
08030b36  f0ee470a  vmov.f32	s1, s14
08030b3a  2e46      mov	r6, r5
08030b3c  c3ed03ba  vstr	s23, [r3, #12]
08030b40  edeea46a  vfma.f32	s13, s27, s9
08030b44  d0ed02ba  vldr	s23, [r0, #8]
08030b48  d2ee833a  vfnms.f32	s7, s5, s6
08030b4c  d5ee820a  vfnms.f32	s1, s11, s4
08030b50  d0ed00ca  vldr	s25, [r0]
08030b54  b0ee64ba  vmov.f32	s22, s9
08030b58  0435      adds	r5, #4
08030b5a  b0ee44aa  vmov.f32	s20, s8
08030b5e  90ed031a  vldr	s2, [r0, #12]
08030b62  6beee5ba  vnmul.f32	s23, s23, s11
08030b66  90ed01fa  vldr	s30, [r0, #4]
08030b6a  b0ee4c4a  vmov.f32	s8, s24
08030b6e  c1ed025a  vstr	s11, [r1, #8]
08030b72  a2eec3ba  vfms.f32	s22, s5, s6
08030b76  c2ed022a  vstr	s5, [r2, #8]
08030b7a  ecee8aba  vfma.f32	s23, s25, s20
08030b7e  0aee105a  vmov	s20, r5
08030b82  a0ee824a  vfma.f32	s8, s1, s4
08030b86  c1ed030a  vstr	s1, [r1, #12]
08030b8a  e3ee836a  vfma.f32	s13, s7, s6
08030b8e  c2ed033a  vstr	s7, [r2, #12]
08030b92  a5eec27a  vfms.f32	s14, s11, s4
08030b96  1031      adds	r1, #16
08030b98  b8eecaaa  vcvt.f32.s32	s20, s20
08030b9c  1032      adds	r2, #16
08030b9e  b0ee64ca  vmov.f32	s24, s9
08030ba2  1033      adds	r3, #16
08030ba4  23ee811a  vmul.f32	s2, s7, s2
08030ba8  1034      adds	r4, #16
08030baa  f0ee4b4a  vmov.f32	s9, s22
08030bae  81ed004a  vstr	s8, [r1]
08030bb2  2aee27aa  vmul.f32	s20, s20, s15
08030bb6  c2ed006a  vstr	s13, [r2]
08030bba  a4ee027a  vfma.f32	s14, s8, s4
08030bbe  1030      adds	r0, #16
08030bc0  e6ee834a  vfma.f32	s9, s13, s6
08030bc4  afee4c1a  vfms.f32	s2, s30, s24
08030bc8  b4ee4daa  vcmp.f32	s20, s26
08030bcc  78eeab8a  vadd.f32	s17, s17, s23
08030bd0  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030bd4  81ed017a  vstr	s14, [r1, #4]
08030bd8  c2ed014a  vstr	s9, [r2, #4]
08030bdc  36ee016a  vadd.f32	s12, s12, s2
08030be0  3ff57aaf  bmi.w	#-268 ; -> 0x08030ad8 ; branch_target=0x08030ad8
08030be4  38ee867a  vadd.f32	s14, s17, s12
08030be8  dded132a  vldr	s5, [sp, #76]
08030bec  dded230a  vldr	s1, [sp, #140]
08030bf0  9ded26ca  vldr	s24, [sp, #152]
08030bf4  b0eec77a  vabs.f32	s14, s14
08030bf8  dded273a  vldr	s7, [sp, #156]
08030bfc  9ded28ba  vldr	s22, [sp, #160]
08030c00  dded29ba  vldr	s23, [sp, #164]
08030c04  9ded2a1a  vldr	s2, [sp, #168]
08030c08  dded2bca  vldr	s25, [sp, #172]
08030c0c  dded2cfa  vldr	s31, [sp, #176]
08030c10  9ded2dfa  vldr	s30, [sp, #180]
08030c14  dded2e5a  vldr	s11, [sp, #184]
08030c18  2f9b      ldr	r3, [sp, #188]
08030c1a  dfed264a  vldr	s9, [pc, #152] ; [0x08030cb4] = 0x4118234e / f32_bits_interpretation=9.508619308
08030c1e  d3ed006a  vldr	s13, [r3]
08030c22  f4eec76a  vcmpe.f32	s13, s14
08030c26  dfed246a  vldr	s13, [pc, #144] ; [0x08030cb8] = 0x4202b291 / f32_bits_interpretation=32.67438126
08030c2a  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030c2e  48bf      it	mi
08030c30  83ed007a  vstrmi	s14, [r3]
08030c34  9fed217a  vldr	s14, [pc, #132] ; [0x08030cbc] = 0x41de117d / f32_bits_interpretation=27.7585392
08030c38  27ee607a  vnmul.f32	s14, s14, s1
08030c3c  a1ee267a  vfma.f32	s14, s2, s13
08030c40  dfed1f6a  vldr	s13, [pc, #124] ; [0x08030cc0] = 0x41577473 / f32_bits_interpretation=13.46592999
08030c44  60eea66a  vmul.f32	s13, s1, s13
08030c48  37ee247a  vadd.f32	s14, s14, s9
08030c4c  a0ee267a  vfma.f32	s14, s0, s13
08030c50  dfed1c6a  vldr	s13, [pc, #112] ; [0x08030cc4] = 0x3d23d70a / f32_bits_interpretation=0.03999999911
08030c54  f4eee60a  vcmpe.f32	s1, s13
08030c58  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030c5c  05dc      bgt	#10 ; -> 0x08030c6a ; branch_target=0x08030c6a
08030c5e  f3ee096a  vmov.f32	s13, #2.500000e+01
08030c62  60eea60a  vmul.f32	s1, s1, s13
08030c66  27ee207a  vmul.f32	s14, s14, s1
08030c6a  174b      ldr	r3, [pc, #92] ; [0x08030cc8] = 0x20002e50
08030c6c  26ee073a  vmul.f32	s6, s12, s14
08030c70  28ee877a  vmul.f32	s14, s17, s14
08030c74  93ed004a  vldr	s8, [r3]
08030c78  fff742b9  b.w	#-3452 ; -> 0x0802ff00 ; branch_target=0x0802ff00
08030cd4  b7eee84a  vcvt.f64.f32	d4, s17
08030cd8  1fed046a  vldr	s12, [pc, #-16] ; [0x08030ccc] = 0x3f266666 / f32_bits_interpretation=0.6499999762
08030cdc  1fed047a  vldr	s14, [pc, #-16] ; [0x08030cd0] = 0x45000000 / f32_bits_interpretation=2048
08030ce0  28ee866a  vmul.f32	s12, s17, s12
08030ce4  24ee044b  vmul.f64	d4, d4, d4
08030ce8  a9ee2e7a  vfma.f32	s14, s18, s29
08030cec  b7eec66a  vcvt.f64.f32	d6, s12
08030cf0  9fedbf0b  vldr	d0, [pc, #764] ; [0x08030ff0] = 0x66666666 / f64_bits_interpretation=0.34999999999999998
08030cf4  a4ee006b  vfma.f64	d6, d4, d0
08030cf8  b6eec73a  vrintz.f32	s6, s14
08030cfc  37ee433a  vsub.f32	s6, s14, s6
08030d00  bdeec77a  vcvt.s32.f32	s14, s14
08030d04  68ee2e4a  vmul.f32	s9, s16, s29
08030d08  b7ee004a  vmov.f32	s8, #1.000000e+00
08030d0c  17ee103a  vmov	r3, s14
08030d10  9fedb97a  vldr	s14, [pc, #740] ; [0x08030ff8] = 0x45000000 / f32_bits_interpretation=2048
08030d14  5a1c      adds	r2, r3, #1
08030d16  34ee877a  vadd.f32	s14, s9, s14
08030d1a  c3f30c03  ubfx	r3, r3, #0, #13
08030d1e  f7eec60b  vcvt.f32.f64	s1, d6
08030d22  c2f30c02  ubfx	r2, r2, #0, #13
08030d26  f6eee46a  vrintz.f32	s13, s9
08030d2a  b6eec76a  vrintz.f32	s12, s14
08030d2e  37ee466a  vsub.f32	s12, s14, s12
08030d32  bdeec77a  vcvt.s32.f32	s14, s14
08030d36  0beb8303  add.w	r3, r11, r3, lsl #2
08030d3a  0beb8202  add.w	r2, r11, r2, lsl #2
08030d3e  c0fec40a  vminnm.f32	s1, s1, s8
08030d42  93ed004a  vldr	s8, [r3]
08030d46  74eee66a  vsub.f32	s13, s9, s13
08030d4a  17ee101a  vmov	r1, s14
08030d4e  92ed007a  vldr	s14, [r2]
08030d52  30eea02a  vadd.f32	s4, s1, s1
08030d56  37ee447a  vsub.f32	s14, s14, s8
08030d5a  4a1c      adds	r2, r1, #1
08030d5c  c1f30c01  ubfx	r1, r1, #0, #13
08030d60  20eea00a  vmul.f32	s0, s1, s1
08030d64  c2f30c02  ubfx	r2, r2, #0, #13
08030d68  a7ee034a  vfma.f32	s8, s14, s6
08030d6c  0beb8101  add.w	r1, r11, r1, lsl #2
08030d70  bdeee47a  vcvt.s32.f32	s14, s9
08030d74  0beb8202  add.w	r2, r11, r2, lsl #2
08030d78  f1ee408a  vneg.f32	s17, s0
08030d7c  d2ed004a  vldr	s9, [r2]
08030d80  17ee103a  vmov	r3, s14
08030d84  91ed007a  vldr	s14, [r1]
08030d88  9c49      ldr	r1, [pc, #624] ; [0x08030ffc] = 0x20002440
08030d8a  74eec74a  vsub.f32	s9, s9, s14
08030d8e  5a1c      adds	r2, r3, #1
08030d90  c3f30c03  ubfx	r3, r3, #0, #13
08030d94  24ee204a  vmul.f32	s8, s8, s1
08030d98  c2f30c02  ubfx	r2, r2, #0, #13
08030d9c  a4ee867a  vfma.f32	s14, s9, s12
08030da0  0beb8303  add.w	r3, r11, r3, lsl #2
08030da4  0beb8202  add.w	r2, r11, r2, lsl #2
08030da8  24ee043a  vmul.f32	s6, s8, s8
08030dac  d3ed004a  vldr	s9, [r3]
08030db0  b0ee406a  vmov.f32	s12, s0
08030db4  069b      ldr	r3, [sp, #24]
08030db6  34ee041a  vadd.f32	s2, s8, s8
08030dba  81ed004a  vstr	s8, [r1]
08030dbe  c3f10100  rsb.w	r0, r3, #1
08030dc2  22ee072a  vmul.f32	s4, s4, s14
08030dc6  92ed007a  vldr	s14, [r2]
08030dca  8d4a      ldr	r2, [pc, #564] ; [0x08031000] = 0x20000b40
08030dcc  37ee647a  vsub.f32	s14, s14, s9
08030dd0  e7ee264a  vfma.f32	s9, s14, s13
08030dd4  b0ee007a  vmov.f32	s14, #2.000000e+00
08030dd8  93ee076a  vfnms.f32	s12, s6, s14
08030ddc  9fed897a  vldr	s14, [pc, #548] ; [0x08031004] = 0x3de66666 / f32_bits_interpretation=0.112499997
08030de0  64eea04a  vmul.f32	s9, s9, s1
08030de4  f4eec71a  vcmpe.f32	s3, s14
08030de8  24ee823a  vmul.f32	s6, s9, s4
08030dec  c2ed004a  vstr	s9, [r2]
08030df0  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030df4  81ed016a  vstr	s12, [r1, #4]
08030df8  82ed013a  vstr	s6, [r2, #4]
08030dfc  41f13a81  bpl.w	#4724 ; -> 0x08032074 ; branch_target=0x08032074
08030e00  814b      ldr	r3, [pc, #516] ; [0x08031008] = 0x20002a40
08030e02  b1ee41fa  vneg.f32	s30, s2
08030e06  dfed816a  vldr	s13, [pc, #516] ; [0x0803100c] = 0x00000000
08030e0a  b1ee42ea  vneg.f32	s28, s4
08030e0e  03eb0020  add.w	r0, r3, r0, lsl #8
08030e12  7f4c      ldr	r4, [pc, #508] ; [0x08031010] = 0x20000d40
08030e14  b0ee667a  vmov.f32	s14, s13
08030e18  0425      movs	r5, #4
08030e1a  dfed7eda  vldr	s27, [pc, #504] ; [0x08031014] = 0x3ee66666 / f32_bits_interpretation=0.4499999881
08030e1e  8ded13aa  vstr	s20, [sp, #76]
08030e22  cded232a  vstr	s5, [sp, #140]
08030e26  cded267a  vstr	s15, [sp, #152]
08030e2a  cded273a  vstr	s7, [sp, #156]
08030e2e  8ded28ba  vstr	s22, [sp, #160]
08030e32  cded29ba  vstr	s23, [sp, #164]
08030e36  cded2aca  vstr	s25, [sp, #168]
08030e3a  02e0      b	#4 ; -> 0x08030e42 ; branch_target=0x08030e42
08030e3c  3a2e      cmp	r6, #58
08030e3e  00f38380  bgt.w	#262 ; -> 0x08030f48 ; branch_target=0x08030f48
08030e42  93ed00aa  vldr	s20, [r3]
08030e46  f0ee447a  vmov.f32	s15, s8
08030e4a  94ed00ba  vldr	s22, [r4]
08030e4e  f0ee642a  vmov.f32	s5, s9
08030e52  93ed02da  vldr	s26, [r3, #8]
08030e56  f0ee44ca  vmov.f32	s25, s8
08030e5a  3aee0baa  vadd.f32	s20, s20, s22
08030e5e  94ed02ba  vldr	s22, [r4, #8]
08030e62  d1ee067a  vfnms.f32	s15, s2, s12
08030e66  d3ed01ba  vldr	s23, [r3, #4]
08030e6a  d2ee032a  vfnms.f32	s5, s4, s6
08030e6e  d4ed01fa  vldr	s31, [r4, #4]
08030e72  3dee0bda  vadd.f32	s26, s26, s22
08030e76  94ed03ca  vldr	s24, [r4, #12]
08030e7a  93ed03ba  vldr	s22, [r3, #12]
08030e7e  7beeaffa  vadd.f32	s31, s23, s31
08030e82  efee06ca  vfma.f32	s25, s30, s12
08030e86  83ed00aa  vstr	s20, [r3]
08030e8a  3bee0cca  vadd.f32	s24, s22, s24
08030e8e  83ed02da  vstr	s26, [r3, #8]
08030e92  f0ee433a  vmov.f32	s7, s6
08030e96  c3ed01fa  vstr	s31, [r3, #4]
08030e9a  f0ee46aa  vmov.f32	s21, s12
08030e9e  2e46      mov	r6, r5
08030ea0  83ed03ca  vstr	s24, [r3, #12]
08030ea4  eeee034a  vfma.f32	s9, s28, s6
08030ea8  90ed02ca  vldr	s24, [r0, #8]
08030eac  d2ee823a  vfnms.f32	s7, s5, s4
08030eb0  d7ee81aa  vfnms.f32	s21, s15, s2
08030eb4  90ed00da  vldr	s26, [r0]
08030eb8  f0ee43ba  vmov.f32	s23, s6
08030ebc  0435      adds	r5, #4
08030ebe  b0ee44ba  vmov.f32	s22, s8
08030ec2  90ed03aa  vldr	s20, [r0, #12]
08030ec6  2cee67ca  vnmul.f32	s24, s24, s15
08030eca  d0ed01fa  vldr	s31, [r0, #4]
08030ece  b0ee6c4a  vmov.f32	s8, s25
08030ed2  c1ed027a  vstr	s15, [r1, #8]
08030ed6  e2eec2ba  vfms.f32	s23, s5, s4
08030eda  c2ed022a  vstr	s5, [r2, #8]
08030ede  adee0bca  vfma.f32	s24, s26, s22
08030ee2  0bee105a  vmov	s22, r5
08030ee6  aaee814a  vfma.f32	s8, s21, s2
08030eea  c1ed03aa  vstr	s21, [r1, #12]
08030eee  e3ee824a  vfma.f32	s9, s7, s4
08030ef2  c2ed033a  vstr	s7, [r2, #12]
08030ef6  a7eec16a  vfms.f32	s12, s15, s2
08030efa  1031      adds	r1, #16
08030efc  b8eecbba  vcvt.f32.s32	s22, s22
08030f00  1032      adds	r2, #16
08030f02  f0ee43ca  vmov.f32	s25, s6
08030f06  1033      adds	r3, #16
08030f08  23ee8aaa  vmul.f32	s20, s7, s20
08030f0c  1034      adds	r4, #16
08030f0e  b0ee6b3a  vmov.f32	s6, s23
08030f12  81ed004a  vstr	s8, [r1]
08030f16  2bee21ba  vmul.f32	s22, s22, s3
08030f1a  c2ed004a  vstr	s9, [r2]
08030f1e  a4ee016a  vfma.f32	s12, s8, s2
08030f22  1030      adds	r0, #16
08030f24  a4ee823a  vfma.f32	s6, s9, s4
08030f28  afeeecaa  vfms.f32	s20, s31, s25
08030f2c  b4ee6dba  vcmp.f32	s22, s27
08030f30  37ee0c7a  vadd.f32	s14, s14, s24
08030f34  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030f38  81ed016a  vstr	s12, [r1, #4]
08030f3c  82ed013a  vstr	s6, [r2, #4]
08030f40  76ee8a6a  vadd.f32	s13, s13, s20
08030f44  3ff57aaf  bmi.w	#-268 ; -> 0x08030e3c ; branch_target=0x08030e3c
08030f48  b0eee64a  vabs.f32	s8, s13
08030f4c  9ded13aa  vldr	s20, [sp, #76]
08030f50  f0eec74a  vabs.f32	s9, s14
08030f54  dded232a  vldr	s5, [sp, #140]
08030f58  dded267a  vldr	s15, [sp, #152]
08030f5c  dded273a  vldr	s7, [sp, #156]
08030f60  9ded28ba  vldr	s22, [sp, #160]
08030f64  dded29ba  vldr	s23, [sp, #164]
08030f68  dded2aca  vldr	s25, [sp, #168]
08030f6c  259b      ldr	r3, [sp, #148]
08030f6e  93ed006a  vldr	s12, [r3]
08030f72  b4eee46a  vcmpe.f32	s12, s9
08030f76  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030f7a  03d5      bpl	#6 ; -> 0x08030f84 ; branch_target=0x08030f84
08030f7c  b0ee646a  vmov.f32	s12, s9
08030f80  c3ed004a  vstr	s9, [r3]
08030f84  b4eec64a  vcmpe.f32	s8, s12
08030f88  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030f8c  02dd      ble	#4 ; -> 0x08030f94 ; branch_target=0x08030f94
08030f8e  259b      ldr	r3, [sp, #148]
08030f90  83ed004a  vstr	s8, [r3]
08030f94  9fed206a  vldr	s12, [pc, #128] ; [0x08031018] = 0x41de117d / f32_bits_interpretation=27.7585392
08030f98  dfed204a  vldr	s9, [pc, #128] ; [0x0803101c] = 0x4202b291 / f32_bits_interpretation=32.67438126
08030f9c  26ee606a  vnmul.f32	s12, s12, s1
08030fa0  9fed1f4a  vldr	s8, [pc, #124] ; [0x08031020] = 0x4118234e / f32_bits_interpretation=9.508619308
08030fa4  a0ee246a  vfma.f32	s12, s0, s9
08030fa8  dfed1e4a  vldr	s9, [pc, #120] ; [0x08031024] = 0x41577473 / f32_bits_interpretation=13.46592999
08030fac  60eea44a  vmul.f32	s9, s1, s9
08030fb0  36ee046a  vadd.f32	s12, s12, s8
08030fb4  9fed1c4a  vldr	s8, [pc, #112] ; [0x08031028] = 0x3d23d70a / f32_bits_interpretation=0.03999999911
08030fb8  f4eec40a  vcmpe.f32	s1, s8
08030fbc  a4eea86a  vfma.f32	s12, s9, s17
08030fc0  f1ee10fa  vmrs	APSR_nzcv, fpscr
08030fc4  05dc      bgt	#10 ; -> 0x08030fd2 ; branch_target=0x08030fd2
08030fc6  f3ee094a  vmov.f32	s9, #2.500000e+01
08030fca  60eea40a  vmul.f32	s1, s1, s9
08030fce  26ee206a  vmul.f32	s12, s12, s1
08030fd2  164b      ldr	r3, [pc, #88] ; [0x0803102c] = 0x20001140
08030fd4  27ee067a  vmul.f32	s14, s14, s12
08030fd8  66ee866a  vmul.f32	s13, s13, s12
08030fdc  d3ed004a  vldr	s9, [r3]
08030fe0  22eea46a  vmul.f32	s12, s5, s9
08030fe4  dded074a  vldr	s9, [sp, #28]
08030fe8  26ee24ca  vmul.f32	s24, s12, s9
08030fec  fef713be  b.w	#-5082 ; -> 0x0802fc16 ; branch_target=0x0802fc16
08031038  29ee2e7a  vmul.f32	s14, s18, s29
0803103c  5fed046a  vldr	s13, [pc, #-16] ; [0x08031030] = 0x46800000 / f32_bits_interpretation=16384
08031040  1fed046a  vldr	s12, [pc, #-16] ; [0x08031034] = 0x46c35000 / f32_bits_interpretation=25000
08031044  68ee266a  vmul.f32	s13, s16, s13
08031048  b6eec74a  vrintz.f32	s8, s14
0803104c  37ee444a  vsub.f32	s8, s14, s8
08031050  bdeec77a  vcvt.s32.f32	s14, s14
08031054  20ee866a  vmul.f32	s12, s1, s12
08031058  17ee102a  vmov	r2, s14
0803105c  bdeee67a  vcvt.s32.f32	s14, s13
08031060  bdeec66a  vcvt.s32.f32	s12, s12
08031064  501c      adds	r0, r2, #1
08031066  c2f30c02  ubfx	r2, r2, #0, #13
0803106a  17ee104a  vmov	r4, s14
0803106e  b6eee67a  vrintz.f32	s14, s13
08031072  c0f30c01  ubfx	r1, r0, #0, #13
08031076  0beb8202  add.w	r2, r11, r2, lsl #2
0803107a  76eec76a  vsub.f32	s13, s13, s14
0803107e  0beb8100  add.w	r0, r11, r1, lsl #2
08031082  d2ed004a  vldr	s9, [r2]
08031086  c4f30c01  ubfx	r1, r4, #0, #13
0803108a  16ee102a  vmov	r2, s12
0803108e  90ed007a  vldr	s14, [r0]
08031092  601c      adds	r0, r4, #1
08031094  0beb8101  add.w	r1, r11, r1, lsl #2
08031098  c0f30c00  ubfx	r0, r0, #0, #13
0803109c  37ee647a  vsub.f32	s14, s14, s9
080310a0  0beb8000  add.w	r0, r11, r0, lsl #2
080310a4  e7ee044a  vfma.f32	s9, s14, s8
080310a8  91ed004a  vldr	s8, [r1]
080310ac  90ed007a  vldr	s14, [r0]
080310b0  c2f30a01  ubfx	r1, r2, #0, #11
080310b4  d212      asrs	r2, r2, #11
080310b6  37ee447a  vsub.f32	s14, s14, s8
080310ba  9340      lsls	r3, r2
080310bc  a7ee264a  vfma.f32	s8, s14, s13
080310c0  07ee103a  vmov	s14, r3
080310c4  089b      ldr	r3, [sp, #32]
080310c6  b8eec77a  vcvt.f32.s32	s14, s14
080310ca  03eb8101  add.w	r1, r3, r1, lsl #2
080310ce  7b4b      ldr	r3, [pc, #492] ; [0x080312bc] = 0x200023d0
080310d0  1b68      ldr	r3, [r3]
080310d2  d1ed006a  vldr	s13, [r1]
080310d6  002b      cmp	r3, #0
080310d8  40f0f083  bne.w	#2016 ; -> 0x080318bc ; branch_target=0x080318bc
080310dc  b1ee006a  vmov.f32	s12, #4.000000e+00
080310e0  66ee866a  vmul.f32	s13, s13, s12
080310e4  26ee877a  vmul.f32	s14, s13, s14
080310e8  dfed756a  vldr	s13, [pc, #468] ; [0x080312c0] = 0x37aec33e / f32_bits_interpretation=2.083333311e-05
080310ec  754a      ldr	r2, [pc, #468] ; [0x080312c4] = 0x2000239c
080310ee  27ee266a  vmul.f32	s12, s14, s13
080310f2  b2ee047a  vmov.f32	s14, #1.000000e+01
080310f6  d2ed006a  vldr	s13, [r2]
080310fa  82ed016a  vstr	s12, [r2, #4]
080310fe  f4eec76a  vcmpe.f32	s13, s14
08031102  92ed027a  vldr	s14, [r2, #8]
08031106  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803110a  c8bf      it	gt
0803110c  dfed6e6a  vldrgt	s13, [pc, #440] ; [0x080312c8] = 0x00000000
08031110  e6ee076a  vfma.f32	s13, s12, s14
08031114  b7ee007a  vmov.f32	s14, #1.000000e+00
08031118  f4eec76a  vcmpe.f32	s13, s14
0803111c  c2ed006a  vstr	s13, [r2]
08031120  f1ee10fa  vmrs	APSR_nzcv, fpscr
08031124  c0f2d486  blt.w	#3496 ; -> 0x08031ed0 ; branch_target=0x08031ed0
08031128  6848      ldr	r0, [pc, #416] ; [0x080312cc] = 0x200023cc
0803112a  beee001a  vmov.f32	s2, #-5.000000e-01
0803112e  684c      ldr	r4, [pc, #416] ; [0x080312d0] = 0x0bb38435
08031130  0368      ldr	r3, [r0]
08031132  6849      ldr	r1, [pc, #416] ; [0x080312d4] = 0x3619636b / f32_bits_interpretation=2.28566455e-06
08031134  dfed680a  vldr	s1, [pc, #416] ; [0x080312d8] = 0x2f80000d / f32_bits_interpretation=2.328310045e-10
08031138  04fb0311  mla	r1, r4, r3, r1
0803113c  07ee101a  vmov	s14, r1
08031140  0160      str	r1, [r0]
08031142  b8ee477a  vcvt.f32.u32	s14, s14
08031146  a7ee201a  vfma.f32	s2, s14, s1
0803114a  92ed057a  vldr	s14, [r2, #20]
0803114e  f0ee000a  vmov.f32	s1, #2.000000e+00
08031152  b0ee470a  vmov.f32	s0, s14
08031156  82ed037a  vstr	s14, [r2, #12]
0803115a  91ee200a  vfnms.f32	s0, s2, s1
0803115e  b6eee61a  vrintz.f32	s2, s13
08031162  76eec16a  vsub.f32	s13, s13, s2
08031166  c2ed006a  vstr	s13, [r2]
0803116a  82ed040a  vstr	s0, [r2, #16]
0803116e  feeeca6a  vcvt.s32.f32	s13, s13, #12
08031172  9fed5a1a  vldr	s2, [pc, #360] ; [0x080312dc] = 0x437f0000 / f32_bits_interpretation=255
08031176  5a49      ldr	r1, [pc, #360] ; [0x080312e0] = 0x08042674
08031178  f6ee00aa  vmov.f32	s21, #5.000000e-01
0803117c  22ee012a  vmul.f32	s4, s4, s2
08031180  9fed581a  vldr	s2, [pc, #352] ; [0x080312e4] = 0x403cff8b / f32_bits_interpretation=2.953097105
08031184  16ee903a  vmov	r3, s13
08031188  dfed570a  vldr	s1, [pc, #348] ; [0x080312e8] = 0x3f666666 / f32_bits_interpretation=0.8999999762
0803118c  20ee2a0a  vmul.f32	s0, s0, s21
08031190  a3f50063  sub.w	r3, r3, #2048
08031194  bdeec22a  vcvt.s32.f32	s4, s4
08031198  23ee203a  vmul.f32	s6, s6, s1
0803119c  c3f30c03  ubfx	r3, r3, #0, #13
080311a0  0beb8303  add.w	r3, r11, r3, lsl #2
080311a4  d3ed006a  vldr	s13, [r3]
080311a8  12ee103a  vmov	r3, s4
080311ac  01eb8303  add.w	r3, r1, r3, lsl #2
080311b0  93ed002a  vldr	s4, [r3]
080311b4  4d4b      ldr	r3, [pc, #308] ; [0x080312ec] = 0x2000233c
080311b6  22ee012a  vmul.f32	s4, s4, s2
080311ba  d3ed038a  vldr	s17, [r3, #12]
080311be  93ed021a  vldr	s2, [r3, #8]
080311c2  22ee062a  vmul.f32	s4, s4, s12
080311c6  b7ee006a  vmov.f32	s12, #1.000000e+00
080311ca  e2ee018a  vfma.f32	s17, s4, s2
080311ce  83ed002a  vstr	s4, [r3]
080311d2  76ee866a  vadd.f32	s13, s13, s12
080311d6  93ed0e6a  vldr	s12, [r3, #56]
080311da  23ee023a  vmul.f32	s6, s6, s4
080311de  a6ee807a  vfma.f32	s14, s13, s0
080311e2  93ed0f0a  vldr	s0, [r3, #60]
080311e6  83ed0c3a  vstr	s6, [r3, #48]
080311ea  f0ee686a  vmov.f32	s13, s17
080311ee  c3ed038a  vstr	s17, [r3, #12]
080311f2  a3ee060a  vfma.f32	s0, s6, s12
080311f6  e6ee6a6a  vfms.f32	s13, s12, s21
080311fa  82ed057a  vstr	s14, [r2, #20]
080311fe  83ed0f0a  vstr	s0, [r3, #60]
08031202  76eec06a  vsub.f32	s13, s13, s0
08031206  a3ee266a  vfma.f32	s12, s6, s13
0803120a  c3ed106a  vstr	s13, [r3, #64]
0803120e  b0eee63a  vabs.f32	s6, s13
08031212  83ed0e6a  vstr	s12, [r3, #56]
08031216  36ee866a  vadd.f32	s12, s13, s12
0803121a  83ed116a  vstr	s12, [r3, #68]
0803121e  9fed346a  vldr	s12, [pc, #208] ; [0x080312f0] = 0x3f99999a / f32_bits_interpretation=1.200000048
08031222  e1ee068a  vfma.f32	s17, s2, s12
08031226  37ee687a  vsub.f32	s14, s14, s17
0803122a  a2ee071a  vfma.f32	s2, s4, s14
0803122e  83ed047a  vstr	s14, [r3, #16]
08031232  37ee017a  vadd.f32	s14, s14, s2
08031236  83ed021a  vstr	s2, [r3, #8]
0803123a  83ed057a  vstr	s14, [r3, #20]
0803123e  2d4b      ldr	r3, [pc, #180] ; [0x080312f4] = 0x20002e98
08031240  93ed007a  vldr	s14, [r3]
08031244  b4eec37a  vcmpe.f32	s14, s6
08031248  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803124c  40f12d83  bpl.w	#1626 ; -> 0x080318aa ; branch_target=0x080318aa
08031250  27ee206a  vmul.f32	s12, s14, s1
08031254  9fed287a  vldr	s14, [pc, #160] ; [0x080312f8] = 0x3dcccccd / f32_bits_interpretation=0.1000000015
08031258  a3ee076a  vfma.f32	s12, s6, s14
0803125c  9fed272a  vldr	s4, [pc, #156] ; [0x080312fc] = 0x3f8ccccd / f32_bits_interpretation=1.100000024
08031260  26eea47a  vmul.f32	s14, s13, s9
08031264  83ed006a  vstr	s12, [r3]
08031268  66ee846a  vmul.f32	s13, s13, s8
0803126c  c2ee064a  vdiv.f32	s9, s4, s12
08031270  234b      ldr	r3, [pc, #140] ; [0x08031300] = 0x20001140
08031272  9ded074a  vldr	s8, [sp, #28]
08031276  93ed003a  vldr	s6, [r3]
0803127a  22ee836a  vmul.f32	s12, s5, s6
0803127e  26ee04ca  vmul.f32	s24, s12, s8
08031282  27ee247a  vmul.f32	s14, s14, s9
08031286  66eea46a  vmul.f32	s13, s13, s9
0803128a  fef7c4bc  b.w	#-5752 ; -> 0x0802fc16 ; branch_target=0x0802fc16
0803128e  1d4b      ldr	r3, [pc, #116] ; [0x08031304] = 0x200012c0
08031290  db68      ldr	r3, [r3, #12]
08031292  9342      cmp	r3, r2
08031294  40f3cb82  ble.w	#1430 ; -> 0x0803182e ; branch_target=0x0803182e
08031298  511a      subs	r1, r2, r1
0803129a  1b4b      ldr	r3, [pc, #108] ; [0x08031308] = 0x20001240
0803129c  07ee101a  vmov	s14, r1
080312a0  93ed026a  vldr	s12, [r3, #8]
080312a4  f8eec76a  vcvt.f32.s32	s13, s14
080312a8  9fed187a  vldr	s14, [pc, #96] ; [0x0803130c] = 0x45800000 / f32_bits_interpretation=4096
080312ac  a6ee867a  vfma.f32	s14, s13, s12
080312b0  bdeec77a  vcvt.s32.f32	s14, s14
080312b4  17ee103a  vmov	r3, s14
080312b8  fdf741be  b.w	#-9086 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
08031310  039b      ldr	r3, [sp, #12]
08031312  022b      cmp	r3, #2
08031314  00f02286  beq.w	#3140 ; -> 0x08031f5c ; branch_target=0x08031f5c
08031318  d34b      ldr	r3, [pc, #844] ; [0x08031668] = 0x20002400
0803131a  b0ee643a  vmov.f32	s6, s9
0803131e  c3ed004a  vstr	s9, [r3]
08031322  d24b      ldr	r3, [pc, #840] ; [0x0803166c] = 0x20002404
08031324  c3ed004a  vstr	s9, [r3]
08031328  d14b      ldr	r3, [pc, #836] ; [0x08031670] = 0x20002424
0803132a  dfedd26a  vldr	s13, [pc, #840] ; [0x08031674] = 0x46800000 / f32_bits_interpretation=16384
0803132e  93ed00ea  vldr	s28, [r3]
08031332  d14b      ldr	r3, [pc, #836] ; [0x08031678] = 0x20002420
08031334  2eee2e4a  vmul.f32	s8, s28, s29
08031338  d3ed00aa  vldr	s21, [r3]
0803133c  6aeea66a  vmul.f32	s13, s21, s13
08031340  24ee034a  vmul.f32	s8, s8, s6
08031344  66eea44a  vmul.f32	s9, s13, s9
08031348  f6eec48a  vrintz.f32	s17, s8
0803134c  fdeee46a  vcvt.s32.f32	s13, s9
08031350  16ee902a  vmov	r2, s13
08031354  dfedc96a  vldr	s13, [pc, #804] ; [0x0803167c] = 0x46c35000 / f32_bits_interpretation=25000
08031358  61ee266a  vmul.f32	s13, s2, s13
0803135c  511c      adds	r1, r2, #1
0803135e  c2f30c02  ubfx	r2, r2, #0, #13
08031362  c1f30c01  ubfx	r1, r1, #0, #13
08031366  fdeee66a  vcvt.s32.f32	s13, s13
0803136a  0beb8202  add.w	r2, r11, r2, lsl #2
0803136e  0beb8101  add.w	r1, r11, r1, lsl #2
08031372  92ed001a  vldr	s2, [r2]
08031376  16ee900a  vmov	r0, s13
0803137a  fdeec46a  vcvt.s32.f32	s13, s8
0803137e  34ee684a  vsub.f32	s8, s8, s17
08031382  91ed003a  vldr	s6, [r1]
08031386  16ee903a  vmov	r3, s13
0803138a  33ee413a  vsub.f32	s6, s6, s2
0803138e  5a1c      adds	r2, r3, #1
08031390  c3f30c03  ubfx	r3, r3, #0, #13
08031394  c2f30c02  ubfx	r2, r2, #0, #13
08031398  0beb8303  add.w	r3, r11, r3, lsl #2
0803139c  0beb8202  add.w	r2, r11, r2, lsl #2
080313a0  d3ed000a  vldr	s1, [r3]
080313a4  c0f30a03  ubfx	r3, r0, #0, #11
080313a8  c012      asrs	r0, r0, #11
080313aa  92ed002a  vldr	s4, [r2]
080313ae  089a      ldr	r2, [sp, #32]
080313b0  32ee602a  vsub.f32	s4, s4, s1
080313b4  02eb8303  add.w	r3, r2, r3, lsl #2
080313b8  e2ee040a  vfma.f32	s1, s4, s8
080313bc  b6eee44a  vrintz.f32	s8, s9
080313c0  74eec44a  vsub.f32	s9, s9, s8
080313c4  93ed000a  vldr	s0, [r3]
080313c8  0123      movs	r3, #1
080313ca  a3ee241a  vfma.f32	s2, s6, s9
080313ce  8340      lsls	r3, r0
080313d0  06ee903a  vmov	s13, r3
080313d4  aa4b      ldr	r3, [pc, #680] ; [0x08031680] = 0x20000820
080313d6  f0ee608a  vmov.f32	s17, s1
080313da  1b68      ldr	r3, [r3]
080313dc  f8eee66a  vcvt.f32.s32	s13, s13
080313e0  b0ee41aa  vmov.f32	s20, s2
080313e4  002b      cmp	r3, #0
080313e6  40f04282  bne.w	#1156 ; -> 0x0803186e ; branch_target=0x0803186e
080313ea  f1ee004a  vmov.f32	s9, #4.000000e+00
080313ee  60ee244a  vmul.f32	s9, s0, s9
080313f2  64eea66a  vmul.f32	s13, s9, s13
080313f6  dfeda34a  vldr	s9, [pc, #652] ; [0x08031684] = 0x37aec33e / f32_bits_interpretation=2.083333311e-05
080313fa  a34a      ldr	r2, [pc, #652] ; [0x08031688] = 0x2000239c
080313fc  26eea44a  vmul.f32	s8, s13, s9
08031400  f2ee046a  vmov.f32	s13, #1.000000e+01
08031404  d2ed064a  vldr	s9, [r2, #24]
08031408  82ed074a  vstr	s8, [r2, #28]
0803140c  f4eee64a  vcmpe.f32	s9, s13
08031410  f7ee006a  vmov.f32	s13, #1.000000e+00
08031414  f1ee10fa  vmrs	APSR_nzcv, fpscr
08031418  d4bf      ite	le
0803141a  74ee244a  vaddle.f32	s9, s8, s9
0803141e  f0ee444a  vmovgt.f32	s9, s8
08031422  f4eee64a  vcmpe.f32	s9, s13
08031426  c2ed064a  vstr	s9, [r2, #24]
0803142a  f1ee10fa  vmrs	APSR_nzcv, fpscr
0803142e  c0f25585  blt.w	#2730 ; -> 0x08031edc ; branch_target=0x08031edc
08031432  9648      ldr	r0, [pc, #600] ; [0x0803168c] = 0x200023cc
08031434  beee002a  vmov.f32	s4, #-5.000000e-01
08031438  954c      ldr	r4, [pc, #596] ; [0x08031690] = 0x0bb38435
0803143a  b0ee001a  vmov.f32	s2, #2.000000e+00
0803143e  0368      ldr	r3, [r0]
08031440  9449      ldr	r1, [pc, #592] ; [0x08031694] = 0x3619636b / f32_bits_interpretation=2.28566455e-06
08031442  9fed953a  vldr	s6, [pc, #596] ; [0x08031698] = 0x2f80000d / f32_bits_interpretation=2.328310045e-10
08031446  04fb0311  mla	r1, r4, r3, r1
0803144a  06ee901a  vmov	s13, r1
0803144e  0160      str	r1, [r0]
08031450  f8ee666a  vcvt.f32.u32	s13, s13
08031454  a6ee832a  vfma.f32	s4, s13, s6
08031458  d2ed0b6a  vldr	s13, [r2, #44]
0803145c  b0ee663a  vmov.f32	s6, s13
08031460  c2ed096a  vstr	s13, [r2, #36]
08031464  92ee013a  vfnms.f32	s6, s4, s2
08031468  b6eee42a  vrintz.f32	s4, s9
0803146c  74eec24a  vsub.f32	s9, s9, s4
08031470  c2ed064a  vstr	s9, [r2, #24]
08031474  82ed0a3a  vstr	s6, [r2, #40]
08031478  feeeca4a  vcvt.s32.f32	s9, s9, #12
0803147c  9fed871a  vldr	s2, [pc, #540] ; [0x0803169c] = 0x3f666666 / f32_bits_interpretation=0.8999999762
08031480  f6ee000a  vmov.f32	s1, #5.000000e-01
08031484  b7ee000a  vmov.f32	s0, #1.000000e+00
08031488  14ee903a  vmov	r3, s9
0803148c  dfed844a  vldr	s9, [pc, #528] ; [0x080316a0] = 0x437f0000 / f32_bits_interpretation=255
08031490  23ee203a  vmul.f32	s6, s6, s1
08031494  66ee244a  vmul.f32	s9, s12, s9
08031498  a3f50063  sub.w	r3, r3, #2048
0803149c  27ee017a  vmul.f32	s14, s14, s2
080314a0  c3f30c03  ubfx	r3, r3, #0, #13
080314a4  fdeee44a  vcvt.s32.f32	s9, s9
080314a8  0beb8303  add.w	r3, r11, r3, lsl #2
080314ac  14ee901a  vmov	r1, s9
080314b0  93ed002a  vldr	s4, [r3]
080314b4  7b4b      ldr	r3, [pc, #492] ; [0x080316a4] = 0x08042674
080314b6  dfed7c4a  vldr	s9, [pc, #496] ; [0x080316a8] = 0x403cff8b / f32_bits_interpretation=2.953097105
080314ba  32ee000a  vadd.f32	s0, s4, s0
080314be  03eb8103  add.w	r3, r3, r1, lsl #2
080314c2  93ed006a  vldr	s12, [r3]
080314c6  e0ee036a  vfma.f32	s13, s0, s6
080314ca  784b      ldr	r3, [pc, #480] ; [0x080316ac] = 0x2000233c
080314cc  26ee246a  vmul.f32	s12, s12, s9
080314d0  d3ed094a  vldr	s9, [r3, #36]
080314d4  93ed142a  vldr	s4, [r3, #80]
080314d8  26ee046a  vmul.f32	s12, s12, s8
080314dc  93ed084a  vldr	s8, [r3, #32]
080314e0  c2ed0b6a  vstr	s13, [r2, #44]
080314e4  e6ee044a  vfma.f32	s9, s12, s8
080314e8  83ed066a  vstr	s12, [r3, #24]
080314ec  27ee067a  vmul.f32	s14, s14, s12
080314f0  83ed127a  vstr	s14, [r3, #72]
080314f4  b0ee643a  vmov.f32	s6, s9
080314f8  c3ed094a  vstr	s9, [r3, #36]
080314fc  a2ee603a  vfms.f32	s6, s4, s1
08031500  d3ed150a  vldr	s1, [r3, #84]
08031504  e7ee020a  vfma.f32	s1, s14, s4
08031508  33ee603a  vsub.f32	s6, s6, s1
0803150c  c3ed150a  vstr	s1, [r3, #84]
08031510  a7ee032a  vfma.f32	s4, s14, s6
08031514  9fed667a  vldr	s14, [pc, #408] ; [0x080316b0] = 0x3f99999a / f32_bits_interpretation=1.200000048
08031518  83ed163a  vstr	s6, [r3, #88]
0803151c  e4ee074a  vfma.f32	s9, s8, s14
08031520  83ed142a  vstr	s4, [r3, #80]
08031524  33ee022a  vadd.f32	s4, s6, s4
08031528  76eee46a  vsub.f32	s13, s13, s9
0803152c  83ed172a  vstr	s4, [r3, #92]
08031530  a6ee264a  vfma.f32	s8, s12, s13
08031534  c3ed0a6a  vstr	s13, [r3, #40]
08031538  76ee846a  vadd.f32	s13, s13, s8
0803153c  83ed084a  vstr	s8, [r3, #32]
08031540  c3ed0b6a  vstr	s13, [r3, #44]
08031544  f0eec36a  vabs.f32	s13, s6
08031548  5a4b      ldr	r3, [pc, #360] ; [0x080316b4] = 0x200011c0
0803154a  93ed007a  vldr	s14, [r3]
0803154e  b4eee67a  vcmpe.f32	s14, s13
08031552  f1ee10fa  vmrs	APSR_nzcv, fpscr
08031556  40f18181  bpl.w	#770 ; -> 0x0803185c ; branch_target=0x0803185c
0803155a  27ee017a  vmul.f32	s14, s14, s2
0803155e  9fed566a  vldr	s12, [pc, #344] ; [0x080316b8] = 0x3dcccccd / f32_bits_interpretation=0.1000000015
08031562  a6ee867a  vfma.f32	s14, s13, s12
08031566  9fed556a  vldr	s12, [pc, #340] ; [0x080316bc] = 0x3f8ccccd / f32_bits_interpretation=1.100000024
0803156a  63ee288a  vmul.f32	s17, s6, s17
0803156e  23ee0a3a  vmul.f32	s6, s6, s20
08031572  83ed007a  vstr	s14, [r3]
08031576  c6ee076a  vdiv.f32	s13, s12, s14
0803157a  514b      ldr	r3, [pc, #324] ; [0x080316c0] = 0x20002e50
0803157c  93ed004a  vldr	s8, [r3]
08031580  28eea67a  vmul.f32	s14, s17, s13
08031584  23ee263a  vmul.f32	s6, s6, s13
08031588  fef7babc  b.w	#-5772 ; -> 0x0802ff00 ; branch_target=0x0802ff00
0803158c  4d4b      ldr	r3, [pc, #308] ; [0x080316c4] = 0x20001300
0803158e  db68      ldr	r3, [r3, #12]
08031590  9342      cmp	r3, r2
08031592  40f37381  ble.w	#742 ; -> 0x0803187c ; branch_target=0x0803187c
08031596  511a      subs	r1, r2, r1
08031598  4b4b      ldr	r3, [pc, #300] ; [0x080316c8] = 0x20001280
0803159a  07ee901a  vmov	s15, r1
0803159e  d3ed026a  vldr	s13, [r3, #8]
080315a2  b8eee77a  vcvt.f32.s32	s14, s15
080315a6  dfed497a  vldr	s15, [pc, #292] ; [0x080316cc] = 0x45800000 / f32_bits_interpretation=4096
080315aa  e7ee267a  vfma.f32	s15, s14, s13
080315ae  fdeee77a  vcvt.s32.f32	s15, s15
080315b2  17ee902a  vmov	r2, s15
080315b6  fdf779bc  b.w	#-9998 ; -> 0x0802eeac ; branch_target=0x0802eeac
080315ba  454b      ldr	r3, [pc, #276] ; [0x080316d0] = 0x08036674
080315bc  1946      mov	r1, r3
080315be  fff7e4b8  b.w	#-3640 ; -> 0x0803078a ; branch_target=0x0803078a
080315c2  434b      ldr	r3, [pc, #268] ; [0x080316d0] = 0x08036674
080315c4  1a46      mov	r2, r3
080315c6  fff734b9  b.w	#-3480 ; -> 0x08030832 ; branch_target=0x08030832
080315ca  f1ee666a  vneg.f32	s13, s13
080315ce  fef7eabc  b.w	#-5676 ; -> 0x0802ffa6 ; branch_target=0x0802ffa6
080315d2  f1ee633a  vneg.f32	s7, s7
080315d6  fdf761be  b.w	#-9022 ; -> 0x0802f29c ; branch_target=0x0802f29c
080315da  76eec66a  vsub.f32	s13, s13, s12
080315de  9fed3d5a  vldr	s10, [pc, #244] ; [0x080316d4] = 0x3fb6db6e / f32_bits_interpretation=1.428571463
080315e2  9fed3d6a  vldr	s12, [pc, #244] ; [0x080316d8] = 0x38d1b717 / f32_bits_interpretation=9.999999747e-05
080315e6  66ee856a  vmul.f32	s13, s13, s10
080315ea  77eec65a  vsub.f32	s11, s15, s12
080315ee  a5eea66a  vfma.f32	s12, s11, s13
080315f2  fff7aeb8  b.w	#-3748 ; -> 0x08030752 ; branch_target=0x08030752
080315f6  37ee667a  vsub.f32	s14, s14, s13
080315fa  dfed365a  vldr	s11, [pc, #216] ; [0x080316d4] = 0x3fb6db6e / f32_bits_interpretation=1.428571463
080315fe  dfed366a  vldr	s13, [pc, #216] ; [0x080316d8] = 0x38d1b717 / f32_bits_interpretation=9.999999747e-05
08031602  27ee257a  vmul.f32	s14, s14, s11
08031606  32eee66a  vsub.f32	s12, s5, s13
0803160a  e6ee076a  vfma.f32	s13, s12, s14
0803160e  fff77db8  b.w	#-3846 ; -> 0x0803070c ; branch_target=0x0803070c
08031612  324b      ldr	r3, [pc, #200] ; [0x080316dc] = 0x200023d4
08031614  d3ed006a  vldr	s13, [r3]
08031618  314b      ldr	r3, [pc, #196] ; [0x080316e0] = 0x20000840
0803161a  d3ed004a  vldr	s9, [r3]
0803161e  74eee64a  vsub.f32	s9, s9, s13
08031622  e6ee246a  vfma.f32	s13, s12, s9
08031626  fef7bebc  b.w	#-5764 ; -> 0x0802ffa6 ; branch_target=0x0802ffa6
0803162a  2e4b      ldr	r3, [pc, #184] ; [0x080316e4] = 0x200023dc
0803162c  dded096a  vldr	s13, [sp, #36]
08031630  d3ed003a  vldr	s7, [r3]
08031634  2c4b      ldr	r3, [pc, #176] ; [0x080316e8] = 0x200023d8
08031636  93ed007a  vldr	s14, [r3]
0803163a  37ee637a  vsub.f32	s14, s14, s7
0803163e  e7ee263a  vfma.f32	s7, s14, s13
08031642  fdf72bbe  b.w	#-9130 ; -> 0x0802f29c ; branch_target=0x0802f29c
08031646  059b      ldr	r3, [sp, #20]
08031648  43b9      cbnz	r3, #16 ; -> 0x0803165c ; branch_target=0x0803165c
0803164a  284b      ldr	r3, [pc, #160] ; [0x080316ec] = 0x20002ea8
0803164c  f1ee004a  vmov.f32	s9, #4.000000e+00
08031650  d3ed006a  vldr	s13, [r3]
08031654  66eea46a  vmul.f32	s13, s13, s9
08031658  fef7a5bc  b.w	#-5814 ; -> 0x0802ffa6 ; branch_target=0x0802ffa6
0803165c  012b      cmp	r3, #1
0803165e  7ef4a2ac  bne.w	#-5820 ; -> 0x0802ffa6 ; branch_target=0x0802ffa6
08031662  fff7c0b8  b.w	#-3712 ; -> 0x080307e6 ; branch_target=0x080307e6
080316f0  079b      ldr	r3, [sp, #28]
080316f2  13f5fa5f  cmn.w	r3, #8000
080316f6  11dd      ble	#34 ; -> 0x0803171c ; branch_target=0x0803171c
080316f8  239b      ldr	r3, [sp, #140]
080316fa  b7ee006a  vmov.f32	s12, #1.000000e+00
080316fe  9c11      asrs	r4, r3, #6
08031700  724b      ldr	r3, [pc, #456] ; [0x080318cc] = 0x20001380
08031702  9c61      str	r4, [r3, #24]
08031704  724b      ldr	r3, [pc, #456] ; [0x080318d0] = 0x20001340
08031706  9b69      ldr	r3, [r3, #24]
08031708  1b1b      subs	r3, r3, r4
0803170a  07ee103a  vmov	s14, r3
0803170e  714b      ldr	r3, [pc, #452] ; [0x080318d4] = 0x20001200
08031710  b8eec77a  vcvt.f32.s32	s14, s14
08031714  c6ee076a  vdiv.f32	s13, s12, s14
08031718  c3ed066a  vstr	s13, [r3, #24]
0803171c  099b      ldr	r3, [sp, #36]
0803171e  13f5fa5f  cmn.w	r3, #8000
08031722  11dd      ble	#34 ; -> 0x08031748 ; branch_target=0x08031748
08031724  0a9b      ldr	r3, [sp, #40]
08031726  b7ee006a  vmov.f32	s12, #1.000000e+00
0803172a  9811      asrs	r0, r3, #6
0803172c  674b      ldr	r3, [pc, #412] ; [0x080318cc] = 0x20001380
0803172e  d861      str	r0, [r3, #28]
08031730  674b      ldr	r3, [pc, #412] ; [0x080318d0] = 0x20001340
08031732  db69      ldr	r3, [r3, #28]
08031734  1b1a      subs	r3, r3, r0
08031736  07ee103a  vmov	s14, r3
0803173a  664b      ldr	r3, [pc, #408] ; [0x080318d4] = 0x20001200
0803173c  b8eec77a  vcvt.f32.s32	s14, s14
08031740  c6ee076a  vdiv.f32	s13, s12, s14
08031744  c3ed076a  vstr	s13, [r3, #28]
08031748  b5f5fa5f  cmp.w	r5, #8000
0803174c  10da      bge	#32 ; -> 0x08031770 ; branch_target=0x08031770
0803174e  8911      asrs	r1, r1, #6
08031750  5e4b      ldr	r3, [pc, #376] ; [0x080318cc] = 0x20001380
08031752  b7ee006a  vmov.f32	s12, #1.000000e+00
08031756  1962      str	r1, [r3, #32]
08031758  5d4b      ldr	r3, [pc, #372] ; [0x080318d0] = 0x20001340
0803175a  1b6a      ldr	r3, [r3, #32]
0803175c  5b1a      subs	r3, r3, r1
0803175e  07ee103a  vmov	s14, r3
08031762  5c4b      ldr	r3, [pc, #368] ; [0x080318d4] = 0x20001200
08031764  b8eec77a  vcvt.f32.s32	s14, s14
08031768  c6ee076a  vdiv.f32	s13, s12, s14
0803176c  c3ed086a  vstr	s13, [r3, #32]
08031770  bef5fa5f  cmp.w	lr, #8000
08031774  11da      bge	#34 ; -> 0x0803179a ; branch_target=0x0803179a
08031776  4feaac11  asr.w	r1, r12, #6
0803177a  544b      ldr	r3, [pc, #336] ; [0x080318cc] = 0x20001380
0803177c  b7ee006a  vmov.f32	s12, #1.000000e+00
08031780  5962      str	r1, [r3, #36]
08031782  534b      ldr	r3, [pc, #332] ; [0x080318d0] = 0x20001340
08031784  5b6a      ldr	r3, [r3, #36]
08031786  5b1a      subs	r3, r3, r1
08031788  07ee103a  vmov	s14, r3
0803178c  514b      ldr	r3, [pc, #324] ; [0x080318d4] = 0x20001200
0803178e  b8eec77a  vcvt.f32.s32	s14, s14
08031792  c6ee076a  vdiv.f32	s13, s12, s14
08031796  c3ed096a  vstr	s13, [r3, #36]
0803179a  b6f5fa5f  cmp.w	r6, #8000
0803179e  10da      bge	#32 ; -> 0x080317c2 ; branch_target=0x080317c2
080317a0  bf11      asrs	r7, r7, #6
080317a2  4a4b      ldr	r3, [pc, #296] ; [0x080318cc] = 0x20001380
080317a4  b7ee006a  vmov.f32	s12, #1.000000e+00
080317a8  9f62      str	r7, [r3, #40]
080317aa  494b      ldr	r3, [pc, #292] ; [0x080318d0] = 0x20001340
080317ac  9b6a      ldr	r3, [r3, #40]
080317ae  db1b      subs	r3, r3, r7
080317b0  07ee103a  vmov	s14, r3
080317b4  474b      ldr	r3, [pc, #284] ; [0x080318d4] = 0x20001200
080317b6  b8eec77a  vcvt.f32.s32	s14, s14
080317ba  c6ee076a  vdiv.f32	s13, s12, s14
080317be  c3ed0a6a  vstr	s13, [r3, #40]
080317c2  b8f5fa5f  cmp.w	r8, #8000
080317c6  10da      bge	#32 ; -> 0x080317ea ; branch_target=0x080317ea
080317c8  9211      asrs	r2, r2, #6
080317ca  404b      ldr	r3, [pc, #256] ; [0x080318cc] = 0x20001380
080317cc  b7ee006a  vmov.f32	s12, #1.000000e+00
080317d0  da62      str	r2, [r3, #44]
080317d2  3f4b      ldr	r3, [pc, #252] ; [0x080318d0] = 0x20001340
080317d4  db6a      ldr	r3, [r3, #44]
080317d6  9b1a      subs	r3, r3, r2
080317d8  07ee103a  vmov	s14, r3
080317dc  3d4b      ldr	r3, [pc, #244] ; [0x080318d4] = 0x20001200
080317de  b8eec77a  vcvt.f32.s32	s14, s14
080317e2  c6ee076a  vdiv.f32	s13, s12, s14
080317e6  c3ed0b6a  vstr	s13, [r3, #44]
080317ea  6ff4fa53  mvn	r3, #8000
080317ee  079a      ldr	r2, [sp, #28]
080317f0  0999      ldr	r1, [sp, #36]
080317f2  9942      cmp	r1, r3
080317f4  c8bf      it	gt
080317f6  9a42      cmpgt	r2, r3
080317f8  d4bf      ite	le
080317fa  0123      movle	r3, #1
080317fc  0023      movgt	r3, #0
080317fe  bef5fa5f  cmp.w	lr, #8000
08031802  c8bf      it	gt
08031804  43f00103  orrgt	r3, r3, #1
08031808  b5f5fa5f  cmp.w	r5, #8000
0803180c  c8bf      it	gt
0803180e  43f00103  orrgt	r3, r3, #1
08031812  b6f5fa5f  cmp.w	r6, #8000
08031816  c8bf      it	gt
08031818  43f00103  orrgt	r3, r3, #1
0803181c  002b      cmp	r3, #0
0803181e  7ef47cae  bne.w	#-4872 ; -> 0x0803051a ; branch_target=0x0803051a
08031822  b8f5fa5f  cmp.w	r8, #8000
08031826  3ef778ae  bgt.w	#-4880 ; -> 0x0803051a ; branch_target=0x0803051a
0803182a  fef792be  b.w	#-4828 ; -> 0x08030552 ; branch_target=0x08030552
0803182e  2a49      ldr	r1, [pc, #168] ; [0x080318d8] = 0x200012c0
08031830  0969      ldr	r1, [r1, #16]
08031832  9142      cmp	r1, r2
08031834  40f39a83  ble.w	#1844 ; -> 0x08031f6c ; branch_target=0x08031f6c
08031838  d31a      subs	r3, r2, r3
0803183a  07ee103a  vmov	s14, r3
0803183e  274b      ldr	r3, [pc, #156] ; [0x080318dc] = 0x20001240
08031840  f8eec76a  vcvt.f32.s32	s13, s14
08031844  93ed036a  vldr	s12, [r3, #12]
08031848  9fed257a  vldr	s14, [pc, #148] ; [0x080318e0] = 0x45c00000 / f32_bits_interpretation=6144
0803184c  a6ee867a  vfma.f32	s14, s13, s12
08031850  bdeec77a  vcvt.s32.f32	s14, s14
08031854  17ee103a  vmov	r3, s14
08031858  fdf771bb  b.w	#-10526 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
0803185c  9fed216a  vldr	s12, [pc, #132] ; [0x080318e4] = 0x3f7ffcb9 / f32_bits_interpretation=0.9999499917
08031860  27ee067a  vmul.f32	s14, s14, s12
08031864  9fed206a  vldr	s12, [pc, #128] ; [0x080318e8] = 0x3851b717 / f32_bits_interpretation=4.999999874e-05
08031868  a6ee867a  vfma.f32	s14, s13, s12
0803186c  7be6      b	#-778 ; -> 0x08031566 ; branch_target=0x08031566
0803186e  dfed1f4a  vldr	s9, [pc, #124] ; [0x080318ec] = 0x3d800000 / f32_bits_interpretation=0.0625
08031872  20ee240a  vmul.f32	s0, s0, s9
08031876  60ee266a  vmul.f32	s13, s0, s13
0803187a  bce5      b	#-1160 ; -> 0x080313f6 ; branch_target=0x080313f6
0803187c  1c49      ldr	r1, [pc, #112] ; [0x080318f0] = 0x20001300
0803187e  0969      ldr	r1, [r1, #16]
08031880  9142      cmp	r1, r2
08031882  40f38983  ble.w	#1810 ; -> 0x08031f98 ; branch_target=0x08031f98
08031886  d31a      subs	r3, r2, r3
08031888  07ee903a  vmov	s15, r3
0803188c  194b      ldr	r3, [pc, #100] ; [0x080318f4] = 0x20001280
0803188e  b8eee77a  vcvt.f32.s32	s14, s15
08031892  d3ed036a  vldr	s13, [r3, #12]
08031896  dfed127a  vldr	s15, [pc, #72] ; [0x080318e0] = 0x45c00000 / f32_bits_interpretation=6144
0803189a  e7ee267a  vfma.f32	s15, s14, s13
0803189e  fdeee77a  vcvt.s32.f32	s15, s15
080318a2  17ee902a  vmov	r2, s15
080318a6  fdf701bb  b.w	#-10750 ; -> 0x0802eeac ; branch_target=0x0802eeac
080318aa  9fed0e6a  vldr	s12, [pc, #56] ; [0x080318e4] = 0x3f7ffcb9 / f32_bits_interpretation=0.9999499917
080318ae  27ee066a  vmul.f32	s12, s14, s12
080318b2  9fed0d7a  vldr	s14, [pc, #52] ; [0x080318e8] = 0x3851b717 / f32_bits_interpretation=4.999999874e-05
080318b6  a3ee076a  vfma.f32	s12, s6, s14
080318ba  cfe4      b	#-1634 ; -> 0x0803125c ; branch_target=0x0803125c
080318bc  9fed0b6a  vldr	s12, [pc, #44] ; [0x080318ec] = 0x3d800000 / f32_bits_interpretation=0.0625
080318c0  66ee866a  vmul.f32	s13, s13, s12
080318c4  26ee877a  vmul.f32	s14, s13, s14
080318c8  0ee4      b	#-2020 ; -> 0x080310e8 ; branch_target=0x080310e8
080318f8  f1f788fd  bl	#-58608 ; -> 0x0802340c ; branch_target=0x0802340c
080318fc  0122      movs	r2, #1
080318fe  8021      movs	r1, #128
08031900  d048      ldr	r0, [pc, #832] ; [0x08031c44] = 0x58020800
08031902  f1f783fd  bl	#-58618 ; -> 0x0802340c ; branch_target=0x0802340c
08031906  d04b      ldr	r3, [pc, #832] ; [0x08031c48] = 0x200144d4
08031908  d048      ldr	r0, [pc, #832] ; [0x08031c4c] = 0x20001380
0803190a  41f6a476  movw	r6, #8100
0803190e  d3f80080  ldr.w	r8, [r3]
08031912  216a      ldr	r1, [r4, #32]
08031914  a8f10302  sub.w	r2, r8, #3
08031918  a36a      ldr	r3, [r4, #40]
0803191a  876a      ldr	r7, [r0, #40]
0803191c  a8f10404  sub.w	r4, r8, #4
08031920  02fb06f6  mul	r6, r2, r6
08031924  0292      str	r2, [sp, #8]
08031926  c7eba317  rsb	r7, r7, r3, asr #6
0803192a  026a      ldr	r2, [r0, #32]
0803192c  a6f2dc53  subw	r3, r6, #1500
08031930  0494      str	r4, [sp, #16]
08031932  06f2dc56  addw	r6, r6, #1500
08031936  c2eba112  rsb	r2, r2, r1, asr #6
0803193a  9a42      cmp	r2, r3
0803193c  1546      mov	r5, r2
0803193e  1cdd      ble	#56 ; -> 0x0803197a ; branch_target=0x0803197a
08031940  b242      cmp	r2, r6
08031942  1ada      bge	#52 ; -> 0x0803197a ; branch_target=0x0803197a
08031944  0022      movs	r2, #0
08031946  4021      movs	r1, #64
08031948  be48      ldr	r0, [pc, #760] ; [0x08031c44] = 0x58020800
0803194a  0593      str	r3, [sp, #20]
0803194c  f1f75efd  bl	#-58692 ; -> 0x0802340c ; branch_target=0x0802340c
08031950  bf4a      ldr	r2, [pc, #764] ; [0x08031c50] = 0x20001300
08031952  0299      ldr	r1, [sp, #8]
08031954  9fedbf7a  vldr	s14, [pc, #764] ; [0x08031c54] = 0x45000000 / f32_bits_interpretation=2048
08031958  42f82150  str.w	r5, [r2, r1, lsl #2]
0803195c  52f82420  ldr.w	r2, [r2, r4, lsl #2]
08031960  bd4b      ldr	r3, [pc, #756] ; [0x08031c58] = 0x20001280
08031962  aa1a      subs	r2, r5, r2
08031964  07ee902a  vmov	s15, r2
08031968  03eb8402  add.w	r2, r3, r4, lsl #2
0803196c  059b      ldr	r3, [sp, #20]
0803196e  f8eee77a  vcvt.f32.s32	s15, s15
08031972  87ee277a  vdiv.f32	s14, s14, s15
08031976  82ed007a  vstr	s14, [r2]
0803197a  9f42      cmp	r7, r3
0803197c  1bdd      ble	#54 ; -> 0x080319b6 ; branch_target=0x080319b6
0803197e  b742      cmp	r7, r6
08031980  19da      bge	#50 ; -> 0x080319b6 ; branch_target=0x080319b6
08031982  0022      movs	r2, #0
08031984  8021      movs	r1, #128
08031986  af48      ldr	r0, [pc, #700] ; [0x08031c44] = 0x58020800
08031988  f1f740fd  bl	#-58752 ; -> 0x0802340c ; branch_target=0x0802340c
0803198c  b34b      ldr	r3, [pc, #716] ; [0x08031c5c] = 0x200012c0
0803198e  029a      ldr	r2, [sp, #8]
08031990  9fedb07a  vldr	s14, [pc, #704] ; [0x08031c54] = 0x45000000 / f32_bits_interpretation=2048
08031994  43f82270  str.w	r7, [r3, r2, lsl #2]
08031998  049a      ldr	r2, [sp, #16]
0803199a  53f82230  ldr.w	r3, [r3, r2, lsl #2]
0803199e  ff1a      subs	r7, r7, r3
080319a0  af4b      ldr	r3, [pc, #700] ; [0x08031c60] = 0x20001240
080319a2  07ee907a  vmov	s15, r7
080319a6  03eb8203  add.w	r3, r3, r2, lsl #2
080319aa  f8eee77a  vcvt.f32.s32	s15, s15
080319ae  87ee277a  vdiv.f32	s14, s14, s15
080319b2  83ed007a  vstr	s14, [r3]
080319b6  049b      ldr	r3, [sp, #16]
080319b8  0d2b      cmp	r3, #13
080319ba  3df7f6ab  bgt.w	#-10260 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
080319be  a44a      ldr	r2, [pc, #656] ; [0x08031c50] = 0x20001300
080319c0  1946      mov	r1, r3
080319c2  029b      ldr	r3, [sp, #8]
080319c4  4fea880c  lsl.w	r12, r8, #2
080319c8  a448      ldr	r0, [pc, #656] ; [0x08031c5c] = 0x200012c0
080319ca  1546      mov	r5, r2
080319cc  52f82360  ldr.w	r6, [r2, r3, lsl #2]
080319d0  acf10c07  sub.w	r7, r12, #12
080319d4  52f82130  ldr.w	r3, [r2, r1, lsl #2]
080319d8  029a      ldr	r2, [sp, #8]
080319da  50f82110  ldr.w	r1, [r0, r1, lsl #2]
080319de  f61a      subs	r6, r6, r3
080319e0  50f82220  ldr.w	r2, [r0, r2, lsl #2]
080319e4  acf11003  sub.w	r3, r12, #16
080319e8  521a      subs	r2, r2, r1
080319ea  c159      ldr	r1, [r0, r7]
080319ec  5018      adds	r0, r2, r1
080319ee  e959      ldr	r1, [r5, r7]
080319f0  994f      ldr	r7, [pc, #612] ; [0x08031c58] = 0x20001280
080319f2  9b4d      ldr	r5, [pc, #620] ; [0x08031c60] = 0x20001240
080319f4  3144      add	r1, r6
080319f6  3c46      mov	r4, r7
080319f8  984e      ldr	r6, [pc, #608] ; [0x08031c5c] = 0x200012c0
080319fa  ea18      adds	r2, r5, r3
080319fc  3b44      add	r3, r7
080319fe  a8f10207  sub.w	r7, r8, #2
08031a02  d3ed007a  vldr	s15, [r3]
08031a06  2346      mov	r3, r4
08031a08  029c      ldr	r4, [sp, #8]
08031a0a  d2f800e0  ldr.w	lr, [r2]
08031a0e  03eb8403  add.w	r3, r3, r4, lsl #2
08031a12  8f4a      ldr	r2, [pc, #572] ; [0x08031c50] = 0x20001300
08031a14  0e2c      cmp	r4, #14
08031a16  46f82700  str.w	r0, [r6, r7, lsl #2]
08031a1a  c3ed007a  vstr	s15, [r3]
08031a1e  05eb8403  add.w	r3, r5, r4, lsl #2
08031a22  42f82710  str.w	r1, [r2, r7, lsl #2]
08031a26  c3f800e0  str.w	lr, [r3]
08031a2a  3df4beab  beq.w	#-10372 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031a2e  1346      mov	r3, r2
08031a30  52f82420  ldr.w	r2, [r2, r4, lsl #2]
08031a34  049c      ldr	r4, [sp, #16]
08031a36  0e2f      cmp	r7, #14
08031a38  53f82430  ldr.w	r3, [r3, r4, lsl #2]
08031a3c  a2eb0303  sub.w	r3, r2, r3
08031a40  03eb0102  add.w	r2, r3, r1
08031a44  029b      ldr	r3, [sp, #8]
08031a46  56f82310  ldr.w	r1, [r6, r3, lsl #2]
08031a4a  56f82430  ldr.w	r3, [r6, r4, lsl #2]
08031a4e  a1eb0303  sub.w	r3, r1, r3
08031a52  7f49      ldr	r1, [pc, #508] ; [0x08031c50] = 0x20001300
08031a54  03eb0006  add.w	r6, r3, r0
08031a58  08f1ff30  add.w	r0, r8, #4294967295
08031a5c  7f4b      ldr	r3, [pc, #508] ; [0x08031c5c] = 0x200012c0
08031a5e  41f82020  str.w	r2, [r1, r0, lsl #2]
08031a62  43f82060  str.w	r6, [r3, r0, lsl #2]
08031a66  7c4b      ldr	r3, [pc, #496] ; [0x08031c58] = 0x20001280
08031a68  03eb8703  add.w	r3, r3, r7, lsl #2
08031a6c  c3ed007a  vstr	s15, [r3]
08031a70  05eb8703  add.w	r3, r5, r7, lsl #2
08031a74  c3f800e0  str.w	lr, [r3]
08031a78  3df497ab  beq.w	#-10450 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031a7c  0f46      mov	r7, r1
08031a7e  029b      ldr	r3, [sp, #8]
08031a80  0e28      cmp	r0, #14
08031a82  51f82310  ldr.w	r1, [r1, r3, lsl #2]
08031a86  57f82430  ldr.w	r3, [r7, r4, lsl #2]
08031a8a  a1eb0303  sub.w	r3, r1, r3
08031a8e  0299      ldr	r1, [sp, #8]
08031a90  1a44      add	r2, r3
08031a92  724b      ldr	r3, [pc, #456] ; [0x08031c5c] = 0x200012c0
08031a94  53f82110  ldr.w	r1, [r3, r1, lsl #2]
08031a98  53f82430  ldr.w	r3, [r3, r4, lsl #2]
08031a9c  47f82820  str.w	r2, [r7, r8, lsl #2]
08031aa0  a1eb0303  sub.w	r3, r1, r3
08031aa4  6c49      ldr	r1, [pc, #432] ; [0x08031c58] = 0x20001280
08031aa6  1e44      add	r6, r3
08031aa8  6c4b      ldr	r3, [pc, #432] ; [0x08031c5c] = 0x200012c0
08031aaa  0c46      mov	r4, r1
08031aac  43f82860  str.w	r6, [r3, r8, lsl #2]
08031ab0  01eb8003  add.w	r3, r1, r0, lsl #2
08031ab4  c3ed007a  vstr	s15, [r3]
08031ab8  05eb8003  add.w	r3, r5, r0, lsl #2
08031abc  c3f800e0  str.w	lr, [r3]
08031ac0  3df473ab  beq.w	#-10522 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031ac4  029b      ldr	r3, [sp, #8]
08031ac6  b8f10e0f  cmp.w	r8, #14
08031aca  57f82310  ldr.w	r1, [r7, r3, lsl #2]
08031ace  049b      ldr	r3, [sp, #16]
08031ad0  57f82330  ldr.w	r3, [r7, r3, lsl #2]
08031ad4  a1eb0303  sub.w	r3, r1, r3
08031ad8  03eb0200  add.w	r0, r3, r2
08031adc  5f4b      ldr	r3, [pc, #380] ; [0x08031c5c] = 0x200012c0
08031ade  029a      ldr	r2, [sp, #8]
08031ae0  1946      mov	r1, r3
08031ae2  53f82220  ldr.w	r2, [r3, r2, lsl #2]
08031ae6  049b      ldr	r3, [sp, #16]
08031ae8  51f82330  ldr.w	r3, [r1, r3, lsl #2]
08031aec  a2eb0303  sub.w	r3, r2, r3
08031af0  2246      mov	r2, r4
08031af2  6244      add	r2, r12
08031af4  1e44      add	r6, r3
08031af6  08f10103  add.w	r3, r8, #1
08031afa  c2ed007a  vstr	s15, [r2]
08031afe  05eb0c02  add.w	r2, r5, r12
08031b02  47f82300  str.w	r0, [r7, r3, lsl #2]
08031b06  41f82360  str.w	r6, [r1, r3, lsl #2]
08031b0a  c2f800e0  str.w	lr, [r2]
08031b0e  3df44cab  beq.w	#-10600 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031b12  049c      ldr	r4, [sp, #16]
08031b14  0e2b      cmp	r3, #14
08031b16  029a      ldr	r2, [sp, #8]
08031b18  57f82210  ldr.w	r1, [r7, r2, lsl #2]
08031b1c  57f82420  ldr.w	r2, [r7, r4, lsl #2]
08031b20  a1eb0202  sub.w	r2, r1, r2
08031b24  4d49      ldr	r1, [pc, #308] ; [0x08031c5c] = 0x200012c0
08031b26  1044      add	r0, r2
08031b28  029a      ldr	r2, [sp, #8]
08031b2a  51f82210  ldr.w	r1, [r1, r2, lsl #2]
08031b2e  4b4a      ldr	r2, [pc, #300] ; [0x08031c5c] = 0x200012c0
08031b30  52f82420  ldr.w	r2, [r2, r4, lsl #2]
08031b34  a1eb0202  sub.w	r2, r1, r2
08031b38  08f10201  add.w	r1, r8, #2
08031b3c  1644      add	r6, r2
08031b3e  474a      ldr	r2, [pc, #284] ; [0x08031c5c] = 0x200012c0
08031b40  47f82100  str.w	r0, [r7, r1, lsl #2]
08031b44  1446      mov	r4, r2
08031b46  42f82160  str.w	r6, [r2, r1, lsl #2]
08031b4a  434a      ldr	r2, [pc, #268] ; [0x08031c58] = 0x20001280
08031b4c  02eb8302  add.w	r2, r2, r3, lsl #2
08031b50  c2ed007a  vstr	s15, [r2]
08031b54  05eb8302  add.w	r2, r5, r3, lsl #2
08031b58  c2f800e0  str.w	lr, [r2]
08031b5c  3df425ab  beq.w	#-10678 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031b60  029b      ldr	r3, [sp, #8]
08031b62  0e29      cmp	r1, #14
08031b64  57f82320  ldr.w	r2, [r7, r3, lsl #2]
08031b68  049b      ldr	r3, [sp, #16]
08031b6a  57f82330  ldr.w	r3, [r7, r3, lsl #2]
08031b6e  049f      ldr	r7, [sp, #16]
08031b70  a2eb0303  sub.w	r3, r2, r3
08031b74  029a      ldr	r2, [sp, #8]
08031b76  1844      add	r0, r3
08031b78  54f82220  ldr.w	r2, [r4, r2, lsl #2]
08031b7c  54f82730  ldr.w	r3, [r4, r7, lsl #2]
08031b80  a2eb0303  sub.w	r3, r2, r3
08031b84  324a      ldr	r2, [pc, #200] ; [0x08031c50] = 0x20001300
08031b86  03eb0607  add.w	r7, r3, r6
08031b8a  334b      ldr	r3, [pc, #204] ; [0x08031c58] = 0x20001280
08031b8c  08f10306  add.w	r6, r8, #3
08031b90  03eb8103  add.w	r3, r3, r1, lsl #2
08031b94  42f82600  str.w	r0, [r2, r6, lsl #2]
08031b98  c3ed007a  vstr	s15, [r3]
08031b9c  05eb8103  add.w	r3, r5, r1, lsl #2
08031ba0  44f82670  str.w	r7, [r4, r6, lsl #2]
08031ba4  c3f800e0  str.w	lr, [r3]
08031ba8  3df4ffaa  beq.w	#-10754 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031bac  0299      ldr	r1, [sp, #8]
08031bae  1346      mov	r3, r2
08031bb0  0e2e      cmp	r6, #14
08031bb2  52f82120  ldr.w	r2, [r2, r1, lsl #2]
08031bb6  0499      ldr	r1, [sp, #16]
08031bb8  53f82130  ldr.w	r3, [r3, r1, lsl #2]
08031bbc  a2eb0303  sub.w	r3, r2, r3
08031bc0  03eb0001  add.w	r1, r3, r0
08031bc4  029b      ldr	r3, [sp, #8]
08031bc6  08f10400  add.w	r0, r8, #4
08031bca  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031bce  049b      ldr	r3, [sp, #16]
08031bd0  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031bd4  a2eb0303  sub.w	r3, r2, r3
08031bd8  1f44      add	r7, r3
08031bda  1d4b      ldr	r3, [pc, #116] ; [0x08031c50] = 0x20001300
08031bdc  1a46      mov	r2, r3
08031bde  43f82010  str.w	r1, [r3, r0, lsl #2]
08031be2  1d4b      ldr	r3, [pc, #116] ; [0x08031c58] = 0x20001280
08031be4  44f82070  str.w	r7, [r4, r0, lsl #2]
08031be8  03eb8603  add.w	r3, r3, r6, lsl #2
08031bec  c3ed007a  vstr	s15, [r3]
08031bf0  05eb8603  add.w	r3, r5, r6, lsl #2
08031bf4  c3f800e0  str.w	lr, [r3]
08031bf8  3df4d7aa  beq.w	#-10834 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031bfc  029e      ldr	r6, [sp, #8]
08031bfe  1346      mov	r3, r2
08031c00  0e28      cmp	r0, #14
08031c02  52f82620  ldr.w	r2, [r2, r6, lsl #2]
08031c06  049e      ldr	r6, [sp, #16]
08031c08  53f82630  ldr.w	r3, [r3, r6, lsl #2]
08031c0c  08f10506  add.w	r6, r8, #5
08031c10  a2eb0303  sub.w	r3, r2, r3
08031c14  1944      add	r1, r3
08031c16  029b      ldr	r3, [sp, #8]
08031c18  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031c1c  049b      ldr	r3, [sp, #16]
08031c1e  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031c22  a2eb0303  sub.w	r3, r2, r3
08031c26  1f44      add	r7, r3
08031c28  094b      ldr	r3, [pc, #36] ; [0x08031c50] = 0x20001300
08031c2a  1a46      mov	r2, r3
08031c2c  43f82610  str.w	r1, [r3, r6, lsl #2]
08031c30  094b      ldr	r3, [pc, #36] ; [0x08031c58] = 0x20001280
08031c32  44f82670  str.w	r7, [r4, r6, lsl #2]
08031c36  03eb8003  add.w	r3, r3, r0, lsl #2
08031c3a  c3ed007a  vstr	s15, [r3]
08031c3e  05eb8003  add.w	r3, r5, r0, lsl #2
08031c42  0fe0      b	#30 ; -> 0x08031c64 ; branch_target=0x08031c64
08031c64  c3f800e0  str.w	lr, [r3]
08031c68  3df49faa  beq.w	#-10946 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031c6c  0298      ldr	r0, [sp, #8]
08031c6e  1346      mov	r3, r2
08031c70  0e2e      cmp	r6, #14
08031c72  52f82020  ldr.w	r2, [r2, r0, lsl #2]
08031c76  0498      ldr	r0, [sp, #16]
08031c78  53f82030  ldr.w	r3, [r3, r0, lsl #2]
08031c7c  08f10600  add.w	r0, r8, #6
08031c80  a2eb0303  sub.w	r3, r2, r3
08031c84  1944      add	r1, r3
08031c86  029b      ldr	r3, [sp, #8]
08031c88  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031c8c  049b      ldr	r3, [sp, #16]
08031c8e  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031c92  a2eb0303  sub.w	r3, r2, r3
08031c96  1f44      add	r7, r3
08031c98  d64b      ldr	r3, [pc, #856] ; [0x08031ff4] = 0x20001300
08031c9a  1a46      mov	r2, r3
08031c9c  43f82010  str.w	r1, [r3, r0, lsl #2]
08031ca0  d54b      ldr	r3, [pc, #852] ; [0x08031ff8] = 0x20001280
08031ca2  44f82070  str.w	r7, [r4, r0, lsl #2]
08031ca6  03eb8603  add.w	r3, r3, r6, lsl #2
08031caa  c3ed007a  vstr	s15, [r3]
08031cae  05eb8603  add.w	r3, r5, r6, lsl #2
08031cb2  c3f800e0  str.w	lr, [r3]
08031cb6  3df478aa  beq.w	#-11024 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031cba  029e      ldr	r6, [sp, #8]
08031cbc  1346      mov	r3, r2
08031cbe  0e28      cmp	r0, #14
08031cc0  52f82620  ldr.w	r2, [r2, r6, lsl #2]
08031cc4  049e      ldr	r6, [sp, #16]
08031cc6  53f82630  ldr.w	r3, [r3, r6, lsl #2]
08031cca  08f10706  add.w	r6, r8, #7
08031cce  a2eb0303  sub.w	r3, r2, r3
08031cd2  1944      add	r1, r3
08031cd4  029b      ldr	r3, [sp, #8]
08031cd6  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031cda  049b      ldr	r3, [sp, #16]
08031cdc  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031ce0  a2eb0303  sub.w	r3, r2, r3
08031ce4  1f44      add	r7, r3
08031ce6  c34b      ldr	r3, [pc, #780] ; [0x08031ff4] = 0x20001300
08031ce8  1a46      mov	r2, r3
08031cea  43f82610  str.w	r1, [r3, r6, lsl #2]
08031cee  c24b      ldr	r3, [pc, #776] ; [0x08031ff8] = 0x20001280
08031cf0  44f82670  str.w	r7, [r4, r6, lsl #2]
08031cf4  03eb8003  add.w	r3, r3, r0, lsl #2
08031cf8  c3ed007a  vstr	s15, [r3]
08031cfc  05eb8003  add.w	r3, r5, r0, lsl #2
08031d00  c3f800e0  str.w	lr, [r3]
08031d04  3df451aa  beq.w	#-11102 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031d08  0298      ldr	r0, [sp, #8]
08031d0a  1346      mov	r3, r2
08031d0c  0e2e      cmp	r6, #14
08031d0e  52f82020  ldr.w	r2, [r2, r0, lsl #2]
08031d12  0498      ldr	r0, [sp, #16]
08031d14  53f82030  ldr.w	r3, [r3, r0, lsl #2]
08031d18  08f10800  add.w	r0, r8, #8
08031d1c  a2eb0303  sub.w	r3, r2, r3
08031d20  1944      add	r1, r3
08031d22  029b      ldr	r3, [sp, #8]
08031d24  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031d28  049b      ldr	r3, [sp, #16]
08031d2a  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031d2e  a2eb0303  sub.w	r3, r2, r3
08031d32  1f44      add	r7, r3
08031d34  af4b      ldr	r3, [pc, #700] ; [0x08031ff4] = 0x20001300
08031d36  1a46      mov	r2, r3
08031d38  43f82010  str.w	r1, [r3, r0, lsl #2]
08031d3c  ae4b      ldr	r3, [pc, #696] ; [0x08031ff8] = 0x20001280
08031d3e  44f82070  str.w	r7, [r4, r0, lsl #2]
08031d42  03eb8603  add.w	r3, r3, r6, lsl #2
08031d46  c3ed007a  vstr	s15, [r3]
08031d4a  05eb8603  add.w	r3, r5, r6, lsl #2
08031d4e  c3f800e0  str.w	lr, [r3]
08031d52  3df42aaa  beq.w	#-11180 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031d56  029e      ldr	r6, [sp, #8]
08031d58  1346      mov	r3, r2
08031d5a  0e28      cmp	r0, #14
08031d5c  52f82620  ldr.w	r2, [r2, r6, lsl #2]
08031d60  049e      ldr	r6, [sp, #16]
08031d62  53f82630  ldr.w	r3, [r3, r6, lsl #2]
08031d66  08f10906  add.w	r6, r8, #9
08031d6a  a2eb0303  sub.w	r3, r2, r3
08031d6e  1944      add	r1, r3
08031d70  029b      ldr	r3, [sp, #8]
08031d72  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031d76  049b      ldr	r3, [sp, #16]
08031d78  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031d7c  a2eb0303  sub.w	r3, r2, r3
08031d80  1f44      add	r7, r3
08031d82  9c4b      ldr	r3, [pc, #624] ; [0x08031ff4] = 0x20001300
08031d84  1a46      mov	r2, r3
08031d86  43f82610  str.w	r1, [r3, r6, lsl #2]
08031d8a  9b4b      ldr	r3, [pc, #620] ; [0x08031ff8] = 0x20001280
08031d8c  44f82670  str.w	r7, [r4, r6, lsl #2]
08031d90  03eb8003  add.w	r3, r3, r0, lsl #2
08031d94  c3ed007a  vstr	s15, [r3]
08031d98  05eb8003  add.w	r3, r5, r0, lsl #2
08031d9c  c3f800e0  str.w	lr, [r3]
08031da0  3df403aa  beq.w	#-11258 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031da4  0298      ldr	r0, [sp, #8]
08031da6  1346      mov	r3, r2
08031da8  0e2e      cmp	r6, #14
08031daa  52f82020  ldr.w	r2, [r2, r0, lsl #2]
08031dae  0498      ldr	r0, [sp, #16]
08031db0  53f82030  ldr.w	r3, [r3, r0, lsl #2]
08031db4  08f10a00  add.w	r0, r8, #10
08031db8  a2eb0303  sub.w	r3, r2, r3
08031dbc  1944      add	r1, r3
08031dbe  029b      ldr	r3, [sp, #8]
08031dc0  54f82320  ldr.w	r2, [r4, r3, lsl #2]
08031dc4  049b      ldr	r3, [sp, #16]
08031dc6  54f82330  ldr.w	r3, [r4, r3, lsl #2]
08031dca  a2eb0303  sub.w	r3, r2, r3
08031dce  1f44      add	r7, r3
08031dd0  884b      ldr	r3, [pc, #544] ; [0x08031ff4] = 0x20001300
08031dd2  1a46      mov	r2, r3
08031dd4  43f82010  str.w	r1, [r3, r0, lsl #2]
08031dd8  874b      ldr	r3, [pc, #540] ; [0x08031ff8] = 0x20001280
08031dda  44f82070  str.w	r7, [r4, r0, lsl #2]
08031dde  03eb8603  add.w	r3, r3, r6, lsl #2
08031de2  c3ed007a  vstr	s15, [r3]
08031de6  05eb8603  add.w	r3, r5, r6, lsl #2
08031dea  c3f800e0  str.w	lr, [r3]
08031dee  3df4dca9  beq.w	#-11336 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031df2  029e      ldr	r6, [sp, #8]
08031df4  1346      mov	r3, r2
08031df6  0e28      cmp	r0, #14
08031df8  52f82620  ldr.w	r2, [r2, r6, lsl #2]
08031dfc  049e      ldr	r6, [sp, #16]
08031dfe  53f82630  ldr.w	r3, [r3, r6, lsl #2]
08031e02  a2eb0303  sub.w	r3, r2, r3
08031e06  029a      ldr	r2, [sp, #8]
08031e08  1944      add	r1, r3
08031e0a  54f82220  ldr.w	r2, [r4, r2, lsl #2]
08031e0e  54f82630  ldr.w	r3, [r4, r6, lsl #2]
08031e12  08f10b06  add.w	r6, r8, #11
08031e16  a2eb0303  sub.w	r3, r2, r3
08031e1a  764a      ldr	r2, [pc, #472] ; [0x08031ff4] = 0x20001300
08031e1c  1f44      add	r7, r3
08031e1e  764b      ldr	r3, [pc, #472] ; [0x08031ff8] = 0x20001280
08031e20  42f82610  str.w	r1, [r2, r6, lsl #2]
08031e24  03eb8003  add.w	r3, r3, r0, lsl #2
08031e28  44f82670  str.w	r7, [r4, r6, lsl #2]
08031e2c  c3ed007a  vstr	s15, [r3]
08031e30  05eb8003  add.w	r3, r5, r0, lsl #2
08031e34  c3f800e0  str.w	lr, [r3]
08031e38  3df4b7a9  beq.w	#-11410 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031e3c  029b      ldr	r3, [sp, #8]
08031e3e  1046      mov	r0, r2
08031e40  0e2e      cmp	r6, #14
08031e42  52f82320  ldr.w	r2, [r2, r3, lsl #2]
08031e46  049b      ldr	r3, [sp, #16]
08031e48  50f82330  ldr.w	r3, [r0, r3, lsl #2]
08031e4c  a2eb0303  sub.w	r3, r2, r3
08031e50  029a      ldr	r2, [sp, #8]
08031e52  01eb030c  add.w	r12, r1, r3
08031e56  0499      ldr	r1, [sp, #16]
08031e58  54f82220  ldr.w	r2, [r4, r2, lsl #2]
08031e5c  54f82130  ldr.w	r3, [r4, r1, lsl #2]
08031e60  a2eb0303  sub.w	r3, r2, r3
08031e64  08f10c02  add.w	r2, r8, #12
08031e68  1f44      add	r7, r3
08031e6a  634b      ldr	r3, [pc, #396] ; [0x08031ff8] = 0x20001280
08031e6c  40f822c0  str.w	r12, [r0, r2, lsl #2]
08031e70  03eb8603  add.w	r3, r3, r6, lsl #2
08031e74  44f82270  str.w	r7, [r4, r2, lsl #2]
08031e78  c3ed007a  vstr	s15, [r3]
08031e7c  05eb8603  add.w	r3, r5, r6, lsl #2
08031e80  c3f800e0  str.w	lr, [r3]
08031e84  3df491a9  beq.w	#-11486 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031e88  029d      ldr	r5, [sp, #8]
08031e8a  0646      mov	r6, r0
08031e8c  08f10d03  add.w	r3, r8, #13
08031e90  50f82500  ldr.w	r0, [r0, r5, lsl #2]
08031e94  2546      mov	r5, r4
08031e96  029c      ldr	r4, [sp, #8]
08031e98  55f82440  ldr.w	r4, [r5, r4, lsl #2]
08031e9c  3546      mov	r5, r6
08031e9e  56f82160  ldr.w	r6, [r6, r1, lsl #2]
08031ea2  801b      subs	r0, r0, r6
08031ea4  6044      add	r0, r12
08031ea6  45f82300  str.w	r0, [r5, r3, lsl #2]
08031eaa  5448      ldr	r0, [pc, #336] ; [0x08031ffc] = 0x200012c0
08031eac  544d      ldr	r5, [pc, #336] ; [0x08032000] = 0x20001240
08031eae  50f82110  ldr.w	r1, [r0, r1, lsl #2]
08031eb2  611a      subs	r1, r4, r1
08031eb4  3944      add	r1, r7
08031eb6  40f82310  str.w	r1, [r0, r3, lsl #2]
08031eba  4f4b      ldr	r3, [pc, #316] ; [0x08031ff8] = 0x20001280
08031ebc  03eb8203  add.w	r3, r3, r2, lsl #2
08031ec0  05eb8202  add.w	r2, r5, r2, lsl #2
08031ec4  c3ed007a  vstr	s15, [r3]
08031ec8  c2f800e0  str.w	lr, [r2]
08031ecc  fdf76db9  b.w	#-11558 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031ed0  92ed040a  vldr	s0, [r2, #16]
08031ed4  92ed037a  vldr	s14, [r2, #12]
08031ed8  fff749b9  b.w	#-3438 ; -> 0x0803116e ; branch_target=0x0803116e
08031edc  92ed0a3a  vldr	s6, [r2, #40]
08031ee0  d2ed096a  vldr	s13, [r2, #36]
08031ee4  fff7c8ba  b.w	#-2672 ; -> 0x08031478 ; branch_target=0x08031478
08031ee8  f0ee476a  vmov.f32	s13, s14
08031eec  9fed457a  vldr	s14, [pc, #276] ; [0x08032004] = 0x44800000 / f32_bits_interpretation=1024
08031ef0  4549      ldr	r1, [pc, #276] ; [0x08032008] = 0x08039674
08031ef2  26ee877a  vmul.f32	s14, s13, s14
08031ef6  fdeec76a  vcvt.s32.f32	s13, s14
08031efa  16ee903a  vmov	r3, s13
08031efe  5a1c      adds	r2, r3, #1
08031f00  c3f30903  ubfx	r3, r3, #0, #10
08031f04  c2f30902  ubfx	r2, r2, #0, #10
08031f08  01eb8303  add.w	r3, r1, r3, lsl #2
08031f0c  01eb8202  add.w	r2, r1, r2, lsl #2
08031f10  d3ed003a  vldr	s7, [r3]
08031f14  d2ed006a  vldr	s13, [r2]
08031f18  fef7a9bc  b.w	#-5806 ; -> 0x0803086e ; branch_target=0x0803086e
08031f1c  dfed394a  vldr	s9, [pc, #228] ; [0x08032004] = 0x44800000 / f32_bits_interpretation=1024
08031f20  3a48      ldr	r0, [pc, #232] ; [0x0803200c] = 0x08037674
08031f22  66ee244a  vmul.f32	s9, s12, s9
08031f26  fdeee46a  vcvt.s32.f32	s13, s9
08031f2a  16ee903a  vmov	r3, s13
08031f2e  591c      adds	r1, r3, #1
08031f30  c3f30903  ubfx	r3, r3, #0, #10
08031f34  c1f30901  ubfx	r1, r1, #0, #10
08031f38  00eb8303  add.w	r3, r0, r3, lsl #2
08031f3c  00eb8101  add.w	r1, r0, r1, lsl #2
08031f40  d3ed006a  vldr	s13, [r3]
08031f44  91ed002a  vldr	s4, [r1]
08031f48  fef739bc  b.w	#-6030 ; -> 0x080307be ; branch_target=0x080307be
08031f4c  304b      ldr	r3, [pc, #192] ; [0x08032010] = 0x20002404
08031f4e  93ed007a  vldr	s14, [r3]
08031f52  304b      ldr	r3, [pc, #192] ; [0x08032014] = 0x20002400
08031f54  93ed004a  vldr	s8, [r3]
08031f58  fef7f2bc  b.w	#-5660 ; -> 0x08030940 ; branch_target=0x08030940
08031f5c  2c4b      ldr	r3, [pc, #176] ; [0x08032010] = 0x20002404
08031f5e  93ed003a  vldr	s6, [r3]
08031f62  2c4b      ldr	r3, [pc, #176] ; [0x08032014] = 0x20002400
08031f64  d3ed004a  vldr	s9, [r3]
08031f68  fff7deb9  b.w	#-3140 ; -> 0x08031328 ; branch_target=0x08031328
08031f6c  234b      ldr	r3, [pc, #140] ; [0x08031ffc] = 0x200012c0
08031f6e  5b69      ldr	r3, [r3, #20]
08031f70  9342      cmp	r3, r2
08031f72  69dd      ble	#210 ; -> 0x08032048 ; branch_target=0x08032048
08031f74  511a      subs	r1, r2, r1
08031f76  224b      ldr	r3, [pc, #136] ; [0x08032000] = 0x20001240
08031f78  07ee101a  vmov	s14, r1
08031f7c  93ed046a  vldr	s12, [r3, #16]
08031f80  f8eec76a  vcvt.f32.s32	s13, s14
08031f84  b0ee6e7a  vmov.f32	s14, s29
08031f88  a6ee867a  vfma.f32	s14, s13, s12
08031f8c  bdeec77a  vcvt.s32.f32	s14, s14
08031f90  17ee103a  vmov	r3, s14
08031f94  fcf7d3bf  b.w	#-12378 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
08031f98  164b      ldr	r3, [pc, #88] ; [0x08031ff4] = 0x20001300
08031f9a  5b69      ldr	r3, [r3, #20]
08031f9c  9342      cmp	r3, r2
08031f9e  3ddd      ble	#122 ; -> 0x0803201c ; branch_target=0x0803201c
08031fa0  511a      subs	r1, r2, r1
08031fa2  154b      ldr	r3, [pc, #84] ; [0x08031ff8] = 0x20001280
08031fa4  07ee901a  vmov	s15, r1
08031fa8  d3ed046a  vldr	s13, [r3, #16]
08031fac  b8eee77a  vcvt.f32.s32	s14, s15
08031fb0  f0ee6e7a  vmov.f32	s15, s29
08031fb4  e7ee267a  vfma.f32	s15, s14, s13
08031fb8  fdeee77a  vcvt.s32.f32	s15, s15
08031fbc  17ee902a  vmov	r2, s15
08031fc0  fcf774bf  b.w	#-12568 ; -> 0x0802eeac ; branch_target=0x0802eeac
08031fc4  0022      movs	r2, #0
08031fc6  4021      movs	r1, #64
08031fc8  1348      ldr	r0, [pc, #76] ; [0x08032018] = 0x58020800
08031fca  f1f71ffa  bl	#-60354 ; -> 0x0802340c ; branch_target=0x0802340c
08031fce  094b      ldr	r3, [pc, #36] ; [0x08031ff4] = 0x20001300
08031fd0  a4f11a02  sub.w	r2, r4, #26
08031fd4  1d60      str	r5, [r3]
08031fd6  40f2b573  movw	r3, #1973
08031fda  9a42      cmp	r2, r3
08031fdc  3df6e5a8  bhi.w	#-11830 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
08031fe0  0022      movs	r2, #0
08031fe2  8021      movs	r1, #128
08031fe4  0c48      ldr	r0, [pc, #48] ; [0x08032018] = 0x58020800
08031fe6  f1f711fa  bl	#-60382 ; -> 0x0802340c ; branch_target=0x0802340c
08031fea  044b      ldr	r3, [pc, #16] ; [0x08031ffc] = 0x200012c0
08031fec  1c60      str	r4, [r3]
08031fee  fdf7dcb8  b.w	#-11848 ; -> 0x0802f1aa ; branch_target=0x0802f1aa
0803201c  bf49      ldr	r1, [pc, #764] ; [0x0803231c] = 0x20001280
0803201e  d1ed056a  vldr	s13, [r1, #20]
08032022  bf49      ldr	r1, [pc, #764] ; [0x08032320] = 0x20001300
08032024  8969      ldr	r1, [r1, #24]
08032026  9142      cmp	r1, r2
08032028  36dd      ble	#108 ; -> 0x08032098 ; branch_target=0x08032098
0803202a  d31a      subs	r3, r2, r3
0803202c  07ee903a  vmov	s15, r3
08032030  b8eee77a  vcvt.f32.s32	s14, s15
08032034  dfedbb7a  vldr	s15, [pc, #748] ; [0x08032324] = 0x46200000 / f32_bits_interpretation=10240
08032038  e7ee267a  vfma.f32	s15, s14, s13
0803203c  fdeee77a  vcvt.s32.f32	s15, s15
08032040  17ee902a  vmov	r2, s15
08032044  fcf732bf  b.w	#-12700 ; -> 0x0802eeac ; branch_target=0x0802eeac
08032048  b749      ldr	r1, [pc, #732] ; [0x08032328] = 0x20001240
0803204a  91ed056a  vldr	s12, [r1, #20]
0803204e  b749      ldr	r1, [pc, #732] ; [0x0803232c] = 0x200012c0
08032050  8969      ldr	r1, [r1, #24]
08032052  9142      cmp	r1, r2
08032054  34dd      ble	#104 ; -> 0x080320c0 ; branch_target=0x080320c0
08032056  d31a      subs	r3, r2, r3
08032058  07ee103a  vmov	s14, r3
0803205c  f8eec76a  vcvt.f32.s32	s13, s14
08032060  9fedb07a  vldr	s14, [pc, #704] ; [0x08032324] = 0x46200000 / f32_bits_interpretation=10240
08032064  a6ee867a  vfma.f32	s14, s13, s12
08032068  bdeec77a  vcvt.s32.f32	s14, s14
0803206c  17ee103a  vmov	r3, s14
08032070  fcf765bf  b.w	#-12598 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
08032074  dfedae4a  vldr	s9, [pc, #696] ; [0x08032330] = 0x00000000
08032078  b0ee644a  vmov.f32	s8, s9
0803207c  f0ee646a  vmov.f32	s13, s9
08032080  b0ee647a  vmov.f32	s14, s9
08032084  fef772bf  b.w	#-4380 ; -> 0x08030f6c ; branch_target=0x08030f6c
08032088  9feda97a  vldr	s14, [pc, #676] ; [0x08032330] = 0x00000000
0803208c  b0ee476a  vmov.f32	s12, s14
08032090  f0ee478a  vmov.f32	s17, s14
08032094  fef7c0bd  b.w	#-5248 ; -> 0x08030c18 ; branch_target=0x08030c18
08032098  a14b      ldr	r3, [pc, #644] ; [0x08032320] = 0x20001300
0803209a  db69      ldr	r3, [r3, #28]
0803209c  9342      cmp	r3, r2
0803209e  40f3e480  ble.w	#456 ; -> 0x0803226a ; branch_target=0x0803226a
080320a2  511a      subs	r1, r2, r1
080320a4  07ee901a  vmov	s15, r1
080320a8  b8eee77a  vcvt.f32.s32	s14, s15
080320ac  dfeda17a  vldr	s15, [pc, #644] ; [0x08032334] = 0x46400000 / f32_bits_interpretation=12288
080320b0  e7ee267a  vfma.f32	s15, s14, s13
080320b4  fdeee77a  vcvt.s32.f32	s15, s15
080320b8  17ee902a  vmov	r2, s15
080320bc  fcf7f6be  b.w	#-12820 ; -> 0x0802eeac ; branch_target=0x0802eeac
080320c0  9a4b      ldr	r3, [pc, #616] ; [0x0803232c] = 0x200012c0
080320c2  db69      ldr	r3, [r3, #28]
080320c4  9342      cmp	r3, r2
080320c6  40f3df80  ble.w	#446 ; -> 0x08032288 ; branch_target=0x08032288
080320ca  511a      subs	r1, r2, r1
080320cc  07ee101a  vmov	s14, r1
080320d0  f8eec76a  vcvt.f32.s32	s13, s14
080320d4  9fed977a  vldr	s14, [pc, #604] ; [0x08032334] = 0x46400000 / f32_bits_interpretation=12288
080320d8  a6ee867a  vfma.f32	s14, s13, s12
080320dc  bdeec77a  vcvt.s32.f32	s14, s14
080320e0  17ee103a  vmov	r3, s14
080320e4  fcf72bbf  b.w	#-12714 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
080320e8  dff8c0e2  ldr.w	lr, [pc, #704] ; [0x080323ac] = 0x20002ed8
080320ec  def80030  ldr.w	r3, [lr]
080320f0  002b      cmp	r3, #0
080320f2  36d1      bne	#108 ; -> 0x08032162 ; branch_target=0x08032162
080320f4  9049      ldr	r1, [pc, #576] ; [0x08032338] = 0x20002e7c
080320f6  914a      ldr	r2, [pc, #580] ; [0x0803233c] = 0x20002b40
080320f8  d1f800c0  ldr.w	r12, [r1]
080320fc  904d      ldr	r5, [pc, #576] ; [0x08032340] = 0x20002f74
080320fe  02f58074  add.w	r4, r2, #256
08032102  9048      ldr	r0, [pc, #576] ; [0x08032344] = 0x20002e9c
08032104  6346      mov	r3, r12
08032106  2e68      ldr	r6, [r5]
08032108  f2ec017a  vldmia	r2!, {s15}
0803210c  06eb8303  add.w	r3, r6, r3, lsl #2
08032110  c3ed007a  vstr	s15, [r3]
08032114  90ed007a  vldr	s14, [r0]
08032118  0b68      ldr	r3, [r1]
0803211a  b4eee77a  vcmpe.f32	s14, s15
0803211e  f1ee10fa  vmrs	APSR_nzcv, fpscr
08032122  27d5      bpl	#78 ; -> 0x08032174 ; branch_target=0x08032174
08032124  0133      adds	r3, #1
08032126  9442      cmp	r4, r2
08032128  c0ed007a  vstr	s15, [r0]
0803212c  0b60      str	r3, [r1]
0803212e  ead1      bne	#-44 ; -> 0x08032106 ; branch_target=0x08032106
08032130  9c46      mov	r12, r3
08032132  854b      ldr	r3, [pc, #532] ; [0x08032348] = 0x20002430
08032134  1a68      ldr	r2, [r3]
08032136  854b      ldr	r3, [pc, #532] ; [0x0803234c] = 0x20000a20
08032138  1101      lsls	r1, r2, #4
0803213a  03eb0212  add.w	r2, r3, r2, lsl #4
0803213e  5b58      ldr	r3, [r3, r1]
08032140  03f58031  add.w	r1, r3, #65536
08032144  6145      cmp	r1, r12
08032146  7cf44fab  bne.w	#-14690 ; -> 0x0802e7e8 ; branch_target=0x0802e7e8
0803214a  aceb0303  sub.w	r3, r12, r3
0803214e  9b09      lsrs	r3, r3, #6
08032150  5360      str	r3, [r2, #4]
08032152  0023      movs	r3, #0
08032154  3b60      str	r3, [r7]
08032156  cef80030  str.w	r3, [lr]
0803215a  0123      movs	r3, #1
0803215c  9360      str	r3, [r2, #8]
0803215e  fcf743bb  b.w	#-14714 ; -> 0x0802e7e8 ; branch_target=0x0802e7e8
08032162  012b      cmp	r3, #1
08032164  7cf440ab  bne.w	#-14720 ; -> 0x0802e7e8 ; branch_target=0x0802e7e8
08032168  794b      ldr	r3, [pc, #484] ; [0x08032350] = 0x20002ed0
0803216a  1b68      ldr	r3, [r3]
0803216c  012b      cmp	r3, #1
0803216e  7cf43bab  bne.w	#-14730 ; -> 0x0802e7e8 ; branch_target=0x0802e7e8
08032172  bfe7      b	#-130 ; -> 0x080320f4 ; branch_target=0x080320f4
08032174  0133      adds	r3, #1
08032176  9442      cmp	r4, r2
08032178  0b60      str	r3, [r1]
0803217a  c4d1      bne	#-120 ; -> 0x08032106 ; branch_target=0x08032106
0803217c  d8e7      b	#-80 ; -> 0x08032130 ; branch_target=0x08032130
0803217e  754b      ldr	r3, [pc, #468] ; [0x08032354] = 0x20002eec
08032180  754a      ldr	r2, [pc, #468] ; [0x08032358] = 0x20002eb4
08032182  1968      ldr	r1, [r3]
08032184  1368      ldr	r3, [r2]
08032186  7548      ldr	r0, [pc, #468] ; [0x0803235c] = 0x00017700
08032188  cb1a      subs	r3, r1, r3
0803218a  8342      cmp	r3, r0
0803218c  7cf63eab  bls.w	#-14724 ; -> 0x0802e80c ; branch_target=0x0802e80c
08032190  1160      str	r1, [r2]
08032192  fbf775f8  bl	#-20246 ; -> 0x0802d280 ; branch_target=0x0802d280
08032196  fcf739bb  b.w	#-14734 ; -> 0x0802e80c ; branch_target=0x0802e80c
0803219a  714f      ldr	r7, [pc, #452] ; [0x08032360] = 0x20002ed4
0803219c  3b68      ldr	r3, [r7]
0803219e  002b      cmp	r3, #0
080321a0  32d1      bne	#100 ; -> 0x08032208 ; branch_target=0x08032208
080321a2  7048      ldr	r0, [pc, #448] ; [0x08032364] = 0x200011a0
080321a4  7049      ldr	r1, [pc, #448] ; [0x08032368] = 0x20002d40
080321a6  714e      ldr	r6, [pc, #452] ; [0x0803236c] = 0x20002f70
080321a8  714c      ldr	r4, [pc, #452] ; [0x08032370] = 0x200011e0
080321aa  01f58075  add.w	r5, r1, #256
080321ae  0368      ldr	r3, [r0]
080321b0  3268      ldr	r2, [r6]
080321b2  f1ec017a  vldmia	r1!, {s15}
080321b6  02eb8302  add.w	r2, r2, r3, lsl #2
080321ba  c2ed007a  vstr	s15, [r2]
080321be  94ed007a  vldr	s14, [r4]
080321c2  0368      ldr	r3, [r0]
080321c4  b4eee77a  vcmpe.f32	s14, s15
080321c8  f1ee10fa  vmrs	APSR_nzcv, fpscr
080321cc  25d5      bpl	#74 ; -> 0x0803221a ; branch_target=0x0803221a
080321ce  0133      adds	r3, #1
080321d0  a942      cmp	r1, r5
080321d2  c4ed007a  vstr	s15, [r4]
080321d6  0360      str	r3, [r0]
080321d8  ead1      bne	#-44 ; -> 0x080321b0 ; branch_target=0x080321b0
080321da  664a      ldr	r2, [pc, #408] ; [0x08032374] = 0x20000b20
080321dc  6649      ldr	r1, [pc, #408] ; [0x08032378] = 0x20000920
080321de  1268      ldr	r2, [r2]
080321e0  1001      lsls	r0, r2, #4
080321e2  01eb0212  add.w	r2, r1, r2, lsl #4
080321e6  0958      ldr	r1, [r1, r0]
080321e8  01f58030  add.w	r0, r1, #65536
080321ec  9842      cmp	r0, r3
080321ee  7cf405ab  bne.w	#-14838 ; -> 0x0802e7fc ; branch_target=0x0802e7fc
080321f2  5b1a      subs	r3, r3, r1
080321f4  9b09      lsrs	r3, r3, #6
080321f6  5360      str	r3, [r2, #4]
080321f8  0023      movs	r3, #0
080321fa  ccf80030  str.w	r3, [r12]
080321fe  3b60      str	r3, [r7]
08032200  0123      movs	r3, #1
08032202  9360      str	r3, [r2, #8]
08032204  fcf7faba  b.w	#-14860 ; -> 0x0802e7fc ; branch_target=0x0802e7fc
08032208  012b      cmp	r3, #1
0803220a  7cf4f7aa  bne.w	#-14866 ; -> 0x0802e7fc ; branch_target=0x0802e7fc
0803220e  5b4b      ldr	r3, [pc, #364] ; [0x0803237c] = 0x20002ecc
08032210  1b68      ldr	r3, [r3]
08032212  012b      cmp	r3, #1
08032214  7cf4f2aa  bne.w	#-14876 ; -> 0x0802e7fc ; branch_target=0x0802e7fc
08032218  c3e7      b	#-122 ; -> 0x080321a2 ; branch_target=0x080321a2
0803221a  0133      adds	r3, #1
0803221c  a942      cmp	r1, r5
0803221e  0360      str	r3, [r0]
08032220  c6d1      bne	#-116 ; -> 0x080321b0 ; branch_target=0x080321b0
08032222  dae7      b	#-76 ; -> 0x080321da ; branch_target=0x080321da
08032224  564d      ldr	r5, [pc, #344] ; [0x08032380] = 0x2000000c
08032226  40f69e63  movw	r3, #3742
0803222a  564e      ldr	r6, [pc, #344] ; [0x08032384] = 0x20000008
0803222c  0027      movs	r7, #0
0803222e  0322      movs	r2, #3
08032230  2946      mov	r1, r5
08032232  2b80      strh	r3, [r5]
08032234  5448      ldr	r0, [pc, #336] ; [0x08032388] = 0x20014bb8
08032236  3368      ldr	r3, [r6]
08032238  af70      strb	r7, [r5, #2]
0803223a  f4f735fe  bl	#-45974 ; -> 0x08026ea8 ; branch_target=0x08026ea8
0803223e  40f29e53  movw	r3, #1438
08032242  0222      movs	r2, #2
08032244  2946      mov	r1, r5
08032246  5048      ldr	r0, [pc, #320] ; [0x08032388] = 0x20014bb8
08032248  2b80      strh	r3, [r5]
0803224a  3368      ldr	r3, [r6]
0803224c  f4f72cfe  bl	#-45992 ; -> 0x08026ea8 ; branch_target=0x08026ea8
08032250  3368      ldr	r3, [r6]
08032252  2946      mov	r1, r5
08032254  4d4a      ldr	r2, [pc, #308] ; [0x0803238c] = 0x20002f88
08032256  0093      str	r3, [sp]
08032258  9f23      movs	r3, #159
0803225a  4b48      ldr	r0, [pc, #300] ; [0x08032388] = 0x20014bb8
0803225c  2b70      strb	r3, [r5]
0803225e  0223      movs	r3, #2
08032260  f4f76aff  bl	#-45356 ; -> 0x08027138 ; branch_target=0x08027138
08032264  2770      strb	r7, [r4]
08032266  fcf788ba  b.w	#-15088 ; -> 0x0802e77a ; branch_target=0x0802e77a
0803226a  d31a      subs	r3, r2, r3
0803226c  07ee903a  vmov	s15, r3
08032270  b8eee77a  vcvt.f32.s32	s14, s15
08032274  dfed467a  vldr	s15, [pc, #280] ; [0x08032390] = 0x46600000 / f32_bits_interpretation=14336
08032278  e7ee267a  vfma.f32	s15, s14, s13
0803227c  fdeee77a  vcvt.s32.f32	s15, s15
08032280  17ee902a  vmov	r2, s15
08032284  fcf712be  b.w	#-13276 ; -> 0x0802eeac ; branch_target=0x0802eeac
08032288  d31a      subs	r3, r2, r3
0803228a  07ee103a  vmov	s14, r3
0803228e  f8eec76a  vcvt.f32.s32	s13, s14
08032292  9fed3f7a  vldr	s14, [pc, #252] ; [0x08032390] = 0x46600000 / f32_bits_interpretation=14336
08032296  a6ee867a  vfma.f32	s14, s13, s12
0803229a  bdeec77a  vcvt.s32.f32	s14, s14
0803229e  17ee103a  vmov	r3, s14
080322a2  fcf74cbe  b.w	#-13160 ; -> 0x0802ef3e ; branch_target=0x0802ef3e
080322a6  3b4b      ldr	r3, [pc, #236] ; [0x08032394] = 0x20002e64
080322a8  d3ed006a  vldr	s13, [r3]
080322ac  2293      str	r3, [sp, #136]
080322ae  3a4b      ldr	r3, [pc, #232] ; [0x08032398] = 0x20002e40
080322b0  c3ed006a  vstr	s13, [r3]
080322b4  fcf733bb  b.w	#-14746 ; -> 0x0802e91e ; branch_target=0x0802e91e
080322b8  384b      ldr	r3, [pc, #224] ; [0x0803239c] = 0x20002e5c
080322ba  93ed007a  vldr	s14, [r3]
080322be  2193      str	r3, [sp, #132]
080322c0  374b      ldr	r3, [pc, #220] ; [0x080323a0] = 0x20002e44
080322c2  83ed007a  vstr	s14, [r3]
080322c6  fcf7cfbb  b.w	#-14434 ; -> 0x0802ea68 ; branch_target=0x0802ea68
080322ca  dfed197a  vldr	s15, [pc, #100] ; [0x08032330] = 0x00000000
080322ce  b0ee670a  vmov.f32	s0, s15
080322d2  fcf7bcbb  b.w	#-14472 ; -> 0x0802ea4e ; branch_target=0x0802ea4e
080322d6  dfed167a  vldr	s15, [pc, #88] ; [0x08032330] = 0x00000000
080322da  f0ee670a  vmov.f32	s1, s15
080322de  fcf73dbb  b.w	#-14726 ; -> 0x0802e95c ; branch_target=0x0802e95c
080322e2  0020      movs	r0, #0
080322e4  01f008fc  bl	#6160 ; -> 0x08033af8 ; branch_target=0x08033af8
080322e8  249b      ldr	r3, [sp, #144]
080322ea  b6f90220  ldrsh.w	r2, [r6, #2]
080322ee  1b68      ldr	r3, [r3]
080322f0  9a42      cmp	r2, r3
080322f2  3cf45eaa  beq.w	#-15172 ; -> 0x0802e7b2 ; branch_target=0x0802e7b2
080322f6  fef761b8  b.w	#-7998 ; -> 0x080303bc ; branch_target=0x080303bc
080322fa  3846      mov	r0, r7
080322fc  01f0fcfb  bl	#6136 ; -> 0x08033af8 ; branch_target=0x08033af8
08032300  fcf757ba  b.w	#-15186 ; -> 0x0802e7b2 ; branch_target=0x0802e7b2
08032304  274b      ldr	r3, [pc, #156] ; [0x080323a4] = 0x2001348c
08032306  b3f90430  ldrsh.w	r3, [r3, #4]
0803230a  012b      cmp	r3, #1
0803230c  00f0d180  beq.w	#418 ; -> 0x080324b2 ; branch_target=0x080324b2
08032310  faf74cfe  bl	#-21352 ; -> 0x0802cfac ; branch_target=0x0802cfac
08032314  244b      ldr	r3, [pc, #144] ; [0x080323a8] = 0x200023ec
08032316  1493      str	r3, [sp, #80]
08032318  fcf720bc  b.w	#-14272 ; -> 0x0802eb5c ; branch_target=0x0802eb5c
080323b0  a24b      ldr	r3, [pc, #648] ; [0x0803263c] = 0x2001348c
080323b2  b3f90630  ldrsh.w	r3, [r3, #6]
080323b6  012b      cmp	r3, #1
080323b8  73d0      beq	#230 ; -> 0x080324a2 ; branch_target=0x080324a2
080323ba  faf79dfe  bl	#-21190 ; -> 0x0802d0f8 ; branch_target=0x0802d0f8
080323be  a04b      ldr	r3, [pc, #640] ; [0x08032640] = 0x08043a74
080323c0  0893      str	r3, [sp, #32]
080323c2  a04b      ldr	r3, [pc, #640] ; [0x08032644] = 0x200023f0
080323c4  1593      str	r3, [sp, #84]
080323c6  fcf723bb  b.w	#-14778 ; -> 0x0802ea10 ; branch_target=0x0802ea10
080323ca  0022      movs	r2, #0
080323cc  4021      movs	r1, #64
080323ce  9e48      ldr	r0, [pc, #632] ; [0x08032648] = 0x58020800
080323d0  f1f71cf8  bl	#-61384 ; -> 0x0802340c ; branch_target=0x0802340c
080323d4  8021      movs	r1, #128
080323d6  0022      movs	r2, #0
080323d8  9b48      ldr	r0, [pc, #620] ; [0x08032648] = 0x58020800
080323da  f1f717f8  bl	#-61394 ; -> 0x0802340c ; branch_target=0x0802340c
080323de  9b4b      ldr	r3, [pc, #620] ; [0x0803264c] = 0x20002000
080323e0  1968      ldr	r1, [r3]
080323e2  4ef66023  movw	r3, #60000
080323e6  9942      cmp	r1, r3
080323e8  03dd      ble	#6 ; -> 0x080323f2 ; branch_target=0x080323f2
080323ea  2368      ldr	r3, [r4]
080323ec  984a      ldr	r2, [pc, #608] ; [0x08032650] = 0x20001340
080323ee  9b11      asrs	r3, r3, #6
080323f0  1360      str	r3, [r2]
080323f2  964b      ldr	r3, [pc, #600] ; [0x0803264c] = 0x20002000
080323f4  4ef66022  movw	r2, #60000
080323f8  5b68      ldr	r3, [r3, #4]
080323fa  9342      cmp	r3, r2
080323fc  03dd      ble	#6 ; -> 0x08032406 ; branch_target=0x08032406
080323fe  6268      ldr	r2, [r4, #4]
08032400  9348      ldr	r0, [pc, #588] ; [0x08032650] = 0x20001340
08032402  9211      asrs	r2, r2, #6
08032404  4260      str	r2, [r0, #4]
08032406  914a      ldr	r2, [pc, #580] ; [0x0803264c] = 0x20002000
08032408  9068      ldr	r0, [r2, #8]
0803240a  4ef66022  movw	r2, #60000
0803240e  9042      cmp	r0, r2
08032410  03dd      ble	#6 ; -> 0x0803241a ; branch_target=0x0803241a
08032412  a268      ldr	r2, [r4, #8]
08032414  8e4d      ldr	r5, [pc, #568] ; [0x08032650] = 0x20001340
08032416  9211      asrs	r2, r2, #6
08032418  aa60      str	r2, [r5, #8]
0803241a  8c4a      ldr	r2, [pc, #560] ; [0x0803264c] = 0x20002000
0803241c  d668      ldr	r6, [r2, #12]
0803241e  4ef66022  movw	r2, #60000
08032422  9642      cmp	r6, r2
08032424  03dd      ble	#6 ; -> 0x0803242e ; branch_target=0x0803242e
08032426  e268      ldr	r2, [r4, #12]
08032428  894d      ldr	r5, [pc, #548] ; [0x08032650] = 0x20001340
0803242a  9211      asrs	r2, r2, #6
0803242c  ea60      str	r2, [r5, #12]
0803242e  874a      ldr	r2, [pc, #540] ; [0x0803264c] = 0x20002000
08032430  1769      ldr	r7, [r2, #16]
08032432  4ef66022  movw	r2, #60000
08032436  9742      cmp	r7, r2
08032438  03dd      ble	#6 ; -> 0x08032442 ; branch_target=0x08032442
0803243a  2269      ldr	r2, [r4, #16]
0803243c  844d      ldr	r5, [pc, #528] ; [0x08032650] = 0x20001340
0803243e  9211      asrs	r2, r2, #6
08032440  2a61      str	r2, [r5, #16]
08032442  824a      ldr	r2, [pc, #520] ; [0x0803264c] = 0x20002000
08032444  5569      ldr	r5, [r2, #20]
08032446  4ef66022  movw	r2, #60000
0803244a  9542      cmp	r5, r2
0803244c  03dd      ble	#6 ; -> 0x08032456 ; branch_target=0x08032456
0803244e  6269      ldr	r2, [r4, #20]
08032450  7f4c      ldr	r4, [pc, #508] ; [0x08032650] = 0x20001340
08032452  9211      asrs	r2, r2, #6
08032454  6261      str	r2, [r4, #20]
08032456  4ef65f22  movw	r2, #59999
0803245a  9142      cmp	r1, r2
0803245c  c8bf      it	gt
0803245e  9342      cmpgt	r3, r2
08032460  d4bf      ite	le
08032462  0123      movle	r3, #1
08032464  0023      movgt	r3, #0
08032466  9042      cmp	r0, r2
08032468  d8bf      it	le
0803246a  43f00103  orrle	r3, r3, #1
0803246e  9642      cmp	r6, r2
08032470  d8bf      it	le
08032472  43f00103  orrle	r3, r3, #1
08032476  9742      cmp	r7, r2
08032478  d8bf      it	le
0803247a  43f00103  orrle	r3, r3, #1
0803247e  0bb9      cbnz	r3, #2 ; -> 0x08032484 ; branch_target=0x08032484
08032480  9542      cmp	r5, r2
08032482  09dc      bgt	#18 ; -> 0x08032498 ; branch_target=0x08032498
08032484  0122      movs	r2, #1
08032486  4021      movs	r1, #64
08032488  6f48      ldr	r0, [pc, #444] ; [0x08032648] = 0x58020800
0803248a  f0f7bfff  bl	#-61570 ; -> 0x0802340c ; branch_target=0x0802340c
0803248e  0122      movs	r2, #1
08032490  8021      movs	r1, #128
08032492  6d48      ldr	r0, [pc, #436] ; [0x08032648] = 0x58020800
08032494  f0f7baff  bl	#-61580 ; -> 0x0802340c ; branch_target=0x0802340c
08032498  6e4b      ldr	r3, [pc, #440] ; [0x08032654] = 0x200144d4
0803249a  1b68      ldr	r3, [r3]
0803249c  0493      str	r3, [sp, #16]
0803249e  fcf7f9bb  b.w	#-14350 ; -> 0x0802ec94 ; branch_target=0x0802ec94
080324a2  faf7dbfc  bl	#-22090 ; -> 0x0802ce5c ; branch_target=0x0802ce5c
080324a6  664b      ldr	r3, [pc, #408] ; [0x08032640] = 0x08043a74
080324a8  0893      str	r3, [sp, #32]
080324aa  664b      ldr	r3, [pc, #408] ; [0x08032644] = 0x200023f0
080324ac  1593      str	r3, [sp, #84]
080324ae  fcf7afba  b.w	#-15010 ; -> 0x0802ea10 ; branch_target=0x0802ea10
080324b2  faf72bfc  bl	#-22442 ; -> 0x0802cd0c ; branch_target=0x0802cd0c
080324b6  684b      ldr	r3, [pc, #416] ; [0x08032658] = 0x200023ec
080324b8  1493      str	r3, [sp, #80]
080324ba  fcf74fbb  b.w	#-14690 ; -> 0x0802eb5c ; branch_target=0x0802eb5c
080324be  f0ee407a  vmov.f32	s15, s0
080324c2  fcf7c4ba  b.w	#-14968 ; -> 0x0802ea4e ; branch_target=0x0802ea4e
080324c6  f0ee407a  vmov.f32	s15, s0
080324ca  fcf71bba  b.w	#-15306 ; -> 0x0802e904 ; branch_target=0x0802e904
080324ce  f0ee607a  vmov.f32	s15, s1
080324d2  fcf743ba  b.w	#-15226 ; -> 0x0802e95c ; branch_target=0x0802e95c
080324d6  0022      movs	r2, #0
080324d8  4021      movs	r1, #64
080324da  5b48      ldr	r0, [pc, #364] ; [0x08032648] = 0x58020800
080324dc  f0f796ff  bl	#-61652 ; -> 0x0802340c ; branch_target=0x0802340c
080324e0  8021      movs	r1, #128
080324e2  0022      movs	r2, #0
080324e4  5848      ldr	r0, [pc, #352] ; [0x08032648] = 0x58020800
080324e6  f0f791ff  bl	#-61662 ; -> 0x0802340c ; branch_target=0x0802340c
080324ea  584b      ldr	r3, [pc, #352] ; [0x0803264c] = 0x20002000
080324ec  1968      ldr	r1, [r3]
080324ee  b1f5fa5f  cmp.w	r1, #8000
080324f2  11da      bge	#34 ; -> 0x08032518 ; branch_target=0x08032518
080324f4  2268      ldr	r2, [r4]
080324f6  f7ee006a  vmov.f32	s13, #1.000000e+00
080324fa  584b      ldr	r3, [pc, #352] ; [0x0803265c] = 0x20001380
080324fc  9211      asrs	r2, r2, #6
080324fe  1a60      str	r2, [r3]
08032500  534b      ldr	r3, [pc, #332] ; [0x08032650] = 0x20001340
08032502  1b68      ldr	r3, [r3]
08032504  9b1a      subs	r3, r3, r2
08032506  07ee903a  vmov	s15, r3
0803250a  554b      ldr	r3, [pc, #340] ; [0x08032660] = 0x20001200
0803250c  f8eee77a  vcvt.f32.s32	s15, s15
08032510  86eea77a  vdiv.f32	s14, s13, s15
08032514  83ed007a  vstr	s14, [r3]
08032518  4c4b      ldr	r3, [pc, #304] ; [0x0803264c] = 0x20002000
0803251a  5b68      ldr	r3, [r3, #4]
0803251c  b3f5fa5f  cmp.w	r3, #8000
08032520  11da      bge	#34 ; -> 0x08032546 ; branch_target=0x08032546
08032522  6068      ldr	r0, [r4, #4]
08032524  f7ee006a  vmov.f32	s13, #1.000000e+00
08032528  4c4a      ldr	r2, [pc, #304] ; [0x0803265c] = 0x20001380
0803252a  8011      asrs	r0, r0, #6
0803252c  5060      str	r0, [r2, #4]
0803252e  484a      ldr	r2, [pc, #288] ; [0x08032650] = 0x20001340
08032530  5268      ldr	r2, [r2, #4]
08032532  121a      subs	r2, r2, r0
08032534  07ee902a  vmov	s15, r2
08032538  494a      ldr	r2, [pc, #292] ; [0x08032660] = 0x20001200
0803253a  f8eee77a  vcvt.f32.s32	s15, s15
0803253e  86eea77a  vdiv.f32	s14, s13, s15
08032542  82ed017a  vstr	s14, [r2, #4]
08032546  414a      ldr	r2, [pc, #260] ; [0x0803264c] = 0x20002000
08032548  9068      ldr	r0, [r2, #8]
0803254a  b0f5fa5f  cmp.w	r0, #8000
0803254e  11da      bge	#34 ; -> 0x08032574 ; branch_target=0x08032574
08032550  a268      ldr	r2, [r4, #8]
08032552  f7ee006a  vmov.f32	s13, #1.000000e+00
08032556  414d      ldr	r5, [pc, #260] ; [0x0803265c] = 0x20001380
08032558  9211      asrs	r2, r2, #6
0803255a  aa60      str	r2, [r5, #8]
0803255c  3c4d      ldr	r5, [pc, #240] ; [0x08032650] = 0x20001340
0803255e  ad68      ldr	r5, [r5, #8]
08032560  ad1a      subs	r5, r5, r2
08032562  3f4a      ldr	r2, [pc, #252] ; [0x08032660] = 0x20001200
08032564  07ee905a  vmov	s15, r5
08032568  f8eee77a  vcvt.f32.s32	s15, s15
0803256c  86eea77a  vdiv.f32	s14, s13, s15
08032570  82ed027a  vstr	s14, [r2, #8]
08032574  354a      ldr	r2, [pc, #212] ; [0x0803264c] = 0x20002000
08032576  d668      ldr	r6, [r2, #12]
08032578  b6f5fa5f  cmp.w	r6, #8000
0803257c  11da      bge	#34 ; -> 0x080325a2 ; branch_target=0x080325a2
0803257e  e268      ldr	r2, [r4, #12]
08032580  f7ee006a  vmov.f32	s13, #1.000000e+00
08032584  354d      ldr	r5, [pc, #212] ; [0x0803265c] = 0x20001380
08032586  9211      asrs	r2, r2, #6
08032588  ea60      str	r2, [r5, #12]
0803258a  314d      ldr	r5, [pc, #196] ; [0x08032650] = 0x20001340
0803258c  ed68      ldr	r5, [r5, #12]
0803258e  ad1a      subs	r5, r5, r2
08032590  334a      ldr	r2, [pc, #204] ; [0x08032660] = 0x20001200
08032592  07ee905a  vmov	s15, r5
08032596  f8eee77a  vcvt.f32.s32	s15, s15
0803259a  86eea77a  vdiv.f32	s14, s13, s15
0803259e  82ed037a  vstr	s14, [r2, #12]
080325a2  2a4a      ldr	r2, [pc, #168] ; [0x0803264c] = 0x20002000
080325a4  1769      ldr	r7, [r2, #16]
080325a6  b7f5fa5f  cmp.w	r7, #8000
080325aa  11da      bge	#34 ; -> 0x080325d0 ; branch_target=0x080325d0
080325ac  2269      ldr	r2, [r4, #16]
080325ae  f7ee006a  vmov.f32	s13, #1.000000e+00
080325b2  2a4d      ldr	r5, [pc, #168] ; [0x0803265c] = 0x20001380
080325b4  9211      asrs	r2, r2, #6
080325b6  2a61      str	r2, [r5, #16]
080325b8  254d      ldr	r5, [pc, #148] ; [0x08032650] = 0x20001340
080325ba  2d69      ldr	r5, [r5, #16]
080325bc  ad1a      subs	r5, r5, r2
080325be  284a      ldr	r2, [pc, #160] ; [0x08032660] = 0x20001200
080325c0  07ee905a  vmov	s15, r5
080325c4  f8eee77a  vcvt.f32.s32	s15, s15
080325c8  86eea77a  vdiv.f32	s14, s13, s15
080325cc  82ed047a  vstr	s14, [r2, #16]
080325d0  1e4a      ldr	r2, [pc, #120] ; [0x0803264c] = 0x20002000
080325d2  5569      ldr	r5, [r2, #20]
080325d4  b5f5fa5f  cmp.w	r5, #8000
080325d8  11da      bge	#34 ; -> 0x080325fe ; branch_target=0x080325fe
080325da  6469      ldr	r4, [r4, #20]
080325dc  f7ee006a  vmov.f32	s13, #1.000000e+00
080325e0  1e4a      ldr	r2, [pc, #120] ; [0x0803265c] = 0x20001380
080325e2  a411      asrs	r4, r4, #6
080325e4  5461      str	r4, [r2, #20]
080325e6  1a4a      ldr	r2, [pc, #104] ; [0x08032650] = 0x20001340
080325e8  5269      ldr	r2, [r2, #20]
080325ea  121b      subs	r2, r2, r4
080325ec  07ee902a  vmov	s15, r2
080325f0  1b4a      ldr	r2, [pc, #108] ; [0x08032660] = 0x20001200
080325f2  f8eee77a  vcvt.f32.s32	s15, s15
080325f6  86eea77a  vdiv.f32	s14, s13, s15
080325fa  82ed057a  vstr	s14, [r2, #20]
080325fe  b1f5fa5f  cmp.w	r1, #8000
08032602  d8bf      it	le
08032604  b3f5fa5f  cmple.w	r3, #8000
08032608  ccbf      ite	gt
0803260a  0123      movgt	r3, #1
0803260c  0023      movle	r3, #0
0803260e  b0f5fa5f  cmp.w	r0, #8000
08032612  c8bf      it	gt
08032614  43f00103  orrgt	r3, r3, #1
08032618  b6f5fa5f  cmp.w	r6, #8000
0803261c  c8bf      it	gt
0803261e  43f00103  orrgt	r3, r3, #1
08032622  b7f5fa5f  cmp.w	r7, #8000
08032626  c8bf      it	gt
08032628  43f00103  orrgt	r3, r3, #1
0803262c  002b      cmp	r3, #0
0803262e  7ff429af  bne.w	#-430 ; -> 0x08032484 ; branch_target=0x08032484
08032632  b5f5fa5f  cmp.w	r5, #8000
08032636  3ff725af  bgt.w	#-438 ; -> 0x08032484 ; branch_target=0x08032484
0803263a  2de7      b	#-422 ; -> 0x08032498 ; branch_target=0x08032498
