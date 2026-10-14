; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
08027518  2de9f04f  push.w       {r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802751c  454c      ldr          r4, [pc, #276]  ; [0x08027634] = 0xe0001000 (f32=-3.69115025e+19)
0802751e  464d      ldr          r5, [pc, #280]  ; [0x08027638] = 0x20021394 (f32=1.10179062e-19)
08027520  464e      ldr          r6, [pc, #280]  ; [0x0802763c] = 0x20021e14 (f32=1.10213803e-19)
08027522  0023      movs         r3, #0
08027524  2ded108b  vpush        {d8, d9, d10, d11, d12, d13, d14, d15}
08027528  6360      str          r3, [r4, #4]
0802752a  454c      ldr          r4, [pc, #276]  ; [0x08027640] = 0x2002205c (f32=1.10221351e-19)
0802752c  3360      str          r3, [r6]
0802752e  2f68      ldr          r7, [r5]
08027530  2368      ldr          r3, [r4]
08027532  444e      ldr          r6, [pc, #272]  ; [0x08027644] = 0x200012e4 (f32=1.08482721e-19)
08027534  d9b0      sub          sp, #356
08027536  0137      adds         r7, #1
08027538  9046      mov          r8, r2
0802753a  012b      cmp          r3, #1
0802753c  3268      ldr          r2, [r6]
0802753e  2f60      str          r7, [r5]
08027540  1892      str          r2, [sp, #96]
08027542  01f09780  beq.w        #4398  ; -> 0x08028674
08027546  404d      ldr          r5, [pc, #256]  ; [0x08027648] = 0x2002411c (f32=1.10329712e-19)
08027548  404e      ldr          r6, [pc, #256]  ; [0x0802764c] = 0x2002409c (f32=1.10328058e-19)
0802754a  2c68      ldr          r4, [r5]
0802754c  dff830c1  ldr.w        r12, [pc, #304]  ; [0x08027680] = 0x20024124 (f32=1.10329815e-19)
08027550  56f82430  ldr.w        r3, [r6, r4, lsl #2]
08027554  dcf80020  ldr.w        r2, [r12]
08027558  8246      mov          r10, r0
0802755a  3d48      ldr          r0, [pc, #244]  ; [0x08027650] = 0x20021c68 (f32=1.10208272e-19)
0802755c  8946      mov          r9, r1
0802755e  c168      ldr          r1, [r0, #12]
08027560  3c48      ldr          r0, [pc, #240]  ; [0x08027654] = 0x200220a4 (f32=1.10222282e-19)
08027562  c768      ldr          r7, [r0, #12]
08027564  c160      str          r1, [r0, #12]
08027566  d31a      subs         r3, r2, r3
08027568  ca1b      subs         r2, r1, r7
0802756a  3b4f      ldr          r7, [pc, #236]  ; [0x08027658] = 0x20024120 (f32=1.10329764e-19)
0802756c  46f82420  str.w        r2, [r6, r4, lsl #2]
08027570  601c      adds         r0, r4, #1
08027572  3e68      ldr          r6, [r7]
08027574  00f01f00  and          r0, r0, #31
08027578  1344      add          r3, r2
0802757a  2860      str          r0, [r5]
0802757c  304d      ldr          r5, [pc, #192]  ; [0x08027640] = 0x2002205c (f32=1.10221351e-19)
0802757e  ccf80030  str.w        r3, [r12]
08027582  0124      movs         r4, #1
08027584  b342      cmp          r3, r6
08027586  2c60      str          r4, [r5]
08027588  48dd      ble          #144  ; -> 0x0802761c
0802758a  002b      cmp          r3, #0
0802758c  46dd      ble          #140  ; -> 0x0802761c
0802758e  3b60      str          r3, [r7]
08027590  324b      ldr          r3, [pc, #200]  ; [0x0802765c] = 0x20021cb0 (f32=1.10209202e-19)
08027592  1a68      ldr          r2, [r3]
08027594  1ab9      cbnz         r2, #6  ; -> 0x0802759e
08027596  324f      ldr          r7, [pc, #200]  ; [0x08027660] = 0x20022144 (f32=1.1022435e-19)
08027598  4ff07e56  mov.w        r6, #1065353216
0802759c  3e60      str          r6, [r7]
0802759e  dff8e4e0  ldr.w        lr, [pc, #228]  ; [0x08027684] = 0x20021338 (f32=1.10177873e-19)
080275a2  304c      ldr          r4, [pc, #192]  ; [0x08027664] = 0x20021ca8 (f32=1.10209099e-19)
080275a4  def80030  ldr.w        r3, [lr]
080275a8  2268      ldr          r2, [r4]
080275aa  002b      cmp          r3, #0
080275ac  6cd1      bne          #216  ; -> 0x08027688
080275ae  2e49      ldr          r1, [pc, #184]  ; [0x08027668] = 0x20021cac (f32=1.1020915e-19)
080275b0  0888      ldrh         r0, [r1]
080275b2  0028      cmp          r0, #0
080275b4  68d1      bne          #208  ; -> 0x08027688
080275b6  2d48      ldr          r0, [pc, #180]  ; [0x0802766c] = 0x40021800 (f32=2.03271484)
080275b8  0569      ldr          r5, [r0, #16]
080275ba  6805      lsls         r0, r5, #21
080275bc  64d5      bpl          #200  ; -> 0x08027688
080275be  2c4d      ldr          r5, [pc, #176]  ; [0x08027670] = 0x20022068 (f32=1.10221506e-19)
080275c0  2c4e      ldr          r6, [pc, #176]  ; [0x08027674] = 0x20021cb4 (f32=1.10209254e-19)
080275c2  2c68      ldr          r4, [r5]
080275c4  4d96      str          r6, [sp, #308]
080275c6  d200      lsls         r2, r2, #3
080275c8  b2f5166f  cmp.w        r2, #2400
080275cc  a2eb0400  sub.w        r0, r2, r4
080275d0  3260      str          r2, [r6]
080275d2  00f14000  add.w        r0, r0, #64
080275d6  3cbf      itt          lo
080275d8  4ff41662  movlo.w      r2, #2400
080275dc  3260      strlo        r2, [r6]
080275de  0127      movs         r7, #1
080275e0  8028      cmp          r0, #128
080275e2  98bf      it           ls
080275e4  4d9a      ldrls        r2, [sp, #308]
080275e6  0f80      strh         r7, [r1]
080275e8  1e49      ldr          r1, [pc, #120]  ; [0x08027664] = 0x20021ca8 (f32=1.10209099e-19)
080275ea  96bf      itet         ls
080275ec  1460      strls        r4, [r2]
080275ee  2a60      strhi        r2, [r5]
080275f0  2246      movls        r2, r4
080275f2  0f60      str          r7, [r1]
080275f4  4e97      str          r7, [sp, #312]
080275f6  002a      cmp          r2, #0
080275f8  61d0      beq          #194  ; -> 0x080276be
080275fa  1f4d      ldr          r5, [pc, #124]  ; [0x08027678] = 0x0803ef50 (f32=3.9702689e-34)
080275fc  9fed1f7a  vldr         s14, [pc, #124]  ; [0x0802767c] = 0x45bb8000 (f32=6000)
08027600  174e      ldr          r6, [pc, #92]  ; [0x08027660] = 0x20022144 (f32=1.1022435e-19)
08027602  164f      ldr          r7, [pc, #88]  ; [0x0802765c] = 0x20021cb0 (f32=1.10209202e-19)
08027604  5209      lsrs         r2, r2, #5
08027606  05eb8204  add.w        r4, r5, r2, lsl #2
0802760a  d4ed007a  vldr         s15, [r4]
0802760e  27ee870a  vmul.f32     s0, s15, s14
08027612  0120      movs         r0, #1
08027614  86ed000a  vstr         s0, [r6]
08027618  3860      str          r0, [r7]
0802761a  50e0      b            #160  ; -> 0x080276be
0802761c  5942      rsbs         r1, r3, #0
0802761e  8e42      cmp          r6, r1
08027620  b6da      bge          #-148  ; -> 0x08027590
08027622  002b      cmp          r3, #0
08027624  0d4b      ldr          r3, [pc, #52]  ; [0x0802765c] = 0x20021cb0 (f32=1.10209202e-19)
08027626  b8bf      it           lt
08027628  3960      strlt        r1, [r7]
0802762a  1a68      ldr          r2, [r3]
0802762c  002a      cmp          r2, #0
0802762e  b6d1      bne          #-148  ; -> 0x0802759e
08027630  b1e7      b            #-158  ; -> 0x08027596
08027688  c44f      ldr          r7, [pc, #784]  ; [0x0802799c] = 0x40021800 (f32=2.03271484)
0802768a  c54d      ldr          r5, [pc, #788]  ; [0x080279a0] = 0x20021ca8 (f32=1.10209099e-19)
0802768c  3e69      ldr          r6, [r7, #16]
0802768e  16f48064  ands         r4, r6, #1024
08027692  08bf      it           eq
08027694  c348      ldreq        r0, [pc, #780]  ; [0x080279a4] = 0x20021cac (f32=1.1020915e-19)
08027696  02f10102  add.w        r2, r2, #1
0802769a  45f6bf51  movw         r1, #23999
0802769e  08bf      it           eq
080276a0  0480      strheq       r4, [r0]
080276a2  2a60      str          r2, [r5]
080276a4  8a42      cmp          r2, r1
080276a6  c04a      ldr          r2, [pc, #768]  ; [0x080279a8] = 0x20021cb4 (f32=1.10209254e-19)
080276a8  4d92      str          r2, [sp, #308]
080276aa  40f2e887  bls.w        #4048  ; -> 0x0802867e
080276ae  bf4f      ldr          r7, [pc, #764]  ; [0x080279ac] = 0x20022038 (f32=1.10220886e-19)
080276b0  bf4c      ldr          r4, [pc, #764]  ; [0x080279b0] = 0x20021cb0 (f32=1.10209202e-19)
080276b2  2960      str          r1, [r5]
080276b4  0026      movs         r6, #0
080276b6  1660      str          r6, [r2]
080276b8  2660      str          r6, [r4]
080276ba  3e60      str          r6, [r7]
080276bc  4e96      str          r6, [sp, #312]
080276be  bd49      ldr          r1, [pc, #756]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
080276c0  dfedbd6a  vldr         s13, [pc, #756]  ; [0x080279b8] = 0x398a26fe (f32=0.000263504626)
080276c4  0d68      ldr          r5, [r1]
080276c6  dfedbd0a  vldr         s1, [pc, #756]  ; [0x080279bc] = 0x3f864064 (f32=1.04884005)
080276ca  01ee105a  vmov         s2, r5
080276ce  f8eec11a  vcvt.f32.s32 s3, s2
080276d2  21eea62a  vmul.f32     s4, s3, s13
080276d6  b4eee02a  vcmpe.f32    s4, s1
080276da  f1ee10fa  vmrs         APSR_nzcv, fpscr
080276de  41f3b780  ble.w        #4462  ; -> 0x08028850
080276e2  b7ee004a  vmov.f32     s8, #1.000000e+00
080276e6  8ded314a  vstr         s8, [sp, #196]
080276ea  b54e      ldr          r6, [pc, #724]  ; [0x080279c0] = 0x20022080 (f32=1.10221817e-19)
080276ec  b14c      ldr          r4, [pc, #708]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
080276ee  3268      ldr          r2, [r6]
080276f0  a068      ldr          r0, [r4, #8]
080276f2  b44f      ldr          r7, [pc, #720]  ; [0x080279c4] = 0x20022078 (f32=1.10221713e-19)
080276f4  9042      cmp          r0, r2
080276f6  3968      ldr          r1, [r7]
080276f8  c1f29980  blt.w        #4402  ; -> 0x0802882e
080276fc  861a      subs         r6, r0, r2
080276fe  04ee906a  vmov         s9, r6
08027702  b14c      ldr          r4, [pc, #708]  ; [0x080279c8] = 0x200220a0 (f32=1.1022223e-19)
08027704  f8eee48a  vcvt.f32.s32 s17, s9
08027708  8842      cmp          r0, r1
0802770a  01f30c80  bgt.w        #4120  ; -> 0x08028726
0802770e  d4ed009a  vldr         s19, [r4]
08027712  9fedaeba  vldr         s22, [pc, #696]  ; [0x080279cc] = 0x00000000 (f32=0)
08027716  ae4f      ldr          r7, [pc, #696]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
08027718  ae4a      ldr          r2, [pc, #696]  ; [0x080279d4] = 0x20021cbc (f32=1.10209357e-19)
0802771a  b968      ldr          r1, [r7, #8]
0802771c  1468      ldr          r4, [r2]
0802771e  ae4f      ldr          r7, [pc, #696]  ; [0x080279d8] = 0x2002208c (f32=1.10221972e-19)
08027720  68eea9aa  vmul.f32     s21, s17, s19
08027724  f7ee00ba  vmov.f32     s23, #1.000000e+00
08027728  f4eecbaa  vcmpe.f32    s21, s22
0802772c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027730  b8bf      it           lt
08027732  f0ee4baa  vmovlt.f32   s21, s22
08027736  f4eeebaa  vcmpe.f32    s21, s23
0802773a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802773e  88bf      it           hi
08027740  f0ee6baa  vmovhi.f32   s21, s23
08027744  461a      subs         r6, r0, r1
08027746  48bf      it           mi
08027748  0e1a      submi        r6, r1, r0
0802774a  3a68      ldr          r2, [r7]
0802774c  002c      cmp          r4, #0
0802774e  40f0af87  bne.w        #3934  ; -> 0x080286b0
08027752  002a      cmp          r2, #0
08027754  40f3ac87  ble.w        #3928  ; -> 0x080286b0
08027758  964c      ldr          r4, [pc, #600]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
0802775a  9d49      ldr          r1, [pc, #628]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
0802775c  6768      ldr          r7, [r4, #4]
0802775e  8860      str          r0, [r1, #8]
08027760  a346      mov          r11, r4
08027762  e668      ldr          r6, [r4, #12]
08027764  dbf81810  ldr.w        r1, [r11, #24]
08027768  dbf81400  ldr.w        r0, [r11, #20]
0802776c  2469      ldr          r4, [r4, #16]
0802776e  01f1640b  add.w        r11, r1, #100
08027772  9749      ldr          r1, [pc, #604]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
08027774  6435      adds         r5, #100
08027776  6437      adds         r7, #100
08027778  c1e90057  strd         r5, r7, [r1]
0802777c  8d4d      ldr          r5, [pc, #564]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
0802777e  0f46      mov          r7, r1
08027780  6436      adds         r6, #100
08027782  6434      adds         r4, #100
08027784  ce60      str          r6, [r1, #12]
08027786  6430      adds         r0, #100
08027788  e969      ldr          r1, [r5, #28]
0802778a  3c61      str          r4, [r7, #16]
0802778c  934e      ldr          r6, [pc, #588]  ; [0x080279dc] = 0x20024128 (f32=1.10329867e-19)
0802778e  7861      str          r0, [r7, #20]
08027790  3c46      mov          r4, r7
08027792  c7f818b0  str.w        r11, [r7, #24]
08027796  924f      ldr          r7, [pc, #584]  ; [0x080279e0] = 0x20021dec (f32=1.10213286e-19)
08027798  4f96      str          r6, [sp, #316]
0802779a  6431      adds         r1, #100
0802779c  4ff4f075  mov.w        r5, #480
080277a0  1297      str          r7, [sp, #72]
080277a2  e161      str          r1, [r4, #28]
080277a4  3560      str          r5, [r6]
080277a6  c7ed02aa  vstr         s21, [r7, #8]
080277aa  002b      cmp          r3, #0
080277ac  40f08e87  bne.w        #3868  ; -> 0x080286cc
080277b0  9fed8cfa  vldr         s30, [pc, #560]  ; [0x080279e4] = 0x3a83126f (f32=0.00100000005)
080277b4  8748      ldr          r0, [pc, #540]  ; [0x080279d4] = 0x20021cbc (f32=1.10209357e-19)
080277b6  07ee102a  vmov         s14, r2
080277ba  7aeecffa  vsub.f32     s31, s21, s30
080277be  f8eec77a  vcvt.f32.s32 s15, s14
080277c2  27eeaf0a  vmul.f32     s0, s15, s31
080277c6  fdeec06a  vcvt.s32.f32 s13, s0
080277ca  16ee903a  vmov         r3, s13
080277ce  0133      adds         r3, #1
080277d0  0360      str          r3, [r0]
080277d2  012a      cmp          r2, #1
080277d4  844e      ldr          r6, [pc, #528]  ; [0x080279e8] = 0x20001040 (f32=1.08473984e-19)
080277d6  03f03a81  beq.w        #12916  ; -> 0x0802aa4e
080277da  3396      str          r6, [sp, #204]
080277dc  def80030  ldr.w        r3, [lr]
080277e0  3068      ldr          r0, [r6]
080277e2  002a      cmp          r2, #0
080277e4  41f06680  bne.w        #4300  ; -> 0x080288b4
080277e8  804c      ldr          r4, [pc, #512]  ; [0x080279ec] = 0x200230fc (f32=1.10276359e-19)
080277ea  7a4d      ldr          r5, [pc, #488]  ; [0x080279d4] = 0x20021cbc (f32=1.10209357e-19)
080277ec  0c94      str          r4, [sp, #48]
080277ee  4ff0000b  mov.w        r11, #0
080277f2  b0f57a7f  cmp.w        r0, #1000
080277f6  2a60      str          r2, [r5]
080277f8  c4f800b0  str.w        r11, [r4]
080277fc  01f07180  beq.w        #4322  ; -> 0x080288e2
08027800  012b      cmp          r3, #1
08027802  01f05981  beq.w        #4786  ; -> 0x08028ab8
08027806  6b48      ldr          r0, [pc, #428]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
08027808  c668      ldr          r6, [r0, #12]
0802780a  4069      ldr          r0, [r0, #20]
0802780c  784c      ldr          r4, [pc, #480]  ; [0x080279f0] = 0x20021e64 (f32=1.10214837e-19)
0802780e  d4ed001a  vldr         s3, [r4]
08027812  94ed012a  vldr         s4, [r4, #4]
08027816  d4ed022a  vldr         s5, [r4, #8]
0802781a  94ed033a  vldr         s6, [r4, #12]
0802781e  71ee823a  vadd.f32     s7, s3, s4
08027822  33eea24a  vadd.f32     s8, s7, s5
08027826  34ee038a  vadd.f32     s16, s8, s6
0802782a  b5eec08a  vcmpe.f32    s16, #0
0802782e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027832  0bdd      ble          #22  ; -> 0x0802784c
08027834  f1ee044a  vmov.f32     s9, #5.000000e+00
08027838  b4eee48a  vcmpe.f32    s16, s9
0802783c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027840  04d5      bpl          #8  ; -> 0x0802784c
08027842  6c49      ldr          r1, [pc, #432]  ; [0x080279f4] = 0x20021cb8 (f32=1.10209306e-19)
08027844  6c4a      ldr          r2, [pc, #432]  ; [0x080279f8] = 0x20021fb8 (f32=1.10219232e-19)
08027846  0f68      ldr          r7, [r1]
08027848  52f82720  ldr.w        r2, [r2, r7, lsl #2]
0802784c  6b4d      ldr          r5, [pc, #428]  ; [0x080279fc] = 0x20001294 (f32=1.08481687e-19)
0802784e  6c49      ldr          r1, [pc, #432]  ; [0x08027a00] = 0x20021de4 (f32=1.10213183e-19)
08027850  2c68      ldr          r4, [r5]
08027852  0a60      str          r2, [r1]
08027854  b442      cmp          r4, r6
08027856  0fdc      bgt          #30  ; -> 0x08027878
08027858  361b      subs         r6, r6, r4
0802785a  6a4f      ldr          r7, [pc, #424]  ; [0x08027a04] = 0x20021410 (f32=1.10180664e-19)
0802785c  05ee106a  vmov         s10, r6
08027860  97ed006a  vldr         s12, [r7]
08027864  f8eec55a  vcvt.f32.s32 s11, s10
08027868  25ee869a  vmul.f32     s18, s11, s12
0802786c  b5eec09a  vcmpe.f32    s18, #0
08027870  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027874  43f13382  bpl.w        #13414  ; -> 0x0802acde
08027878  9fed549a  vldr         s18, [pc, #336]  ; [0x080279cc] = 0x00000000 (f32=0)
0802787c  c728      cmp          r0, #199
0802787e  40f3d187  ble.w        #4002  ; -> 0x08028824
08027882  614d      ldr          r5, [pc, #388]  ; [0x08027a08] = 0x20021e14 (f32=1.10213803e-19)
08027884  2c68      ldr          r4, [r5]
08027886  129f      ldr          r7, [sp, #72]
08027888  9fed56ba  vldr         s22, [pc, #344]  ; [0x080279e4] = 0x3a83126f (f32=0.00100000005)
0802788c  d7ed039a  vldr         s19, [r7, #12]
08027890  79eec9aa  vsub.f32     s21, s19, s18
08027894  f5eec0aa  vcmpe.f32    s21, #0
08027898  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802789c  48bf      it           mi
0802789e  79ee69aa  vsubmi.f32   s21, s18, s19
080278a2  f4eecbaa  vcmpe.f32    s21, s22
080278a6  f1ee10fa  vmrs         APSR_nzcv, fpscr
080278aa  80f28787  bge.w        #3854  ; -> 0x080287bc
080278ae  4f9e      ldr          r6, [sp, #316]
080278b0  3568      ldr          r5, [r6]
080278b2  691e      subs         r1, r5, #1
080278b4  b1f5f07f  cmp.w        r1, #480
080278b8  03f38f81  bgt.w        #13086  ; -> 0x0802abda
080278bc  4f9e      ldr          r6, [sp, #316]
080278be  0027      movs         r7, #0
080278c0  012c      cmp          r4, #1
080278c2  3760      str          r7, [r6]
080278c4  01f0d180  beq.w        #4514  ; -> 0x08028a6a
080278c8  384c      ldr          r4, [pc, #224]  ; [0x080279ac] = 0x20022038 (f32=1.10220886e-19)
080278ca  2760      str          r7, [r4]
080278cc  4049      ldr          r1, [pc, #256]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
080278ce  4f4c      ldr          r4, [pc, #316]  ; [0x08027a0c] = 0x2002215c (f32=1.1022466e-19)
080278d0  4f69      ldr          r7, [r1, #20]
080278d2  54f82260  ldr.w        r6, [r4, r2, lsl #2]
080278d6  02f1ff3b  add.w        r11, r2, #4294967295
080278da  c51b      subs         r5, r0, r7
080278dc  48bf      it           mi
080278de  3d1a      submi        r5, r7, r0
080278e0  54f82b20  ldr.w        r2, [r4, r11, lsl #2]
080278e4  4a4f      ldr          r7, [pc, #296]  ; [0x08027a10] = 0x200220c4 (f32=1.10222696e-19)
080278e6  202d      cmp          r5, #32
080278e8  a6eb0201  sub.w        r1, r6, r2
080278ec  03dc      bgt          #6  ; -> 0x080278f6
080278ee  3d68      ldr          r5, [r7]
080278f0  8d42      cmp          r5, r1
080278f2  03f01281  beq.w        #12836  ; -> 0x0802ab1a
080278f6  07ee901a  vmov         s15, r1
080278fa  dfed466a  vldr         s13, [pc, #280]  ; [0x08027a14] = 0x490ca000 (f32=576000)
080278fe  464c      ldr          r4, [pc, #280]  ; [0x08027a18] = 0x20021f90 (f32=1.10218715e-19)
08027900  334e      ldr          r6, [pc, #204]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
08027902  3960      str          r1, [r7]
08027904  b8eee70a  vcvt.f32.s32 s0, s15
08027908  40f2314b  movw         r11, #1073
0802790c  b4eee60a  vcmpe.f32    s0, s13
08027910  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027914  1494      str          r4, [sp, #80]
08027916  7061      str          r0, [r6, #20]
08027918  84ed000a  vstr         s0, [r4]
0802791c  abeba007  sub.w        r7, r11, r0, asr #2
08027920  b0ee401a  vmov.f32     s2, s0
08027924  08dd      ble          #16  ; -> 0x08027938
08027926  f6ee000a  vmov.f32     s1, #5.000000e-01
0802792a  21ee201a  vmul.f32     s2, s2, s1
0802792e  b4eee61a  vcmpe.f32    s2, s13
08027932  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027936  f8dc      bgt          #-16  ; -> 0x0802792a
08027938  3849      ldr          r1, [pc, #224]  ; [0x08027a1c] = 0x08044d10 (f32=3.98128916e-34)
0802793a  1d4a      ldr          r2, [pc, #116]  ; [0x080279b0] = 0x20021cb0 (f32=1.10209202e-19)
0802793c  149c      ldr          r4, [sp, #80]
0802793e  1568      ldr          r5, [r2]
08027940  01eb8700  add.w        r0, r1, r7, lsl #2
08027944  d0ed001a  vldr         s3, [r0]
08027948  21eea12a  vmul.f32     s4, s3, s3
0802794c  012d      cmp          r5, #1
0802794e  62ee212a  vmul.f32     s5, s4, s3
08027952  22ee817a  vmul.f32     s14, s5, s2
08027956  84ed007a  vstr         s14, [r4]
0802795a  03f00781  beq.w        #12814  ; -> 0x0802ab6c
0802795e  012b      cmp          r3, #1
08027960  03d1      bne          #6  ; -> 0x0802796a
08027962  2f49      ldr          r1, [pc, #188]  ; [0x08027a20] = 0x200220e8 (f32=1.10223161e-19)
08027964  4ff07e54  mov.w        r4, #1065353216
08027968  0c60      str          r4, [r1]
0802796a  124e      ldr          r6, [pc, #72]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
0802796c  dfed2d0a  vldr         s1, [pc, #180]  ; [0x08027a24] = 0x4207f5c3 (f32=33.9900017)
08027970  7768      ldr          r7, [r6, #4]
08027972  3269      ldr          r2, [r6, #16]
08027974  9fed101a  vldr         s2, [pc, #64]  ; [0x080279b8] = 0x398a26fe (f32=0.000263504626)
08027978  2b48      ldr          r0, [pc, #172]  ; [0x08027a28] = 0x200003e0 (f32=1.08433039e-19)
0802797a  2c49      ldr          r1, [pc, #176]  ; [0x08027a2c] = 0x20000358 (f32=1.08431281e-19)
0802797c  2c4d      ldr          r5, [pc, #176]  ; [0x08027a30] = 0x200004f0 (f32=1.08436554e-19)
0802797e  dfed2d1a  vldr         s3, [pc, #180]  ; [0x08027a34] = 0x3d480c74 (f32=0.0488400012)
08027982  0097      str          r7, [sp]
08027984  0fee907a  vmov         s31, r7
08027988  faeecafa  vcvt.f32.s32 s31, s31, #12
0802798c  02ee102a  vmov         s4, r2
08027990  2feea03a  vmul.f32     s6, s31, s1
08027994  f8eec22a  vcvt.f32.s32 s5, s4
08027998  4ee0      b            #156  ; -> 0x08027a38
08027a38  fdeec33a  vcvt.s32.f32 s7, s6
08027a3c  62ee817a  vmul.f32     s15, s5, s2
08027a40  13ee906a  vmov         r6, s7
08027a44  152e      cmp          r6, #21
08027a46  a8bf      it           ge
08027a48  1526      movge        r6, #21
08027a4a  b400      lsls         r4, r6, #2
08027a4c  2044      add          r0, r4
08027a4e  2144      add          r1, r4
08027a50  90ed005a  vldr         s10, [r0]
08027a54  55f82670  ldr.w        r7, [r5, r6, lsl #2]
08027a58  d1ed004a  vldr         s9, [r1]
08027a5c  c748      ldr          r0, [pc, #796]  ; [0x08027d7c] = 0x20021f48 (f32=1.10217784e-19)
08027a5e  c84d      ldr          r5, [pc, #800]  ; [0x08027d80] = 0x2002209c (f32=1.10222179e-19)
08027a60  0192      str          r2, [sp, #4]
08027a62  f4eee17a  vcmpe.f32    s15, s3
08027a66  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027a6a  3297      str          r7, [sp, #200]
08027a6c  80ed005a  vstr         s10, [r0]
08027a70  c5ed004a  vstr         s9, [r5]
08027a74  00f1fd86  bmi.w        #3578  ; -> 0x08028872
08027a78  9fedc26a  vldr         s12, [pc, #776]  ; [0x08027d84] = 0x3f864064 (f32=1.04884005)
08027a7c  f4eec67a  vcmpe.f32    s15, s12
08027a80  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027a84  b8bf      it           lt
08027a86  77eee17a  vsublt.f32   s15, s15, s3
08027a8a  01db      blt          #2  ; -> 0x08027a90
08027a8c  dfedbe7a  vldr         s15, [pc, #760]  ; [0x08027d88] = 0x3f7fffef (f32=0.999998987)
08027a90  be4a      ldr          r2, [pc, #760]  ; [0x08027d8c] = 0x2002208c (f32=1.10221972e-19)
08027a92  129e      ldr          r6, [sp, #72]
08027a94  1268      ldr          r2, [r2]
08027a96  002a      cmp          r2, #0
08027a98  40f04e86  bne.w        #3228  ; -> 0x08028738
08027a9c  c6ed047a  vstr         s15, [r6, #16]
08027aa0  f0ee678a  vmov.f32     s17, s15
08027aa4  ba4f      ldr          r7, [pc, #744]  ; [0x08027d90] = 0x20022140 (f32=1.10224298e-19)
08027aa6  3868      ldr          r0, [r7]
08027aa8  0028      cmp          r0, #0
08027aaa  43f34d80  ble.w        #12442  ; -> 0x0802ab48
08027aae  451e      subs         r5, r0, #1
08027ab0  3d60      str          r5, [r7]
08027ab2  002d      cmp          r5, #0
08027ab4  43f03281  bne.w        #12900  ; -> 0x0802ad1c
08027ab8  129e      ldr          r6, [sp, #72]
08027aba  c6ed047a  vstr         s15, [r6, #16]
08027abe  b549      ldr          r1, [pc, #724]  ; [0x08027d94] = 0x20021e14 (f32=1.10213803e-19)
08027ac0  0c68      ldr          r4, [r1]
08027ac2  012c      cmp          r4, #1
08027ac4  00f06286  beq.w        #3268  ; -> 0x0802878c
08027ac8  b348      ldr          r0, [pc, #716]  ; [0x08027d98] = 0x2002214c (f32=1.10224453e-19)
08027aca  b24e      ldr          r6, [pc, #712]  ; [0x08027d94] = 0x20021e14 (f32=1.10213803e-19)
08027acc  0025      movs         r5, #0
08027ace  0560      str          r5, [r0]
08027ad0  3560      str          r5, [r6]
08027ad2  b249      ldr          r1, [pc, #712]  ; [0x08027d9c] = 0x20021e64 (f32=1.10214837e-19)
08027ad4  b24f      ldr          r7, [pc, #712]  ; [0x08027da0] = 0x20000468 (f32=1.08434796e-19)
08027ad6  91ed049a  vldr         s18, [r1, #16]
08027ada  91ed05ba  vldr         s22, [r1, #20]
08027ade  b14c      ldr          r4, [pc, #708]  ; [0x08027da4] = 0x20022094 (f32=1.10222075e-19)
08027ae0  79ee0bba  vadd.f32     s23, s18, s22
08027ae4  b7ee00ca  vmov.f32     s24, #1.000000e+00
08027ae8  7bee886a  vadd.f32     s13, s23, s16
08027aec  b1ee088a  vmov.f32     s16, #6.000000e+00
08027af0  76eeccca  vsub.f32     s25, s13, s24
08027af4  94ed00da  vldr         s26, [r4]
08027af8  6cee88da  vmul.f32     s27, s25, s16
08027afc  bdeeedea  vcvt.s32.f32 s28, s27
08027b00  1eee100a  vmov         r0, s28
08027b04  0330      adds         r0, #3
08027b06  2128      cmp          r0, #33
08027b08  a8bf      it           ge
08027b0a  2120      movge        r0, #33
08027b0c  20eae075  bic.w        r5, r0, r0, asr #31
08027b10  07eb850b  add.w        r11, r7, r5, lsl #2
08027b14  012b      cmp          r3, #1
08027b16  9bed001a  vldr         s2, [r11]
08027b1a  00f0ef86  beq.w        #3550  ; -> 0x080288fc
08027b1e  b2ee00fa  vmov.f32     s30, #8.000000e+00
08027b22  b4eecf7a  vcmpe.f32    s14, s30
08027b26  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027b2a  40f1d387  bpl.w        #4006  ; -> 0x08028ad4
08027b2e  b5eec07a  vcmpe.f32    s14, #0
08027b32  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027b36  c2f2f687  blt.w        #12268  ; -> 0x0802ab26
08027b3a  1499      ldr          r1, [sp, #80]
08027b3c  b4ee4fda  vcmp.f32     s26, s30
08027b40  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027b44  81ed00fa  vstr         s30, [r1]
08027b48  42f09f87  bne.w        #12094  ; -> 0x0802aa8a
08027b4c  964f      ldr          r7, [pc, #600]  ; [0x08027da8] = 0x20021e0c (f32=1.102137e-19)
08027b4e  4197      str          r7, [sp, #260]
08027b50  f1ee001a  vmov.f32     s3, #4.000000e+00
08027b54  c7ed001a  vstr         s3, [r7]
08027b58  944e      ldr          r6, [pc, #592]  ; [0x08027dac] = 0x20021e84 (f32=1.10215251e-19)
08027b5a  dfed953a  vldr         s7, [pc, #596]  ; [0x08027db0] = 0x437a0000 (f32=250)
08027b5e  96ed006a  vldr         s12, [r6]
08027b62  9449      ldr          r1, [pc, #592]  ; [0x08027db4] = 0x20021e08 (f32=1.10213648e-19)
08027b64  4291      str          r1, [sp, #264]
08027b66  76ee068a  vadd.f32     s17, s12, s12
08027b6a  f4eee31a  vcmpe.f32    s3, s7
08027b6e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027b72  c1ed008a  vstr         s17, [r1]
08027b76  00f3f686  bgt.w        #3564  ; -> 0x08028966
08027b7a  b7ee004a  vmov.f32     s8, #1.000000e+00
08027b7e  b4ee445a  vcmp.f32     s10, s8
08027b82  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027b86  05d1      bne          #10  ; -> 0x08027b94
08027b88  419f      ldr          r7, [sp, #260]
08027b8a  4298      ldr          r0, [sp, #264]
08027b8c  8a4e      ldr          r6, [pc, #552]  ; [0x08027db8] = 0x437a0000 (f32=250)
08027b8e  8b49      ldr          r1, [pc, #556]  ; [0x08027dbc] = 0x3b83126f (f32=0.00400000019)
08027b90  3e60      str          r6, [r7]
08027b92  0160      str          r1, [r0]
08027b94  8a4e      ldr          r6, [pc, #552]  ; [0x08027dc0] = 0x200012e0 (f32=1.08482669e-19)
08027b96  3468      ldr          r4, [r6]
08027b98  012c      cmp          r4, #1
08027b9a  02f0d586  beq.w        #11690  ; -> 0x0802a948
08027b9e  0134      adds         r4, #1
08027ba0  02f03187  beq.w        #11874  ; -> 0x0802aa06
08027ba4  874e      ldr          r6, [pc, #540]  ; [0x08027dc4] = 0x20021320 (f32=1.10177562e-19)
08027ba6  3468      ldr          r4, [r6]
08027ba8  012c      cmp          r4, #1
08027baa  02f0dd86  beq.w        #11706  ; -> 0x0802a968
08027bae  dcf80020  ldr.w        r2, [r12]
08027bb2  0cee902a  vmov         s25, r2
08027bb6  002a      cmp          r2, #0
08027bb8  ccbf      ite          gt
08027bba  dfed831a  vldrgt       s3, [pc, #524]  ; [0x08027dc8] = 0x3c23d70a (f32=0.00999999978)
08027bbe  dfed831a  vldrle       s3, [pc, #524]  ; [0x08027dcc] = 0xbc23d70a (f32=-0.00999999978)
08027bc2  b8eeecda  vcvt.f32.s32 s26, s25
08027bc6  b7ee00ea  vmov.f32     s28, #1.000000e+00
08027bca  6dee21da  vmul.f32     s27, s26, s3
08027bce  f4eeceda  vcmpe.f32    s27, s28
08027bd2  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027bd6  42f35086  ble.w        #11424  ; -> 0x0802a87a
08027bda  8ded30ea  vstr         s28, [sp, #192]
08027bde  7c48      ldr          r0, [pc, #496]  ; [0x08027dd0] = 0x20022154 (f32=1.10224557e-19)
08027be0  dfed7c0a  vldr         s1, [pc, #496]  ; [0x08027dd4] = 0x3ca3d70a (f32=0.0199999996)
08027be4  d0ed005a  vldr         s11, [r0]
08027be8  75eee74a  vsub.f32     s9, s11, s15
08027bec  f4eee04a  vcmpe.f32    s9, s1
08027bf0  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027bf4  02f3e885  bgt.w        #11216  ; -> 0x0802a7c8
08027bf8  9fed775a  vldr         s10, [pc, #476]  ; [0x08027dd8] = 0xbca3d70a (f32=-0.0199999996)
08027bfc  dfed771a  vldr         s3, [pc, #476]  ; [0x08027ddc] = 0x38d1b717 (f32=9.99999975e-05)
08027c00  f4eec54a  vcmpe.f32    s9, s10
08027c04  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027c08  b7ee007a  vmov.f32     s14, #1.000000e+00
08027c0c  58bf      it           pl
08027c0e  b0ee617a  vmovpl.f32   s14, s3
08027c12  8ded367a  vstr         s14, [sp, #216]
08027c16  f0ee002a  vmov.f32     s5, #2.000000e+00
08027c1a  f4eee26a  vcmpe.f32    s13, s5
08027c1e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027c22  cdf8bc80  str.w        r8, [sp, #188]
08027c26  cde934a9  strd         r10, r9, [sp, #208]
08027c2a  4646      mov          r6, r8
08027c2c  01f34e84  bgt.w        #6300  ; -> 0x080294cc
08027c30  002e      cmp          r6, #0
08027c32  40f31385  ble.w        #2598  ; -> 0x0802865c
08027c36  9fed643a  vldr         s6, [pc, #400]  ; [0x08027dc8] = 0x3c23d70a (f32=0.00999999978)
08027c3a  694d      ldr          r5, [pc, #420]  ; [0x08027de0] = 0x20022038 (f32=1.10220886e-19)
08027c3c  694b      ldr          r3, [pc, #420]  ; [0x08027de4] = 0x20021cb8 (f32=1.10209306e-19)
08027c3e  6a4c      ldr          r4, [pc, #424]  ; [0x08027de8] = 0x200220e8 (f32=1.10223161e-19)
08027c40  6a4a      ldr          r2, [pc, #424]  ; [0x08027dec] = 0x200220ec (f32=1.10223212e-19)
08027c42  6b4f      ldr          r7, [pc, #428]  ; [0x08027df0] = 0x200012a8 (f32=1.08481946e-19)
08027c44  6b49      ldr          r1, [pc, #428]  ; [0x08027df4] = 0x20022108 (f32=1.10223574e-19)
08027c46  6c48      ldr          r0, [pc, #432]  ; [0x08027df8] = 0x200213a8 (f32=1.1017932e-19)
08027c48  6c4e      ldr          r6, [pc, #432]  ; [0x08027dfc] = 0x20022114 (f32=1.10223729e-19)
08027c4a  2695      str          r5, [sp, #152]
08027c4c  2393      str          r3, [sp, #140]
08027c4e  6c4d      ldr          r5, [pc, #432]  ; [0x08027e00] = 0x200220e0 (f32=1.10223057e-19)
08027c50  6c4b      ldr          r3, [pc, #432]  ; [0x08027e04] = 0x20022084 (f32=1.10221868e-19)
08027c52  2094      str          r4, [sp, #128]
08027c54  6c4c      ldr          r4, [pc, #432]  ; [0x08027e08] = 0x200220d4 (f32=1.10222902e-19)
08027c56  1e92      str          r2, [sp, #120]
08027c58  21ee031a  vmul.f32     s2, s2, s6
08027c5c  6b4a      ldr          r2, [pc, #428]  ; [0x08027e0c] = 0x200220f8 (f32=1.10223368e-19)
08027c5e  1097      str          r7, [sp, #64]
08027c60  2b91      str          r1, [sp, #172]
08027c62  6b4f      ldr          r7, [pc, #428]  ; [0x08027e10] = 0x20022124 (f32=1.10223936e-19)
08027c64  6b49      ldr          r1, [pc, #428]  ; [0x08027e14] = 0x20001054 (f32=1.08474242e-19)
08027c66  2890      str          r0, [sp, #160]
08027c68  2c96      str          r6, [sp, #176]
08027c6a  6b48      ldr          r0, [pc, #428]  ; [0x08027e18] = 0x20021e18 (f32=1.10213855e-19)
08027c6c  6b4e      ldr          r6, [pc, #428]  ; [0x08027e1c] = 0x20021e20 (f32=1.10213958e-19)
08027c6e  3c95      str          r5, [sp, #240]
08027c70  1593      str          r3, [sp, #84]
08027c72  6b4d      ldr          r5, [pc, #428]  ; [0x08027e20] = 0x20021f4c (f32=1.10217836e-19)
08027c74  6b4b      ldr          r3, [pc, #428]  ; [0x08027e24] = 0x20022088 (f32=1.1022192e-19)
08027c76  3994      str          r4, [sp, #228]
08027c78  6b4c      ldr          r4, [pc, #428]  ; [0x08027e28] = 0x20022158 (f32=1.10224608e-19)
08027c7a  1192      str          r2, [sp, #68]
08027c7c  3a97      str          r7, [sp, #232]
08027c7e  3791      str          r1, [sp, #220]
08027c80  2490      str          r0, [sp, #144]
08027c82  1f96      str          r6, [sp, #124]
08027c84  0695      str          r5, [sp, #24]
08027c86  4093      str          r3, [sp, #256]
08027c88  3d94      str          r4, [sp, #244]
08027c8a  684a      ldr          r2, [pc, #416]  ; [0x08027e2c] = 0x20022064 (f32=1.10221455e-19)
08027c8c  684f      ldr          r7, [pc, #416]  ; [0x08027e30] = 0x20022090 (f32=1.10222023e-19)
08027c8e  6949      ldr          r1, [pc, #420]  ; [0x08027e34] = 0x200213c0 (f32=1.1017963e-19)
08027c90  6948      ldr          r0, [pc, #420]  ; [0x08027e38] = 0x200220c8 (f32=1.10222747e-19)
08027c92  6a4e      ldr          r6, [pc, #424]  ; [0x08027e3c] = 0x20021dc0 (f32=1.10212718e-19)
08027c94  6a4d      ldr          r5, [pc, #424]  ; [0x08027e40] = 0x20021cc0 (f32=1.10209409e-19)
08027c96  6b4b      ldr          r3, [pc, #428]  ; [0x08027e44] = 0x20021c88 (f32=1.10208685e-19)
08027c98  9fed6bea  vldr         s28, [pc, #428]  ; [0x08027e48] = 0x00000000 (f32=0)
08027c9c  8ded4c3a  vstr         s6, [sp, #304]
08027ca0  0024      movs         r4, #0
08027ca2  3892      str          r2, [sp, #224]
08027ca4  2197      str          r7, [sp, #132]
08027ca6  2d91      str          r1, [sp, #180]
08027ca8  1390      str          r0, [sp, #76]
08027caa  3e96      str          r6, [sp, #248]
08027cac  3b95      str          r5, [sp, #236]
08027cae  3f93      str          r3, [sp, #252]
08027cb0  8ded4b1a  vstr         s2, [sp, #300]
08027cb4  2594      str          r4, [sp, #148]
08027cb6  f7ee00ba  vmov.f32     s23, #1.000000e+00
08027cba  b0ee673a  vmov.f32     s6, s15
08027cbe  3498      ldr          r0, [sp, #208]
08027cc0  dfed628a  vldr         s17, [pc, #392]  ; [0x08027e4c] = 0x38000100 (f32=3.05185094e-05)
08027cc4  30f91420  ldrsh.w      r2, [r0, r4, lsl #1]
08027cc8  399d      ldr          r5, [sp, #228]
08027cca  159b      ldr          r3, [sp, #84]
08027ccc  d5ed009a  vldr         s19, [r5]
08027cd0  93ed009a  vldr         s18, [r3]
08027cd4  129e      ldr          r6, [sp, #72]
08027cd6  3c9f      ldr          r7, [sp, #240]
08027cd8  2099      ldr          r1, [sp, #128]
08027cda  d6ed007a  vldr         s15, [r6]
08027cde  97ed007a  vldr         s14, [r7]
08027ce2  d1ed00aa  vldr         s21, [r1]
08027ce6  9ded31fa  vldr         s30, [sp, #196]
08027cea  9ded4c1a  vldr         s2, [sp, #304]
08027cee  dfed583a  vldr         s7, [pc, #352]  ; [0x08027e50] = 0x3f7fffac (f32=0.999994993)
08027cf2  dded360a  vldr         s1, [sp, #216]
08027cf6  dfed572a  vldr         s5, [pc, #348]  ; [0x08027e54] = 0x3a83126f (f32=0.00100000005)
08027cfa  5749      ldr          r1, [pc, #348]  ; [0x08027e58] = 0x0bb38435 (f32=6.9147215e-32)
08027cfc  6400      lsls         r4, r4, #1
08027cfe  06ee902a  vmov         s13, r2
08027d02  4494      str          r4, [sp, #272]
08027d04  0234      adds         r4, #2
08027d06  b8eee66a  vcvt.f32.s32 s12, s13
08027d0a  005f      ldrsh        r0, [r0, r4]
08027d0c  119a      ldr          r2, [sp, #68]
08027d0e  4594      str          r4, [sp, #276]
08027d10  26ee28ba  vmul.f32     s22, s12, s17
08027d14  0cee100a  vmov         s24, r0
08027d18  b7ee088a  vmov.f32     s16, #1.500000e+00
08027d1c  b0eecbda  vabs.f32     s26, s22
08027d20  2bee082a  vmul.f32     s4, s22, s16
08027d24  f8eeccca  vcvt.f32.s32 s25, s24
08027d28  b4eee9da  vcmpe.f32    s26, s19
08027d2c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027d30  6ceea8da  vmul.f32     s27, s25, s17
08027d34  b4eeeb2a  vcmpe.f32    s4, s23
08027d38  b8bf      it           lt
08027d3a  b0ee69da  vmovlt.f32   s26, s19
08027d3e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027d42  ffee00ea  vmov.f32     s29, #-1.000000e+00
08027d46  88bf      it           hi
08027d48  b0ee6b2a  vmovhi.f32   s4, s23
08027d4c  6dee881a  vmul.f32     s3, s27, s16
08027d50  3feec95a  vsub.f32     s10, s31, s18
08027d54  b4eeee2a  vcmpe.f32    s4, s29
08027d58  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027d5c  33ee654a  vsub.f32     s8, s6, s11
08027d60  f4eeeb1a  vcmpe.f32    s3, s23
08027d64  a5ee019a  vfma.f32     s18, s10, s2
08027d68  1368      ldr          r3, [r2]
08027d6a  3c4a      ldr          r2, [pc, #240]  ; [0x08027e5c] = 0x3619636b (f32=2.28566455e-06)
08027d6c  9ded303a  vldr         s6, [sp, #192]
08027d70  3fee670a  vsub.f32     s0, s30, s15
08027d74  7aeec74a  vsub.f32     s9, s21, s14
08027d78  72e0      b            #228  ; -> 0x08027e60
08027e60  b8bf      it           lt
08027e62  b0ee6e2a  vmovlt.f32   s4, s29
08027e66  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027e6a  88bf      it           hi
08027e6c  f0ee6b1a  vmovhi.f32   s3, s23
08027e70  e4ee205a  vfma.f32     s11, s8, s1
08027e74  01fb0323  mla          r3, r1, r3, r2
08027e78  1199      ldr          r1, [sp, #68]
08027e7a  ca4a      ldr          r2, [pc, #808]  ; [0x080281a4] = 0x20022154 (f32=1.10224557e-19)
08027e7c  0b60      str          r3, [r1]
08027e7e  e0ee227a  vfma.f32     s15, s0, s5
08027e82  a4ee837a  vfma.f32     s14, s9, s6
08027e86  6dee236a  vmul.f32     s13, s26, s7
08027e8a  f4eeee1a  vcmpe.f32    s3, s29
08027e8e  f5ee008a  vmov.f32     s17, #2.500000e-01
08027e92  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027e96  c5ed006a  vstr         s13, [r5]
08027e9a  b4eee89a  vcmpe.f32    s18, s17
08027e9e  159d      ldr          r5, [sp, #84]
08027ea0  c2ed005a  vstr         s11, [r2]
08027ea4  b8bf      it           lt
08027ea6  f0ee6e1a  vmovlt.f32   s3, s29
08027eaa  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027eae  c6ed007a  vstr         s15, [r6]
08027eb2  87ed007a  vstr         s14, [r7]
08027eb6  85ed009a  vstr         s18, [r5]
08027eba  41f29180  bls.w        #4386  ; -> 0x08028fe0
08027ebe  ba4e      ldr          r6, [pc, #744]  ; [0x080281a8] = 0x20021c68 (f32=1.10208272e-19)
08027ec0  dfedba9a  vldr         s19, [pc, #744]  ; [0x080281ac] = 0x44fa0000 (f32=2000)
08027ec4  7769      ldr          r7, [r6, #20]
08027ec6  0097      str          r7, [sp]
08027ec8  dded007a  vldr         s15, [sp]
08027ecc  b84c      ldr          r4, [pc, #736]  ; [0x080281b0] = 0x20022098 (f32=1.10222127e-19)
08027ece  f8eee7aa  vcvt.f32.s32 s21, s15
08027ed2  d4ed003a  vldr         s7, [r4]
08027ed6  f4eee9aa  vcmpe.f32    s21, s19
08027eda  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027ede  01f17f80  bmi.w        #4350  ; -> 0x08028fe0
08027ee2  06ee103a  vmov         s12, r3
08027ee6  b8ee46ba  vcvt.f32.u32 s22, s12
08027eea  feee007a  vmov.f32     s15, #-5.000000e-01
08027eee  ebee237a  vfma.f32     s15, s22, s7
08027ef2  9fedb08a  vldr         s16, [pc, #704]  ; [0x080281b4] = 0x461c4000 (f32=10000)
08027ef6  67ee88ca  vmul.f32     s25, s15, s16
08027efa  2cee89da  vmul.f32     s26, s25, s18
08027efe  2dee099a  vmul.f32     s18, s26, s18
08027f02  1098      ldr          r0, [sp, #64]
08027f04  036a      ldr          r3, [r0, #32]
08027f06  53b1      cbz          r3, #20  ; -> 0x08027f1e
08027f08  dfedabea  vldr         s29, [pc, #684]  ; [0x080281b8] = 0x3a83126f (f32=0.00100000005)
08027f0c  f4eeee6a  vcmpe.f32    s13, s29
08027f10  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027f14  44bf      itt          mi
08027f16  f0ee08ea  vmovmi.f32   s29, #3.000000e+00
08027f1a  adeeae7a  vfmami.f32   s14, s27, s29
08027f1e  a749      ldr          r1, [pc, #668]  ; [0x080281bc] = 0x20021e14 (f32=1.10213803e-19)
08027f20  fdeec7da  vcvt.s32.f32 s27, s14
08027f24  0a68      ldr          r2, [r1]
08027f26  b8eeed4a  vcvt.f32.s32 s8, s27
08027f2a  012a      cmp          r2, #1
08027f2c  37ee44fa  vsub.f32     s30, s14, s8
08027f30  01f0cc80  beq.w        #4504  ; -> 0x080290cc
08027f34  0125      movs         r5, #1
08027f36  b0ee4eca  vmov.f32     s24, s28
08027f3a  1995      str          r5, [sp, #100]
08027f3c  3a9e      ldr          r6, [sp, #232]
08027f3e  cded50da  vstr         s27, [sp, #320]
08027f42  d6ed000a  vldr         s1, [r6]
08027f46  d6ed012a  vldr         s5, [r6, #4]
08027f4a  96ed023a  vldr         s6, [r6, #8]
08027f4e  8ded54fa  vstr         s30, [sp, #336]
08027f52  67ee208a  vmul.f32     s17, s14, s1
08027f56  67ee226a  vmul.f32     s13, s14, s5
08027f5a  67ee039a  vmul.f32     s19, s14, s6
08027f5e  fdeee8aa  vcvt.s32.f32 s21, s17
08027f62  fdeee63a  vcvt.s32.f32 s7, s13
08027f66  bdeee9ba  vcvt.s32.f32 s22, s19
08027f6a  b8eeea6a  vcvt.f32.s32 s12, s21
08027f6e  f8eee37a  vcvt.f32.s32 s15, s7
08027f72  b8eecb1a  vcvt.f32.s32 s2, s22
08027f76  38eec68a  vsub.f32     s16, s17, s12
08027f7a  76eee7ca  vsub.f32     s25, s13, s15
08027f7e  39eec1da  vsub.f32     s26, s19, s2
08027f82  b5eec0fa  vcmpe.f32    s30, #0
08027f86  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027f8a  cded51aa  vstr         s21, [sp, #324]
08027f8e  8ded558a  vstr         s16, [sp, #340]
08027f92  cded523a  vstr         s7, [sp, #328]
08027f96  cded56ca  vstr         s25, [sp, #344]
08027f9a  8ded53ba  vstr         s22, [sp, #332]
08027f9e  8ded57da  vstr         s26, [sp, #348]
08027fa2  08d5      bpl          #16  ; -> 0x08027fb6
08027fa4  1dee909a  vmov         r9, s27
08027fa8  7fee2bea  vadd.f32     s29, s30, s23
08027fac  09f1ff37  add.w        r7, r9, #4294967295
08027fb0  cded54ea  vstr         s29, [sp, #336]
08027fb4  5097      str          r7, [sp, #320]
08027fb6  b5eec08a  vcmpe.f32    s16, #0
08027fba  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027fbe  08d5      bpl          #16  ; -> 0x08027fd2
08027fc0  1aee90aa  vmov         r10, s21
08027fc4  78ee2bda  vadd.f32     s27, s16, s23
08027fc8  0af1ff34  add.w        r4, r10, #4294967295
08027fcc  cded55da  vstr         s27, [sp, #340]
08027fd0  5194      str          r4, [sp, #324]
08027fd2  f5eec0ca  vcmpe.f32    s25, #0
08027fd6  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027fda  08d5      bpl          #16  ; -> 0x08027fee
08027fdc  13ee90ca  vmov         r12, s7
08027fe0  3ceeab4a  vadd.f32     s8, s25, s23
08027fe4  0cf1ff30  add.w        r0, r12, #4294967295
08027fe8  8ded564a  vstr         s8, [sp, #344]
08027fec  5290      str          r0, [sp, #328]
08027fee  b5eec0da  vcmpe.f32    s26, #0
08027ff2  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027ff6  08d5      bpl          #16  ; -> 0x0802800a
08027ff8  1bee10ba  vmov         r11, s22
08027ffc  3dee2bfa  vadd.f32     s30, s26, s23
08028000  0bf1ff33  add.w        r3, r11, #4294967295
08028004  8ded57fa  vstr         s30, [sp, #348]
08028008  5393      str          r3, [sp, #332]
0802800a  6d49      ldr          r1, [pc, #436]  ; [0x080281c0] = 0x20021cb0 (f32=1.10209202e-19)
0802800c  6d4e      ldr          r6, [pc, #436]  ; [0x080281c4] = 0x20021f48 (f32=1.10217784e-19)
0802800e  0d68      ldr          r5, [r1]
08028010  96ed000a  vldr         s0, [r6]
08028014  012d      cmp          r5, #1
08028016  01f08f80  beq.w        #4382  ; -> 0x08029138
0802801a  6b48      ldr          r0, [pc, #428]  ; [0x080281c8] = 0x20021ca8 (f32=1.10209099e-19)
0802801c  1499      ldr          r1, [sp, #80]
0802801e  0368      ldr          r3, [r0]
08028020  91ed005a  vldr         s10, [r1]
08028024  012b      cmp          r3, #1
08028026  01f03581  beq.w        #4714  ; -> 0x08029294
0802802a  379d      ldr          r5, [sp, #220]
0802802c  2e68      ldr          r6, [r5]
0802802e  012e      cmp          r6, #1
08028030  01f07081  beq.w        #4832  ; -> 0x08029314
08028034  239b      ldr          r3, [sp, #140]
08028036  069a      ldr          r2, [sp, #24]
08028038  1f68      ldr          r7, [r3]
0802803a  1f9c      ldr          r4, [sp, #124]
0802803c  dfed638a  vldr         s17, [pc, #396]  ; [0x080281cc] = 0x463b8000 (f32=12000)
08028040  02eb870a  add.w        r10, r2, r7, lsl #2
08028044  9aed003a  vldr         s6, [r10]
08028048  20ee055a  vmul.f32     s10, s0, s10
0802804c  23ee000a  vmul.f32     s0, s6, s0
08028050  b4eec50a  vcmpe.f32    s0, s10
08028054  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028058  d8bf      it           le
0802805a  b0ee405a  vmovle.f32   s10, s0
0802805e  b4eeeb5a  vcmpe.f32    s10, s23
08028062  d4bf      ite          le
08028064  84ed000a  vstrle       s0, [r4]
08028068  84ed005a  vstrgt       s10, [r4]
0802806c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028070  d4bf      ite          le
08028072  b0ee6b6a  vmovle.f32   s12, s23
08028076  b0ee456a  vmovgt.f32   s12, s10
0802807a  76ee439a  vsub.f32     s19, s12, s6
0802807e  f4eee89a  vcmpe.f32    s19, s17
08028082  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028086  0add      ble          #20  ; -> 0x0802809e
08028088  f6ee006a  vmov.f32     s13, #5.000000e-01
0802808c  69eea69a  vmul.f32     s19, s19, s13
08028090  f4eee89a  vcmpe.f32    s19, s17
08028094  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028098  f8dc      bgt          #-16  ; -> 0x0802808c
0802809a  33ee296a  vadd.f32     s12, s6, s19
0802809e  1f9d      ldr          r5, [sp, #124]
080280a0  4b48      ldr          r0, [pc, #300]  ; [0x080281d0] = 0x2002208c (f32=1.10221972e-19)
080280a2  1899      ldr          r1, [sp, #96]
080280a4  85ed006a  vstr         s12, [r5]
080280a8  0668      ldr          r6, [r0]
080280aa  0029      cmp          r1, #0
080280ac  40f0aa87  bne.w        #3924  ; -> 0x08029004
080280b0  1e9a      ldr          r2, [sp, #120]
080280b2  1068      ldr          r0, [r2]
080280b4  0128      cmp          r0, #1
080280b6  00f01885  beq.w        #2608  ; -> 0x08028aea
080280ba  002e      cmp          r6, #0
080280bc  00f02385  beq.w        #2630  ; -> 0x08028b06
080280c0  109e      ldr          r6, [sp, #64]
080280c2  4448      ldr          r0, [pc, #272]  ; [0x080281d4] = 0x20021cbc (f32=1.10209357e-19)
080280c4  f268      ldr          r2, [r6, #12]
080280c6  0068      ldr          r0, [r0]
080280c8  012a      cmp          r2, #1
080280ca  01f0e781  beq.w        #5070  ; -> 0x0802949c
080280ce  249d      ldr          r5, [sp, #144]
080280d0  4099      ldr          r1, [sp, #256]
080280d2  d5ed00aa  vldr         s21, [r5]
080280d6  269a      ldr          r2, [sp, #152]
080280d8  0e68      ldr          r6, [r1]
080280da  0b96      str          r6, [sp, #44]
080280dc  f4eec6aa  vcmpe.f32    s21, s12
080280e0  f1ee10fa  vmrs         APSR_nzcv, fpscr
080280e4  1368      ldr          r3, [r2]
080280e6  08db      blt          #16  ; -> 0x080280fa
080280e8  189c      ldr          r4, [sp, #96]
080280ea  012c      cmp          r4, #1
080280ec  02f0f083  beq.w        #10208  ; -> 0x0802a8d0
080280f0  109d      ldr          r5, [sp, #64]
080280f2  a968      ldr          r1, [r5, #8]
080280f4  0229      cmp          r1, #2
080280f6  02f0eb83  beq.w        #10198  ; -> 0x0802a8d0
080280fa  329f      ldr          r7, [sp, #200]
080280fc  9f42      cmp          r7, r3
080280fe  02dc      bgt          #4  ; -> 0x08028106
08028100  269d      ldr          r5, [sp, #152]
08028102  0023      movs         r3, #0
08028104  2b60      str          r3, [r5]
08028106  0c9e      ldr          r6, [sp, #48]
08028108  334c      ldr          r4, [pc, #204]  ; [0x080281d8] = 0x2002215c (f32=1.1022466e-19)
0802810a  3d99      ldr          r1, [sp, #244]
0802810c  54f82050  ldr.w        r5, [r4, r0, lsl #2]
08028110  06eb8009  add.w        r9, r6, r0, lsl #2
08028114  d9ed003a  vldr         s7, [r9]
08028118  65eea35a  vmul.f32     s11, s11, s7
0802811c  421e      subs         r2, r0, #1
0802811e  bdeee5ba  vcvt.s32.f32 s22, s11
08028122  54f82270  ldr.w        r7, [r4, r2, lsl #2]
08028126  f8eecb7a  vcvt.f32.s32 s15, s22
0802812a  37ee899a  vadd.f32     s18, s15, s18
0802812e  bdeec91a  vcvt.s32.f32 s2, s18
08028132  11ee103a  vmov         r3, s2
08028136  3b44      add          r3, r7
08028138  ab42      cmp          r3, r5
0802813a  0b60      str          r3, [r1]
0802813c  a4bf      itt          ge
0802813e  05f1ff33  addge.w      r3, r5, #4294967295
08028142  0b60      strge        r3, [r1]
08028144  0b9a      ldr          r2, [sp, #44]
08028146  1e99      ldr          r1, [sp, #120]
08028148  189c      ldr          r4, [sp, #96]
0802814a  0c60      str          r4, [r1]
0802814c  002a      cmp          r2, #0
0802814e  42f38e83  ble.w        #10012  ; -> 0x0802a86e
08028152  1598      ldr          r0, [sp, #84]
08028154  4299      ldr          r1, [sp, #264]
08028156  d0ed004a  vldr         s9, [r0]
0802815a  2048      ldr          r0, [pc, #128]  ; [0x080281dc] = 0x200220fc (f32=1.10223419e-19)
0802815c  91ed00ba  vldr         s22, [r1]
08028160  134f      ldr          r7, [pc, #76]  ; [0x080281b0] = 0x20022098 (f32=1.10222127e-19)
08028162  1f49      ldr          r1, [pc, #124]  ; [0x080281e0] = 0x20021de8 (f32=1.10213235e-19)
08028164  0068      ldr          r0, [r0]
08028166  1f4d      ldr          r5, [pc, #124]  ; [0x080281e4] = 0x2002214c (f32=1.10224453e-19)
08028168  4791      str          r1, [sp, #284]
0802816a  1f4e      ldr          r6, [pc, #124]  ; [0x080281e8] = 0x20021ca0 (f32=1.10208995e-19)
0802816c  1f49      ldr          r1, [pc, #124]  ; [0x080281ec] = 0x20022070 (f32=1.1022161e-19)
0802816e  204c      ldr          r4, [pc, #128]  ; [0x080281f0] = 0x20001290 (f32=1.08481635e-19)
08028170  97ed005a  vldr         s10, [r7]
08028174  4691      str          r1, [sp, #280]
08028176  1f4f      ldr          r7, [pc, #124]  ; [0x080281f4] = 0x2000104c (f32=1.08474139e-19)
08028178  9fed1f3a  vldr         s6, [pc, #124]  ; [0x080281f8] = 0x00000000 (f32=0)
0802817c  0a90      str          r0, [sp, #40]
0802817e  1549      ldr          r1, [pc, #84]  ; [0x080281d4] = 0x20021cbc (f32=1.10209357e-19)
08028180  1748      ldr          r0, [pc, #92]  ; [0x080281e0] = 0x20021de8 (f32=1.10213235e-19)
08028182  2a68      ldr          r2, [r5]
08028184  9fed1dda  vldr         s26, [pc, #116]  ; [0x080281fc] = 0x3f19999a (f32=0.600000024)
08028188  3668      ldr          r6, [r6]
0802818a  2468      ldr          r4, [r4]
0802818c  dff874e0  ldr.w        lr, [pc, #116]  ; [0x08028204] = 0x20022100 (f32=1.10223471e-19)
08028190  1b4d      ldr          r5, [pc, #108]  ; [0x08028200] = 0x20021314 (f32=1.10177407e-19)
08028192  0968      ldr          r1, [r1]
08028194  4897      str          r7, [sp, #288]
08028196  b4eec37a  vcmpe.f32    s14, s6
0802819a  3f68      ldr          r7, [r7]
0802819c  0496      str          r6, [sp, #16]
0802819e  2794      str          r4, [sp, #156]
080281a0  0668      ldr          r6, [r0]
080281a2  31e0      b            #98  ; -> 0x08028208
08028208  3d4c      ldr          r4, [pc, #244]  ; [0x08028300] = 0x20021e14 (f32=1.10213803e-19)
0802820a  3e48      ldr          r0, [pc, #248]  ; [0x08028304] = 0x20022070 (f32=1.1022161e-19)
0802820c  2d68      ldr          r5, [r5]
0802820e  0791      str          r1, [sp, #28]
08028210  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028214  def80010  ldr.w        r1, [lr]
08028218  0d97      str          r7, [sp, #52]
0802821a  f4eecd4a  vcmpe.f32    s9, s26
0802821e  149f      ldr          r7, [sp, #80]
08028220  1795      str          r5, [sp, #92]
08028222  1344      add          r3, r2
08028224  0568      ldr          r5, [r0]
08028226  0891      str          r1, [sp, #32]
08028228  74eecd6a  vsub.f32     s13, s9, s26
0802822c  2168      ldr          r1, [r4]
0802822e  9fed368a  vldr         s16, [pc, #216]  ; [0x08028308] = 0x411ffbe7 (f32=9.9989996)
08028232  9fed366a  vldr         s12, [pc, #216]  ; [0x0802830c] = 0x447a0000 (f32=1000)
08028236  1695      str          r5, [sp, #88]
08028238  54bf      ite          pl
0802823a  b0ee4eda  vmovpl.f32   s26, s28
0802823e  b0ee6bda  vmovmi.f32   s26, s23
08028242  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028246  09ee903a  vmov         s19, r3
0802824a  1a91      str          r1, [sp, #104]
0802824c  97ed004a  vldr         s8, [r7]
08028250  8ded2a5a  vstr         s10, [sp, #168]
08028254  ccbf      ite          gt
08028256  0123      movgt        r3, #1
08028258  0023      movle        r3, #0
0802825a  dfed2d0a  vldr         s1, [pc, #180]  ; [0x08028310] = 0x43fa0000 (f32=500)
0802825e  2996      str          r6, [sp, #164]
08028260  419e      ldr          r6, [sp, #260]
08028262  1198      ldr          r0, [sp, #68]
08028264  1c93      str          r3, [sp, #112]
08028266  b6ee007a  vmov.f32     s14, #5.000000e-01
0802826a  079b      ldr          r3, [sp, #28]
0802826c  d0f80090  ldr.w        r9, [r0]
08028270  284d      ldr          r5, [pc, #160]  ; [0x08028314] = 0x20021ee4 (f32=1.10216492e-19)
08028272  294f      ldr          r7, [pc, #164]  ; [0x08028318] = 0x20021f28 (f32=1.10217371e-19)
08028274  294c      ldr          r4, [pc, #164]  ; [0x0802831c] = 0x20021f08 (f32=1.10216957e-19)
08028276  2a48      ldr          r0, [pc, #168]  ; [0x08028320] = 0x20021f70 (f32=1.10218301e-19)
08028278  d6ed008a  vldr         s17, [r6]
0802827c  0395      str          r5, [sp, #12]
0802827e  25ee080a  vmul.f32     s0, s10, s16
08028282  65ee073a  vmul.f32     s7, s10, s14
08028286  6bee065a  vmul.f32     s11, s22, s12
0802828a  6bee207a  vmul.f32     s15, s22, s1
0802828e  5a1e      subs         r2, r3, #1
08028290  2449      ldr          r1, [pc, #144]  ; [0x08028324] = 0x20021f94 (f32=1.10218766e-19)
08028292  254e      ldr          r6, [pc, #148]  ; [0x08028328] = 0x20021fd8 (f32=1.10219645e-19)
08028294  254d      ldr          r5, [pc, #148]  ; [0x0802832c] = 0x20021ff8 (f32=1.10220059e-19)
08028296  1b92      str          r2, [sp, #108]
08028298  b1ee4b1a  vneg.f32     s2, s22
0802829c  66ee80aa  vmul.f32     s21, s13, s0
080282a0  b8eee98a  vcvt.f32.s32 s16, s19
080282a4  8ded492a  vstr         s4, [sp, #292]
080282a8  cded4a1a  vstr         s3, [sp, #296]
080282ac  dff890b0  ldr.w        r11, [pc, #144]  ; [0x08028340] = 0x20021e64 (f32=1.10214837e-19)
080282b0  dff890c0  ldr.w        r12, [pc, #144]  ; [0x08028344] = 0x20021ec8 (f32=1.1021613e-19)
080282b4  dff890e0  ldr.w        lr, [pc, #144]  ; [0x08028348] = 0x20022018 (f32=1.10220472e-19)
080282b8  0291      str          r1, [sp, #8]
080282ba  0f96      str          r6, [sp, #60]
080282bc  0e95      str          r5, [sp, #56]
080282be  9fed1c9a  vldr         s18, [pc, #112]  ; [0x08028330] = 0xc7d90380 (f32=-111111)
080282c2  dfed1cea  vldr         s29, [pc, #112]  ; [0x08028334] = 0x3ff33333 (f32=1.89999998)
080282c6  dfed1cca  vldr         s25, [pc, #112]  ; [0x08028338] = 0x3db4c251 (f32=0.0882612541)
080282ca  dfed1cda  vldr         s27, [pc, #112]  ; [0x0802833c] = 0xb22bcc77 (f32=-9.99999994e-09)
080282ce  cded225a  vstr         s11, [sp, #136]
080282d2  74eec79a  vsub.f32     s19, s9, s14
080282d6  8ded1d6a  vstr         s12, [sp, #116]
080282da  cded2e7a  vstr         s15, [sp, #184]
080282de  f0ee432a  vmov.f32     s5, s6
080282e2  b0ee4e0a  vmov.f32     s0, s28
080282e6  8ded431a  vstr         s2, [sp, #268]
080282ea  0022      movs         r2, #0
080282ec  b0ee4a2a  vmov.f32     s4, s20
080282f0  f0ee631a  vmov.f32     s3, s7
080282f4  cde90074  strd         r7, r4, [sp]
080282f8  cdf82490  str.w        r9, [sp, #36]
080282fc  8246      mov          r10, r0
080282fe  8de1      b            #794  ; -> 0x0802861c
0802834c  0023      movs         r3, #0
0802834e  019c      ldr          r4, [sp, #4]
08028350  cd4e      ldr          r6, [pc, #820]  ; [0x08028688] = 0x20021ea8 (f32=1.10215716e-19)
08028352  ccf80030  str.w        r3, [r12]
08028356  1a9b      ldr          r3, [sp, #104]
08028358  cc4d      ldr          r5, [pc, #816]  ; [0x0802868c] = 0x2002215c (f32=1.1022466e-19)
0802835a  84ed00ea  vstr         s28, [r4]
0802835e  9100      lsls         r1, r2, #2
08028360  0798      ldr          r0, [sp, #28]
08028362  1b9c      ldr          r4, [sp, #108]
08028364  0bed01ea  vstr         s28, [r11, #-4]
08028368  0e44      add          r6, r1
0802836a  0327      movs         r7, #3
0802836c  012b      cmp          r3, #1
0802836e  86ed00ea  vstr         s28, [r6]
08028372  cef80070  str.w        r7, [lr]
08028376  55f82060  ldr.w        r6, [r5, r0, lsl #2]
0802837a  55f82450  ldr.w        r5, [r5, r4, lsl #2]
0802837e  04d1      bne          #8  ; -> 0x0802838a
08028380  701b      subs         r0, r6, r5
08028382  04ee100a  vmov         s8, r0
08028386  b8eec44a  vcvt.f32.s32 s8, s8
0802838a  b0ee48fa  vmov.f32     s30, s16
0802838e  adee04fa  vfma.f32     s30, s26, s8
08028392  0e9c      ldr          r4, [sp, #56]
08028394  0f98      ldr          r0, [sp, #60]
08028396  44f82250  str.w        r5, [r4, r2, lsl #2]
0802839a  f4ee6cfa  vcmp.f32     s31, s25
0802839e  f1ee10fa  vmrs         APSR_nzcv, fpscr
080283a2  bdeecf5a  vcvt.s32.f32 s10, s30
080283a6  029c      ldr          r4, [sp, #8]
080283a8  40f82260  str.w        r6, [r0, r2, lsl #2]
080283ac  0cbf      ite          eq
080283ae  0127      moveq        r7, #1
080283b0  0027      movne        r7, #0
080283b2  74ee687a  vsub.f32     s15, s8, s17
080283b6  2760      str          r7, [r4]
080283b8  15ee103a  vmov         r3, s10
080283bc  009f      ldr          r7, [sp]
080283be  0698      ldr          r0, [sp, #24]
080283c0  dff8e892  ldr.w        r9, [pc, #744]  ; [0x080286ac] = 0x20021fb8 (f32=1.10219232e-19)
080283c4  87ed005a  vstr         s10, [r7]
080283c8  bdeee7aa  vcvt.s32.f32 s20, s15
080283cc  4418      adds         r4, r0, r1
080283ce  079f      ldr          r7, [sp, #28]
080283d0  8aed00aa  vstr         s20, [r10]
080283d4  ab42      cmp          r3, r5
080283d6  49f82270  str.w        r7, [r9, r2, lsl #2]
080283da  84ed004a  vstr         s8, [r4]
080283de  0fda      bge          #30  ; -> 0x08028400
080283e0  0c98      ldr          r0, [sp, #48]
080283e2  009c      ldr          r4, [sp]
080283e4  00eb8708  add.w        r8, r0, r7, lsl #2
080283e8  d8ed004a  vldr         s9, [r8]
080283ec  b8eec51a  vcvt.f32.s32 s2, s10
080283f0  31ee247a  vadd.f32     s14, s2, s9
080283f4  fdeec70a  vcvt.s32.f32 s1, s14
080283f8  10ee903a  vmov         r3, s1
080283fc  c4ed000a  vstr         s1, [r4]
08028400  9e42      cmp          r6, r3
08028402  12da      bge          #36  ; -> 0x0802842a
08028404  0c9f      ldr          r7, [sp, #48]
08028406  0798      ldr          r0, [sp, #28]
08028408  009c      ldr          r4, [sp]
0802840a  05ee903a  vmov         s11, r3
0802840e  07eb8009  add.w        r9, r7, r0, lsl #2
08028412  d9ed006a  vldr         s13, [r9]
08028416  f8eee57a  vcvt.f32.s32 s15, s11
0802841a  37eee6fa  vsub.f32     s30, s15, s13
0802841e  bdeecf5a  vcvt.s32.f32 s10, s30
08028422  15ee103a  vmov         r3, s10
08028426  84ed005a  vstr         s10, [r4]
0802842a  0998      ldr          r0, [sp, #36]
0802842c  984c      ldr          r4, [pc, #608]  ; [0x08028690] = 0x0bb38435 (f32=6.9147215e-32)
0802842e  994f      ldr          r7, [pc, #612]  ; [0x08028694] = 0x3619636b (f32=2.28566455e-06)
08028430  04fb0078  mla          r8, r4, r0, r7
08028434  0aee108a  vmov         s20, r8
08028438  f8ee4a4a  vcvt.f32.u32 s9, s20
0802843c  9548      ldr          r0, [pc, #596]  ; [0x08028694] = 0x3619636b (f32=2.28566455e-06)
0802843e  64eea13a  vmul.f32     s7, s9, s3
08028442  04fb0807  mla          r7, r4, r8, r0
08028446  01ee107a  vmov         s2, r7
0802844a  f4eee39a  vcmpe.f32    s19, s7
0802844e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028452  0997      str          r7, [sp, #36]
08028454  f8ee410a  vcvt.f32.u32 s1, s2
08028458  c0f25784  blt.w        #2222  ; -> 0x08028d0a
0802845c  04fb0704  mla          r4, r4, r7, r0
08028460  dded2a6a  vldr         s13, [sp, #168]
08028464  0994      str          r4, [sp, #36]
08028466  05ee904a  vmov         s11, r4
0802846a  26eea07a  vmul.f32     s14, s13, s1
0802846e  f8ee650a  vcvt.f32.u32 s1, s11
08028472  6aeea07a  vmul.f32     s15, s21, s1
08028476  8848      ldr          r0, [pc, #544]  ; [0x08028698] = 0x20021e88 (f32=1.10215303e-19)
08028478  1c9c      ldr          r4, [sp, #112]
0802847a  bdeee7fa  vcvt.s32.f32 s30, s15
0802847e  0144      add          r1, r0
08028480  1fee107a  vmov         r7, s30
08028484  0398      ldr          r0, [sp, #12]
08028486  81ed007a  vstr         s14, [r1]
0802848a  04fb07f7  mul          r7, r4, r7
0802848e  0499      ldr          r1, [sp, #16]
08028490  4760      str          r7, [r0, #4]
08028492  0024      movs         r4, #0
08028494  0327      movs         r7, #3
08028496  1940      ands         r1, r3
08028498  0597      str          r7, [sp, #20]
0802849a  0d94      str          r4, [sp, #52]
0802849c  f0ee4e7a  vmov.f32     s15, s28
080284a0  0598      ldr          r0, [sp, #20]
080284a2  0028      cmp          r0, #0
080284a4  00f0de83  beq.w        #1980  ; -> 0x08028c64
080284a8  069f      ldr          r7, [sp, #24]
080284aa  9aed00aa  vldr         s20, [r10]
080284ae  4fea8209  lsl.w        r9, r2, #2
080284b2  07eb0908  add.w        r8, r7, r9
080284b6  98ed006a  vldr         s12, [r8]
080284ba  74ee684a  vsub.f32     s9, s8, s17
080284be  b4eec46a  vcmpe.f32    s12, s8
080284c2  b8eeca6a  vcvt.f32.s32 s12, s20
080284c6  f1ee10fa  vmrs         APSR_nzcv, fpscr
080284ca  b4eee46a  vcmpe.f32    s12, s9
080284ce  c8bf      it           gt
080284d0  88ed004a  vstrgt       s8, [r8]
080284d4  f1ee10fa  vmrs         APSR_nzcv, fpscr
080284d8  c8bf      it           gt
080284da  bdeee4aa  vcvtgt.s32.f32 s20, s9
080284de  f5eec07a  vcmpe.f32    s15, #0
080284e2  c8bf      it           gt
080284e4  8aed00aa  vstrgt       s20, [r10]
080284e8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080284ec  dcf80080  ldr.w        r8, [r12]
080284f0  00f11884  bmi.w        #2096  ; -> 0x08028d24
080284f4  1aee104a  vmov         r4, s20
080284f8  4445      cmp          r4, r8
080284fa  80f21a84  bge.w        #2100  ; -> 0x08028d32
080284fe  37eecb5a  vsub.f32     s10, s15, s22
08028502  9ded432a  vldr         s4, [sp, #268]
08028506  0124      movs         r4, #1
08028508  9ded1d1a  vldr         s2, [sp, #116]
0802850c  b4eec14a  vcmpe.f32    s8, s2
08028510  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028514  40f1b983  bpl.w        #1906  ; -> 0x08028c8a
08028518  0d9f      ldr          r7, [sp, #52]
0802851a  002f      cmp          r7, #0
0802851c  40f0d083  bne.w        #1952  ; -> 0x08028cc0
08028520  0427      movs         r7, #4
08028522  2997      str          r7, [sp, #164]
08028524  a1f13f00  sub.w        r0, r1, #63
08028528  a842      cmp          r0, r5
0802852a  0adc      bgt          #20  ; -> 0x08028542
0802852c  4f1b      subs         r7, r1, r5
0802852e  05ee907a  vmov         s11, r7
08028532  dfed5a7a  vldr         s15, [pc, #360]  ; [0x0802869c] = 0x3c800000 (f32=0.015625)
08028536  b8eee5fa  vcvt.f32.s32 s30, s11
0802853a  2fee27aa  vmul.f32     s20, s30, s15
0802853e  25ee0a5a  vmul.f32     s10, s10, s20
08028542  01f13f00  add.w        r0, r1, #63
08028546  b042      cmp          r0, r6
08028548  0adb      blt          #20  ; -> 0x08028560
0802854a  711a      subs         r1, r6, r1
0802854c  04ee901a  vmov         s9, r1
08028550  9fed521a  vldr         s2, [pc, #328]  ; [0x0802869c] = 0x3c800000 (f32=0.015625)
08028554  f8eee43a  vcvt.f32.s32 s7, s9
08028558  63ee810a  vmul.f32     s1, s7, s2
0802855c  25ee205a  vmul.f32     s10, s10, s1
08028560  494f      ldr          r7, [pc, #292]  ; [0x08028688] = 0x20021ea8 (f32=1.10215716e-19)
08028562  1998      ldr          r0, [sp, #100]
08028564  cef80040  str.w        r4, [lr]
08028568  b944      add          r9, r7
0802856a  99ed007a  vldr         s14, [r9]
0802856e  0bed015a  vstr         s10, [r11, #-4]
08028572  7cee076a  vadd.f32     s13, s24, s14
08028576  4044      add          r0, r8
08028578  f4eeeb6a  vcmpe.f32    s13, s23
0802857c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028580  ccf80000  str.w        r0, [r12]
08028584  c0f2c583  blt.w        #1930  ; -> 0x08028d12
08028588  36eeebfa  vsub.f32     s30, s13, s23
0802858c  0130      adds         r0, #1
0802858e  89ed00fa  vstr         s30, [r9]
08028592  ccf80000  str.w        r0, [r12]
08028596  039f      ldr          r7, [sp, #12]
08028598  019c      ldr          r4, [sp, #4]
0802859a  57f8041f  ldr          r1, [r7, #4]!
0802859e  0397      str          r7, [sp, #12]
080285a0  b4eec50a  vcmpe.f32    s0, s10
080285a4  0df5b078  add.w        r8, sp, #352
080285a8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080285ac  08eb8100  add.w        r0, r8, r1, lsl #2
080285b0  48bf      it           mi
080285b2  b0ee450a  vmovmi.f32   s0, s10
080285b6  50ed047a  vldr         s15, [r0, #-16]
080285ba  94ed005a  vldr         s10, [r4]
080285be  50f8207c  ldr          r7, [r0, #-32]
080285c2  0099      ldr          r1, [sp]
080285c4  48bf      it           mi
080285c6  1692      strmi        r2, [sp, #88]
080285c8  35ee27aa  vadd.f32     s20, s10, s15
080285cc  3b44      add          r3, r7
080285ce  b4eeebaa  vcmpe.f32    s20, s23
080285d2  f1ee10fa  vmrs         APSR_nzcv, fpscr
080285d6  0b60      str          r3, [r1]
080285d8  c0f24b83  blt.w        #1686  ; -> 0x08028c72
080285dc  7aee6b4a  vsub.f32     s9, s20, s23
080285e0  0133      adds         r3, #1
080285e2  c4ed004a  vstr         s9, [r4]
080285e6  9e42      cmp          r6, r3
080285e8  03db      blt          #6  ; -> 0x080285f2
080285ea  9d42      cmp          r5, r3
080285ec  ccbf      ite          gt
080285ee  3546      movgt        r5, r6
080285f0  1d46      movle        r5, r3
080285f2  009e      ldr          r6, [sp]
080285f4  019b      ldr          r3, [sp, #4]
080285f6  0298      ldr          r0, [sp, #8]
080285f8  0b9f      ldr          r7, [sp, #44]
080285fa  46f8045b  str          r5, [r6], #4
080285fe  0132      adds         r2, #1
08028600  0433      adds         r3, #4
08028602  0430      adds         r0, #4
08028604  ba42      cmp          r2, r7
08028606  0096      str          r6, [sp]
08028608  0193      str          r3, [sp, #4]
0802860a  0cf1040c  add.w        r12, r12, #4
0802860e  0ef1040e  add.w        lr, lr, #4
08028612  0290      str          r0, [sp, #8]
08028614  0af1040a  add.w        r10, r10, #4
08028618  00f0c083  beq.w        #1920  ; -> 0x08028d9c
0802861c  fbec017a  vldmia       r11!, {s15}
08028620  f4ee497a  vcmp.f32     s15, s18
08028624  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028628  3ff490ae  beq.w        #-736  ; -> 0x0802834c
0802862c  0f99      ldr          r1, [sp, #60]
0802862e  009f      ldr          r7, [sp]
08028630  0e9c      ldr          r4, [sp, #56]
08028632  3b68      ldr          r3, [r7]
08028634  51f82260  ldr.w        r6, [r1, r2, lsl #2]
08028638  0499      ldr          r1, [sp, #16]
0802863a  54f82250  ldr.w        r5, [r4, r2, lsl #2]
0802863e  f4eeee7a  vcmpe.f32    s15, s29
08028642  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028646  01ea0301  and.w        r1, r1, r3
0802864a  40f37182  ble.w        #1250  ; -> 0x08028b30
0802864e  0020      movs         r0, #0
08028650  0bed01ea  vstr         s28, [r11, #-4]
08028654  cef80000  str.w        r0, [lr]
08028658  0590      str          r0, [sp, #20]
0802865a  1fe7      b            #-450  ; -> 0x0802849c
0802865c  1048      ldr          r0, [pc, #64]  ; [0x080286a0] = 0x20021c90 (f32=1.10208789e-19)
0802865e  114a      ldr          r2, [pc, #68]  ; [0x080286a4] = 0xe0001000 (f32=-3.69115025e+19)
08028660  4768      ldr          r7, [r0, #4]
08028662  5368      ldr          r3, [r2, #4]
08028664  104e      ldr          r6, [pc, #64]  ; [0x080286a8] = 0x2002205c (f32=1.10221351e-19)
08028666  0360      str          r3, [r0]
08028668  bb42      cmp          r3, r7
0802866a  4ff00004  mov.w        r4, #0
0802866e  c8bf      it           gt
08028670  4360      strgt        r3, [r0, #4]
08028672  3460      str          r4, [r6]
08028674  59b0      add          sp, #356
08028676  bdec108b  vpop         {d8, d9, d10, d11, d12, d13, d14, d15}
0802867a  bde8f08f  pop.w        {r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802867e  0020      movs         r0, #0
08028680  1268      ldr          r2, [r2]
08028682  4e90      str          r0, [sp, #312]
08028684  fef7b7bf  b.w          #-4242  ; -> 0x080275f6
080286b0  082e      cmp          r6, #8
080286b2  40f3e280  ble.w        #452  ; -> 0x0802887a
080286b6  b74f      ldr          r7, [pc, #732]  ; [0x08028994] = 0x20021dec (f32=1.10213286e-19)
080286b8  b74c      ldr          r4, [pc, #732]  ; [0x08028998] = 0x200220a4 (f32=1.10222282e-19)
080286ba  1297      str          r7, [sp, #72]
080286bc  a060      str          r0, [r4, #8]
080286be  c7ed02aa  vstr         s21, [r7, #8]
080286c2  002b      cmp          r3, #0
080286c4  02f0ba81  beq.w        #9076  ; -> 0x0802aa3c
080286c8  b44d      ldr          r5, [pc, #720]  ; [0x0802899c] = 0x20024128 (f32=1.10329867e-19)
080286ca  4f95      str          r5, [sp, #316]
080286cc  012b      cmp          r3, #1
080286ce  7ff480a8  bne.w        #-3840  ; -> 0x080277d2
080286d2  b349      ldr          r1, [pc, #716]  ; [0x080289a0] = 0x200012a0 (f32=1.08481842e-19)
080286d4  9fedb3ca  vldr         s24, [pc, #716]  ; [0x080289a4] = 0x3a83126f (f32=0.00100000005)
080286d8  0c68      ldr          r4, [r1]
080286da  b34e      ldr          r6, [pc, #716]  ; [0x080289a8] = 0x20001040 (f32=1.08473984e-19)
080286dc  3396      str          r6, [sp, #204]
080286de  601c      adds         r0, r4, #1
080286e0  0cee900a  vmov         s25, r0
080286e4  7aeeccda  vsub.f32     s27, s21, s24
080286e8  b8eeecda  vcvt.f32.s32 s26, s25
080286ec  3768      ldr          r7, [r6]
080286ee  2dee2dea  vmul.f32     s28, s26, s27
080286f2  fdeeceea  vcvt.s32.f32 s29, s28
080286f6  1eee900a  vmov         r0, s29
080286fa  0130      adds         r0, #1
080286fc  0128      cmp          r0, #1
080286fe  b8bf      it           lt
08028700  0120      movlt        r0, #1
08028702  8742      cmp          r7, r0
08028704  00f0c380  beq.w        #390  ; -> 0x0802888e
08028708  a84d      ldr          r5, [pc, #672]  ; [0x080289ac] = 0x2000128c (f32=1.08481584e-19)
0802870a  3060      str          r0, [r6]
0802870c  2968      ldr          r1, [r5]
0802870e  8142      cmp          r1, r0
08028710  dcbf      itt          le
08028712  0846      movle        r0, r1
08028714  3160      strle        r1, [r6]
08028716  8442      cmp          r4, r0
08028718  82f21c83  bge.w        #9784  ; -> 0x0802ad54
0802871c  339d      ldr          r5, [sp, #204]
0802871e  4ff47a70  mov.w        r0, #1000
08028722  2860      str          r0, [r5]
08028724  b3e0      b            #358  ; -> 0x0802888e
08028726  b7ee009a  vmov.f32     s18, #1.000000e+00
0802872a  c9ee289a  vdiv.f32     s19, s18, s17
0802872e  3860      str          r0, [r7]
08028730  c4ed009a  vstr         s19, [r4]
08028734  fef7edbf  b.w          #-4134  ; -> 0x08027712
08028738  d6ed048a  vldr         s17, [r6, #16]
0802873c  9fed9c4a  vldr         s8, [pc, #624]  ; [0x080289b0] = 0x3c03126f (f32=0.00800000038)
08028740  77eee85a  vsub.f32     s11, s15, s17
08028744  f5eec05a  vcmpe.f32    s11, #0
08028748  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802874c  48bf      it           mi
0802874e  78eee75a  vsubmi.f32   s11, s17, s15
08028752  f4eec45a  vcmpe.f32    s11, s8
08028756  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802875a  7ff7a3a9  ble.w        #-3258  ; -> 0x08027aa4
0802875e  189c      ldr          r4, [sp, #96]
08028760  1299      ldr          r1, [sp, #72]
08028762  012c      cmp          r4, #1
08028764  c1ed047a  vstr         s15, [r1, #16]
08028768  08d0      beq          #16  ; -> 0x0802877c
0802876a  924f      ldr          r7, [pc, #584]  ; [0x080289b4] = 0x200012a8 (f32=1.08481946e-19)
0802876c  b868      ldr          r0, [r7, #8]
0802876e  0228      cmp          r0, #2
08028770  04d0      beq          #8  ; -> 0x0802877c
08028772  914d      ldr          r5, [pc, #580]  ; [0x080289b8] = 0x20022140 (f32=1.10224298e-19)
08028774  0026      movs         r6, #0
08028776  2e60      str          r6, [r5]
08028778  fff7abb9  b.w          #-3242  ; -> 0x08027ad2
0802877c  8e49      ldr          r1, [pc, #568]  ; [0x080289b8] = 0x20022140 (f32=1.10224298e-19)
0802877e  9524      movs         r4, #149
08028780  0c60      str          r4, [r1]
08028782  8e49      ldr          r1, [pc, #568]  ; [0x080289bc] = 0x20021e14 (f32=1.10213803e-19)
08028784  0c68      ldr          r4, [r1]
08028786  012c      cmp          r4, #1
08028788  7ff49ea9  bne.w        #-3268  ; -> 0x08027ac8
0802878c  dfed8c9a  vldr         s19, [pc, #560]  ; [0x080289c0] = 0x459c4000 (f32=5000)
08028790  b4eee97a  vcmpe.f32    s14, s19
08028794  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028798  42f30683  ble.w        #9740  ; -> 0x0802ada8
0802879c  f6ee00aa  vmov.f32     s21, #5.000000e-01
080287a0  27ee2a7a  vmul.f32     s14, s14, s21
080287a4  b4eee97a  vcmpe.f32    s14, s19
080287a8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080287ac  f8dc      bgt          #-16  ; -> 0x080287a0
080287ae  149f      ldr          r7, [sp, #80]
080287b0  dfed84fa  vldr         s31, [pc, #528]  ; [0x080289c4] = 0x3db4c251 (f32=0.0882612541)
080287b4  87ed007a  vstr         s14, [r7]
080287b8  fff786b9  b.w          #-3316  ; -> 0x08027ac8
080287bc  129e      ldr          r6, [sp, #72]
080287be  4f9d      ldr          r5, [sp, #316]
080287c0  86ed039a  vstr         s18, [r6, #12]
080287c4  4ff47077  mov.w        r7, #960
080287c8  012c      cmp          r4, #1
080287ca  2f60      str          r7, [r5]
080287cc  02f00d82  beq.w        #9242  ; -> 0x0802abea
080287d0  7d4c      ldr          r4, [pc, #500]  ; [0x080289c8] = 0x20022038 (f32=1.10220886e-19)
080287d2  784f      ldr          r7, [pc, #480]  ; [0x080289b4] = 0x200012a8 (f32=1.08481946e-19)
080287d4  0021      movs         r1, #0
080287d6  2160      str          r1, [r4]
080287d8  3c68      ldr          r4, [r7]
080287da  012c      cmp          r4, #1
080287dc  42f25b81  bls.w        #8886  ; -> 0x0802aa96
080287e0  dfed7aba  vldr         s23, [pc, #488]  ; [0x080289cc] = 0x43a40000 (f32=328)
080287e4  7a4e      ldr          r6, [pc, #488]  ; [0x080289d0] = 0x08045e10 (f32=3.9832875e-34)
080287e6  29ee2bca  vmul.f32     s24, s18, s23
080287ea  40f22715  movw         r5, #295
080287ee  fdeeccca  vcvt.s32.f32 s25, s24
080287f2  b7ee00da  vmov.f32     s26, #1.000000e+00
080287f6  1cee90ba  vmov         r11, s25
080287fa  abf11001  sub.w        r1, r11, #16
080287fe  21eae177  bic.w        r7, r1, r1, asr #31
08028802  af42      cmp          r7, r5
08028804  a8bf      it           ge
08028806  2f46      movge        r7, r5
08028808  79ee0dda  vadd.f32     s27, s18, s26
0802880c  06eb8704  add.w        r4, r6, r7, lsl #2
08028810  704d      ldr          r5, [pc, #448]  ; [0x080289d4] = 0x200220e8 (f32=1.10223161e-19)
08028812  7149      ldr          r1, [pc, #452]  ; [0x080289d8] = 0x200220dc (f32=1.10223006e-19)
08028814  2668      ldr          r6, [r4]
08028816  2e60      str          r6, [r5]
08028818  feeecdda  vcvt.s32.f32 s27, s27, #6
0802881c  c1ed00da  vstr         s27, [r1]
08028820  fff754b8  b.w          #-3928  ; -> 0x080278cc
08028824  6549      ldr          r1, [pc, #404]  ; [0x080289bc] = 0x20021e14 (f32=1.10213803e-19)
08028826  0124      movs         r4, #1
08028828  0c60      str          r4, [r1]
0802882a  fff72cb8  b.w          #-4008  ; -> 0x08027886
0802882e  0a1a      subs         r2, r1, r0
08028830  05ee102a  vmov         s10, r2
08028834  f8eec55a  vcvt.f32.s32 s11, s10
08028838  b7ee006a  vmov.f32     s12, #1.000000e+00
0802883c  86ee258a  vdiv.f32     s16, s12, s11
08028840  664c      ldr          r4, [pc, #408]  ; [0x080289dc] = 0x200220a0 (f32=1.1022223e-19)
08028842  3060      str          r0, [r6]
08028844  dfed668a  vldr         s17, [pc, #408]  ; [0x080289e0] = 0x00000000 (f32=0)
08028848  84ed008a  vstr         s16, [r4]
0802884c  fef75cbf  b.w          #-4424  ; -> 0x08027708
08028850  dfed642a  vldr         s5, [pc, #400]  ; [0x080289e4] = 0x3d480c74 (f32=0.0488400012)
08028854  9fed623a  vldr         s6, [pc, #392]  ; [0x080289e0] = 0x00000000 (f32=0)
08028858  72ee623a  vsub.f32     s7, s4, s5
0802885c  f4eec33a  vcmpe.f32    s7, s6
08028860  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028864  b8bf      it           lt
08028866  f0ee433a  vmovlt.f32   s7, s6
0802886a  cded313a  vstr         s7, [sp, #196]
0802886e  fef73cbf  b.w          #-4488  ; -> 0x080276ea
08028872  dfed5b7a  vldr         s15, [pc, #364]  ; [0x080289e0] = 0x00000000 (f32=0)
08028876  fff70bb9  b.w          #-3562  ; -> 0x08027a90
0802887a  012b      cmp          r3, #1
0802887c  42f0f080  bne.w        #8672  ; -> 0x0802aa60
08028880  4949      ldr          r1, [pc, #292]  ; [0x080289a8] = 0x20001040 (f32=1.08473984e-19)
08028882  4648      ldr          r0, [pc, #280]  ; [0x0802899c] = 0x20024128 (f32=1.10329867e-19)
08028884  4f90      str          r0, [sp, #316]
08028886  434e      ldr          r6, [pc, #268]  ; [0x08028994] = 0x20021dec (f32=1.10213286e-19)
08028888  0868      ldr          r0, [r1]
0802888a  3391      str          r1, [sp, #204]
0802888c  1296      str          r6, [sp, #72]
0802888e  5649      ldr          r1, [pc, #344]  ; [0x080289e8] = 0x2002215c (f32=1.1022466e-19)
08028890  564e      ldr          r6, [pc, #344]  ; [0x080289ec] = 0x0005dc00 (f32=5.3809861e-40)
08028892  51f82240  ldr.w        r4, [r1, r2, lsl #2]
08028896  b442      cmp          r4, r6
08028898  a8bf      it           ge
0802889a  3446      movge        r4, r6
0802889c  0027      movs         r7, #0
0802889e  c1f8b844  str.w        r4, [r1, #1208]
080288a2  c1f8b474  str.w        r7, [r1, #1204]
080288a6  5249      ldr          r1, [pc, #328]  ; [0x080289f0] = 0x20021cbc (f32=1.10209357e-19)
080288a8  4ff49775  mov.w        r5, #302
080288ac  0d60      str          r5, [r1]
080288ae  002a      cmp          r2, #0
080288b0  3ef49aaf  beq.w        #-4300  ; -> 0x080277e8
080288b4  4e4a      ldr          r2, [pc, #312]  ; [0x080289f0] = 0x20021cbc (f32=1.10209357e-19)
080288b6  1268      ldr          r2, [r2]
080288b8  571e      subs         r7, r2, #1
080288ba  4b4e      ldr          r6, [pc, #300]  ; [0x080289e8] = 0x2002215c (f32=1.1022466e-19)
080288bc  4d4d      ldr          r5, [pc, #308]  ; [0x080289f4] = 0x200230fc (f32=1.10276359e-19)
080288be  56f82770  ldr.w        r7, [r6, r7, lsl #2]
080288c2  56f82240  ldr.w        r4, [r6, r2, lsl #2]
080288c6  0c95      str          r5, [sp, #48]
080288c8  e11b      subs         r1, r4, r7
080288ca  00ee901a  vmov         s1, r1
080288ce  b8eee01a  vcvt.f32.s32 s2, s1
080288d2  05eb820b  add.w        r11, r5, r2, lsl #2
080288d6  b0f57a7f  cmp.w        r0, #1000
080288da  8bed001a  vstr         s2, [r11]
080288de  7ef48faf  bne.w        #-4322  ; -> 0x08027800
080288e2  002b      cmp          r3, #0
080288e4  7ef48caf  bne.w        #-4328  ; -> 0x08027800
080288e8  434f      ldr          r7, [pc, #268]  ; [0x080289f8] = 0x20022074 (f32=1.10221662e-19)
080288ea  4448      ldr          r0, [pc, #272]  ; [0x080289fc] = 0x20021c68 (f32=1.10208272e-19)
080288ec  3968      ldr          r1, [r7]
080288ee  c668      ldr          r6, [r0, #12]
080288f0  4069      ldr          r0, [r0, #20]
080288f2  41f48055  orr          r5, r1, #4096
080288f6  3d60      str          r5, [r7]
080288f8  fef788bf  b.w          #-4336  ; -> 0x0802780c
080288fc  dfed40ea  vldr         s29, [pc, #256]  ; [0x08028a00] = 0x48bb8000 (f32=384000)
08028900  b4eeee7a  vcmpe.f32    s14, s29
08028904  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028908  7ff709a9  ble.w        #-3566  ; -> 0x08027b1e
0802890c  b0ee6e7a  vmov.f32     s14, s29
08028910  1498      ldr          r0, [sp, #80]
08028912  dfed3c1a  vldr         s3, [pc, #240]  ; [0x08028a04] = 0x483b8000 (f32=192000)
08028916  c0ed00ea  vstr         s29, [r0]
0802891a  b4ee4d7a  vcmp.f32     s14, s26
0802891e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028922  0ed0      beq          #28  ; -> 0x08028942
08028924  b7ee002a  vmov.f32     s4, #1.000000e+00
08028928  c2ee072a  vdiv.f32     s5, s4, s14
0802892c  364e      ldr          r6, [pc, #216]  ; [0x08028a08] = 0x20021e84 (f32=1.10215251e-19)
0802892e  b5eec07a  vcmpe.f32    s14, #0
08028932  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028936  84ed007a  vstr         s14, [r4]
0802893a  c6ed002a  vstr         s5, [r6]
0802893e  42f2ff80  bls.w        #8702  ; -> 0x0802ab40
08028942  9fed323a  vldr         s6, [pc, #200]  ; [0x08028a0c] = 0x46bb8000 (f32=24000)
08028946  324d      ldr          r5, [pc, #200]  ; [0x08028a10] = 0x20021e0c (f32=1.102137e-19)
08028948  4195      str          r5, [sp, #260]
0802894a  f4eec31a  vcmpe.f32    s3, s6
0802894e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028952  c5ed001a  vstr         s3, [r5]
08028956  7ff7ffa8  ble.w        #-3586  ; -> 0x08027b58
0802895a  2e4f      ldr          r7, [pc, #184]  ; [0x08028a14] = 0x20021e08 (f32=1.10213648e-19)
0802895c  2e48      ldr          r0, [pc, #184]  ; [0x08028a18] = 0x382ec33e (f32=4.16666662e-05)
0802895e  4297      str          r7, [sp, #264]
08028960  85ed003a  vstr         s6, [r5]
08028964  3860      str          r0, [r7]
08028966  f7ee005a  vmov.f32     s11, #1.000000e+00
0802896a  f4eee54a  vcmpe.f32    s9, s11
0802896e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028972  3ff509a9  bmi.w        #-3566  ; -> 0x08027b88
08028976  0f4c      ldr          r4, [pc, #60]  ; [0x080289b4] = 0x200012a8 (f32=1.08481946e-19)
08028978  a56a      ldr          r5, [r4, #40]
0802897a  002d      cmp          r5, #0
0802897c  3ff404a9  beq.w        #-3576  ; -> 0x08027b88
08028980  b7ee004a  vmov.f32     s8, #1.000000e+00
08028984  b4ee445a  vcmp.f32     s10, s8
08028988  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802898c  7ff402a9  bne.w        #-3580  ; -> 0x08027b94
08028990  fff7fab8  b.w          #-3596  ; -> 0x08027b88
08028a20  1fed02ba  vldr         s22, [pc, #-8]  ; [0x08028a1c] = 0x43c80000 (f32=400)
08028a24  cb4e      ldr          r6, [pc, #812]  ; [0x08028d54] = 0x200220dc (f32=1.10223006e-19)
08028a26  69ee0bba  vmul.f32     s23, s18, s22
08028a2a  40f28f1b  movw         r11, #399
08028a2e  bdeeebca  vcvt.s32.f32 s24, s23
08028a32  beeeec9a  vcvt.s32.f32 s18, s18, #7
08028a36  1cee101a  vmov         r1, s24
08028a3a  5945      cmp          r1, r11
08028a3c  a8bf      it           ge
08028a3e  5946      movge        r1, r11
08028a40  86ed009a  vstr         s18, [r6]
08028a44  002f      cmp          r7, #0
08028a46  42f08c81  bne.w        #8984  ; -> 0x0802ad62
08028a4a  21f00103  bic          r3, r1, #1
08028a4e  c24f      ldr          r7, [pc, #776]  ; [0x08028d58] = 0x200220e8 (f32=1.10223161e-19)
08028a50  2097      str          r7, [sp, #128]
08028a52  0020      movs         r0, #0
08028a54  c72b      cmp          r3, #199
08028a56  3860      str          r0, [r7]
08028a58  42f38781  ble.w        #8974  ; -> 0x0802ad6a
08028a5c  bf48      ldr          r0, [pc, #764]  ; [0x08028d5c] = 0x080462b0 (f32=3.98383117e-34)
08028a5e  a3f1c80b  sub.w        r11, r3, #200
08028a62  00eb8b03  add.w        r3, r0, r11, lsl #2
08028a66  1d68      ldr          r5, [r3]
08028a68  3d60      str          r5, [r7]
08028a6a  bd4f      ldr          r7, [pc, #756]  ; [0x08028d60] = 0x200220ec (f32=1.10223212e-19)
08028a6c  3c68      ldr          r4, [r7]
08028a6e  012c      cmp          r4, #1
08028a70  02f03980  beq.w        #8306  ; -> 0x0802aae6
08028a74  bb4e      ldr          r6, [pc, #748]  ; [0x08028d64] = 0x2002215c (f32=1.1022466e-19)
08028a76  bc48      ldr          r0, [pc, #752]  ; [0x08028d68] = 0x20021e64 (f32=1.10214837e-19)
08028a78  56f82210  ldr.w        r1, [r6, r2, lsl #2]
08028a7c  d0ed00da  vldr         s27, [r0]
08028a80  90ed01ea  vldr         s28, [r0, #4]
08028a84  b94d      ldr          r5, [pc, #740]  ; [0x08028d6c] = 0x20021f90 (f32=1.10218715e-19)
08028a86  d0ed02ea  vldr         s29, [r0, #8]
08028a8a  90ed03fa  vldr         s30, [r0, #12]
08028a8e  def80030  ldr.w        r3, [lr]
08028a92  1495      str          r5, [sp, #80]
08028a94  013a      subs         r2, #1
08028a96  7dee8efa  vadd.f32     s31, s27, s28
08028a9a  56f82270  ldr.w        r7, [r6, r2, lsl #2]
08028a9e  cc1b      subs         r4, r1, r7
08028aa0  00ee104a  vmov         s0, r4
08028aa4  7feeae6a  vadd.f32     s13, s31, s29
08028aa8  b8eec07a  vcvt.f32.s32 s14, s0
08028aac  36ee8f8a  vadd.f32     s16, s13, s30
08028ab0  85ed007a  vstr         s14, [r5]
08028ab4  fef753bf  b.w          #-4442  ; -> 0x0802795e
08028ab8  ad4d      ldr          r5, [pc, #692]  ; [0x08028d70] = 0x20021c68 (f32=1.10208272e-19)
08028aba  ae4c      ldr          r4, [pc, #696]  ; [0x08028d74] = 0x200220a4 (f32=1.10222282e-19)
08028abc  0020      movs         r0, #0
08028abe  40f6af46  movw         r6, #3247
08028ac2  40f27d17  movw         r7, #381
08028ac6  ee60      str          r6, [r5, #12]
08028ac8  6f60      str          r7, [r5, #4]
08028aca  c5e90400  strd         r0, r0, [r5, #16]
08028ace  6061      str          r0, [r4, #20]
08028ad0  fef79cbe  b.w          #-4808  ; -> 0x0802780c
08028ad4  b4ee4d7a  vcmp.f32     s14, s26
08028ad8  b6ee000a  vmov.f32     s0, #5.000000e-01
08028adc  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028ae0  67ee001a  vmul.f32     s3, s14, s0
08028ae4  7ff41eaf  bne.w        #-452  ; -> 0x08028924
08028ae8  2be7      b            #-426  ; -> 0x08028942
08028aea  109b      ldr          r3, [sp, #64]
08028aec  9c68      ldr          r4, [r3, #8]
08028aee  012c      cmp          r4, #1
08028af0  7ff4e3aa  bne.w        #-2618  ; -> 0x080280ba
08028af4  a04d      ldr          r5, [pc, #640]  ; [0x08028d78] = 0x20021ec8 (f32=1.1021613e-19)
08028af6  a149      ldr          r1, [pc, #644]  ; [0x08028d7c] = 0x20021f70 (f32=1.10218301e-19)
08028af8  55f82720  ldr.w        r2, [r5, r7, lsl #2]
08028afc  41f82720  str.w        r2, [r1, r7, lsl #2]
08028b00  002e      cmp          r6, #0
08028b02  7ff4ddaa  bne.w        #-2630  ; -> 0x080280c0
08028b06  2698      ldr          r0, [sp, #152]
08028b08  329d      ldr          r5, [sp, #200]
08028b0a  0668      ldr          r6, [r0]
08028b0c  249c      ldr          r4, [sp, #144]
08028b0e  1e9f      ldr          r7, [sp, #120]
08028b10  84ed006a  vstr         s12, [r4]
08028b14  0022      movs         r2, #0
08028b16  b542      cmp          r5, r6
08028b18  3a60      str          r2, [r7]
08028b1a  01dc      bgt          #2  ; -> 0x08028b20
08028b1c  269b      ldr          r3, [sp, #152]
08028b1e  1a60      str          r2, [r3]
08028b20  4098      ldr          r0, [sp, #256]
08028b22  3d99      ldr          r1, [sp, #244]
08028b24  0668      ldr          r6, [r0]
08028b26  0b96      str          r6, [sp, #44]
08028b28  0023      movs         r3, #0
08028b2a  0b60      str          r3, [r1]
08028b2c  fff70abb  b.w          #-2540  ; -> 0x08028144
08028b30  f5eec07a  vcmpe.f32    s15, #0
08028b34  def80000  ldr.w        r0, [lr]
08028b38  0590      str          r0, [sp, #20]
08028b3a  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028b3e  7ff7afac  ble.w        #-1698  ; -> 0x080284a0
08028b42  dff82082  ldr.w        r8, [pc, #544]  ; [0x08028d64] = 0x2002215c (f32=1.1022466e-19)
08028b46  079f      ldr          r7, [sp, #28]
08028b48  179c      ldr          r4, [sp, #92]
08028b4a  58f82790  ldr.w        r9, [r8, r7, lsl #2]
08028b4e  a145      cmp          r9, r4
08028b50  03dd      ble          #6  ; -> 0x08028b5a
08028b52  2798      ldr          r0, [sp, #156]
08028b54  0028      cmp          r0, #0
08028b56  7ff4a3ac  bne.w        #-1722  ; -> 0x080284a0
08028b5a  049f      ldr          r7, [sp, #16]
08028b5c  089c      ldr          r4, [sp, #32]
08028b5e  481e      subs         r0, r1, #1
08028b60  01f10208  add.w        r8, r1, #2
08028b64  00ea0709  and.w        r9, r0, r7
08028b68  08ea0708  and.w        r8, r8, r7
08028b6c  2746      mov          r7, r4
08028b6e  34f91940  ldrsh.w      r4, [r4, r9, lsl #1]
08028b72  37f91870  ldrsh.w      r7, [r7, r8, lsl #1]
08028b76  05ee107a  vmov         s10, r7
08028b7a  0a9f      ldr          r7, [sp, #40]
08028b7c  0aee104a  vmov         s20, r4
08028b80  3c46      mov          r4, r7
08028b82  37f91970  ldrsh.w      r7, [r7, r9, lsl #1]
08028b86  34f91840  ldrsh.w      r4, [r4, r8, lsl #1]
08028b8a  06ee104a  vmov         s12, r4
08028b8e  089c      ldr          r4, [sp, #32]
08028b90  0fee107a  vmov         s30, r7
08028b94  019f      ldr          r7, [sp, #4]
08028b96  34f91100  ldrsh.w      r0, [r4, r1, lsl #1]
08028b9a  97ed001a  vldr         s2, [r7]
08028b9e  049f      ldr          r7, [sp, #16]
08028ba0  00ee900a  vmov         s1, r0
08028ba4  481c      adds         r0, r1, #1
08028ba6  00ea0709  and.w        r9, r0, r7
08028baa  f8eeca5a  vcvt.f32.s32 s11, s20
08028bae  34f91980  ldrsh.w      r8, [r4, r9, lsl #1]
08028bb2  0a9c      ldr          r4, [sp, #40]
08028bb4  0598      ldr          r0, [sp, #20]
08028bb6  34f91170  ldrsh.w      r7, [r4, r1, lsl #1]
08028bba  34f91940  ldrsh.w      r4, [r4, r9, lsl #1]
08028bbe  dff8cc91  ldr.w        r9, [pc, #460]  ; [0x08028d8c] = 0x20021e88 (f32=1.10215303e-19)
08028bc2  f8eec54a  vcvt.f32.s32 s9, s10
08028bc6  b8eee07a  vcvt.f32.s32 s14, s1
08028bca  34eea5aa  vadd.f32     s20, s9, s11
08028bce  05ee108a  vmov         s10, r8
08028bd2  f8eecf3a  vcvt.f32.s32 s7, s30
08028bd6  b8eec55a  vcvt.f32.s32 s10, s10
08028bda  7aee474a  vsub.f32     s9, s20, s14
08028bde  b8eec6fa  vcvt.f32.s32 s30, s12
08028be2  06ee907a  vmov         s13, r7
08028be6  35ee85aa  vadd.f32     s20, s11, s10
08028bea  3fee236a  vadd.f32     s12, s30, s7
08028bee  74eec54a  vsub.f32     s9, s9, s10
08028bf2  f8eee66a  vcvt.f32.s32 s13, s13
08028bf6  75ee655a  vsub.f32     s11, s10, s11
08028bfa  05ee104a  vmov         s10, r4
08028bfe  f6ee000a  vmov.f32     s1, #5.000000e-01
08028c02  21ee20fa  vmul.f32     s30, s2, s1
08028c06  aaee207a  vfma.f32     s14, s20, s1
08028c0a  09eb8208  add.w        r8, r9, r2, lsl #2
08028c0e  b8eec5aa  vcvt.f32.s32 s20, s10
08028c12  36ee666a  vsub.f32     s12, s12, s13
08028c16  e4ee8f5a  vfma.f32     s11, s9, s30
08028c1a  36ee4a6a  vsub.f32     s12, s12, s20
08028c1e  7aee634a  vsub.f32     s9, s20, s7
08028c22  73ee8a3a  vadd.f32     s7, s7, s20
08028c26  e6ee0f4a  vfma.f32     s9, s12, s30
08028c2a  e3eea06a  vfma.f32     s13, s7, s1
08028c2e  a5ee817a  vfma.f32     s14, s11, s2
08028c32  e4ee816a  vfma.f32     s13, s9, s2
08028c36  98ed001a  vldr         s2, [r8]
08028c3a  27ee277a  vmul.f32     s14, s14, s15
08028c3e  7beec10a  vsub.f32     s1, s23, s2
08028c42  66eea76a  vmul.f32     s13, s13, s15
08028c46  20ee876a  vmul.f32     s12, s1, s14
08028c4a  61ee075a  vmul.f32     s11, s2, s14
08028c4e  a1ee266a  vfma.f32     s12, s2, s13
08028c52  e0eea65a  vfma.f32     s11, s1, s13
08028c56  33ee063a  vadd.f32     s6, s6, s12
08028c5a  72eea52a  vadd.f32     s5, s5, s11
08028c5e  0028      cmp          r0, #0
08028c60  7ff422ac  bne.w        #-1980  ; -> 0x080284a8
08028c64  0099      ldr          r1, [sp]
08028c66  9fed465a  vldr         s10, [pc, #280]  ; [0x08028d80] = 0xb22bcc77 (f32=-9.99999994e-09)
08028c6a  0b68      ldr          r3, [r1]
08028c6c  4bed01da  vstr         s27, [r11, #-4]
08028c70  91e4      b            #-1758  ; -> 0x08028596
08028c72  b5eec0aa  vcmpe.f32    s20, #0
08028c76  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028c7a  44bf      itt          mi
08028c7c  3aee2baa  vaddmi.f32   s20, s20, s23
08028c80  03f1ff33  addmi.w      r3, r3, #4294967295
08028c84  84ed00aa  vstr         s20, [r4]
08028c88  ade4      b            #-1702  ; -> 0x080285e6
08028c8a  1aee100a  vmov         r0, s20
08028c8e  08f2ed27  addw         r7, r8, #749
08028c92  8742      cmp          r7, r0
08028c94  14db      blt          #40  ; -> 0x08028cc0
08028c96  0d9f      ldr          r7, [sp, #52]
08028c98  97b9      cbnz         r7, #36  ; -> 0x08028cc0
08028c9a  022c      cmp          r4, #2
08028c9c  10d1      bne          #32  ; -> 0x08028cc0
08028c9e  dfed390a  vldr         s1, [pc, #228]  ; [0x08028d84] = 0x437a0000 (f32=250)
08028ca2  f4ee608a  vcmp.f32     s17, s1
08028ca6  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028caa  09d1      bne          #18  ; -> 0x08028cc0
08028cac  f4ee6cfa  vcmp.f32     s31, s25
08028cb0  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028cb4  04d0      beq          #8  ; -> 0x08028cc0
08028cb6  029f      ldr          r7, [sp, #8]
08028cb8  3868      ldr          r0, [r7]
08028cba  0028      cmp          r0, #0
08028cbc  3ff430ac  beq.w        #-1952  ; -> 0x08028520
08028cc0  9ded227a  vldr         s14, [sp, #136]
08028cc4  b4eec57a  vcmpe.f32    s14, s10
08028cc8  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028ccc  7ff72aac  ble.w        #-1964  ; -> 0x08028524
08028cd0  dded2e6a  vldr         s13, [sp, #184]
08028cd4  f4eec56a  vcmpe.f32    s13, s10
08028cd8  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028cdc  7ff522ac  bpl.w        #-1980  ; -> 0x08028524
08028ce0  0d98      ldr          r0, [sp, #52]
08028ce2  0028      cmp          r0, #0
08028ce4  7ff41eac  bne.w        #-1988  ; -> 0x08028524
08028ce8  012c      cmp          r4, #1
08028cea  7ff41bac  bne.w        #-1994  ; -> 0x08028524
08028cee  f4ee6cfa  vcmp.f32     s31, s25
08028cf2  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028cf6  3ff415ac  beq.w        #-2006  ; -> 0x08028524
08028cfa  0298      ldr          r0, [sp, #8]
08028cfc  0768      ldr          r7, [r0]
08028cfe  2998      ldr          r0, [sp, #164]
08028d00  002f      cmp          r7, #0
08028d02  08bf      it           eq
08028d04  0420      moveq        r0, #4
08028d06  2990      str          r0, [sp, #164]
08028d08  0ce4      b            #-2024  ; -> 0x08028524
08028d0a  b0ee4e7a  vmov.f32     s14, s28
08028d0e  fff7b0bb  b.w          #-2208  ; -> 0x08028472
08028d12  f5eec06a  vcmpe.f32    s13, #0
08028d16  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028d1a  00f16581  bmi.w        #714  ; -> 0x08028fe8
08028d1e  c9ed006a  vstr         s13, [r9]
08028d22  38e4      b            #-1936  ; -> 0x08028596
08028d24  dfed183a  vldr         s7, [pc, #96]  ; [0x08028d88] = 0x322bcc77 (f32=9.99999994e-09)
08028d28  0024      movs         r4, #0
08028d2a  32ee635a  vsub.f32     s10, s4, s7
08028d2e  fff7ebbb  b.w          #-2090  ; -> 0x08028508
08028d32  f4eeeb7a  vcmpe.f32    s15, s23
08028d36  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028d3a  03db      blt          #6  ; -> 0x08028d44
08028d3c  0598      ldr          r0, [sp, #20]
08028d3e  0328      cmp          r0, #3
08028d40  00f0ff82  beq.w        #1534  ; -> 0x08029342
08028d44  3bee275a  vadd.f32     s10, s22, s15
08028d48  b0ee4b2a  vmov.f32     s4, s22
08028d4c  0324      movs         r4, #3
08028d4e  fff7dbbb  b.w          #-2122  ; -> 0x08028508
08028d9c  149a      ldr          r2, [sp, #80]
08028d9e  0d99      ldr          r1, [sp, #52]
08028da0  82ed004a  vstr         s8, [r2]
08028da4  489a      ldr          r2, [sp, #288]
08028da6  299d      ldr          r5, [sp, #164]
08028da8  1160      str          r1, [r2]
08028daa  479a      ldr          r2, [sp, #284]
08028dac  119c      ldr          r4, [sp, #68]
08028dae  1560      str          r5, [r2]
08028db0  469a      ldr          r2, [sp, #280]
08028db2  ddf824b0  ldr.w        r11, [sp, #36]
08028db6  169b      ldr          r3, [sp, #88]
08028db8  dded4a1a  vldr         s3, [sp, #296]
08028dbc  c4f800b0  str.w        r11, [r4]
08028dc0  b0ee42aa  vmov.f32     s20, s4
08028dc4  9ded492a  vldr         s4, [sp, #292]
08028dc8  1360      str          r3, [r2]
08028dca  3898      ldr          r0, [sp, #224]
08028dcc  1fed10ba  vldr         s22, [pc, #-64]  ; [0x08028d90] = 0x3f7d70a4 (f32=0.99000001)
08028dd0  90ed00ca  vldr         s24, [r0]
08028dd4  dded4b7a  vldr         s15, [sp, #300]
08028dd8  339f      ldr          r7, [sp, #204]
08028dda  ecee0b7a  vfma.f32     s15, s24, s22
08028dde  3968      ldr          r1, [r7]
08028de0  b1f57a7f  cmp.w        r1, #1000
08028de4  c0ed007a  vstr         s15, [r0]
08028de8  80f20781  bge.w        #526  ; -> 0x08028ffa
08028dec  1fed179a  vldr         s18, [pc, #-92]  ; [0x08028d94] = 0x38000100 (f32=3.05185094e-05)
08028df0  67ee89ea  vmul.f32     s29, s15, s18
08028df4  62eeae2a  vmul.f32     s5, s5, s29
08028df8  23ee2e3a  vmul.f32     s6, s6, s29
08028dfc  209d      ldr          r5, [sp, #128]
08028dfe  219c      ldr          r4, [sp, #132]
08028e00  95ed00da  vldr         s26, [r5]
08028e04  d4ed00ca  vldr         s25, [r4]
08028e08  5fed1dda  vldr         s27, [pc, #-116]  ; [0x08028d98] = 0x3f7fbe77 (f32=0.999000013)
08028e0c  129a      ldr          r2, [sp, #72]
08028e0e  109e      ldr          r6, [sp, #64]
08028e10  92ed001a  vldr         s2, [r2]
08028e14  2c9b      ldr          r3, [sp, #176]
08028e16  b349      ldr          r1, [pc, #716]  ; [0x080290e4] = 0x20021de4 (f32=1.10213183e-19)
08028e18  706a      ldr          r0, [r6, #36]
08028e1a  1b68      ldr          r3, [r3]
08028e1c  0c68      ldr          r4, [r1]
08028e1e  219f      ldr          r7, [sp, #132]
08028e20  b5ee40da  vcmp.f32     s26, #0
08028e24  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e28  6ceeadaa  vmul.f32     s21, s25, s27
08028e2c  1cbf      itt          ne
08028e2e  9fedaeda  vldrne       s26, [pc, #696]  ; [0x080290e8] = 0x3a83126f (f32=0.00100000005)
08028e32  7aee8daa  vaddne.f32   s21, s21, s26
08028e36  bfee000a  vmov.f32     s0, #-1.000000e+00
08028e3a  22eeaa8a  vmul.f32     s16, s5, s21
08028e3e  63ee2a9a  vmul.f32     s19, s6, s21
08028e42  b4eeeb8a  vcmpe.f32    s16, s23
08028e46  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e4a  f4eeeb9a  vcmpe.f32    s19, s23
08028e4e  88bf      it           hi
08028e50  b0ee6b8a  vmovhi.f32   s16, s23
08028e54  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e58  88bf      it           hi
08028e5a  f0ee6b9a  vmovhi.f32   s19, s23
08028e5e  b4eec08a  vcmpe.f32    s16, s0
08028e62  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e66  f4eec09a  vcmpe.f32    s19, s0
08028e6a  b8bf      it           lt
08028e6c  b0ee408a  vmovlt.f32   s16, s0
08028e70  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e74  b8bf      it           lt
08028e76  f0ee409a  vmovlt.f32   s19, s0
08028e7a  38ee424a  vsub.f32     s8, s16, s4
08028e7e  79eee13a  vsub.f32     s7, s19, s3
08028e82  f0ee426a  vmov.f32     s13, s4
08028e86  b0ee617a  vmov.f32     s14, s3
08028e8a  a1ee237a  vfma.f32     s14, s2, s7
08028e8e  0028      cmp          r0, #0
08028e90  e4ee016a  vfma.f32     s13, s8, s2
08028e94  08bf      it           eq
08028e96  f0ee471a  vmoveq.f32   s3, s14
08028e9a  08bf      it           eq
08028e9c  b0ee662a  vmoveq.f32   s4, s13
08028ea0  a342      cmp          r3, r4
08028ea2  c7ed00aa  vstr         s21, [r7]
08028ea6  b0ee47fa  vmov.f32     s30, s14
08028eaa  23d1      bne          #70  ; -> 0x08028ef4
08028eac  8f4d      ldr          r5, [pc, #572]  ; [0x080290ec] = 0x20024134 (f32=1.10330022e-19)
08028eae  904e      ldr          r6, [pc, #576]  ; [0x080290f0] = 0x2002412c (f32=1.10329919e-19)
08028eb0  9048      ldr          r0, [pc, #576]  ; [0x080290f4] = 0x20024138 (f32=1.10330074e-19)
08028eb2  914a      ldr          r2, [pc, #580]  ; [0x080290f8] = 0x20024130 (f32=1.1032997e-19)
08028eb4  d5ed000a  vldr         s1, [r5]
08028eb8  d6ed005a  vldr         s11, [r6]
08028ebc  dfed8f4a  vldr         s9, [pc, #572]  ; [0x080290fc] = 0x3f7f3b64 (f32=0.996999979)
08028ec0  90ed005a  vldr         s10, [r0]
08028ec4  92ed006a  vldr         s12, [r2]
08028ec8  9fed8dca  vldr         s24, [pc, #564]  ; [0x08029100] = 0x3f333333 (f32=0.699999988)
08028ecc  d0eea45a  vfnms.f32    s11, s1, s9
08028ed0  95ee246a  vfnms.f32    s12, s10, s9
08028ed4  22ee0cba  vmul.f32     s22, s4, s24
08028ed8  61ee8c7a  vmul.f32     s15, s3, s24
08028edc  35ee8b2a  vadd.f32     s4, s11, s22
08028ee0  76ee271a  vadd.f32     s3, s12, s15
08028ee4  86ed00ba  vstr         s22, [r6]
08028ee8  85ed002a  vstr         s4, [r5]
08028eec  c0ed001a  vstr         s3, [r0]
08028ef0  c2ed007a  vstr         s15, [r2]
08028ef4  2b9f      ldr          r7, [sp, #172]
08028ef6  3968      ldr          r1, [r7]
08028ef8  0129      cmp          r1, #1
08028efa  00f06d81  beq.w        #730  ; -> 0x080291d8
08028efe  2898      ldr          r0, [sp, #160]
08028f00  0468      ldr          r4, [r0]
08028f02  012c      cmp          r4, #1
08028f04  00f0b881  beq.w        #880  ; -> 0x08029278
08028f08  2d9e      ldr          r6, [sp, #180]
08028f0a  3368      ldr          r3, [r6]
08028f0c  012b      cmp          r3, #1
08028f0e  04d1      bne          #8  ; -> 0x08028f1a
08028f10  7c4f      ldr          r7, [pc, #496]  ; [0x08029104] = 0x200213b4 (f32=1.10179475e-19)
08028f12  3868      ldr          r0, [r7]
08028f14  0028      cmp          r0, #0
08028f16  41f09784  bne.w        #6446  ; -> 0x0802a848
08028f1a  109f      ldr          r7, [sp, #64]
08028f1c  139b      ldr          r3, [sp, #76]
08028f1e  7969      ldr          r1, [r7, #20]
08028f20  d3ed00da  vldr         s27, [r3]
08028f24  b5eec0fa  vcmpe.f32    s30, #0
08028f28  f0eee6ca  vabs.f32     s25, s13
08028f2c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028f30  4cbf      ite          mi
08028f32  3ceecf6a  vsubmi.f32   s12, s25, s30
08028f36  3fee2c6a  vaddpl.f32   s12, s30, s25
08028f3a  0029      cmp          r1, #0
08028f3c  40f09680  bne.w        #300  ; -> 0x0802906c
08028f40  b4eeed6a  vcmpe.f32    s12, s27
08028f44  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028f48  40f3f181  ble.w        #994  ; -> 0x0802932e
08028f4c  dfed6e4a  vldr         s9, [pc, #440]  ; [0x08029108] = 0x3f666666 (f32=0.899999976)
08028f50  dfed6e5a  vldr         s11, [pc, #440]  ; [0x0802910c] = 0x3dcccccd (f32=0.100000001)
08028f54  2deea45a  vmul.f32     s10, s27, s9
08028f58  1946      mov          r1, r3
08028f5a  a6ee255a  vfma.f32     s10, s12, s11
08028f5e  b4eeeb5a  vcmpe.f32    s10, s23
08028f62  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028f66  81ed005a  vstr         s10, [r1]
08028f6a  40f3a380  ble.w        #326  ; -> 0x080290b4
08028f6e  139d      ldr          r5, [sp, #76]
08028f70  b7ee005a  vmov.f32     s10, #1.000000e+00
08028f74  c5ed00ba  vstr         s23, [r5]
08028f78  3e9c      ldr          r4, [sp, #248]
08028f7a  9fed656a  vldr         s12, [pc, #404]  ; [0x08029110] = 0x46fffe00 (f32=32767)
08028f7e  2768      ldr          r7, [r4]
08028f80  3f9d      ldr          r5, [sp, #252]
08028f82  3b9b      ldr          r3, [sp, #236]
08028f84  2a68      ldr          r2, [r5]
08028f86  2598      ldr          r0, [sp, #148]
08028f88  66ee866a  vmul.f32     s13, s13, s12
08028f8c  2fee06fa  vmul.f32     s30, s30, s12
08028f90  7e1c      adds         r6, r7, #1
08028f92  bdeee6ca  vcvt.s32.f32 s24, s13
08028f96  bdeecfba  vcvt.s32.f32 s22, s30
08028f9a  06f03f01  and          r1, r6, #63
08028f9e  2160      str          r1, [r4]
08028fa0  03eb870c  add.w        r12, r3, r7, lsl #2
08028fa4  449c      ldr          r4, [sp, #272]
08028fa6  359f      ldr          r7, [sp, #212]
08028fa8  459e      ldr          r6, [sp, #276]
08028faa  8ced005a  vstr         s10, [r12]
08028fae  1cee10aa  vmov         r10, s24
08028fb2  1bee101a  vmov         r1, s22
08028fb6  0232      adds         r2, #2
08028fb8  27f804a0  strh.w       r10, [r7, r4]
08028fbc  0230      adds         r0, #2
08028fbe  b953      strh         r1, [r7, r6]
08028fc0  2a60      str          r2, [r5]
08028fc2  2f9d      ldr          r5, [sp, #188]
08028fc4  00b2      sxth         r0, r0
08028fc6  8542      cmp          r5, r0
08028fc8  2590      str          r0, [sp, #148]
08028fca  7ff747ab  ble.w        #-2418  ; -> 0x0802865c
08028fce  129a      ldr          r2, [sp, #72]
08028fd0  504b      ldr          r3, [pc, #320]  ; [0x08029114] = 0x20022154 (f32=1.10224557e-19)
08028fd2  92ed043a  vldr         s6, [r2, #16]
08028fd6  d3ed005a  vldr         s11, [r3]
08028fda  0446      mov          r4, r0
08028fdc  fef76fbe  b.w          #-4898  ; -> 0x08027cbe
08028fe0  b0ee4e9a  vmov.f32     s18, s28
08028fe4  fef78dbf  b.w          #-4326  ; -> 0x08027f02
08028fe8  76eeab5a  vadd.f32     s11, s13, s23
08028fec  411e      subs         r1, r0, #1
08028fee  c9ed005a  vstr         s11, [r9]
08028ff2  ccf80010  str.w        r1, [r12]
08028ff6  fff7ceba  b.w          #-2660  ; -> 0x08028596
08028ffa  b0ee4e3a  vmov.f32     s6, s28
08028ffe  f0ee4e2a  vmov.f32     s5, s28
08029002  fbe6      b            #-522  ; -> 0x08028dfc
08029004  002e      cmp          r6, #0
08029006  3ff47ead  beq.w        #-1284  ; -> 0x08028b06
0802900a  189d      ldr          r5, [sp, #96]
0802900c  109b      ldr          r3, [sp, #64]
0802900e  424c      ldr          r4, [pc, #264]  ; [0x08029118] = 0x20021cbc (f32=1.10209357e-19)
08029010  da68      ldr          r2, [r3, #12]
08029012  2068      ldr          r0, [r4]
08029014  012d      cmp          r5, #1
08029016  7ff457a8  bne.w        #-3922  ; -> 0x080280c8
0802901a  1e9e      ldr          r6, [sp, #120]
0802901c  3168      ldr          r1, [r6]
0802901e  0029      cmp          r1, #0
08029020  7ff452a8  bne.w        #-3932  ; -> 0x080280c8
08029024  012a      cmp          r2, #1
08029026  00f03e82  beq.w        #1148  ; -> 0x080294a6
0802902a  3c4e      ldr          r6, [pc, #240]  ; [0x0802911c] = 0x20021ec8 (f32=1.1021613e-19)
0802902c  409a      ldr          r2, [sp, #256]
0802902e  3c49      ldr          r1, [pc, #240]  ; [0x08029120] = 0x20021f70 (f32=1.10218301e-19)
08029030  1468      ldr          r4, [r2]
08029032  56f82730  ldr.w        r3, [r6, r7, lsl #2]
08029036  41f82730  str.w        r3, [r1, r7, lsl #2]
0802903a  0137      adds         r7, #1
0802903c  a742      cmp          r7, r4
0802903e  239e      ldr          r6, [sp, #140]
08029040  0b94      str          r4, [sp, #44]
08029042  269c      ldr          r4, [sp, #152]
08029044  3749      ldr          r1, [pc, #220]  ; [0x08029124] = 0x20021e64 (f32=1.10214837e-19)
08029046  3760      str          r7, [r6]
08029048  2368      ldr          r3, [r4]
0802904a  249d      ldr          r5, [sp, #144]
0802904c  364a      ldr          r2, [pc, #216]  ; [0x08029128] = 0xc7d90380 (f32=-111111)
0802904e  85ed00ea  vstr         s28, [r5]
08029052  08bf      it           eq
08029054  0027      moveq        r7, #0
08029056  01eb8708  add.w        r8, r1, r7, lsl #2
0802905a  03f10103  add.w        r3, r3, #1
0802905e  08bf      it           eq
08029060  3760      streq        r7, [r6]
08029062  c8f80020  str.w        r2, [r8]
08029066  2360      str          r3, [r4]
08029068  fff747b8  b.w          #-3954  ; -> 0x080280fa
0802906c  239a      ldr          r2, [sp, #140]
0802906e  2f4d      ldr          r5, [pc, #188]  ; [0x0802912c] = 0x20021e84 (f32=1.10215251e-19)
08029070  2f48      ldr          r0, [pc, #188]  ; [0x08029130] = 0x2002209c (f32=1.10222179e-19)
08029072  2a4c      ldr          r4, [pc, #168]  ; [0x0802911c] = 0x20021ec8 (f32=1.1021613e-19)
08029074  1668      ldr          r6, [r2]
08029076  90ed00da  vldr         s26, [r0]
0802907a  d5ed00aa  vldr         s21, [r5]
0802907e  54f82670  ldr.w        r7, [r4, r6, lsl #2]
08029082  9fed2c8a  vldr         s16, [pc, #176]  ; [0x08029134] = 0x3d75c28f (f32=0.0599999987)
08029086  9fed200a  vldr         s0, [pc, #128]  ; [0x08029108] = 0x3f666666 (f32=0.899999976)
0802908a  1399      ldr          r1, [sp, #76]
0802908c  09ee907a  vmov         s19, r7
08029090  2aee8d4a  vmul.f32     s8, s21, s26
08029094  f8eee93a  vcvt.f32.s32 s7, s19
08029098  24ee081a  vmul.f32     s2, s8, s16
0802909c  2dee805a  vmul.f32     s10, s27, s0
080290a0  a3ee815a  vfma.f32     s10, s7, s2
080290a4  b4eeeb5a  vcmpe.f32    s10, s23
080290a8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080290ac  81ed005a  vstr         s10, [r1]
080290b0  3ff75daf  bgt.w        #-326  ; -> 0x08028f6e
080290b4  b5eec05a  vcmpe.f32    s10, #0
080290b8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080290bc  7ff55caf  bpl.w        #-328  ; -> 0x08028f78
080290c0  139b      ldr          r3, [sp, #76]
080290c2  b0ee4e5a  vmov.f32     s10, s28
080290c6  83ed00ea  vstr         s28, [r3]
080290ca  55e7      b            #-342  ; -> 0x08028f78
080290cc  b5eec07a  vcmpe.f32    s14, #0
080290d0  f1ee10fa  vmrs         APSR_nzcv, fpscr
080290d4  40f3ea81  ble.w        #980  ; -> 0x080294ac
080290d8  b0ee4fca  vmov.f32     s24, s30
080290dc  cded19da  vstr         s27, [sp, #100]
080290e0  fef72cbf  b.w          #-4520  ; -> 0x08027f3c
08029138  f6ee004a  vmov.f32     s9, #5.000000e-01
0802913c  109f      ldr          r7, [sp, #64]
0802913e  b4eee40a  vcmpe.f32    s0, s9
08029142  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029146  bc69      ldr          r4, [r7, #24]
08029148  40f16d81  bpl.w        #730  ; -> 0x08029426
0802914c  012c      cmp          r4, #1
0802914e  3ef464af  beq.w        #-4408  ; -> 0x0802801a
08029152  1499      ldr          r1, [sp, #80]
08029154  91ed005a  vldr         s10, [r1]
08029158  002a      cmp          r2, #0
0802915a  7ef466af  bne.w        #-4404  ; -> 0x0802802a
0802915e  b44b      ldr          r3, [pc, #720]  ; [0x08029430] = 0x20021c68 (f32=1.10208272e-19)
08029160  b44d      ldr          r5, [pc, #720]  ; [0x08029434] = 0x20022148 (f32=1.10224402e-19)
08029162  b548      ldr          r0, [pc, #724]  ; [0x08029438] = 0x20022144 (f32=1.1022435e-19)
08029164  d968      ldr          r1, [r3, #12]
08029166  95ed006a  vldr         s12, [r5]
0802916a  d0ed007a  vldr         s15, [r0]
0802916e  b1f5fa6f  cmp.w        r1, #2000
08029172  b4bf      ite          lt
08029174  36ee676a  vsublt.f32   s12, s12, s15
08029178  36ee276a  vaddge.f32   s12, s12, s15
0802917c  af49      ldr          r1, [pc, #700]  ; [0x0802943c] = 0x2002214c (f32=1.10224453e-19)
0802917e  bdeec61a  vcvt.s32.f32 s2, s12
08029182  0b68      ldr          r3, [r1]
08029184  b8eec18a  vcvt.f32.s32 s16, s2
08029188  11ee106a  vmov         r6, s2
0802918c  76ee48ca  vsub.f32     s25, s12, s16
08029190  3344      add          r3, r6
08029192  002e      cmp          r6, #0
08029194  c5ed00ca  vstr         s25, [r5]
08029198  0b60      str          r3, [r1]
0802919a  c1f26e84  blt.w        #6364  ; -> 0x0802aa7a
0802919e  a84f      ldr          r7, [pc, #672]  ; [0x08029440] = 0x20021de4 (f32=1.10213183e-19)
080291a0  0c9d      ldr          r5, [sp, #48]
080291a2  3a68      ldr          r2, [r7]
080291a4  379c      ldr          r4, [sp, #220]
080291a6  0dee903a  vmov         s27, r3
080291aa  05eb8209  add.w        r9, r5, r2, lsl #2
080291ae  d9ed00ea  vldr         s29, [r9]
080291b2  2768      ldr          r7, [r4]
080291b4  b8eeed4a  vcvt.f32.s32 s8, s27
080291b8  b4eeee4a  vcmpe.f32    s8, s29
080291bc  f1ee10fa  vmrs         APSR_nzcv, fpscr
080291c0  01f33584  bgt.w        #6250  ; -> 0x0802aa2e
080291c4  002b      cmp          r3, #0
080291c6  80f29e80  bge.w        #316  ; -> 0x08029306
080291ca  3eeeebfa  vsub.f32     s30, s29, s23
080291ce  fdeecf4a  vcvt.s32.f32 s9, s30
080291d2  c1ed004a  vstr         s9, [r1]
080291d6  96e0      b            #300  ; -> 0x08029306
080291d8  9a4c      ldr          r4, [pc, #616]  ; [0x08029444] = 0x2002210c (f32=1.10223626e-19)
080291da  9b4d      ldr          r5, [pc, #620]  ; [0x08029448] = 0x20021ca4 (f32=1.10209047e-19)
080291dc  2068      ldr          r0, [r4]
080291de  2968      ldr          r1, [r5]
080291e0  9a4a      ldr          r2, [pc, #616]  ; [0x0802944c] = 0x200213b4 (f32=1.10179475e-19)
080291e2  0026      movs         r6, #0
080291e4  8842      cmp          r0, r1
080291e6  1660      str          r6, [r2]
080291e8  15da      bge          #42  ; -> 0x08029216
080291ea  dfed998a  vldr         s17, [pc, #612]  ; [0x08029450] = 0x46fffe00 (f32=32767)
080291ee  994f      ldr          r7, [pc, #612]  ; [0x08029454] = 0x200220fc (f32=1.10223419e-19)
080291f0  994a      ldr          r2, [pc, #612]  ; [0x08029458] = 0x20022100 (f32=1.10223471e-19)
080291f2  3e68      ldr          r6, [r7]
080291f4  1268      ldr          r2, [r2]
080291f6  22ee283a  vmul.f32     s6, s4, s17
080291fa  61eea82a  vmul.f32     s5, s3, s17
080291fe  bdeec39a  vcvt.s32.f32 s18, s6
08029202  fdeee2ea  vcvt.s32.f32 s29, s5
08029206  19ee10aa  vmov         r10, s18
0802920a  1eee907a  vmov         r7, s29
0802920e  26f810a0  strh.w       r10, [r6, r0, lsl #1]
08029212  22f81070  strh.w       r7, [r2, r0, lsl #1]
08029216  914a      ldr          r2, [pc, #580]  ; [0x0802945c] = 0x2002208c (f32=1.10221972e-19)
08029218  914e      ldr          r6, [pc, #580]  ; [0x08029460] = 0x20001290 (f32=1.08481635e-19)
0802921a  1768      ldr          r7, [r2]
0802921c  3268      ldr          r2, [r6]
0802921e  0130      adds         r0, #1
08029220  2060      str          r0, [r4]
08029222  002a      cmp          r2, #0
08029224  40f0e080  bne.w        #448  ; -> 0x080293e8
08029228  bb42      cmp          r3, r7
0802922a  40f3f280  ble.w        #484  ; -> 0x08029412
0802922e  8142      cmp          r1, r0
08029230  40f38e80  ble.w        #284  ; -> 0x08029350
08029234  8b4f      ldr          r7, [pc, #556]  ; [0x08029464] = 0x20021320 (f32=1.10177562e-19)
08029236  3d68      ldr          r5, [r7]
08029238  002d      cmp          r5, #0
0802923a  7ff460ae  bne.w        #-832  ; -> 0x08028efe
0802923e  2b9a      ldr          r2, [sp, #172]
08029240  1068      ldr          r0, [r2]
08029242  0128      cmp          r0, #1
08029244  7ff45bae  bne.w        #-842  ; -> 0x08028efe
08029248  4e9c      ldr          r4, [sp, #312]
0802924a  24b9      cbnz         r4, #8  ; -> 0x08029256
0802924c  864e      ldr          r6, [pc, #536]  ; [0x08029468] = 0x20021cb0 (f32=1.10209202e-19)
0802924e  3368      ldr          r3, [r6]
08029250  002b      cmp          r3, #0
08029252  7ff454ae  bne.w        #-856  ; -> 0x08028efe
08029256  2c99      ldr          r1, [sp, #176]
08029258  804f      ldr          r7, [pc, #512]  ; [0x0802945c] = 0x2002208c (f32=1.10221972e-19)
0802925a  0868      ldr          r0, [r1]
0802925c  3d68      ldr          r5, [r7]
0802925e  4d9a      ldr          r2, [sp, #308]
08029260  a842      cmp          r0, r5
08029262  d4bf      ite          le
08029264  0020      movle        r0, #0
08029266  0120      movgt        r0, #1
08029268  1168      ldr          r1, [r2]
0802926a  fef76ff8  bl           #-7970  ; -> 0x0802734c
0802926e  2898      ldr          r0, [sp, #160]
08029270  0468      ldr          r4, [r0]
08029272  012c      cmp          r4, #1
08029274  7ff448ae  bne.w        #-880  ; -> 0x08028f08
08029278  7c4e      ldr          r6, [pc, #496]  ; [0x0802946c] = 0x20022070 (f32=1.1022161e-19)
0802927a  7d4f      ldr          r7, [pc, #500]  ; [0x08029470] = 0x20021f28 (f32=1.10217371e-19)
0802927c  3168      ldr          r1, [r6]
0802927e  734b      ldr          r3, [pc, #460]  ; [0x0802944c] = 0x200213b4 (f32=1.10179475e-19)
08029280  289a      ldr          r2, [sp, #160]
08029282  2d9e      ldr          r6, [sp, #180]
08029284  57f82150  ldr.w        r5, [r7, r1, lsl #2]
08029288  1d60      str          r5, [r3]
0802928a  0220      movs         r0, #2
0802928c  0024      movs         r4, #0
0802928e  1060      str          r0, [r2]
08029290  3460      str          r4, [r6]
08029292  42e6      b            #-892  ; -> 0x08028f1a
08029294  259d      ldr          r5, [sp, #148]
08029296  002d      cmp          r5, #0
08029298  7ef4c7ae  bne.w        #-4722  ; -> 0x0802802a
0802929c  674e      ldr          r6, [pc, #412]  ; [0x0802943c] = 0x2002214c (f32=1.10224453e-19)
0802929e  644f      ldr          r7, [pc, #400]  ; [0x08029430] = 0x20021c68 (f32=1.10208272e-19)
080292a0  d6ed000a  vldr         s1, [r6]
080292a4  fc68      ldr          r4, [r7, #12]
080292a6  6648      ldr          r0, [pc, #408]  ; [0x08029440] = 0x20021de4 (f32=1.10213183e-19)
080292a8  0c99      ldr          r1, [sp, #48]
080292aa  0368      ldr          r3, [r0]
080292ac  f8eee02a  vcvt.f32.s32 s5, s1
080292b0  b4f5fa6f  cmp.w        r4, #2000
080292b4  b4bf      ite          lt
080292b6  72eec52a  vsublt.f32   s5, s5, s10
080292ba  72ee852a  vaddge.f32   s5, s5, s10
080292be  01eb8308  add.w        r8, r1, r3, lsl #2
080292c2  bdeee23a  vcvt.s32.f32 s6, s5
080292c6  d8ed006a  vldr         s13, [r8]
080292ca  86ed003a  vstr         s6, [r6]
080292ce  f8eec38a  vcvt.f32.s32 s17, s6
080292d2  f4eee68a  vcmpe.f32    s17, s13
080292d6  f1ee10fa  vmrs         APSR_nzcv, fpscr
080292da  01f3a183  bgt.w        #5954  ; -> 0x0802aa20
080292de  f5eec08a  vcmpe.f32    s17, #0
080292e2  f1ee10fa  vmrs         APSR_nzcv, fpscr
080292e6  05d5      bpl          #10  ; -> 0x080292f4
080292e8  76eeeb9a  vsub.f32     s19, s13, s23
080292ec  fdeee9aa  vcvt.s32.f32 s21, s19
080292f0  c6ed00aa  vstr         s21, [r6]
080292f4  379e      ldr          r6, [sp, #220]
080292f6  012a      cmp          r2, #1
080292f8  3768      ldr          r7, [r6]
080292fa  01f08382  beq.w        #5382  ; -> 0x0802a804
080292fe  1f9c      ldr          r4, [sp, #124]
08029300  249b      ldr          r3, [sp, #144]
08029302  2068      ldr          r0, [r4]
08029304  1860      str          r0, [r3]
08029306  012f      cmp          r7, #1
08029308  7ef494ae  bne.w        #-4824  ; -> 0x08028034
0802930c  249e      ldr          r6, [sp, #144]
0802930e  d6ed000a  vldr         s1, [r6]
08029312  05e0      b            #10  ; -> 0x08029320
08029314  249f      ldr          r7, [sp, #144]
08029316  012a      cmp          r2, #1
08029318  d7ed000a  vldr         s1, [r7]
0802931c  01f07882  beq.w        #5360  ; -> 0x0802a810
08029320  70eeab2a  vadd.f32     s5, s1, s23
08029324  2499      ldr          r1, [sp, #144]
08029326  c1ed002a  vstr         s5, [r1]
0802932a  fef783be  b.w          #-4858  ; -> 0x08028034
0802932e  9fed517a  vldr         s14, [pc, #324]  ; [0x08029474] = 0x3f7fbe77 (f32=0.999000013)
08029332  dfed510a  vldr         s1, [pc, #324]  ; [0x08029478] = 0x3a83126f (f32=0.00100000005)
08029336  2dee875a  vmul.f32     s10, s27, s14
0802933a  1946      mov          r1, r3
0802933c  a6ee205a  vfma.f32     s10, s12, s1
08029340  0de6      b            #-998  ; -> 0x08028f5e
08029342  b0ee4e2a  vmov.f32     s4, s28
08029346  b7ee005a  vmov.f32     s10, #1.000000e+00
0802934a  0224      movs         r4, #2
0802934c  fff7dcb8  b.w          #-3656  ; -> 0x08028508
08029350  4a49      ldr          r1, [pc, #296]  ; [0x0802947c] = 0x20021314 (f32=1.10177407e-19)
08029352  4b4f      ldr          r7, [pc, #300]  ; [0x08029480] = 0x20021c9c (f32=1.10208944e-19)
08029354  0b68      ldr          r3, [r1]
08029356  0093      str          r3, [sp]
08029358  424b      ldr          r3, [pc, #264]  ; [0x08029464] = 0x20021320 (f32=1.10177562e-19)
0802935a  3a60      str          r2, [r7]
0802935c  1a60      str          r2, [r3]
0802935e  2b9b      ldr          r3, [sp, #172]
08029360  1a60      str          r2, [r3]
08029362  009a      ldr          r2, [sp]
08029364  9042      cmp          r0, r2
08029366  c8bf      it           gt
08029368  0860      strgt        r0, [r1]
0802936a  fdf755f9  bl           #-11606  ; -> 0x08026618
0802936e  3068      ldr          r0, [r6]
08029370  3799      ldr          r1, [sp, #220]
08029372  2f4b      ldr          r3, [pc, #188]  ; [0x08029430] = 0x20021c68 (f32=1.10208272e-19)
08029374  0126      movs         r6, #1
08029376  b042      cmp          r0, r6
08029378  08bf      it           eq
0802937a  4248      ldreq        r0, [pc, #264]  ; [0x08029484] = 0x20022074 (f32=1.10221662e-19)
0802937c  0e60      str          r6, [r1]
0802937e  0abf      itet         eq
08029380  0668      ldreq        r6, [r0]
08029382  4148      ldrne        r0, [pc, #260]  ; [0x08029488] = 0x20021328 (f32=1.10177666e-19)
08029384  46f40866  orreq        r6, r6, #2176
08029388  0660      str          r6, [r0]
0802938a  9b46      mov          r11, r3
0802938c  d3e90027  ldrd         r2, r7, [r3]
08029390  1e69      ldr          r6, [r3, #16]
08029392  2968      ldr          r1, [r5]
08029394  dbf81850  ldr.w        r5, [r11, #24]
08029398  0195      str          r5, [sp, #4]
0802939a  0097      str          r7, [sp]
0802939c  06f1640c  add.w        r12, r6, #100
080293a0  019e      ldr          r6, [sp, #4]
080293a2  009d      ldr          r5, [sp]
080293a4  06f16408  add.w        r8, r6, #100
080293a8  384e      ldr          r6, [pc, #224]  ; [0x0802948c] = 0x200220a4 (f32=1.10222282e-19)
080293aa  6432      adds         r2, #100
080293ac  05f16409  add.w        r9, r5, #100
080293b0  d3e90207  ldrd         r0, r7, [r3, #8]
080293b4  c6e90029  strd         r2, r9, [r6]
080293b8  1d4a      ldr          r2, [pc, #116]  ; [0x08029430] = 0x20021c68 (f32=1.10208272e-19)
080293ba  5b69      ldr          r3, [r3, #20]
080293bc  d269      ldr          r2, [r2, #28]
080293be  c6f810c0  str.w        r12, [r6, #16]
080293c2  6430      adds         r0, #100
080293c4  6437      adds         r7, #100
080293c6  6433      adds         r3, #100
080293c8  b060      str          r0, [r6, #8]
080293ca  f760      str          r7, [r6, #12]
080293cc  2c98      ldr          r0, [sp, #176]
080293ce  234f      ldr          r7, [pc, #140]  ; [0x0802945c] = 0x2002208c (f32=1.10221972e-19)
080293d0  7361      str          r3, [r6, #20]
080293d2  6432      adds         r2, #100
080293d4  c6f81880  str.w        r8, [r6, #24]
080293d8  f261      str          r2, [r6, #28]
080293da  4f9e      ldr          r6, [sp, #316]
080293dc  0368      ldr          r3, [r0]
080293de  3f68      ldr          r7, [r7]
080293e0  2068      ldr          r0, [r4]
080293e2  4ff4f075  mov.w        r5, #480
080293e6  3560      str          r5, [r6]
080293e8  bb42      cmp          r3, r7
080293ea  12dd      ble          #36  ; -> 0x08029412
080293ec  8842      cmp          r0, r1
080293ee  fff621af  blt.w        #-446  ; -> 0x08029234
080293f2  4d99      ldr          r1, [sp, #308]
080293f4  264c      ldr          r4, [pc, #152]  ; [0x08029490] = 0x200213b8 (f32=1.10179527e-19)
080293f6  274e      ldr          r6, [pc, #156]  ; [0x08029494] = 0x2002133c (f32=1.10177924e-19)
080293f8  0968      ldr          r1, [r1]
080293fa  9f42      cmp          r7, r3
080293fc  a8bf      it           ge
080293fe  0020      movge        r0, #0
08029400  4ff4fa63  mov.w        r3, #2000
08029404  b8bf      it           lt
08029406  0120      movlt        r0, #1
08029408  2360      str          r3, [r4]
0802940a  3360      str          r3, [r6]
0802940c  fdf79eff  bl           #-8388  ; -> 0x0802734c
08029410  10e7      b            #-480  ; -> 0x08029234
08029412  214d      ldr          r5, [pc, #132]  ; [0x08029498] = 0x2002215c (f32=1.1022466e-19)
08029414  55f82320  ldr.w        r2, [r5, r3, lsl #2]
08029418  8242      cmp          r2, r0
0802941a  e7dc      bgt          #-50  ; -> 0x080293ec
0802941c  581e      subs         r0, r3, #1
0802941e  55f82000  ldr.w        r0, [r5, r0, lsl #2]
08029422  2060      str          r0, [r4]
08029424  e2e7      b            #-60  ; -> 0x080293ec
08029426  022c      cmp          r4, #2
08029428  7ef4f7ad  bne.w        #-5138  ; -> 0x0802801a
0802942c  91e6      b            #-734  ; -> 0x08029152
0802949c  094b      ldr          r3, [pc, #36]  ; [0x080294c4] = 0x20021de4 (f32=1.10213183e-19)
0802949e  1c68      ldr          r4, [r3]
080294a0  8442      cmp          r4, r0
080294a2  3ef414ae  beq.w        #-5080  ; -> 0x080280ce
080294a6  074b      ldr          r3, [pc, #28]  ; [0x080294c4] = 0x20021de4 (f32=1.10213183e-19)
080294a8  1860      str          r0, [r3]
080294aa  bee5      b            #-1156  ; -> 0x0802902a
080294ac  b1ee470a  vneg.f32     s0, s14
080294b0  fdeec04a  vcvt.s32.f32 s9, s0
080294b4  b8eee45a  vcvt.f32.s32 s10, s9
080294b8  cded194a  vstr         s9, [sp, #100]
080294bc  30ee45ca  vsub.f32     s24, s0, s10
080294c0  fef73cbd  b.w          #-5512  ; -> 0x08027f3c
080294cc  002e      cmp          r6, #0
080294ce  7ff7c5a8  ble.w        #-3702  ; -> 0x0802865c
080294d2  5fed03ba  vldr         s23, [pc, #-12]  ; [0x080294c8] = 0x3c23d70a (f32=0.00999999978)
080294d6  ca48      ldr          r0, [pc, #808]  ; [0x08029800] = 0x20022038 (f32=1.10220886e-19)
080294d8  ca4f      ldr          r7, [pc, #808]  ; [0x08029804] = 0x20021cb8 (f32=1.10209306e-19)
080294da  cb4c      ldr          r4, [pc, #812]  ; [0x08029808] = 0x20022114 (f32=1.10223729e-19)
080294dc  cb4e      ldr          r6, [pc, #812]  ; [0x0802980c] = 0x200220e8 (f32=1.10223161e-19)
080294de  cc49      ldr          r1, [pc, #816]  ; [0x08029810] = 0x200220ec (f32=1.10223212e-19)
080294e0  cc4d      ldr          r5, [pc, #816]  ; [0x08029814] = 0x200012a8 (f32=1.08481946e-19)
080294e2  cd4a      ldr          r2, [pc, #820]  ; [0x08029818] = 0x20022108 (f32=1.10223574e-19)
080294e4  cd4b      ldr          r3, [pc, #820]  ; [0x0802981c] = 0x200213a8 (f32=1.1017932e-19)
080294e6  2690      str          r0, [sp, #152]
080294e8  2397      str          r7, [sp, #140]
080294ea  cd48      ldr          r0, [pc, #820]  ; [0x08029820] = 0x200220e0 (f32=1.10223057e-19)
080294ec  cd4f      ldr          r7, [pc, #820]  ; [0x08029824] = 0x20022084 (f32=1.10221868e-19)
080294ee  2c94      str          r4, [sp, #176]
080294f0  cd4c      ldr          r4, [pc, #820]  ; [0x08029828] = 0x20021e20 (f32=1.10213958e-19)
080294f2  2096      str          r6, [sp, #128]
080294f4  1e91      str          r1, [sp, #120]
080294f6  cd4e      ldr          r6, [pc, #820]  ; [0x0802982c] = 0x200220d4 (f32=1.10222902e-19)
080294f8  cd49      ldr          r1, [pc, #820]  ; [0x08029830] = 0x200220f8 (f32=1.10223368e-19)
080294fa  1095      str          r5, [sp, #64]
080294fc  2b92      str          r2, [sp, #172]
080294fe  cd4d      ldr          r5, [pc, #820]  ; [0x08029834] = 0x20022124 (f32=1.10223936e-19)
08029500  cd4a      ldr          r2, [pc, #820]  ; [0x08029838] = 0x20001054 (f32=1.08474242e-19)
08029502  2893      str          r3, [sp, #160]
08029504  3c90      str          r0, [sp, #240]
08029506  cd4b      ldr          r3, [pc, #820]  ; [0x0802983c] = 0x20021e18 (f32=1.10213855e-19)
08029508  cd48      ldr          r0, [pc, #820]  ; [0x08029840] = 0x20021f4c (f32=1.10217836e-19)
0802950a  1597      str          r7, [sp, #84]
0802950c  1f94      str          r4, [sp, #124]
0802950e  cd4f      ldr          r7, [pc, #820]  ; [0x08029844] = 0x20022088 (f32=1.1022192e-19)
08029510  cd4c      ldr          r4, [pc, #820]  ; [0x08029848] = 0x20021dc0 (f32=1.10212718e-19)
08029512  3996      str          r6, [sp, #228]
08029514  21ee2bea  vmul.f32     s28, s2, s23
08029518  1191      str          r1, [sp, #68]
0802951a  3a95      str          r5, [sp, #232]
0802951c  3792      str          r2, [sp, #220]
0802951e  2493      str          r3, [sp, #144]
08029520  0690      str          r0, [sp, #24]
08029522  4097      str          r7, [sp, #256]
08029524  c94e      ldr          r6, [pc, #804]  ; [0x0802984c] = 0x20022158 (f32=1.10224608e-19)
08029526  ca49      ldr          r1, [pc, #808]  ; [0x08029850] = 0x20022064 (f32=1.10221455e-19)
08029528  ca4d      ldr          r5, [pc, #808]  ; [0x08029854] = 0x20022090 (f32=1.10222023e-19)
0802952a  cb4a      ldr          r2, [pc, #812]  ; [0x08029858] = 0x200213c0 (f32=1.1017963e-19)
0802952c  cb4b      ldr          r3, [pc, #812]  ; [0x0802985c] = 0x200220c8 (f32=1.10222747e-19)
0802952e  3e94      str          r4, [sp, #248]
08029530  cb48      ldr          r0, [pc, #812]  ; [0x08029860] = 0x20021cc0 (f32=1.10209409e-19)
08029532  cc4f      ldr          r7, [pc, #816]  ; [0x08029864] = 0x20021c88 (f32=1.10208685e-19)
08029534  9fedcc5a  vldr         s10, [pc, #816]  ; [0x08029868] = 0x00000000 (f32=0)
08029538  3d96      str          r6, [sp, #244]
0802953a  0024      movs         r4, #0
0802953c  3891      str          r1, [sp, #224]
0802953e  2195      str          r5, [sp, #132]
08029540  2d92      str          r2, [sp, #180]
08029542  1393      str          r3, [sp, #76]
08029544  3b90      str          r0, [sp, #236]
08029546  3f97      str          r7, [sp, #252]
08029548  8ded45ea  vstr         s28, [sp, #276]
0802954c  cded46ba  vstr         s23, [sp, #280]
08029550  1c94      str          r4, [sp, #112]
08029552  f7ee006a  vmov.f32     s13, #1.000000e+00
08029556  f0ee4aea  vmov.f32     s29, s20
0802955a  b0ee670a  vmov.f32     s0, s15
0802955e  3498      ldr          r0, [sp, #208]
08029560  dfedc28a  vldr         s17, [pc, #776]  ; [0x0802986c] = 0x38000100 (f32=3.05185094e-05)
08029564  30f91460  ldrsh.w      r6, [r0, r4, lsl #1]
08029568  399d      ldr          r5, [sp, #228]
0802956a  159a      ldr          r2, [sp, #84]
0802956c  95ed002a  vldr         s4, [r5]
08029570  92ed009a  vldr         s18, [r2]
08029574  3c9f      ldr          r7, [sp, #240]
08029576  2099      ldr          r1, [sp, #128]
08029578  97ed007a  vldr         s14, [r7]
0802957c  d1ed007a  vldr         s15, [r1]
08029580  9ded46ea  vldr         s28, [sp, #280]
08029584  dfedba1a  vldr         s3, [pc, #744]  ; [0x08029870] = 0x3f7fffac (f32=0.999994993)
08029588  119b      ldr          r3, [sp, #68]
0802958a  9ded36ca  vldr         s24, [sp, #216]
0802958e  9fedb9ba  vldr         s22, [pc, #740]  ; [0x08029874] = 0x3a83126f (f32=0.00100000005)
08029592  dded30ba  vldr         s23, [sp, #192]
08029596  1b68      ldr          r3, [r3]
08029598  b749      ldr          r1, [pc, #732]  ; [0x08029878] = 0x0bb38435 (f32=6.9147215e-32)
0802959a  b84a      ldr          r2, [pc, #736]  ; [0x0802987c] = 0x3619636b (f32=2.28566455e-06)
0802959c  6400      lsls         r4, r4, #1
0802959e  0aee106a  vmov         s20, r6
080295a2  2994      str          r4, [sp, #164]
080295a4  0234      adds         r4, #2
080295a6  f8eecaca  vcvt.f32.s32 s25, s20
080295aa  005f      ldrsh        r0, [r0, r4]
080295ac  129e      ldr          r6, [sp, #72]
080295ae  2a94      str          r4, [sp, #168]
080295b0  6ceea8da  vmul.f32     s27, s25, s17
080295b4  0dee100a  vmov         s26, r0
080295b8  f7ee08aa  vmov.f32     s21, #1.500000e+00
080295bc  f0eeed9a  vabs.f32     s19, s27
080295c0  6deeaa3a  vmul.f32     s7, s27, s21
080295c4  b8eecd8a  vcvt.f32.s32 s16, s26
080295c8  f4eec29a  vcmpe.f32    s19, s4
080295cc  f1ee10fa  vmrs         APSR_nzcv, fpscr
080295d0  28ee284a  vmul.f32     s8, s16, s17
080295d4  f4eee63a  vcmpe.f32    s7, s13
080295d8  b8bf      it           lt
080295da  f0ee429a  vmovlt.f32   s19, s4
080295de  f1ee10fa  vmrs         APSR_nzcv, fpscr
080295e2  ffee004a  vmov.f32     s9, #-1.000000e+00
080295e6  d6ed002a  vldr         s5, [r6]
080295ea  88bf      it           hi
080295ec  f0ee663a  vmovhi.f32   s7, s13
080295f0  24ee2a3a  vmul.f32     s6, s8, s21
080295f4  30ee651a  vsub.f32     s2, s0, s11
080295f8  7feec90a  vsub.f32     s1, s31, s18
080295fc  9ded310a  vldr         s0, [sp, #196]
08029600  f4eee43a  vcmpe.f32    s7, s9
08029604  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029608  a0ee8e9a  vfma.f32     s18, s1, s28
0802960c  01fb0323  mla          r3, r1, r3, r2
08029610  1199      ldr          r1, [sp, #68]
08029612  9b4a      ldr          r2, [pc, #620]  ; [0x08029880] = 0x20022154 (f32=1.10224557e-19)
08029614  0b60      str          r3, [r1]
08029616  b4eee63a  vcmpe.f32    s6, s13
0802961a  30ee626a  vsub.f32     s12, s0, s5
0802961e  37eec7fa  vsub.f32     s30, s15, s14
08029622  b8bf      it           lt
08029624  f0ee643a  vmovlt.f32   s7, s9
08029628  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802962c  88bf      it           hi
0802962e  b0ee663a  vmovhi.f32   s6, s13
08029632  e1ee0c5a  vfma.f32     s11, s2, s24
08029636  e6ee0b2a  vfma.f32     s5, s12, s22
0802963a  afee2b7a  vfma.f32     s14, s30, s23
0802963e  29eea1aa  vmul.f32     s20, s19, s3
08029642  b4eee43a  vcmpe.f32    s6, s9
08029646  f5ee008a  vmov.f32     s17, #2.500000e-01
0802964a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802964e  85ed00aa  vstr         s20, [r5]
08029652  b4eee89a  vcmpe.f32    s18, s17
08029656  159d      ldr          r5, [sp, #84]
08029658  c2ed005a  vstr         s11, [r2]
0802965c  b8bf      it           lt
0802965e  b0ee643a  vmovlt.f32   s6, s9
08029662  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029666  c6ed002a  vstr         s5, [r6]
0802966a  87ed007a  vstr         s14, [r7]
0802966e  85ed009a  vstr         s18, [r5]
08029672  40f21c86  bls.w        #3128  ; -> 0x0802a2ae
08029676  834e      ldr          r6, [pc, #524]  ; [0x08029884] = 0x20021c68 (f32=1.10208272e-19)
08029678  dfed835a  vldr         s11, [pc, #524]  ; [0x08029888] = 0x44fa0000 (f32=2000)
0802967c  7769      ldr          r7, [r6, #20]
0802967e  0097      str          r7, [sp]
08029680  9ded002a  vldr         s4, [sp]
08029684  814c      ldr          r4, [pc, #516]  ; [0x0802988c] = 0x20022098 (f32=1.10222127e-19)
08029686  f8eec27a  vcvt.f32.s32 s15, s4
0802968a  d4ed002a  vldr         s5, [r4]
0802968e  f4eee57a  vcmpe.f32    s15, s11
08029692  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029696  00f10a86  bmi.w        #3092  ; -> 0x0802a2ae
0802969a  01ee903a  vmov         s3, r3
0802969e  f8ee61ca  vcvt.f32.u32 s25, s3
080296a2  feee007a  vmov.f32     s15, #-5.000000e-01
080296a6  eceea27a  vfma.f32     s15, s25, s5
080296aa  9fed79da  vldr         s26, [pc, #484]  ; [0x08029890] = 0x461c4000 (f32=10000)
080296ae  67ee8daa  vmul.f32     s21, s15, s26
080296b2  2aee898a  vmul.f32     s16, s21, s18
080296b6  28ee099a  vmul.f32     s18, s16, s18
080296ba  1098      ldr          r0, [sp, #64]
080296bc  036a      ldr          r3, [r0, #32]
080296be  53b1      cbz          r3, #20  ; -> 0x080296d6
080296c0  dfed6c9a  vldr         s19, [pc, #432]  ; [0x08029874] = 0x3a83126f (f32=0.00100000005)
080296c4  b4eee9aa  vcmpe.f32    s20, s19
080296c8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080296cc  44bf      itt          mi
080296ce  f0ee089a  vmovmi.f32   s19, #3.000000e+00
080296d2  a4ee297a  vfmami.f32   s14, s8, s19
080296d6  6f49      ldr          r1, [pc, #444]  ; [0x08029894] = 0x20021e14 (f32=1.10213803e-19)
080296d8  fdeec74a  vcvt.s32.f32 s9, s14
080296dc  0a68      ldr          r2, [r1]
080296de  b8eee44a  vcvt.f32.s32 s8, s9
080296e2  012a      cmp          r2, #1
080296e4  37ee44fa  vsub.f32     s30, s14, s8
080296e8  00f0dc86  beq.w        #3512  ; -> 0x0802a4a4
080296ec  0125      movs         r5, #1
080296ee  b0ee45ba  vmov.f32     s22, s10
080296f2  0f95      str          r5, [sp, #60]
080296f4  3a9e      ldr          r6, [sp, #232]
080296f6  cded504a  vstr         s9, [sp, #320]
080296fa  d6ed000a  vldr         s1, [r6]
080296fe  96ed01ca  vldr         s24, [r6, #4]
08029702  d6ed02ba  vldr         s23, [r6, #8]
08029706  8ded54fa  vstr         s30, [sp, #336]
0802970a  27ee20ea  vmul.f32     s28, s14, s1
0802970e  27ee0caa  vmul.f32     s20, s14, s24
08029712  67ee2b8a  vmul.f32     s17, s14, s23
08029716  fdeece2a  vcvt.s32.f32 s5, s28
0802971a  fdeeca1a  vcvt.s32.f32 s3, s20
0802971e  bdeee82a  vcvt.s32.f32 s4, s17
08029722  f8eee25a  vcvt.f32.s32 s11, s5
08029726  f8eee1ca  vcvt.f32.s32 s25, s3
0802972a  f8eec27a  vcvt.f32.s32 s15, s4
0802972e  7eee65da  vsub.f32     s27, s28, s11
08029732  3aee6cda  vsub.f32     s26, s20, s25
08029736  78eee7aa  vsub.f32     s21, s17, s15
0802973a  b5eec0fa  vcmpe.f32    s30, #0
0802973e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029742  cded512a  vstr         s5, [sp, #324]
08029746  cded55da  vstr         s27, [sp, #340]
0802974a  cded521a  vstr         s3, [sp, #328]
0802974e  8ded56da  vstr         s26, [sp, #344]
08029752  8ded532a  vstr         s4, [sp, #332]
08029756  cded57aa  vstr         s21, [sp, #348]
0802975a  08d5      bpl          #16  ; -> 0x0802976e
0802975c  14ee909a  vmov         r9, s9
08029760  3fee268a  vadd.f32     s16, s30, s13
08029764  09f1ff37  add.w        r7, r9, #4294967295
08029768  8ded548a  vstr         s16, [sp, #336]
0802976c  5097      str          r7, [sp, #320]
0802976e  f5eec0da  vcmpe.f32    s27, #0
08029772  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029776  08d5      bpl          #16  ; -> 0x0802978a
08029778  12ee90ca  vmov         r12, s5
0802977c  7deea69a  vadd.f32     s19, s27, s13
08029780  0cf1ff34  add.w        r4, r12, #4294967295
08029784  cded559a  vstr         s19, [sp, #340]
08029788  5194      str          r4, [sp, #324]
0802978a  b5eec0da  vcmpe.f32    s26, #0
0802978e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029792  08d5      bpl          #16  ; -> 0x080297a6
08029794  11ee908a  vmov         r8, s3
08029798  7dee264a  vadd.f32     s9, s26, s13
0802979c  08f1ff30  add.w        r0, r8, #4294967295
080297a0  cded564a  vstr         s9, [sp, #344]
080297a4  5290      str          r0, [sp, #328]
080297a6  f5eec0aa  vcmpe.f32    s21, #0
080297aa  f1ee10fa  vmrs         APSR_nzcv, fpscr
080297ae  08d5      bpl          #16  ; -> 0x080297c2
080297b0  12ee10aa  vmov         r10, s4
080297b4  3aeea64a  vadd.f32     s8, s21, s13
080297b8  0af1ff33  add.w        r3, r10, #4294967295
080297bc  8ded574a  vstr         s8, [sp, #348]
080297c0  5393      str          r3, [sp, #332]
080297c2  3549      ldr          r1, [pc, #212]  ; [0x08029898] = 0x20021cb0 (f32=1.10209202e-19)
080297c4  0d68      ldr          r5, [r1]
080297c6  012d      cmp          r5, #1
080297c8  00f01086  beq.w        #3104  ; -> 0x0802a3ec
080297cc  3348      ldr          r0, [pc, #204]  ; [0x0802989c] = 0x20021ca8 (f32=1.10209099e-19)
080297ce  1499      ldr          r1, [sp, #80]
080297d0  0368      ldr          r3, [r0]
080297d2  91ed00fa  vldr         s30, [r1]
080297d6  012b      cmp          r3, #1
080297d8  00f0ce86  beq.w        #3484  ; -> 0x0802a578
080297dc  304e      ldr          r6, [pc, #192]  ; [0x080298a0] = 0x20021f48 (f32=1.10217784e-19)
080297de  96ed00aa  vldr         s20, [r6]
080297e2  379f      ldr          r7, [sp, #220]
080297e4  3868      ldr          r0, [r7]
080297e6  0128      cmp          r0, #1
080297e8  00f00a87  beq.w        #3604  ; -> 0x0802a600
080297ec  239d      ldr          r5, [sp, #140]
080297ee  069f      ldr          r7, [sp, #24]
080297f0  2e68      ldr          r6, [r5]
080297f2  1f99      ldr          r1, [sp, #124]
080297f4  9fed2bca  vldr         s24, [pc, #172]  ; [0x080298a4] = 0x463b8000 (f32=12000)
080297f8  07eb860c  add.w        r12, r7, r6, lsl #2
080297fc  54e0      b            #168  ; -> 0x080298a8
080298a8  9ced000a  vldr         s0, [r12]
080298ac  6aee0f0a  vmul.f32     s1, s20, s30
080298b0  20ee0a6a  vmul.f32     s12, s0, s20
080298b4  b4eee06a  vcmpe.f32    s12, s1
080298b8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080298bc  d8bf      it           le
080298be  f0ee460a  vmovle.f32   s1, s12
080298c2  f4eee60a  vcmpe.f32    s1, s13
080298c6  d4bf      ite          le
080298c8  81ed006a  vstrle       s12, [r1]
080298cc  c1ed000a  vstrgt       s1, [r1]
080298d0  f1ee10fa  vmrs         APSR_nzcv, fpscr
080298d4  d4bf      ite          le
080298d6  f0ee665a  vmovle.f32   s11, s13
080298da  f0ee605a  vmovgt.f32   s11, s1
080298de  35eec0ea  vsub.f32     s28, s11, s0
080298e2  b4eeccea  vcmpe.f32    s28, s24
080298e6  f1ee10fa  vmrs         APSR_nzcv, fpscr
080298ea  0add      ble          #20  ; -> 0x08029902
080298ec  f6ee00ba  vmov.f32     s23, #5.000000e-01
080298f0  2eee2bea  vmul.f32     s28, s28, s23
080298f4  b4eeccea  vcmpe.f32    s28, s24
080298f8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080298fc  f8dc      bgt          #-16  ; -> 0x080298f0
080298fe  70ee0e5a  vadd.f32     s11, s0, s28
08029902  1f98      ldr          r0, [sp, #124]
08029904  794b      ldr          r3, [pc, #484]  ; [0x08029aec] = 0x2002208c (f32=1.10221972e-19)
08029906  189a      ldr          r2, [sp, #96]
08029908  c0ed005a  vstr         s11, [r0]
0802990c  1c68      ldr          r4, [r3]
0802990e  002a      cmp          r2, #0
08029910  40f00985  bne.w        #2578  ; -> 0x0802a326
08029914  1e98      ldr          r0, [sp, #120]
08029916  0268      ldr          r2, [r0]
08029918  012a      cmp          r2, #1
0802991a  00f0a982  beq.w        #1362  ; -> 0x08029e70
0802991e  002c      cmp          r4, #0
08029920  00f0b482  beq.w        #1384  ; -> 0x08029e8c
08029924  109c      ldr          r4, [sp, #64]
08029926  724d      ldr          r5, [pc, #456]  ; [0x08029af0] = 0x20021cbc (f32=1.10209357e-19)
08029928  e268      ldr          r2, [r4, #12]
0802992a  2f68      ldr          r7, [r5]
0802992c  012a      cmp          r2, #1
0802992e  00f03787  beq.w        #3694  ; -> 0x0802a7a0
08029932  2498      ldr          r0, [sp, #144]
08029934  409c      ldr          r4, [sp, #256]
08029936  90ed00aa  vldr         s20, [r0]
0802993a  269d      ldr          r5, [sp, #152]
0802993c  2268      ldr          r2, [r4]
0802993e  0892      str          r2, [sp, #32]
08029940  b4eee5aa  vcmpe.f32    s20, s11
08029944  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029948  2b68      ldr          r3, [r5]
0802994a  08db      blt          #16  ; -> 0x0802995e
0802994c  1899      ldr          r1, [sp, #96]
0802994e  0129      cmp          r1, #1
08029950  00f0a987  beq.w        #3922  ; -> 0x0802a8a6
08029954  1098      ldr          r0, [sp, #64]
08029956  8468      ldr          r4, [r0, #8]
08029958  022c      cmp          r4, #2
0802995a  00f0a487  beq.w        #3912  ; -> 0x0802a8a6
0802995e  329e      ldr          r6, [sp, #200]
08029960  9e42      cmp          r6, r3
08029962  02dc      bgt          #4  ; -> 0x0802996a
08029964  269b      ldr          r3, [sp, #152]
08029966  0025      movs         r5, #0
08029968  1d60      str          r5, [r3]
0802996a  0c9a      ldr          r2, [sp, #48]
0802996c  6148      ldr          r0, [pc, #388]  ; [0x08029af4] = 0x20022154 (f32=1.10224557e-19)
0802996e  624c      ldr          r4, [pc, #392]  ; [0x08029af8] = 0x2002215c (f32=1.1022466e-19)
08029970  d0ed002a  vldr         s5, [r0]
08029974  3d9d      ldr          r5, [sp, #244]
08029976  02eb8709  add.w        r9, r2, r7, lsl #2
0802997a  d9ed008a  vldr         s17, [r9]
0802997e  68eea21a  vmul.f32     s3, s17, s5
08029982  791e      subs         r1, r7, #1
08029984  bdeee12a  vcvt.s32.f32 s4, s3
08029988  54f82160  ldr.w        r6, [r4, r1, lsl #2]
0802998c  54f82770  ldr.w        r7, [r4, r7, lsl #2]
08029990  f8eec2ca  vcvt.f32.s32 s25, s4
08029994  3cee899a  vadd.f32     s18, s25, s18
08029998  fdeec97a  vcvt.s32.f32 s15, s18
0802999c  17ee903a  vmov         r3, s15
080299a0  3344      add          r3, r6
080299a2  bb42      cmp          r3, r7
080299a4  2b60      str          r3, [r5]
080299a6  a4bf      itt          ge
080299a8  07f1ff33  addge.w      r3, r7, #4294967295
080299ac  2b60      strge        r3, [r5]
080299ae  089e      ldr          r6, [sp, #32]
080299b0  1e99      ldr          r1, [sp, #120]
080299b2  189c      ldr          r4, [sp, #96]
080299b4  0c60      str          r4, [r1]
080299b6  002e      cmp          r6, #0
080299b8  40f35487  ble.w        #3752  ; -> 0x0802a864
080299bc  4299      ldr          r1, [sp, #264]
080299be  4f48      ldr          r0, [pc, #316]  ; [0x08029afc] = 0x200220fc (f32=1.10223419e-19)
080299c0  d1ed00aa  vldr         s21, [r1]
080299c4  159a      ldr          r2, [sp, #84]
080299c6  4e4f      ldr          r7, [pc, #312]  ; [0x08029b00] = 0x20022098 (f32=1.10222127e-19)
080299c8  4e49      ldr          r1, [pc, #312]  ; [0x08029b04] = 0x20021de8 (f32=1.10213235e-19)
080299ca  0068      ldr          r0, [r0]
080299cc  4e4d      ldr          r5, [pc, #312]  ; [0x08029b08] = 0x2002214c (f32=1.10224453e-19)
080299ce  4391      str          r1, [sp, #268]
080299d0  dfed4e4a  vldr         s9, [pc, #312]  ; [0x08029b0c] = 0x00000000 (f32=0)
080299d4  4e49      ldr          r1, [pc, #312]  ; [0x08029b10] = 0x20022070 (f32=1.1022161e-19)
080299d6  4f4e      ldr          r6, [pc, #316]  ; [0x08029b14] = 0x20021ca0 (f32=1.10208995e-19)
080299d8  4f4c      ldr          r4, [pc, #316]  ; [0x08029b18] = 0x20001290 (f32=1.08481635e-19)
080299da  92ed00fa  vldr         s30, [r2]
080299de  97ed00ea  vldr         s28, [r7]
080299e2  dfed4e5a  vldr         s11, [pc, #312]  ; [0x08029b1c] = 0x3f19999a (f32=0.600000024)
080299e6  4e4f      ldr          r7, [pc, #312]  ; [0x08029b20] = 0x2000104c (f32=1.08474139e-19)
080299e8  2e91      str          r1, [sp, #184]
080299ea  1a90      str          r0, [sp, #104]
080299ec  4049      ldr          r1, [pc, #256]  ; [0x08029af0] = 0x20021cbc (f32=1.10209357e-19)
080299ee  4548      ldr          r0, [pc, #276]  ; [0x08029b04] = 0x20021de8 (f32=1.10213235e-19)
080299f0  2a68      ldr          r2, [r5]
080299f2  3668      ldr          r6, [r6]
080299f4  2468      ldr          r4, [r4]
080299f6  dff870e1  ldr.w        lr, [pc, #368]  ; [0x08029b68] = 0x20022100 (f32=1.10223471e-19)
080299fa  4a4d      ldr          r5, [pc, #296]  ; [0x08029b24] = 0x20021314 (f32=1.10177407e-19)
080299fc  0968      ldr          r1, [r1]
080299fe  4497      str          r7, [sp, #272]
08029a00  b4eee47a  vcmpe.f32    s14, s9
08029a04  3f68      ldr          r7, [r7]
08029a06  0596      str          r6, [sp, #20]
08029a08  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029a0c  0668      ldr          r6, [r0]
08029a0e  1d94      str          r4, [sp, #116]
08029a10  3f48      ldr          r0, [pc, #252]  ; [0x08029b10] = 0x20022070 (f32=1.1022161e-19)
08029a12  454c      ldr          r4, [pc, #276]  ; [0x08029b28] = 0x20021e14 (f32=1.10213803e-19)
08029a14  2d68      ldr          r5, [r5]
08029a16  0491      str          r1, [sp, #16]
08029a18  b4eee5fa  vcmpe.f32    s30, s11
08029a1c  1344      add          r3, r2
08029a1e  def80010  ldr.w        r1, [lr]
08029a22  0997      str          r7, [sp, #36]
08029a24  2296      str          r6, [sp, #136]
08029a26  149f      ldr          r7, [sp, #80]
08029a28  419e      ldr          r6, [sp, #260]
08029a2a  dfed409a  vldr         s19, [pc, #256]  ; [0x08029b2c] = 0x411ffbe7 (f32=9.9989996)
08029a2e  0e95      str          r5, [sp, #56]
08029a30  54bf      ite          pl
08029a32  b0ee45ca  vmovpl.f32   s24, s10
08029a36  b0ee66ca  vmovmi.f32   s24, s13
08029a3a  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029a3e  0568      ldr          r5, [r0]
08029a40  1b91      str          r1, [sp, #108]
08029a42  07ee103a  vmov         s14, r3
08029a46  2168      ldr          r1, [r4]
08029a48  9fed391a  vldr         s2, [pc, #228]  ; [0x08029b30] = 0x43fa0000 (f32=500)
08029a4c  0d95      str          r5, [sp, #52]
08029a4e  ccbf      ite          gt
08029a50  0123      movgt        r3, #1
08029a52  0023      movle        r3, #0
08029a54  1691      str          r1, [sp, #88]
08029a56  97ed006a  vldr         s12, [r7]
08029a5a  96ed000a  vldr         s0, [r6]
08029a5e  1198      ldr          r0, [sp, #68]
08029a60  1993      str          r3, [sp, #100]
08029a62  049b      ldr          r3, [sp, #16]
08029a64  334d      ldr          r5, [pc, #204]  ; [0x08029b34] = 0x20021ee4 (f32=1.10216492e-19)
08029a66  9fed349a  vldr         s18, [pc, #208]  ; [0x08029b38] = 0x447a0000 (f32=1000)
08029a6a  344f      ldr          r7, [pc, #208]  ; [0x08029b3c] = 0x20021f70 (f32=1.10218301e-19)
08029a6c  d0f80090  ldr.w        r9, [r0]
08029a70  334c      ldr          r4, [pc, #204]  ; [0x08029b40] = 0x20021f08 (f32=1.10216957e-19)
08029a72  0295      str          r5, [sp, #8]
08029a74  6eee290a  vmul.f32     s1, s28, s19
08029a78  3fee65aa  vsub.f32     s20, s30, s11
08029a7c  6aee811a  vmul.f32     s3, s21, s2
08029a80  f6ee002a  vmov.f32     s5, #5.000000e-01
08029a84  5a1e      subs         r2, r3, #1
08029a86  2f49      ldr          r1, [pc, #188]  ; [0x08029b44] = 0x20021f94 (f32=1.10218766e-19)
08029a88  2f4e      ldr          r6, [pc, #188]  ; [0x08029b48] = 0x20021fd8 (f32=1.10219645e-19)
08029a8a  304d      ldr          r5, [pc, #192]  ; [0x08029b4c] = 0x20021ff8 (f32=1.10220059e-19)
08029a8c  cded251a  vstr         s3, [sp, #148]
08029a90  b1ee6a2a  vneg.f32     s4, s21
08029a94  2aee20aa  vmul.f32     s20, s20, s1
08029a98  7fee628a  vsub.f32     s17, s30, s5
08029a9c  1792      str          r2, [sp, #92]
08029a9e  dff8ccb0  ldr.w        r11, [pc, #204]  ; [0x08029b6c] = 0x20021e64 (f32=1.10214837e-19)
08029aa2  2b48      ldr          r0, [pc, #172]  ; [0x08029b50] = 0x20021f28 (f32=1.10217371e-19)
08029aa4  dff8c8c0  ldr.w        r12, [pc, #200]  ; [0x08029b70] = 0x20021ec8 (f32=1.1021613e-19)
08029aa8  dff8c8e0  ldr.w        lr, [pc, #200]  ; [0x08029b74] = 0x20022018 (f32=1.10220472e-19)
08029aac  0191      str          r1, [sp, #4]
08029aae  0b96      str          r6, [sp, #44]
08029ab0  0a95      str          r5, [sp, #40]
08029ab2  9fed288a  vldr         s16, [pc, #160]  ; [0x08029b54] = 0xc7d90380 (f32=-111111)
08029ab6  9fed28da  vldr         s26, [pc, #160]  ; [0x08029b58] = 0x3ff33333 (f32=1.89999998)
08029aba  dfed28ba  vldr         s23, [pc, #160]  ; [0x08029b5c] = 0x3db4c251 (f32=0.0882612541)
08029abe  dfed28ca  vldr         s25, [pc, #160]  ; [0x08029b60] = 0xb22bcc77 (f32=-9.99999994e-09)
08029ac2  dfed28da  vldr         s27, [pc, #160]  ; [0x08029b64] = 0x3c800000 (f32=0.015625)
08029ac6  8ded272a  vstr         s4, [sp, #156]
08029aca  f8eec70a  vcvt.f32.s32 s1, s14
08029ace  6eee229a  vmul.f32     s19, s28, s5
08029ad2  2aee89fa  vmul.f32     s30, s21, s18
08029ad6  b0ee644a  vmov.f32     s8, s9
08029ada  f0ee451a  vmov.f32     s3, s10
08029ade  0022      movs         r2, #0
08029ae0  cdf81c90  str.w        r9, [sp, #28]
08029ae4  ba46      mov          r10, r7
08029ae6  0094      str          r4, [sp]
08029ae8  a4e1      b            #840  ; -> 0x08029e34
08029b78  0023      movs         r3, #0
08029b7a  0099      ldr          r1, [sp]
08029b7c  cd4e      ldr          r6, [pc, #820]  ; [0x08029eb4] = 0x20021ea8 (f32=1.10215716e-19)
08029b7e  ccf80030  str.w        r3, [r12]
08029b82  169b      ldr          r3, [sp, #88]
08029b84  cc4d      ldr          r5, [pc, #816]  ; [0x08029eb8] = 0x2002215c (f32=1.1022466e-19)
08029b86  81ed005a  vstr         s10, [r1]
08029b8a  4fea8209  lsl.w        r9, r2, #2
08029b8e  049f      ldr          r7, [sp, #16]
08029b90  1799      ldr          r1, [sp, #92]
08029b92  0bed015a  vstr         s10, [r11, #-4]
08029b96  4e44      add          r6, r9
08029b98  0324      movs         r4, #3
08029b9a  012b      cmp          r3, #1
08029b9c  86ed005a  vstr         s10, [r6]
08029ba0  cef80040  str.w        r4, [lr]
08029ba4  55f82760  ldr.w        r6, [r5, r7, lsl #2]
08029ba8  55f82150  ldr.w        r5, [r5, r1, lsl #2]
08029bac  04d1      bne          #8  ; -> 0x08029bb8
08029bae  771b      subs         r7, r6, r5
08029bb0  06ee107a  vmov         s12, r7
08029bb4  b8eec66a  vcvt.f32.s32 s12, s12
08029bb8  b0ee602a  vmov.f32     s4, s1
08029bbc  a6ee0c2a  vfma.f32     s4, s12, s24
08029bc0  0a99      ldr          r1, [sp, #40]
08029bc2  0b9f      ldr          r7, [sp, #44]
08029bc4  41f82250  str.w        r5, [r1, r2, lsl #2]
08029bc8  f4ee6bfa  vcmp.f32     s31, s23
08029bcc  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029bd0  bdeec21a  vcvt.s32.f32 s2, s4
08029bd4  0199      ldr          r1, [sp, #4]
08029bd6  dff8f882  ldr.w        r8, [pc, #760]  ; [0x08029ed0] = 0x20021fb8 (f32=1.10219232e-19)
08029bda  47f82260  str.w        r6, [r7, r2, lsl #2]
08029bde  0cbf      ite          eq
08029be0  0124      moveq        r4, #1
08029be2  0024      movne        r4, #0
08029be4  76ee407a  vsub.f32     s15, s12, s0
08029be8  0c60      str          r4, [r1]
08029bea  11ee103a  vmov         r3, s2
08029bee  069c      ldr          r4, [sp, #24]
08029bf0  0499      ldr          r1, [sp, #16]
08029bf2  80ed001a  vstr         s2, [r0]
08029bf6  fdeee72a  vcvt.s32.f32 s5, s15
08029bfa  04eb0907  add.w        r7, r4, r9
08029bfe  ab42      cmp          r3, r5
08029c00  caed002a  vstr         s5, [r10]
08029c04  48f82210  str.w        r1, [r8, r2, lsl #2]
08029c08  87ed006a  vstr         s12, [r7]
08029c0c  0eda      bge          #28  ; -> 0x08029c2c
08029c0e  0c9c      ldr          r4, [sp, #48]
08029c10  04eb8108  add.w        r8, r4, r1, lsl #2
08029c14  98ed007a  vldr         s14, [r8]
08029c18  b8eec12a  vcvt.f32.s32 s4, s2
08029c1c  32ee071a  vadd.f32     s2, s4, s14
08029c20  fdeec12a  vcvt.s32.f32 s5, s2
08029c24  12ee903a  vmov         r3, s5
08029c28  c0ed002a  vstr         s5, [r0]
08029c2c  9e42      cmp          r6, r3
08029c2e  11da      bge          #34  ; -> 0x08029c54
08029c30  0c9f      ldr          r7, [sp, #48]
08029c32  0499      ldr          r1, [sp, #16]
08029c34  07ee903a  vmov         s15, r3
08029c38  07eb8104  add.w        r4, r7, r1, lsl #2
08029c3c  94ed007a  vldr         s14, [r4]
08029c40  b8eee72a  vcvt.f32.s32 s4, s15
08029c44  32ee471a  vsub.f32     s2, s4, s14
08029c48  fdeec12a  vcvt.s32.f32 s5, s2
08029c4c  12ee903a  vmov         r3, s5
08029c50  c0ed002a  vstr         s5, [r0]
08029c54  0799      ldr          r1, [sp, #28]
08029c56  994c      ldr          r4, [pc, #612]  ; [0x08029ebc] = 0x0bb38435 (f32=6.9147215e-32)
08029c58  994f      ldr          r7, [pc, #612]  ; [0x08029ec0] = 0x3619636b (f32=2.28566455e-06)
08029c5a  04fb0178  mla          r8, r4, r1, r7
08029c5e  07ee108a  vmov         s14, r8
08029c62  f8ee477a  vcvt.f32.u32 s15, s14
08029c66  9649      ldr          r1, [pc, #600]  ; [0x08029ec0] = 0x3619636b (f32=2.28566455e-06)
08029c68  27eea92a  vmul.f32     s4, s15, s19
08029c6c  04fb0817  mla          r7, r4, r8, r1
08029c70  01ee107a  vmov         s2, r7
08029c74  f4eec28a  vcmpe.f32    s17, s4
08029c78  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029c7c  0797      str          r7, [sp, #28]
08029c7e  f8ee412a  vcvt.f32.u32 s5, s2
08029c82  c0f2d581  blt.w        #938  ; -> 0x0802a030
08029c86  04fb0714  mla          r4, r4, r7, r1
08029c8a  07ee904a  vmov         s15, r4
08029c8e  2eee227a  vmul.f32     s14, s28, s5
08029c92  f8ee672a  vcvt.f32.u32 s5, s15
08029c96  0794      str          r4, [sp, #28]
08029c98  2aee222a  vmul.f32     s4, s20, s5
08029c9c  8949      ldr          r1, [pc, #548]  ; [0x08029ec4] = 0x20021e88 (f32=1.10215303e-19)
08029c9e  199c      ldr          r4, [sp, #100]
08029ca0  bdeec21a  vcvt.s32.f32 s2, s4
08029ca4  8944      add          r9, r1
08029ca6  11ee107a  vmov         r7, s2
08029caa  0299      ldr          r1, [sp, #8]
08029cac  89ed007a  vstr         s14, [r9]
08029cb0  04fb07f7  mul          r7, r4, r7
08029cb4  4f60      str          r7, [r1, #4]
08029cb6  0599      ldr          r1, [sp, #20]
08029cb8  0327      movs         r7, #3
08029cba  0024      movs         r4, #0
08029cbc  1940      ands         r1, r3
08029cbe  0397      str          r7, [sp, #12]
08029cc0  0994      str          r4, [sp, #36]
08029cc2  f0ee457a  vmov.f32     s15, s10
08029cc6  039f      ldr          r7, [sp, #12]
08029cc8  002f      cmp          r7, #0
08029cca  00f05f81  beq.w        #702  ; -> 0x08029f8c
08029cce  069c      ldr          r4, [sp, #24]
08029cd0  9aed007a  vldr         s14, [r10]
08029cd4  4fea8209  lsl.w        r9, r2, #2
08029cd8  04eb0908  add.w        r8, r4, r9
08029cdc  d8ed005a  vldr         s11, [r8]
08029ce0  b8eec72a  vcvt.f32.s32 s4, s14
08029ce4  f4eec65a  vcmpe.f32    s11, s12
08029ce8  76ee405a  vsub.f32     s11, s12, s0
08029cec  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029cf0  b4eee52a  vcmpe.f32    s4, s11
08029cf4  c8bf      it           gt
08029cf6  88ed006a  vstrgt       s12, [r8]
08029cfa  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029cfe  c8bf      it           gt
08029d00  bdeee57a  vcvtgt.s32.f32 s14, s11
08029d04  f5eec07a  vcmpe.f32    s15, #0
08029d08  c8bf      it           gt
08029d0a  8aed007a  vstrgt       s14, [r10]
08029d0e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029d12  dcf80080  ldr.w        r8, [r12]
08029d16  00f18e81  bmi.w        #796  ; -> 0x0802a036
08029d1a  17ee107a  vmov         r7, s14
08029d1e  4745      cmp          r7, r8
08029d20  80f29981  bge.w        #818  ; -> 0x0802a056
08029d24  0127      movs         r7, #1
08029d26  77eeea2a  vsub.f32     s5, s15, s21
08029d2a  dded27ea  vldr         s29, [sp, #156]
08029d2e  0397      str          r7, [sp, #12]
08029d30  b4eec96a  vcmpe.f32    s12, s18
08029d34  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029d38  40f13a81  bpl.w        #628  ; -> 0x08029fb0
08029d3c  099f      ldr          r7, [sp, #36]
08029d3e  002f      cmp          r7, #0
08029d40  40f05281  bne.w        #676  ; -> 0x08029fe8
08029d44  0427      movs         r7, #4
08029d46  2297      str          r7, [sp, #136]
08029d48  a1f13f04  sub.w        r4, r1, #63
08029d4c  ac42      cmp          r4, r5
08029d4e  08dc      bgt          #16  ; -> 0x08029d62
08029d50  4f1b      subs         r7, r1, r5
08029d52  02ee107a  vmov         s4, r7
08029d56  b8eec21a  vcvt.f32.s32 s2, s4
08029d5a  61ee2d7a  vmul.f32     s15, s2, s27
08029d5e  62eea72a  vmul.f32     s5, s5, s15
08029d62  01f13f04  add.w        r4, r1, #63
08029d66  b442      cmp          r4, r6
08029d68  08db      blt          #16  ; -> 0x08029d7c
08029d6a  711a      subs         r1, r6, r1
08029d6c  07ee101a  vmov         s14, r1
08029d70  b8eec72a  vcvt.f32.s32 s4, s14
08029d74  22ee2d1a  vmul.f32     s2, s4, s27
08029d78  62ee812a  vmul.f32     s5, s5, s2
08029d7c  4d4f      ldr          r7, [pc, #308]  ; [0x08029eb4] = 0x20021ea8 (f32=1.10215716e-19)
08029d7e  0f99      ldr          r1, [sp, #60]
08029d80  039c      ldr          r4, [sp, #12]
08029d82  cef80040  str.w        r4, [lr]
08029d86  b944      add          r9, r7
08029d88  d9ed007a  vldr         s15, [r9]
08029d8c  4bed012a  vstr         s5, [r11, #-4]
08029d90  3bee277a  vadd.f32     s14, s22, s15
08029d94  4144      add          r1, r8
08029d96  b4eee67a  vcmpe.f32    s14, s13
08029d9a  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029d9e  ccf80010  str.w        r1, [r12]
08029da2  c0f24f81  blt.w        #670  ; -> 0x0802a044
08029da6  37ee661a  vsub.f32     s2, s14, s13
08029daa  0131      adds         r1, #1
08029dac  89ed001a  vstr         s2, [r9]
08029db0  ccf80010  str.w        r1, [r12]
08029db4  029c      ldr          r4, [sp, #8]
08029db6  009f      ldr          r7, [sp]
08029db8  54f8041f  ldr          r1, [r4, #4]!
08029dbc  0294      str          r4, [sp, #8]
08029dbe  f4eee21a  vcmpe.f32    s3, s5
08029dc2  0df5b078  add.w        r8, sp, #352
08029dc6  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029dca  08eb8104  add.w        r4, r8, r1, lsl #2
08029dce  48bf      it           mi
08029dd0  f0ee621a  vmovmi.f32   s3, s5
08029dd4  54ed047a  vldr         s15, [r4, #-16]
08029dd8  d7ed002a  vldr         s5, [r7]
08029ddc  54f8201c  ldr          r1, [r4, #-32]
08029de0  48bf      it           mi
08029de2  0d92      strmi        r2, [sp, #52]
08029de4  32eea77a  vadd.f32     s14, s5, s15
08029de8  0b44      add          r3, r1
08029dea  b4eee67a  vcmpe.f32    s14, s13
08029dee  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029df2  0360      str          r3, [r0]
08029df4  c0f2d080  blt.w        #416  ; -> 0x08029f98
08029df8  37ee662a  vsub.f32     s4, s14, s13
08029dfc  0133      adds         r3, #1
08029dfe  87ed002a  vstr         s4, [r7]
08029e02  9e42      cmp          r6, r3
08029e04  03db      blt          #6  ; -> 0x08029e0e
08029e06  9d42      cmp          r5, r3
08029e08  ccbf      ite          gt
08029e0a  3546      movgt        r5, r6
08029e0c  1d46      movle        r5, r3
08029e0e  009e      ldr          r6, [sp]
08029e10  019b      ldr          r3, [sp, #4]
08029e12  089c      ldr          r4, [sp, #32]
08029e14  40f8045b  str          r5, [r0], #4
08029e18  0132      adds         r2, #1
08029e1a  0436      adds         r6, #4
08029e1c  0433      adds         r3, #4
08029e1e  a242      cmp          r2, r4
08029e20  0096      str          r6, [sp]
08029e22  0cf1040c  add.w        r12, r12, #4
08029e26  0ef1040e  add.w        lr, lr, #4
08029e2a  0193      str          r3, [sp, #4]
08029e2c  0af1040a  add.w        r10, r10, #4
08029e30  00f02181  beq.w        #578  ; -> 0x0802a076
08029e34  fbec017a  vldmia       r11!, {s15}
08029e38  f4ee487a  vcmp.f32     s15, s16
08029e3c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029e40  3ff49aae  beq.w        #-716  ; -> 0x08029b78
08029e44  0b99      ldr          r1, [sp, #44]
08029e46  0368      ldr          r3, [r0]
08029e48  0a9c      ldr          r4, [sp, #40]
08029e4a  51f82260  ldr.w        r6, [r1, r2, lsl #2]
08029e4e  0599      ldr          r1, [sp, #20]
08029e50  54f82250  ldr.w        r5, [r4, r2, lsl #2]
08029e54  f4eecd7a  vcmpe.f32    s15, s26
08029e58  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029e5c  01ea0301  and.w        r1, r1, r3
08029e60  38dd      ble          #112  ; -> 0x08029ed4
08029e62  0027      movs         r7, #0
08029e64  0bed015a  vstr         s10, [r11, #-4]
08029e68  cef80070  str.w        r7, [lr]
08029e6c  0397      str          r7, [sp, #12]
08029e6e  28e7      b            #-432  ; -> 0x08029cc2
08029e70  109d      ldr          r5, [sp, #64]
08029e72  af68      ldr          r7, [r5, #8]
08029e74  012f      cmp          r7, #1
08029e76  7ff452ad  bne.w        #-1372  ; -> 0x0802991e
08029e7a  1349      ldr          r1, [pc, #76]  ; [0x08029ec8] = 0x20021ec8 (f32=1.1021613e-19)
08029e7c  134b      ldr          r3, [pc, #76]  ; [0x08029ecc] = 0x20021f70 (f32=1.10218301e-19)
08029e7e  51f82600  ldr.w        r0, [r1, r6, lsl #2]
08029e82  43f82600  str.w        r0, [r3, r6, lsl #2]
08029e86  002c      cmp          r4, #0
08029e88  7ff44cad  bne.w        #-1384  ; -> 0x08029924
08029e8c  269a      ldr          r2, [sp, #152]
08029e8e  329f      ldr          r7, [sp, #200]
08029e90  1068      ldr          r0, [r2]
08029e92  249c      ldr          r4, [sp, #144]
08029e94  1e99      ldr          r1, [sp, #120]
08029e96  c4ed005a  vstr         s11, [r4]
08029e9a  0026      movs         r6, #0
08029e9c  8742      cmp          r7, r0
08029e9e  0e60      str          r6, [r1]
08029ea0  01dc      bgt          #2  ; -> 0x08029ea6
08029ea2  269b      ldr          r3, [sp, #152]
08029ea4  1e60      str          r6, [r3]
08029ea6  409a      ldr          r2, [sp, #256]
08029ea8  3d9d      ldr          r5, [sp, #244]
08029eaa  1068      ldr          r0, [r2]
08029eac  0890      str          r0, [sp, #32]
08029eae  0023      movs         r3, #0
08029eb0  2b60      str          r3, [r5]
08029eb2  7ce5      b            #-1288  ; -> 0x080299ae
08029ed4  f5eec07a  vcmpe.f32    s15, #0
08029ed8  def80070  ldr.w        r7, [lr]
08029edc  0397      str          r7, [sp, #12]
08029ede  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029ee2  7ff7f0ae  ble.w        #-544  ; -> 0x08029cc6
08029ee6  dff82c84  ldr.w        r8, [pc, #1068]  ; [0x0802a314] = 0x2002215c (f32=1.1022466e-19)
08029eea  049c      ldr          r4, [sp, #16]
08029eec  0e9f      ldr          r7, [sp, #56]
08029eee  58f82490  ldr.w        r9, [r8, r4, lsl #2]
08029ef2  b945      cmp          r9, r7
08029ef4  03dd      ble          #6  ; -> 0x08029efe
08029ef6  1d9c      ldr          r4, [sp, #116]
08029ef8  002c      cmp          r4, #0
08029efa  7ff4e4ae  bne.w        #-568  ; -> 0x08029cc6
08029efe  059c      ldr          r4, [sp, #20]
08029f00  dff81494  ldr.w        r9, [pc, #1044]  ; [0x0802a318] = 0x20021e88 (f32=1.10215303e-19)
08029f04  4f1c      adds         r7, r1, #1
08029f06  07ea0408  and.w        r8, r7, r4
08029f0a  1b9c      ldr          r4, [sp, #108]
08029f0c  34f91170  ldrsh.w      r7, [r4, r1, lsl #1]
08029f10  34f91840  ldrsh.w      r4, [r4, r8, lsl #1]
08029f14  05ee904a  vmov         s11, r4
08029f18  1a9c      ldr          r4, [sp, #104]
08029f1a  02ee907a  vmov         s5, r7
08029f1e  34f91880  ldrsh.w      r8, [r4, r8, lsl #1]
08029f22  34f91140  ldrsh.w      r4, [r4, r1, lsl #1]
08029f26  009f      ldr          r7, [sp]
08029f28  b8eee27a  vcvt.f32.s32 s14, s5
08029f2c  02ee108a  vmov         s4, r8
08029f30  f8eee55a  vcvt.f32.s32 s11, s11
08029f34  02ee904a  vmov         s5, r4
08029f38  97ed001a  vldr         s2, [r7]
08029f3c  039f      ldr          r7, [sp, #12]
08029f3e  75eec75a  vsub.f32     s11, s11, s14
08029f42  f8eee22a  vcvt.f32.s32 s5, s5
08029f46  b8eec22a  vcvt.f32.s32 s4, s4
08029f4a  a5ee817a  vfma.f32     s14, s11, s2
08029f4e  09eb8208  add.w        r8, r9, r2, lsl #2
08029f52  32ee622a  vsub.f32     s4, s4, s5
08029f56  77eea75a  vadd.f32     s11, s15, s15
08029f5a  e2ee012a  vfma.f32     s5, s4, s2
08029f5e  98ed002a  vldr         s4, [r8]
08029f62  25ee877a  vmul.f32     s14, s11, s14
08029f66  36eec21a  vsub.f32     s2, s13, s4
08029f6a  65eea25a  vmul.f32     s11, s11, s5
08029f6e  61ee072a  vmul.f32     s5, s2, s14
08029f72  22ee077a  vmul.f32     s14, s4, s14
08029f76  e2ee252a  vfma.f32     s5, s4, s11
08029f7a  a1ee257a  vfma.f32     s14, s2, s11
08029f7e  74eea24a  vadd.f32     s9, s9, s5
08029f82  34ee074a  vadd.f32     s8, s8, s14
08029f86  002f      cmp          r7, #0
08029f88  7ff4a1ae  bne.w        #-702  ; -> 0x08029cce
08029f8c  0368      ldr          r3, [r0]
08029f8e  dfedce2a  vldr         s5, [pc, #824]  ; [0x0802a2c8] = 0xb22bcc77 (f32=-9.99999994e-09)
08029f92  4bed01ca  vstr         s25, [r11, #-4]
08029f96  0de7      b            #-486  ; -> 0x08029db4
08029f98  b5eec07a  vcmpe.f32    s14, #0
08029f9c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029fa0  44bf      itt          mi
08029fa2  37ee267a  vaddmi.f32   s14, s14, s13
08029fa6  03f1ff33  addmi.w      r3, r3, #4294967295
08029faa  87ed007a  vstr         s14, [r7]
08029fae  28e7      b            #-432  ; -> 0x08029e02
08029fb0  17ee104a  vmov         r4, s14
08029fb4  08f2ed27  addw         r7, r8, #749
08029fb8  a742      cmp          r7, r4
08029fba  15db      blt          #42  ; -> 0x08029fe8
08029fbc  099f      ldr          r7, [sp, #36]
08029fbe  9fb9      cbnz         r7, #38  ; -> 0x08029fe8
08029fc0  039c      ldr          r4, [sp, #12]
08029fc2  022c      cmp          r4, #2
08029fc4  10d1      bne          #32  ; -> 0x08029fe8
08029fc6  dfedc17a  vldr         s15, [pc, #772]  ; [0x0802a2cc] = 0x437a0000 (f32=250)
08029fca  b4ee670a  vcmp.f32     s0, s15
08029fce  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029fd2  09d1      bne          #18  ; -> 0x08029fe8
08029fd4  f4ee6bfa  vcmp.f32     s31, s23
08029fd8  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029fdc  04d0      beq          #8  ; -> 0x08029fe8
08029fde  019f      ldr          r7, [sp, #4]
08029fe0  3c68      ldr          r4, [r7]
08029fe2  002c      cmp          r4, #0
08029fe4  3ff4aeae  beq.w        #-676  ; -> 0x08029d44
08029fe8  b4eee2fa  vcmpe.f32    s30, s5
08029fec  f1ee10fa  vmrs         APSR_nzcv, fpscr
08029ff0  7ff7aaae  ble.w        #-684  ; -> 0x08029d48
08029ff4  9ded257a  vldr         s14, [sp, #148]
08029ff8  b4eee27a  vcmpe.f32    s14, s5
08029ffc  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a000  7ff5a2ae  bpl.w        #-700  ; -> 0x08029d48
0802a004  099c      ldr          r4, [sp, #36]
0802a006  002c      cmp          r4, #0
0802a008  7ff49eae  bne.w        #-708  ; -> 0x08029d48
0802a00c  039f      ldr          r7, [sp, #12]
0802a00e  012f      cmp          r7, #1
0802a010  7ff49aae  bne.w        #-716  ; -> 0x08029d48
0802a014  f4ee6bfa  vcmp.f32     s31, s23
0802a018  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a01c  3ff494ae  beq.w        #-728  ; -> 0x08029d48
0802a020  019c      ldr          r4, [sp, #4]
0802a022  2768      ldr          r7, [r4]
0802a024  229c      ldr          r4, [sp, #136]
0802a026  002f      cmp          r7, #0
0802a028  08bf      it           eq
0802a02a  0424      moveq        r4, #4
0802a02c  2294      str          r4, [sp, #136]
0802a02e  8be6      b            #-746  ; -> 0x08029d48
0802a030  b0ee457a  vmov.f32     s14, s10
0802a034  30e6      b            #-928  ; -> 0x08029c98
0802a036  9feda61a  vldr         s2, [pc, #664]  ; [0x0802a2d0] = 0x322bcc77 (f32=9.99999994e-09)
0802a03a  0024      movs         r4, #0
0802a03c  0394      str          r4, [sp, #12]
0802a03e  7eeec12a  vsub.f32     s5, s29, s2
0802a042  75e6      b            #-790  ; -> 0x08029d30
0802a044  b5eec07a  vcmpe.f32    s14, #0
0802a048  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a04c  00f13381  bmi.w        #614  ; -> 0x0802a2b6
0802a050  89ed007a  vstr         s14, [r9]
0802a054  aee6      b            #-676  ; -> 0x08029db4
0802a056  f4eee67a  vcmpe.f32    s15, s13
0802a05a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a05e  03db      blt          #6  ; -> 0x0802a068
0802a060  039c      ldr          r4, [sp, #12]
0802a062  032c      cmp          r4, #3
0802a064  00f01e83  beq.w        #1596  ; -> 0x0802a6a4
0802a068  0327      movs         r7, #3
0802a06a  7aeea72a  vadd.f32     s5, s21, s15
0802a06e  f0ee6aea  vmov.f32     s29, s21
0802a072  0397      str          r7, [sp, #12]
0802a074  5ce6      b            #-840  ; -> 0x08029d30
0802a076  449a      ldr          r2, [sp, #272]
0802a078  0999      ldr          r1, [sp, #36]
0802a07a  1160      str          r1, [r2]
0802a07c  439a      ldr          r2, [sp, #268]
0802a07e  229d      ldr          r5, [sp, #136]
0802a080  1560      str          r5, [r2]
0802a082  1498      ldr          r0, [sp, #80]
0802a084  119f      ldr          r7, [sp, #68]
0802a086  2e9a      ldr          r2, [sp, #184]
0802a088  ddf81cb0  ldr.w        r11, [sp, #28]
0802a08c  0d9b      ldr          r3, [sp, #52]
0802a08e  80ed006a  vstr         s12, [r0]
0802a092  c7f800b0  str.w        r11, [r7]
0802a096  1360      str          r3, [r2]
0802a098  389c      ldr          r4, [sp, #224]
0802a09a  9fed8eea  vldr         s28, [pc, #568]  ; [0x0802a2d4] = 0x3f7d70a4 (f32=0.99000001)
0802a09e  94ed00ba  vldr         s22, [r4]
0802a0a2  dded457a  vldr         s15, [sp, #276]
0802a0a6  3398      ldr          r0, [sp, #204]
0802a0a8  ebee0e7a  vfma.f32     s15, s22, s28
0802a0ac  0168      ldr          r1, [r0]
0802a0ae  b1f57a7f  cmp.w        r1, #1000
0802a0b2  c4ed007a  vstr         s15, [r4]
0802a0b6  80f23181  bge.w        #610  ; -> 0x0802a31c
0802a0ba  9fed879a  vldr         s18, [pc, #540]  ; [0x0802a2d8] = 0x38000100 (f32=3.05185094e-05)
0802a0be  27ee890a  vmul.f32     s0, s15, s18
0802a0c2  24ee004a  vmul.f32     s8, s8, s0
0802a0c6  64ee804a  vmul.f32     s9, s9, s0
0802a0ca  209d      ldr          r5, [sp, #128]
0802a0cc  219f      ldr          r7, [sp, #132]
0802a0ce  d5ed00ba  vldr         s23, [r5]
0802a0d2  97ed008a  vldr         s16, [r7]
0802a0d6  9fed81da  vldr         s26, [pc, #516]  ; [0x0802a2dc] = 0x3f7fbe77 (f32=0.999000013)
0802a0da  129a      ldr          r2, [sp, #72]
0802a0dc  109e      ldr          r6, [sp, #64]
0802a0de  d2ed009a  vldr         s19, [r2]
0802a0e2  2c9b      ldr          r3, [sp, #176]
0802a0e4  7e49      ldr          r1, [pc, #504]  ; [0x0802a2e0] = 0x20021de4 (f32=1.10213183e-19)
0802a0e6  746a      ldr          r4, [r6, #36]
0802a0e8  1b68      ldr          r3, [r3]
0802a0ea  0f68      ldr          r7, [r1]
0802a0ec  2198      ldr          r0, [sp, #132]
0802a0ee  f5ee40ba  vcmp.f32     s23, #0
0802a0f2  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a0f6  68ee0dca  vmul.f32     s25, s16, s26
0802a0fa  1cbf      itt          ne
0802a0fc  dfed79ba  vldrne       s23, [pc, #484]  ; [0x0802a2e4] = 0x3a83126f (f32=0.00100000005)
0802a100  7ceeabca  vaddne.f32   s25, s25, s23
0802a104  bfee00aa  vmov.f32     s20, #-1.000000e+00
0802a108  64ee2cda  vmul.f32     s27, s8, s25
0802a10c  24eeacca  vmul.f32     s24, s9, s25
0802a110  f4eee6da  vcmpe.f32    s27, s13
0802a114  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a118  b4eee6ca  vcmpe.f32    s24, s13
0802a11c  88bf      it           hi
0802a11e  f0ee66da  vmovhi.f32   s27, s13
0802a122  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a126  88bf      it           hi
0802a128  b0ee66ca  vmovhi.f32   s24, s13
0802a12c  f4eecada  vcmpe.f32    s27, s20
0802a130  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a134  b4eecaca  vcmpe.f32    s24, s20
0802a138  b8bf      it           lt
0802a13a  f0ee4ada  vmovlt.f32   s27, s20
0802a13e  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a142  b8bf      it           lt
0802a144  b0ee4aca  vmovlt.f32   s24, s20
0802a148  7deee30a  vsub.f32     s1, s27, s7
0802a14c  7cee438a  vsub.f32     s17, s24, s6
0802a150  b0ee63fa  vmov.f32     s30, s7
0802a154  b0ee437a  vmov.f32     s14, s6
0802a158  a9eea87a  vfma.f32     s14, s19, s17
0802a15c  002c      cmp          r4, #0
0802a15e  a0eea9fa  vfma.f32     s30, s1, s19
0802a162  08bf      it           eq
0802a164  b0ee473a  vmoveq.f32   s6, s14
0802a168  08bf      it           eq
0802a16a  f0ee4f3a  vmoveq.f32   s7, s30
0802a16e  bb42      cmp          r3, r7
0802a170  c0ed00ca  vstr         s25, [r0]
0802a174  b0ee47ba  vmov.f32     s22, s14
0802a178  23d1      bne          #70  ; -> 0x0802a1c2
0802a17a  5b4d      ldr          r5, [pc, #364]  ; [0x0802a2e8] = 0x20024134 (f32=1.10330022e-19)
0802a17c  5b4e      ldr          r6, [pc, #364]  ; [0x0802a2ec] = 0x2002412c (f32=1.10329919e-19)
0802a17e  5c4c      ldr          r4, [pc, #368]  ; [0x0802a2f0] = 0x20024138 (f32=1.10330074e-19)
0802a180  5c4a      ldr          r2, [pc, #368]  ; [0x0802a2f4] = 0x20024130 (f32=1.1032997e-19)
0802a182  d5ed001a  vldr         s3, [r5]
0802a186  96ed006a  vldr         s12, [r6]
0802a18a  9fed5b1a  vldr         s2, [pc, #364]  ; [0x0802a2f8] = 0x3f7f3b64 (f32=0.996999979)
0802a18e  d4ed002a  vldr         s5, [r4]
0802a192  d2ed005a  vldr         s11, [r2]
0802a196  9fed592a  vldr         s4, [pc, #356]  ; [0x0802a2fc] = 0x3f333333 (f32=0.699999988)
0802a19a  91ee816a  vfnms.f32    s12, s3, s2
0802a19e  d2ee815a  vfnms.f32    s11, s5, s2
0802a1a2  23ee82ea  vmul.f32     s28, s7, s4
0802a1a6  63ee027a  vmul.f32     s15, s6, s4
0802a1aa  76ee0e3a  vadd.f32     s7, s12, s28
0802a1ae  35eea73a  vadd.f32     s6, s11, s15
0802a1b2  86ed00ea  vstr         s28, [r6]
0802a1b6  c5ed003a  vstr         s7, [r5]
0802a1ba  84ed003a  vstr         s6, [r4]
0802a1be  c2ed007a  vstr         s15, [r2]
0802a1c2  2b98      ldr          r0, [sp, #172]
0802a1c4  0168      ldr          r1, [r0]
0802a1c6  0129      cmp          r1, #1
0802a1c8  00f07881  beq.w        #752  ; -> 0x0802a4bc
0802a1cc  2898      ldr          r0, [sp, #160]
0802a1ce  0468      ldr          r4, [r0]
0802a1d0  012c      cmp          r4, #1
0802a1d2  00f0c381  beq.w        #902  ; -> 0x0802a55c
0802a1d6  2d9e      ldr          r6, [sp, #180]
0802a1d8  3368      ldr          r3, [r6]
0802a1da  012b      cmp          r3, #1
0802a1dc  04d1      bne          #8  ; -> 0x0802a1e8
0802a1de  484f      ldr          r7, [pc, #288]  ; [0x0802a300] = 0x200213b4 (f32=1.10179475e-19)
0802a1e0  3868      ldr          r0, [r7]
0802a1e2  0028      cmp          r0, #0
0802a1e4  40f02383  bne.w        #1606  ; -> 0x0802a82e
0802a1e8  109f      ldr          r7, [sp, #64]
0802a1ea  139b      ldr          r3, [sp, #76]
0802a1ec  7969      ldr          r1, [r7, #20]
0802a1ee  93ed00da  vldr         s26, [r3]
0802a1f2  b5eec0ba  vcmpe.f32    s22, #0
0802a1f6  b0eecf8a  vabs.f32     s16, s30
0802a1fa  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a1fe  4cbf      ite          mi
0802a200  78ee4b5a  vsubmi.f32   s11, s16, s22
0802a204  7bee085a  vaddpl.f32   s11, s22, s16
0802a208  0029      cmp          r1, #0
0802a20a  40f0bf80  bne.w        #382  ; -> 0x0802a38c
0802a20e  f4eecd5a  vcmpe.f32    s11, s26
0802a212  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a216  40f30082  ble.w        #1024  ; -> 0x0802a61a
0802a21a  9fed3a6a  vldr         s12, [pc, #232]  ; [0x0802a304] = 0x3f666666 (f32=0.899999976)
0802a21e  9fed3a1a  vldr         s2, [pc, #232]  ; [0x0802a308] = 0x3dcccccd (f32=0.100000001)
0802a222  6dee062a  vmul.f32     s5, s26, s12
0802a226  1946      mov          r1, r3
0802a228  e5ee812a  vfma.f32     s5, s11, s2
0802a22c  f4eee62a  vcmpe.f32    s5, s13
0802a230  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a234  c1ed002a  vstr         s5, [r1]
0802a238  40f3cc80  ble.w        #408  ; -> 0x0802a3d4
0802a23c  139d      ldr          r5, [sp, #76]
0802a23e  f7ee002a  vmov.f32     s5, #1.000000e+00
0802a242  c5ed006a  vstr         s13, [r5]
0802a246  3e9c      ldr          r4, [sp, #248]
0802a248  dfed305a  vldr         s11, [pc, #192]  ; [0x0802a30c] = 0x46fffe00 (f32=32767)
0802a24c  2768      ldr          r7, [r4]
0802a24e  3f9d      ldr          r5, [sp, #252]
0802a250  3b9b      ldr          r3, [sp, #236]
0802a252  2a68      ldr          r2, [r5]
0802a254  1c98      ldr          r0, [sp, #112]
0802a256  2fee25fa  vmul.f32     s30, s30, s11
0802a25a  2bee25ba  vmul.f32     s22, s22, s11
0802a25e  7e1c      adds         r6, r7, #1
0802a260  bdeecf2a  vcvt.s32.f32 s4, s30
0802a264  bdeecbea  vcvt.s32.f32 s28, s22
0802a268  06f03f01  and          r1, r6, #63
0802a26c  2160      str          r1, [r4]
0802a26e  03eb870c  add.w        r12, r3, r7, lsl #2
0802a272  299c      ldr          r4, [sp, #164]
0802a274  359f      ldr          r7, [sp, #212]
0802a276  2a9e      ldr          r6, [sp, #168]
0802a278  cced002a  vstr         s5, [r12]
0802a27c  12ee10aa  vmov         r10, s4
0802a280  1eee101a  vmov         r1, s28
0802a284  0232      adds         r2, #2
0802a286  27f804a0  strh.w       r10, [r7, r4]
0802a28a  0230      adds         r0, #2
0802a28c  b953      strh         r1, [r7, r6]
0802a28e  2a60      str          r2, [r5]
0802a290  2f9d      ldr          r5, [sp, #188]
0802a292  00b2      sxth         r0, r0
0802a294  8542      cmp          r5, r0
0802a296  1c90      str          r0, [sp, #112]
0802a298  7ef7e0a9  ble.w        #-7232  ; -> 0x0802865c
0802a29c  1299      ldr          r1, [sp, #72]
0802a29e  1c4d      ldr          r5, [pc, #112]  ; [0x0802a310] = 0x20022154 (f32=1.10224557e-19)
0802a2a0  91ed040a  vldr         s0, [r1, #16]
0802a2a4  d5ed005a  vldr         s11, [r5]
0802a2a8  0446      mov          r4, r0
0802a2aa  fff758b9  b.w          #-3408  ; -> 0x0802955e
0802a2ae  b0ee459a  vmov.f32     s18, s10
0802a2b2  fff702ba  b.w          #-3068  ; -> 0x080296ba
0802a2b6  37ee262a  vadd.f32     s4, s14, s13
0802a2ba  4f1e      subs         r7, r1, #1
0802a2bc  89ed002a  vstr         s4, [r9]
0802a2c0  ccf80070  str.w        r7, [r12]
0802a2c4  76e5      b            #-1300  ; -> 0x08029db4
0802a31c  f0ee454a  vmov.f32     s9, s10
0802a320  b0ee454a  vmov.f32     s8, s10
0802a324  d1e6      b            #-606  ; -> 0x0802a0ca
0802a326  002c      cmp          r4, #0
0802a328  3ff4b0ad  beq.w        #-1184  ; -> 0x08029e8c
0802a32c  1899      ldr          r1, [sp, #96]
0802a32e  109d      ldr          r5, [sp, #64]
0802a330  bf4f      ldr          r7, [pc, #764]  ; [0x0802a630] = 0x20021cbc (f32=1.10209357e-19)
0802a332  ea68      ldr          r2, [r5, #12]
0802a334  3f68      ldr          r7, [r7]
0802a336  0129      cmp          r1, #1
0802a338  7ff4f8aa  bne.w        #-2576  ; -> 0x0802992c
0802a33c  1e9b      ldr          r3, [sp, #120]
0802a33e  1c68      ldr          r4, [r3]
0802a340  002c      cmp          r4, #0
0802a342  7ff4f3aa  bne.w        #-2586  ; -> 0x0802992c
0802a346  012a      cmp          r2, #1
0802a348  00f02f82  beq.w        #1118  ; -> 0x0802a7aa
0802a34c  b94a      ldr          r2, [pc, #740]  ; [0x0802a634] = 0x20021ec8 (f32=1.1021613e-19)
0802a34e  409d      ldr          r5, [sp, #256]
0802a350  b949      ldr          r1, [pc, #740]  ; [0x0802a638] = 0x20021f70 (f32=1.10218301e-19)
0802a352  52f82640  ldr.w        r4, [r2, r6, lsl #2]
0802a356  2868      ldr          r0, [r5]
0802a358  41f82640  str.w        r4, [r1, r6, lsl #2]
0802a35c  0136      adds         r6, #1
0802a35e  8642      cmp          r6, r0
0802a360  249b      ldr          r3, [sp, #144]
0802a362  239a      ldr          r2, [sp, #140]
0802a364  0890      str          r0, [sp, #32]
0802a366  2698      ldr          r0, [sp, #152]
0802a368  b449      ldr          r1, [pc, #720]  ; [0x0802a63c] = 0x20021e64 (f32=1.10214837e-19)
0802a36a  1660      str          r6, [r2]
0802a36c  83ed005a  vstr         s10, [r3]
0802a370  08bf      it           eq
0802a372  0026      moveq        r6, #0
0802a374  0368      ldr          r3, [r0]
0802a376  b24c      ldr          r4, [pc, #712]  ; [0x0802a640] = 0xc7d90380 (f32=-111111)
0802a378  08bf      it           eq
0802a37a  1660      streq        r6, [r2]
0802a37c  01eb860b  add.w        r11, r1, r6, lsl #2
0802a380  0133      adds         r3, #1
0802a382  cbf80040  str.w        r4, [r11]
0802a386  0360      str          r3, [r0]
0802a388  fff7e9ba  b.w          #-2606  ; -> 0x0802995e
0802a38c  239a      ldr          r2, [sp, #140]
0802a38e  ad4d      ldr          r5, [pc, #692]  ; [0x0802a644] = 0x20021e84 (f32=1.10215251e-19)
0802a390  ad48      ldr          r0, [pc, #692]  ; [0x0802a648] = 0x2002209c (f32=1.10222179e-19)
0802a392  a84c      ldr          r4, [pc, #672]  ; [0x0802a634] = 0x20021ec8 (f32=1.1021613e-19)
0802a394  1668      ldr          r6, [r2]
0802a396  d0ed00ba  vldr         s23, [r0]
0802a39a  d5ed00ca  vldr         s25, [r5]
0802a39e  54f82670  ldr.w        r7, [r4, r6, lsl #2]
0802a3a2  dfedaada  vldr         s27, [pc, #680]  ; [0x0802a64c] = 0x3d75c28f (f32=0.0599999987)
0802a3a6  9fedaaaa  vldr         s20, [pc, #680]  ; [0x0802a650] = 0x3f666666 (f32=0.899999976)
0802a3aa  1399      ldr          r1, [sp, #76]
0802a3ac  0cee107a  vmov         s24, r7
0802a3b0  6ceeab0a  vmul.f32     s1, s25, s23
0802a3b4  f8eecc8a  vcvt.f32.s32 s17, s24
0802a3b8  60eead9a  vmul.f32     s19, s1, s27
0802a3bc  6dee0a2a  vmul.f32     s5, s26, s20
0802a3c0  e8eea92a  vfma.f32     s5, s17, s19
0802a3c4  f4eee62a  vcmpe.f32    s5, s13
0802a3c8  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a3cc  c1ed002a  vstr         s5, [r1]
0802a3d0  3ff734af  bgt.w        #-408  ; -> 0x0802a23c
0802a3d4  f5eec02a  vcmpe.f32    s5, #0
0802a3d8  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a3dc  7ff533af  bpl.w        #-410  ; -> 0x0802a246
0802a3e0  139b      ldr          r3, [sp, #76]
0802a3e2  f0ee452a  vmov.f32     s5, s10
0802a3e6  83ed005a  vstr         s10, [r3]
0802a3ea  2ce7      b            #-424  ; -> 0x0802a246
0802a3ec  994e      ldr          r6, [pc, #612]  ; [0x0802a654] = 0x20021f48 (f32=1.10217784e-19)
0802a3ee  109f      ldr          r7, [sp, #64]
0802a3f0  96ed00aa  vldr         s20, [r6]
0802a3f4  bc69      ldr          r4, [r7, #24]
0802a3f6  b6ee00fa  vmov.f32     s30, #5.000000e-01
0802a3fa  b4eecfaa  vcmpe.f32    s20, s30
0802a3fe  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a402  40f1b881  bpl.w        #880  ; -> 0x0802a776
0802a406  012c      cmp          r4, #1
0802a408  3ff4e0a9  beq.w        #-3136  ; -> 0x080297cc
0802a40c  002a      cmp          r2, #0
0802a40e  40f0b881  bne.w        #880  ; -> 0x0802a782
0802a412  914e      ldr          r6, [pc, #580]  ; [0x0802a658] = 0x20021c68 (f32=1.10208272e-19)
0802a414  914d      ldr          r5, [pc, #580]  ; [0x0802a65c] = 0x20022148 (f32=1.10224402e-19)
0802a416  9249      ldr          r1, [pc, #584]  ; [0x0802a660] = 0x20022144 (f32=1.1022435e-19)
0802a418  f768      ldr          r7, [r6, #12]
0802a41a  95ed002a  vldr         s4, [r5]
0802a41e  d1ed005a  vldr         s11, [r1]
0802a422  9049      ldr          r1, [pc, #576]  ; [0x0802a664] = 0x2002214c (f32=1.10224453e-19)
0802a424  b7f5fa6f  cmp.w        r7, #2000
0802a428  b4bf      ite          lt
0802a42a  32ee652a  vsublt.f32   s4, s4, s11
0802a42e  32ee252a  vaddge.f32   s4, s4, s11
0802a432  0b68      ldr          r3, [r1]
0802a434  fdeec2ca  vcvt.s32.f32 s25, s4
0802a438  f8eeec7a  vcvt.f32.s32 s15, s25
0802a43c  1cee900a  vmov         r0, s25
0802a440  72ee67da  vsub.f32     s27, s4, s15
0802a444  0344      add          r3, r0
0802a446  0028      cmp          r0, #0
0802a448  c5ed00da  vstr         s27, [r5]
0802a44c  0b60      str          r3, [r1]
0802a44e  c0f20d83  blt.w        #1562  ; -> 0x0802aa6c
0802a452  854c      ldr          r4, [pc, #532]  ; [0x0802a668] = 0x20021de4 (f32=1.10213183e-19)
0802a454  0c9e      ldr          r6, [sp, #48]
0802a456  2268      ldr          r2, [r4]
0802a458  379d      ldr          r5, [sp, #220]
0802a45a  08ee103a  vmov         s16, r3
0802a45e  06eb8209  add.w        r9, r6, r2, lsl #2
0802a462  d9ed00aa  vldr         s21, [r9]
0802a466  2868      ldr          r0, [r5]
0802a468  f8eec89a  vcvt.f32.s32 s19, s16
0802a46c  f4eeea9a  vcmpe.f32    s19, s21
0802a470  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a474  00f3df82  bgt.w        #1470  ; -> 0x0802aa36
0802a478  002b      cmp          r3, #0
0802a47a  80f2b680  bge.w        #364  ; -> 0x0802a5ea
0802a47e  7aeee64a  vsub.f32     s9, s21, s13
0802a482  0128      cmp          r0, #1
0802a484  bdeee44a  vcvt.s32.f32 s8, s9
0802a488  81ed004a  vstr         s8, [r1]
0802a48c  7149      ldr          r1, [pc, #452]  ; [0x0802a654] = 0x20021f48 (f32=1.10217784e-19)
0802a48e  91ed00aa  vldr         s20, [r1]
0802a492  40f0b080  bne.w        #352  ; -> 0x0802a5f6
0802a496  249b      ldr          r3, [sp, #144]
0802a498  149c      ldr          r4, [sp, #80]
0802a49a  d3ed008a  vldr         s17, [r3]
0802a49e  94ed00fa  vldr         s30, [r4]
0802a4a2  b3e0      b            #358  ; -> 0x0802a60c
0802a4a4  b5eec07a  vcmpe.f32    s14, #0
0802a4a8  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a4ac  40f38081  ble.w        #768  ; -> 0x0802a7b0
0802a4b0  b0ee4fba  vmov.f32     s22, s30
0802a4b4  cded0f4a  vstr         s9, [sp, #60]
0802a4b8  fff71cb9  b.w          #-3528  ; -> 0x080296f4
0802a4bc  6b4c      ldr          r4, [pc, #428]  ; [0x0802a66c] = 0x2002210c (f32=1.10223626e-19)
0802a4be  6c4d      ldr          r5, [pc, #432]  ; [0x0802a670] = 0x20021ca4 (f32=1.10209047e-19)
0802a4c0  2068      ldr          r0, [r4]
0802a4c2  2968      ldr          r1, [r5]
0802a4c4  6b4f      ldr          r7, [pc, #428]  ; [0x0802a674] = 0x200213b4 (f32=1.10179475e-19)
0802a4c6  0026      movs         r6, #0
0802a4c8  8842      cmp          r0, r1
0802a4ca  3e60      str          r6, [r7]
0802a4cc  15da      bge          #42  ; -> 0x0802a4fa
0802a4ce  dfed6aaa  vldr         s21, [pc, #424]  ; [0x0802a678] = 0x46fffe00 (f32=32767)
0802a4d2  6a4f      ldr          r7, [pc, #424]  ; [0x0802a67c] = 0x200220fc (f32=1.10223419e-19)
0802a4d4  6a4a      ldr          r2, [pc, #424]  ; [0x0802a680] = 0x20022100 (f32=1.10223471e-19)
0802a4d6  3e68      ldr          r6, [r7]
0802a4d8  1268      ldr          r2, [r2]
0802a4da  63eeaa4a  vmul.f32     s9, s7, s21
0802a4de  23ee2a4a  vmul.f32     s8, s6, s21
0802a4e2  bdeee49a  vcvt.s32.f32 s18, s9
0802a4e6  bdeec40a  vcvt.s32.f32 s0, s8
0802a4ea  19ee10aa  vmov         r10, s18
0802a4ee  10ee107a  vmov         r7, s0
0802a4f2  26f810a0  strh.w       r10, [r6, r0, lsl #1]
0802a4f6  22f81070  strh.w       r7, [r2, r0, lsl #1]
0802a4fa  624a      ldr          r2, [pc, #392]  ; [0x0802a684] = 0x2002208c (f32=1.10221972e-19)
0802a4fc  624e      ldr          r6, [pc, #392]  ; [0x0802a688] = 0x20001290 (f32=1.08481635e-19)
0802a4fe  1768      ldr          r7, [r2]
0802a500  3268      ldr          r2, [r6]
0802a502  0130      adds         r0, #1
0802a504  2060      str          r0, [r4]
0802a506  002a      cmp          r2, #0
0802a508  40f02081  bne.w        #576  ; -> 0x0802a74c
0802a50c  bb42      cmp          r3, r7
0802a50e  40f33d81  ble.w        #634  ; -> 0x0802a78c
0802a512  8142      cmp          r1, r0
0802a514  40f3ce80  ble.w        #412  ; -> 0x0802a6b4
0802a518  5c4f      ldr          r7, [pc, #368]  ; [0x0802a68c] = 0x20021320 (f32=1.10177562e-19)
0802a51a  3d68      ldr          r5, [r7]
0802a51c  002d      cmp          r5, #0
0802a51e  7ff455ae  bne.w        #-854  ; -> 0x0802a1cc
0802a522  2b9a      ldr          r2, [sp, #172]
0802a524  1068      ldr          r0, [r2]
0802a526  0128      cmp          r0, #1
0802a528  7ff450ae  bne.w        #-864  ; -> 0x0802a1cc
0802a52c  4e9c      ldr          r4, [sp, #312]
0802a52e  24b9      cbnz         r4, #8  ; -> 0x0802a53a
0802a530  574e      ldr          r6, [pc, #348]  ; [0x0802a690] = 0x20021cb0 (f32=1.10209202e-19)
0802a532  3368      ldr          r3, [r6]
0802a534  002b      cmp          r3, #0
0802a536  7ff449ae  bne.w        #-878  ; -> 0x0802a1cc
0802a53a  2c99      ldr          r1, [sp, #176]
0802a53c  514f      ldr          r7, [pc, #324]  ; [0x0802a684] = 0x2002208c (f32=1.10221972e-19)
0802a53e  0868      ldr          r0, [r1]
0802a540  3d68      ldr          r5, [r7]
0802a542  4d9a      ldr          r2, [sp, #308]
0802a544  a842      cmp          r0, r5
0802a546  d4bf      ite          le
0802a548  0020      movle        r0, #0
0802a54a  0120      movgt        r0, #1
0802a54c  1168      ldr          r1, [r2]
0802a54e  fcf7fdfe  bl           #-12806  ; -> 0x0802734c
0802a552  2898      ldr          r0, [sp, #160]
0802a554  0468      ldr          r4, [r0]
0802a556  012c      cmp          r4, #1
0802a558  7ff43dae  bne.w        #-902  ; -> 0x0802a1d6
0802a55c  4d4e      ldr          r6, [pc, #308]  ; [0x0802a694] = 0x20022070 (f32=1.1022161e-19)
0802a55e  4e4f      ldr          r7, [pc, #312]  ; [0x0802a698] = 0x20021f28 (f32=1.10217371e-19)
0802a560  3168      ldr          r1, [r6]
0802a562  444b      ldr          r3, [pc, #272]  ; [0x0802a674] = 0x200213b4 (f32=1.10179475e-19)
0802a564  289a      ldr          r2, [sp, #160]
0802a566  2d9e      ldr          r6, [sp, #180]
0802a568  57f82150  ldr.w        r5, [r7, r1, lsl #2]
0802a56c  1d60      str          r5, [r3]
0802a56e  0220      movs         r0, #2
0802a570  0024      movs         r4, #0
0802a572  1060      str          r0, [r2]
0802a574  3460      str          r4, [r6]
0802a576  37e6      b            #-914  ; -> 0x0802a1e8
0802a578  1c9d      ldr          r5, [sp, #112]
0802a57a  002d      cmp          r5, #0
0802a57c  7ff42ea9  bne.w        #-3492  ; -> 0x080297dc
0802a580  384f      ldr          r7, [pc, #224]  ; [0x0802a664] = 0x2002214c (f32=1.10224453e-19)
0802a582  354c      ldr          r4, [pc, #212]  ; [0x0802a658] = 0x20021c68 (f32=1.10208272e-19)
0802a584  97ed001a  vldr         s2, [r7]
0802a588  e068      ldr          r0, [r4, #12]
0802a58a  374b      ldr          r3, [pc, #220]  ; [0x0802a668] = 0x20021de4 (f32=1.10213183e-19)
0802a58c  0c9d      ldr          r5, [sp, #48]
0802a58e  1968      ldr          r1, [r3]
0802a590  b8eec10a  vcvt.f32.s32 s0, s2
0802a594  b0f5fa6f  cmp.w        r0, #2000
0802a598  b4bf      ite          lt
0802a59a  30ee4f0a  vsublt.f32   s0, s0, s30
0802a59e  30ee0f0a  vaddge.f32   s0, s0, s30
0802a5a2  05eb810b  add.w        r11, r5, r1, lsl #2
0802a5a6  bdeec06a  vcvt.s32.f32 s12, s0
0802a5aa  9bed00ca  vldr         s24, [r11]
0802a5ae  87ed006a  vstr         s12, [r7]
0802a5b2  f8eec60a  vcvt.f32.s32 s1, s12
0802a5b6  f4eecc0a  vcmpe.f32    s1, s24
0802a5ba  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a5be  00f33382  bgt.w        #1126  ; -> 0x0802aa28
0802a5c2  f5eec00a  vcmpe.f32    s1, #0
0802a5c6  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a5ca  05d5      bpl          #10  ; -> 0x0802a5d8
0802a5cc  7cee66ba  vsub.f32     s23, s24, s13
0802a5d0  bdeeebea  vcvt.s32.f32 s28, s23
0802a5d4  87ed00ea  vstr         s28, [r7]
0802a5d8  379f      ldr          r7, [sp, #220]
0802a5da  012a      cmp          r2, #1
0802a5dc  3868      ldr          r0, [r7]
0802a5de  00f0f980  beq.w        #498  ; -> 0x0802a7d4
0802a5e2  1f9b      ldr          r3, [sp, #124]
0802a5e4  249d      ldr          r5, [sp, #144]
0802a5e6  1968      ldr          r1, [r3]
0802a5e8  2960      str          r1, [r5]
0802a5ea  1a49      ldr          r1, [pc, #104]  ; [0x0802a654] = 0x20021f48 (f32=1.10217784e-19)
0802a5ec  0128      cmp          r0, #1
0802a5ee  91ed00aa  vldr         s20, [r1]
0802a5f2  3ff450af  beq.w        #-352  ; -> 0x0802a496
0802a5f6  1498      ldr          r0, [sp, #80]
0802a5f8  90ed00fa  vldr         s30, [r0]
0802a5fc  fff7f6b8  b.w          #-3604  ; -> 0x080297ec
0802a600  249c      ldr          r4, [sp, #144]
0802a602  012a      cmp          r2, #1
0802a604  d4ed008a  vldr         s17, [r4]
0802a608  00f0ed80  beq.w        #474  ; -> 0x0802a7e6
0802a60c  38eea61a  vadd.f32     s2, s17, s13
0802a610  249a      ldr          r2, [sp, #144]
0802a612  82ed001a  vstr         s2, [r2]
0802a616  fff7e9b8  b.w          #-3630  ; -> 0x080297ec
0802a61a  9fed207a  vldr         s14, [pc, #128]  ; [0x0802a69c] = 0x3f7fbe77 (f32=0.999000013)
0802a61e  dfed201a  vldr         s3, [pc, #128]  ; [0x0802a6a0] = 0x3a83126f (f32=0.00100000005)
0802a622  6dee072a  vmul.f32     s5, s26, s14
0802a626  1946      mov          r1, r3
0802a628  e5eea12a  vfma.f32     s5, s11, s3
0802a62c  fee5      b            #-1028  ; -> 0x0802a22c
0802a6a4  0224      movs         r4, #2
0802a6a6  f0ee45ea  vmov.f32     s29, s10
0802a6aa  f7ee002a  vmov.f32     s5, #1.000000e+00
0802a6ae  0394      str          r4, [sp, #12]
0802a6b0  fff73ebb  b.w          #-2436  ; -> 0x08029d30
0802a6b4  9149      ldr          r1, [pc, #580]  ; [0x0802a8fc] = 0x20021314 (f32=1.10177407e-19)
0802a6b6  924f      ldr          r7, [pc, #584]  ; [0x0802a900] = 0x20021c9c (f32=1.10208944e-19)
0802a6b8  0b68      ldr          r3, [r1]
0802a6ba  0093      str          r3, [sp]
0802a6bc  914b      ldr          r3, [pc, #580]  ; [0x0802a904] = 0x20021320 (f32=1.10177562e-19)
0802a6be  3a60      str          r2, [r7]
0802a6c0  1a60      str          r2, [r3]
0802a6c2  2b9b      ldr          r3, [sp, #172]
0802a6c4  1a60      str          r2, [r3]
0802a6c6  009a      ldr          r2, [sp]
0802a6c8  9042      cmp          r0, r2
0802a6ca  c8bf      it           gt
0802a6cc  0860      strgt        r0, [r1]
0802a6ce  fbf7a3ff  bl           #-16570  ; -> 0x08026618
0802a6d2  3068      ldr          r0, [r6]
0802a6d4  3799      ldr          r1, [sp, #220]
0802a6d6  8c4b      ldr          r3, [pc, #560]  ; [0x0802a908] = 0x20021c68 (f32=1.10208272e-19)
0802a6d8  0126      movs         r6, #1
0802a6da  b042      cmp          r0, r6
0802a6dc  08bf      it           eq
0802a6de  8b48      ldreq        r0, [pc, #556]  ; [0x0802a90c] = 0x20022074 (f32=1.10221662e-19)
0802a6e0  0e60      str          r6, [r1]
0802a6e2  0abf      itet         eq
0802a6e4  0668      ldreq        r6, [r0]
0802a6e6  8a48      ldrne        r0, [pc, #552]  ; [0x0802a910] = 0x20021328 (f32=1.10177666e-19)
0802a6e8  46f40866  orreq        r6, r6, #2176
0802a6ec  0660      str          r6, [r0]
0802a6ee  9b46      mov          r11, r3
0802a6f0  d3e90027  ldrd         r2, r7, [r3]
0802a6f4  1e69      ldr          r6, [r3, #16]
0802a6f6  2968      ldr          r1, [r5]
0802a6f8  dbf81850  ldr.w        r5, [r11, #24]
0802a6fc  0195      str          r5, [sp, #4]
0802a6fe  0097      str          r7, [sp]
0802a700  06f1640c  add.w        r12, r6, #100
0802a704  019e      ldr          r6, [sp, #4]
0802a706  009d      ldr          r5, [sp]
0802a708  06f16408  add.w        r8, r6, #100
0802a70c  814e      ldr          r6, [pc, #516]  ; [0x0802a914] = 0x200220a4 (f32=1.10222282e-19)
0802a70e  6432      adds         r2, #100
0802a710  05f16409  add.w        r9, r5, #100
0802a714  d3e90207  ldrd         r0, r7, [r3, #8]
0802a718  c6e90029  strd         r2, r9, [r6]
0802a71c  7a4a      ldr          r2, [pc, #488]  ; [0x0802a908] = 0x20021c68 (f32=1.10208272e-19)
0802a71e  5b69      ldr          r3, [r3, #20]
0802a720  d269      ldr          r2, [r2, #28]
0802a722  c6f810c0  str.w        r12, [r6, #16]
0802a726  6430      adds         r0, #100
0802a728  6437      adds         r7, #100
0802a72a  6433      adds         r3, #100
0802a72c  b060      str          r0, [r6, #8]
0802a72e  f760      str          r7, [r6, #12]
0802a730  2c98      ldr          r0, [sp, #176]
0802a732  794f      ldr          r7, [pc, #484]  ; [0x0802a918] = 0x2002208c (f32=1.10221972e-19)
0802a734  7361      str          r3, [r6, #20]
0802a736  6432      adds         r2, #100
0802a738  c6f81880  str.w        r8, [r6, #24]
0802a73c  f261      str          r2, [r6, #28]
0802a73e  4f9e      ldr          r6, [sp, #316]
0802a740  0368      ldr          r3, [r0]
0802a742  3f68      ldr          r7, [r7]
0802a744  2068      ldr          r0, [r4]
0802a746  4ff4f075  mov.w        r5, #480
0802a74a  3560      str          r5, [r6]
0802a74c  bb42      cmp          r3, r7
0802a74e  1ddd      ble          #58  ; -> 0x0802a78c
0802a750  8842      cmp          r0, r1
0802a752  fff6e1ae  blt.w        #-574  ; -> 0x0802a518
0802a756  4d99      ldr          r1, [sp, #308]
0802a758  704c      ldr          r4, [pc, #448]  ; [0x0802a91c] = 0x200213b8 (f32=1.10179527e-19)
0802a75a  714e      ldr          r6, [pc, #452]  ; [0x0802a920] = 0x2002133c (f32=1.10177924e-19)
0802a75c  0968      ldr          r1, [r1]
0802a75e  9f42      cmp          r7, r3
0802a760  a8bf      it           ge
0802a762  0020      movge        r0, #0
0802a764  4ff4fa63  mov.w        r3, #2000
0802a768  b8bf      it           lt
0802a76a  0120      movlt        r0, #1
0802a76c  2360      str          r3, [r4]
0802a76e  3360      str          r3, [r6]
0802a770  fcf7ecfd  bl           #-13352  ; -> 0x0802734c
0802a774  d0e6      b            #-608  ; -> 0x0802a518
0802a776  022c      cmp          r4, #2
0802a778  7ff428a8  bne.w        #-4016  ; -> 0x080297cc
0802a77c  002a      cmp          r2, #0
0802a77e  3ff448ae  beq.w        #-880  ; -> 0x0802a412
0802a782  149e      ldr          r6, [sp, #80]
0802a784  96ed00fa  vldr         s30, [r6]
0802a788  fff72bb8  b.w          #-4010  ; -> 0x080297e2
0802a78c  654d      ldr          r5, [pc, #404]  ; [0x0802a924] = 0x2002215c (f32=1.1022466e-19)
0802a78e  55f82320  ldr.w        r2, [r5, r3, lsl #2]
0802a792  8242      cmp          r2, r0
0802a794  dcdc      bgt          #-72  ; -> 0x0802a750
0802a796  581e      subs         r0, r3, #1
0802a798  55f82000  ldr.w        r0, [r5, r0, lsl #2]
0802a79c  2060      str          r0, [r4]
0802a79e  d7e7      b            #-82  ; -> 0x0802a750
0802a7a0  6149      ldr          r1, [pc, #388]  ; [0x0802a928] = 0x20021de4 (f32=1.10213183e-19)
0802a7a2  0b68      ldr          r3, [r1]
0802a7a4  bb42      cmp          r3, r7
0802a7a6  3ff4c4a8  beq.w        #-3704  ; -> 0x08029932
0802a7aa  5f4b      ldr          r3, [pc, #380]  ; [0x0802a928] = 0x20021de4 (f32=1.10213183e-19)
0802a7ac  1f60      str          r7, [r3]
0802a7ae  cde5      b            #-1126  ; -> 0x0802a34c
0802a7b0  b1ee471a  vneg.f32     s2, s14
0802a7b4  bdeec10a  vcvt.s32.f32 s0, s2
0802a7b8  b8eec06a  vcvt.f32.s32 s12, s0
0802a7bc  8ded0f0a  vstr         s0, [sp, #60]
0802a7c0  31ee46ba  vsub.f32     s22, s2, s12
0802a7c4  fef796bf  b.w          #-4308  ; -> 0x080296f4
0802a7c8  b7ee002a  vmov.f32     s4, #1.000000e+00
0802a7cc  8ded362a  vstr         s4, [sp, #216]
0802a7d0  fdf721ba  b.w          #-11198  ; -> 0x08027c16
0802a7d4  554a      ldr          r2, [pc, #340]  ; [0x0802a92c] = 0x20021f48 (f32=1.10217784e-19)
0802a7d6  0128      cmp          r0, #1
0802a7d8  92ed00aa  vldr         s20, [r2]
0802a7dc  7ff406a8  bne.w        #-4084  ; -> 0x080297ec
0802a7e0  249c      ldr          r4, [sp, #144]
0802a7e2  d4ed008a  vldr         s17, [r4]
0802a7e6  b5eec07a  vcmpe.f32    s14, #0
0802a7ea  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a7ee  00f1b681  bmi.w        #876  ; -> 0x0802ab5e
0802a7f2  7ff70baf  ble.w        #-490  ; -> 0x0802a60c
0802a7f6  77ee282a  vadd.f32     s5, s14, s17
0802a7fa  249a      ldr          r2, [sp, #144]
0802a7fc  c2ed002a  vstr         s5, [r2]
0802a800  fef7f4bf  b.w          #-4120  ; -> 0x080297ec
0802a804  012f      cmp          r7, #1
0802a806  7df415ac  bne.w        #-10198  ; -> 0x08028034
0802a80a  249a      ldr          r2, [sp, #144]
0802a80c  d2ed000a  vldr         s1, [r2]
0802a810  b5eec07a  vcmpe.f32    s14, #0
0802a814  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a818  00f19a81  bmi.w        #820  ; -> 0x0802ab50
0802a81c  7ef780ad  ble.w        #-5376  ; -> 0x08029320
0802a820  77ee203a  vadd.f32     s7, s14, s1
0802a824  249a      ldr          r2, [sp, #144]
0802a826  c2ed003a  vstr         s7, [r2]
0802a82a  fdf703bc  b.w          #-10234  ; -> 0x08028034
0802a82e  4049      ldr          r1, [pc, #256]  ; [0x0802a930] = 0x200213ac (f32=1.10179372e-19)
0802a830  4ff47a75  mov.w        r5, #1000
0802a834  0d60      str          r5, [r1]
0802a836  fbf7effe  bl           #-16930  ; -> 0x08026618
0802a83a  2d9a      ldr          r2, [sp, #180]
0802a83c  289c      ldr          r4, [sp, #160]
0802a83e  0020      movs         r0, #0
0802a840  1060      str          r0, [r2]
0802a842  2060      str          r0, [r4]
0802a844  3860      str          r0, [r7]
0802a846  cfe4      b            #-1634  ; -> 0x0802a1e8
0802a848  3949      ldr          r1, [pc, #228]  ; [0x0802a930] = 0x200213ac (f32=1.10179372e-19)
0802a84a  4ff47a75  mov.w        r5, #1000
0802a84e  0d60      str          r5, [r1]
0802a850  fbf7e2fe  bl           #-16956  ; -> 0x08026618
0802a854  2d9a      ldr          r2, [sp, #180]
0802a856  289c      ldr          r4, [sp, #160]
0802a858  0020      movs         r0, #0
0802a85a  1060      str          r0, [r2]
0802a85c  2060      str          r0, [r4]
0802a85e  3860      str          r0, [r7]
0802a860  fef75bbb  b.w          #-6474  ; -> 0x08028f1a
0802a864  dfed334a  vldr         s9, [pc, #204]  ; [0x0802a934] = 0x00000000 (f32=0)
0802a868  b0ee644a  vmov.f32     s8, s9
0802a86c  14e4      b            #-2008  ; -> 0x0802a098
0802a86e  9fed313a  vldr         s6, [pc, #196]  ; [0x0802a934] = 0x00000000 (f32=0)
0802a872  f0ee432a  vmov.f32     s5, s6
0802a876  fef7a8ba  b.w          #-6832  ; -> 0x08028dca
0802a87a  2f4f      ldr          r7, [pc, #188]  ; [0x0802a938] = 0x200012a8 (f32=1.08481946e-19)
0802a87c  3968      ldr          r1, [r7]
0802a87e  0029      cmp          r1, #0
0802a880  7df4aba9  bne.w        #-11434  ; -> 0x08027bda
0802a884  dfed2dea  vldr         s29, [pc, #180]  ; [0x0802a93c] = 0x3d231090 (f32=0.0398107171)
0802a888  f4eeeeda  vcmpe.f32    s27, s29
0802a88c  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802a890  b8bf      it           lt
0802a892  f0ee6eda  vmovlt.f32   s27, s29
0802a896  2deeadfa  vmul.f32     s30, s27, s27
0802a89a  2fee2d0a  vmul.f32     s0, s30, s27
0802a89e  8ded300a  vstr         s0, [sp, #192]
0802a8a2  fdf79cb9  b.w          #-11464  ; -> 0x08027bde
0802a8a6  089d      ldr          r5, [sp, #32]
0802a8a8  2399      ldr          r1, [sp, #140]
0802a8aa  254c      ldr          r4, [pc, #148]  ; [0x0802a940] = 0x20021e64 (f32=1.10214837e-19)
0802a8ac  249a      ldr          r2, [sp, #144]
0802a8ae  2548      ldr          r0, [pc, #148]  ; [0x0802a944] = 0xc7d90380 (f32=-111111)
0802a8b0  82ed005a  vstr         s10, [r2]
0802a8b4  0136      adds         r6, #1
0802a8b6  ae42      cmp          r6, r5
0802a8b8  08bf      it           eq
0802a8ba  0026      moveq        r6, #0
0802a8bc  04eb8608  add.w        r8, r4, r6, lsl #2
0802a8c0  0e60      str          r6, [r1]
0802a8c2  269e      ldr          r6, [sp, #152]
0802a8c4  c8f80000  str.w        r0, [r8]
0802a8c8  0133      adds         r3, #1
0802a8ca  3360      str          r3, [r6]
0802a8cc  fff747b8  b.w          #-3954  ; -> 0x0802995e
0802a8d0  0b9a      ldr          r2, [sp, #44]
0802a8d2  239c      ldr          r4, [sp, #140]
0802a8d4  1a49      ldr          r1, [pc, #104]  ; [0x0802a940] = 0x20021e64 (f32=1.10214837e-19)
0802a8d6  249e      ldr          r6, [sp, #144]
0802a8d8  1a4d      ldr          r5, [pc, #104]  ; [0x0802a944] = 0xc7d90380 (f32=-111111)
0802a8da  86ed00ea  vstr         s28, [r6]
0802a8de  0137      adds         r7, #1
0802a8e0  9742      cmp          r7, r2
0802a8e2  08bf      it           eq
0802a8e4  0027      moveq        r7, #0
0802a8e6  01eb870c  add.w        r12, r1, r7, lsl #2
0802a8ea  2760      str          r7, [r4]
0802a8ec  269f      ldr          r7, [sp, #152]
0802a8ee  ccf80050  str.w        r5, [r12]
0802a8f2  0133      adds         r3, #1
0802a8f4  3b60      str          r3, [r7]
0802a8f6  fdf700bc  b.w          #-10240  ; -> 0x080280fa
0802a948  002b      cmp          r3, #0
0802a94a  7df42ba9  bne.w        #-11690  ; -> 0x08027ba4
0802a94e  bc4f      ldr          r7, [pc, #752]  ; [0x0802ac40] = 0x20021cbc (f32=1.10209357e-19)
0802a950  3360      str          r3, [r6]
0802a952  3868      ldr          r0, [r7]
0802a954  bb4e      ldr          r6, [pc, #748]  ; [0x0802ac44] = 0x20021320 (f32=1.10177562e-19)
0802a956  0130      adds         r0, #1
0802a958  8242      cmp          r2, r0
0802a95a  b8bf      it           lt
0802a95c  2046      movlt        r0, r4
0802a95e  3468      ldr          r4, [r6]
0802a960  3860      str          r0, [r7]
0802a962  012c      cmp          r4, #1
0802a964  7df423a9  bne.w        #-11706  ; -> 0x08027bae
0802a968  4e9d      ldr          r5, [sp, #312]
0802a96a  25b9      cbnz         r5, #8  ; -> 0x0802a976
0802a96c  b649      ldr          r1, [pc, #728]  ; [0x0802ac48] = 0x20021cb0 (f32=1.10209202e-19)
0802a96e  0b68      ldr          r3, [r1]
0802a970  002b      cmp          r3, #0
0802a972  7df41ca9  bne.w        #-11720  ; -> 0x08027bae
0802a976  b54f      ldr          r7, [pc, #724]  ; [0x0802ac4c] = 0x20022108 (f32=1.10223574e-19)
0802a978  2b97      str          r7, [sp, #172]
0802a97a  3868      ldr          r0, [r7]
0802a97c  0028      cmp          r0, #0
0802a97e  7df416a9  bne.w        #-11732  ; -> 0x08027bae
0802a982  b34d      ldr          r5, [pc, #716]  ; [0x0802ac50] = 0x20022074 (f32=1.10221662e-19)
0802a984  b348      ldr          r0, [pc, #716]  ; [0x0802ac54] = 0x200012a8 (f32=1.08481946e-19)
0802a986  2c68      ldr          r4, [r5]
0802a988  c169      ldr          r1, [r0, #28]
0802a98a  b348      ldr          r0, [pc, #716]  ; [0x0802ac58] = 0x200213a8 (f32=1.1017932e-19)
0802a98c  2890      str          r0, [sp, #160]
0802a98e  44f00803  orr          r3, r4, #8
0802a992  0126      movs         r6, #1
0802a994  2b60      str          r3, [r5]
0802a996  3e60      str          r6, [r7]
0802a998  0568      ldr          r5, [r0]
0802a99a  0029      cmp          r1, #0
0802a99c  00f0c281  beq.w        #900  ; -> 0x0802ad24
0802a9a0  002d      cmp          r5, #0
0802a9a2  00f0c281  beq.w        #900  ; -> 0x0802ad2a
0802a9a6  ad49      ldr          r1, [pc, #692]  ; [0x0802ac5c] = 0x20021cb8 (f32=1.10209306e-19)
0802a9a8  ad4c      ldr          r4, [pc, #692]  ; [0x0802ac60] = 0x20021fb8 (f32=1.10219232e-19)
0802a9aa  0d68      ldr          r5, [r1]
0802a9ac  ad48      ldr          r0, [pc, #692]  ; [0x0802ac64] = 0x20022114 (f32=1.10223729e-19)
0802a9ae  ae4e      ldr          r6, [pc, #696]  ; [0x0802ac68] = 0x20021ca4 (f32=1.10209047e-19)
0802a9b0  54f82530  ldr.w        r3, [r4, r5, lsl #2]
0802a9b4  2c90      str          r0, [sp, #176]
0802a9b6  0360      str          r3, [r0]
0802a9b8  3768      ldr          r7, [r6]
0802a9ba  002a      cmp          r2, #0
0802a9bc  40f0e681  bne.w        #972  ; -> 0x0802ad8c
0802a9c0  aa4d      ldr          r5, [pc, #680]  ; [0x0802ac6c] = 0x2002215c (f32=1.1022466e-19)
0802a9c2  ab4c      ldr          r4, [pc, #684]  ; [0x0802ac70] = 0x2002210c (f32=1.10223626e-19)
0802a9c4  2d68      ldr          r5, [r5]
0802a9c6  2c99      ldr          r1, [sp, #176]
0802a9c8  2560      str          r5, [r4]
0802a9ca  0122      movs         r2, #1
0802a9cc  bd42      cmp          r5, r7
0802a9ce  0a60      str          r2, [r1]
0802a9d0  0cdb      blt          #24  ; -> 0x0802a9ec
0802a9d2  a84f      ldr          r7, [pc, #672]  ; [0x0802ac74] = 0x200213b8 (f32=1.10179527e-19)
0802a9d4  a84b      ldr          r3, [pc, #672]  ; [0x0802ac78] = 0x2002133c (f32=1.10177924e-19)
0802a9d6  a949      ldr          r1, [pc, #676]  ; [0x0802ac7c] = 0x20021c9c (f32=1.10208944e-19)
0802a9d8  2b9c      ldr          r4, [sp, #172]
0802a9da  9a4a      ldr          r2, [pc, #616]  ; [0x0802ac44] = 0x20021320 (f32=1.10177562e-19)
0802a9dc  0026      movs         r6, #0
0802a9de  4ff4fa60  mov.w        r0, #2000
0802a9e2  3860      str          r0, [r7]
0802a9e4  1860      str          r0, [r3]
0802a9e6  0e60      str          r6, [r1]
0802a9e8  2660      str          r6, [r4]
0802a9ea  1660      str          r6, [r2]
0802a9ec  a44f      ldr          r7, [pc, #656]  ; [0x0802ac80] = 0x2002131c (f32=1.10177511e-19)
0802a9ee  a549      ldr          r1, [pc, #660]  ; [0x0802ac84] = 0x20021318 (f32=1.10177459e-19)
0802a9f0  a548      ldr          r0, [pc, #660]  ; [0x0802ac88] = 0x2002207c (f32=1.10221765e-19)
0802a9f2  289c      ldr          r4, [sp, #160]
0802a9f4  a54e      ldr          r6, [pc, #660]  ; [0x0802ac8c] = 0x33d6bf95 (f32=1.00000001e-07)
0802a9f6  0660      str          r6, [r0]
0802a9f8  0023      movs         r3, #0
0802a9fa  ed00      lsls         r5, r5, #3
0802a9fc  3d60      str          r5, [r7]
0802a9fe  0b60      str          r3, [r1]
0802aa00  2360      str          r3, [r4]
0802aa02  fdf7d4b8  b.w          #-11864  ; -> 0x08027bae
0802aa06  002b      cmp          r3, #0
0802aa08  7df4cca8  bne.w        #-11880  ; -> 0x08027ba4
0802aa0c  8c4d      ldr          r5, [pc, #560]  ; [0x0802ac40] = 0x20021cbc (f32=1.10209357e-19)
0802aa0e  3360      str          r3, [r6]
0802aa10  2968      ldr          r1, [r5]
0802aa12  4b1e      subs         r3, r1, #1
0802aa14  002b      cmp          r3, #0
0802aa16  d8bf      it           le
0802aa18  1346      movle        r3, r2
0802aa1a  2b60      str          r3, [r5]
0802aa1c  fdf7c2b8  b.w          #-11900  ; -> 0x08027ba4
0802aa20  0025      movs         r5, #0
0802aa22  3560      str          r5, [r6]
0802aa24  fef766bc  b.w          #-5940  ; -> 0x080292f4
0802aa28  0026      movs         r6, #0
0802aa2a  3e60      str          r6, [r7]
0802aa2c  d4e5      b            #-1112  ; -> 0x0802a5d8
0802aa2e  0020      movs         r0, #0
0802aa30  0860      str          r0, [r1]
0802aa32  fef768bc  b.w          #-5936  ; -> 0x08029306
0802aa36  0027      movs         r7, #0
0802aa38  0f60      str          r7, [r1]
0802aa3a  d6e5      b            #-1108  ; -> 0x0802a5ea
0802aa3c  944b      ldr          r3, [pc, #592]  ; [0x0802ac90] = 0x20024128 (f32=1.10329867e-19)
0802aa3e  4f93      str          r3, [sp, #316]
0802aa40  002a      cmp          r2, #0
0802aa42  3cf7b5ae  bgt.w        #-12950  ; -> 0x080277b0
0802aa46  012a      cmp          r2, #1
0802aa48  924e      ldr          r6, [pc, #584]  ; [0x0802ac94] = 0x20001040 (f32=1.08473984e-19)
0802aa4a  7cf4c6ae  bne.w        #-12916  ; -> 0x080277da
0802aa4e  7c49      ldr          r1, [pc, #496]  ; [0x0802ac40] = 0x20021cbc (f32=1.10209357e-19)
0802aa50  3396      str          r6, [sp, #204]
0802aa52  0a60      str          r2, [r1]
0802aa54  3068      ldr          r0, [r6]
0802aa56  def80030  ldr.w        r3, [lr]
0802aa5a  0027      movs         r7, #0
0802aa5c  fdf72dbf  b.w          #-8614  ; -> 0x080288ba
0802aa60  8b4b      ldr          r3, [pc, #556]  ; [0x0802ac90] = 0x20024128 (f32=1.10329867e-19)
0802aa62  8d4d      ldr          r5, [pc, #564]  ; [0x0802ac98] = 0x20021dec (f32=1.10213286e-19)
0802aa64  4f93      str          r3, [sp, #316]
0802aa66  1295      str          r5, [sp, #72]
0802aa68  fcf7b3be  b.w          #-12954  ; -> 0x080277d2
0802aa6c  3deea6da  vadd.f32     s26, s27, s13
0802aa70  013b      subs         r3, #1
0802aa72  85ed00da  vstr         s26, [r5]
0802aa76  0b60      str          r3, [r1]
0802aa78  ebe4      b            #-1578  ; -> 0x0802a452
0802aa7a  3ceeabda  vadd.f32     s26, s25, s23
0802aa7e  013b      subs         r3, #1
0802aa80  85ed00da  vstr         s26, [r5]
0802aa84  0b60      str          r3, [r1]
0802aa86  fef78abb  b.w          #-6380  ; -> 0x0802919e
0802aa8a  b0ee4f7a  vmov.f32     s14, s30
0802aa8e  f1ee001a  vmov.f32     s3, #4.000000e+00
0802aa92  fdf747bf  b.w          #-8562  ; -> 0x08028924
0802aa96  9fed81ea  vldr         s28, [pc, #516]  ; [0x0802ac9c] = 0x43c80000 (f32=400)
0802aa9a  814f      ldr          r7, [pc, #516]  ; [0x0802aca0] = 0x200220dc (f32=1.10223006e-19)
0802aa9c  69ee0eea  vmul.f32     s29, s18, s28
0802aaa0  40f28f16  movw         r6, #399
0802aaa4  bdeeeefa  vcvt.s32.f32 s30, s29
0802aaa8  beeeec9a  vcvt.s32.f32 s18, s18, #7
0802aaac  1fee105a  vmov         r5, s30
0802aab0  b542      cmp          r5, r6
0802aab2  a8bf      it           ge
0802aab4  3546      movge        r5, r6
0802aab6  87ed009a  vstr         s18, [r7]
0802aaba  002c      cmp          r4, #0
0802aabc  40f02b81  bne.w        #598  ; -> 0x0802ad16
0802aac0  25f00101  bic          r1, r5, #1
0802aac4  774f      ldr          r7, [pc, #476]  ; [0x0802aca4] = 0x200220e8 (f32=1.10223161e-19)
0802aac6  2097      str          r7, [sp, #128]
0802aac8  0024      movs         r4, #0
0802aaca  c729      cmp          r1, #199
0802aacc  3c60      str          r4, [r7]
0802aace  40f31181  ble.w        #546  ; -> 0x0802acf4
0802aad2  754c      ldr          r4, [pc, #468]  ; [0x0802aca8] = 0x080462b0 (f32=3.98383117e-34)
0802aad4  a1f1c805  sub.w        r5, r1, #200
0802aad8  04eb850b  add.w        r11, r4, r5, lsl #2
0802aadc  dbf80060  ldr.w        r6, [r11]
0802aae0  3e60      str          r6, [r7]
0802aae2  fcf7f3be  b.w          #-12826  ; -> 0x080278cc
0802aae6  5d4b      ldr          r3, [pc, #372]  ; [0x0802ac5c] = 0x20021cb8 (f32=1.10209306e-19)
0802aae8  7048      ldr          r0, [pc, #448]  ; [0x0802acac] = 0x20021e64 (f32=1.10214837e-19)
0802aaea  1d68      ldr          r5, [r3]
0802aaec  704e      ldr          r6, [pc, #448]  ; [0x0802acb0] = 0x20021fd8 (f32=1.10219645e-19)
0802aaee  7149      ldr          r1, [pc, #452]  ; [0x0802acb4] = 0x20021ff8 (f32=1.10220059e-19)
0802aaf0  56f82520  ldr.w        r2, [r6, r5, lsl #2]
0802aaf4  51f82570  ldr.w        r7, [r1, r5, lsl #2]
0802aaf8  d0ed007a  vldr         s15, [r0]
0802aafc  90ed017a  vldr         s14, [r0, #4]
0802ab00  6d4d      ldr          r5, [pc, #436]  ; [0x0802acb8] = 0x20021f90 (f32=1.10218715e-19)
0802ab02  d0ed02ea  vldr         s29, [r0, #8]
0802ab06  1495      str          r5, [sp, #80]
0802ab08  90ed03fa  vldr         s30, [r0, #12]
0802ab0c  def80030  ldr.w        r3, [lr]
0802ab10  77ee87fa  vadd.f32     s31, s15, s14
0802ab14  d41b      subs         r4, r2, r7
0802ab16  fdf7c3bf  b.w          #-8314  ; -> 0x08028aa0
0802ab1a  6748      ldr          r0, [pc, #412]  ; [0x0802acb8] = 0x20021f90 (f32=1.10218715e-19)
0802ab1c  1490      str          r0, [sp, #80]
0802ab1e  90ed007a  vldr         s14, [r0]
0802ab22  fcf71cbf  b.w          #-12744  ; -> 0x0802795e
0802ab26  b4ee4d7a  vcmp.f32     s14, s26
0802ab2a  f6ee000a  vmov.f32     s1, #5.000000e-01
0802ab2e  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802ab32  67ee201a  vmul.f32     s3, s14, s1
0802ab36  3df404af  beq.w        #-8696  ; -> 0x08028942
0802ab3a  604e      ldr          r6, [pc, #384]  ; [0x0802acbc] = 0x20021e84 (f32=1.10215251e-19)
0802ab3c  84ed007a  vstr         s14, [r4]
0802ab40  0024      movs         r4, #0
0802ab42  3460      str          r4, [r6]
0802ab44  fdf7fdbe  b.w          #-8710  ; -> 0x08028942
0802ab48  f0ee687a  vmov.f32     s15, s17
0802ab4c  fcf7c1bf  b.w          #-12414  ; -> 0x08027ad2
0802ab50  30eec7ba  vsub.f32     s22, s1, s14
0802ab54  249c      ldr          r4, [sp, #144]
0802ab56  84ed00ba  vstr         s22, [r4]
0802ab5a  fdf76bba  b.w          #-11050  ; -> 0x08028034
0802ab5e  78eec71a  vsub.f32     s3, s17, s14
0802ab62  249b      ldr          r3, [sp, #144]
0802ab64  c3ed001a  vstr         s3, [r3]
0802ab68  fef740be  b.w          #-4992  ; -> 0x080297ec
0802ab6c  b2ee003a  vmov.f32     s6, #8.000000e+00
0802ab70  b4eec37a  vcmpe.f32    s14, s6
0802ab74  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802ab78  7cf7f1ae  ble.w        #-12830  ; -> 0x0802795e
0802ab7c  dfed503a  vldr         s7, [pc, #320]  ; [0x0802acc0] = 0x3f2aaa9f (f32=0.666665971)
0802ab80  20ee236a  vmul.f32     s12, s0, s7
0802ab84  b4eec67a  vcmpe.f32    s14, s12
0802ab88  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802ab8c  58bf      it           pl
0802ab8e  b0ee406a  vmovpl.f32   s12, s0
0802ab92  40f19d80  bpl.w        #314  ; -> 0x0802acd0
0802ab96  b0ee634a  vmov.f32     s8, s7
0802ab9a  f6ee085a  vmov.f32     s11, #7.500000e-01
0802ab9e  f4ee653a  vcmp.f32     s7, s11
0802aba2  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802aba6  66ee044a  vmul.f32     s9, s12, s8
0802abaa  26ee255a  vmul.f32     s10, s12, s11
0802abae  0ad1      bne          #20  ; -> 0x0802abc6
0802abb0  b4eee47a  vcmpe.f32    s14, s9
0802abb4  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802abb8  40f18a80  bpl.w        #276  ; -> 0x0802acd0
0802abbc  b0ee646a  vmov.f32     s12, s9
0802abc0  f0ee443a  vmov.f32     s7, s8
0802abc4  ebe7      b            #-42  ; -> 0x0802ab9e
0802abc6  b4eec57a  vcmpe.f32    s14, s10
0802abca  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802abce  7fd5      bpl          #254  ; -> 0x0802acd0
0802abd0  b0ee456a  vmov.f32     s12, s10
0802abd4  f6ee083a  vmov.f32     s7, #7.500000e-01
0802abd8  e1e7      b            #-62  ; -> 0x0802ab9e
0802abda  4f9d      ldr          r5, [sp, #316]
0802abdc  2960      str          r1, [r5]
0802abde  1299      ldr          r1, [sp, #72]
0802abe0  012c      cmp          r4, #1
0802abe2  81ed039a  vstr         s18, [r1, #12]
0802abe6  7df4f3ad  bne.w        #-9242  ; -> 0x080287d0
0802abea  1a49      ldr          r1, [pc, #104]  ; [0x0802ac54] = 0x200012a8 (f32=1.08481946e-19)
0802abec  354b      ldr          r3, [pc, #212]  ; [0x0802acc4] = 0x2002214c (f32=1.10224453e-19)
0802abee  0f68      ldr          r7, [r1]
0802abf0  0025      movs         r5, #0
0802abf2  012f      cmp          r7, #1
0802abf4  1d60      str          r5, [r3]
0802abf6  7df613af  bls.w        #-8666  ; -> 0x08028a20
0802abfa  9fed338a  vldr         s16, [pc, #204]  ; [0x0802acc8] = 0x43a40000 (f32=328)
0802abfe  334c      ldr          r4, [pc, #204]  ; [0x0802accc] = 0x08045e10 (f32=3.9832875e-34)
0802ac00  69ee088a  vmul.f32     s17, s18, s16
0802ac04  40f22710  movw         r0, #295
0802ac08  fdeee89a  vcvt.s32.f32 s19, s17
0802ac0c  f7ee00aa  vmov.f32     s21, #1.000000e+00
0802ac10  19ee906a  vmov         r6, s19
0802ac14  a6f11001  sub.w        r1, r6, #16
0802ac18  21eae17b  bic.w        r11, r1, r1, asr #31
0802ac1c  8345      cmp          r11, r0
0802ac1e  a8bf      it           ge
0802ac20  8346      movge        r11, r0
0802ac22  39ee2a9a  vadd.f32     s18, s18, s21
0802ac26  04eb8b03  add.w        r3, r4, r11, lsl #2
0802ac2a  1e48      ldr          r0, [pc, #120]  ; [0x0802aca4] = 0x200220e8 (f32=1.10223161e-19)
0802ac2c  1c4c      ldr          r4, [pc, #112]  ; [0x0802aca0] = 0x200220dc (f32=1.10223006e-19)
0802ac2e  1f68      ldr          r7, [r3]
0802ac30  0760      str          r7, [r0]
0802ac32  beeecd9a  vcvt.s32.f32 s18, s18, #6
0802ac36  84ed009a  vstr         s18, [r4]
0802ac3a  fdf716bf  b.w          #-8660  ; -> 0x08028a6a
0802acd0  149e      ldr          r6, [sp, #80]
0802acd2  b0ee467a  vmov.f32     s14, s12
0802acd6  86ed006a  vstr         s12, [r6]
0802acda  fcf740be  b.w          #-13184  ; -> 0x0802795e
0802acde  f7ee008a  vmov.f32     s17, #1.000000e+00
0802ace2  b4eee89a  vcmpe.f32    s18, s17
0802ace6  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802acea  88bf      it           hi
0802acec  b0ee689a  vmovhi.f32   s18, s17
0802acf0  fcf7c4bd  b.w          #-13432  ; -> 0x0802787c
0802acf4  c429      cmp          r1, #196
0802acf6  3cf7e9ad  bgt.w        #-13358  ; -> 0x080278cc
0802acfa  484d      ldr          r5, [pc, #288]  ; [0x0802ae1c] = 0x080462b0 (f32=3.98383117e-34)
0802acfc  209f      ldr          r7, [sp, #128]
0802acfe  c1f1c406  rsb.w        r6, r1, #196
0802ad02  05eb8601  add.w        r1, r5, r6, lsl #2
0802ad06  d1ed00fa  vldr         s31, [r1]
0802ad0a  b1ee6f7a  vneg.f32     s14, s31
0802ad0e  87ed007a  vstr         s14, [r7]
0802ad12  fcf7dbbd  b.w          #-13386  ; -> 0x080278cc
0802ad16  25f00301  bic          r1, r5, #3
0802ad1a  d3e6      b            #-602  ; -> 0x0802aac4
0802ad1c  f0ee687a  vmov.f32     s15, s17
0802ad20  fcf7cdbe  b.w          #-12902  ; -> 0x08027abe
0802ad24  002d      cmp          r5, #0
0802ad26  3ff43eae  beq.w        #-900  ; -> 0x0802a9a6
0802ad2a  3d4f      ldr          r7, [pc, #244]  ; [0x0802ae20] = 0x2002215c (f32=1.1022466e-19)
0802ad2c  3d4e      ldr          r6, [pc, #244]  ; [0x0802ae24] = 0x20021ca4 (f32=1.10209047e-19)
0802ad2e  3e48      ldr          r0, [pc, #248]  ; [0x0802ae28] = 0x20022114 (f32=1.10223729e-19)
0802ad30  3e4b      ldr          r3, [pc, #248]  ; [0x0802ae2c] = 0x2002210c (f32=1.10223626e-19)
0802ad32  57f82250  ldr.w        r5, [r7, r2, lsl #2]
0802ad36  3768      ldr          r7, [r6]
0802ad38  2c90      str          r0, [sp, #176]
0802ad3a  0132      adds         r2, #1
0802ad3c  1d60      str          r5, [r3]
0802ad3e  0260      str          r2, [r0]
0802ad40  bd42      cmp          r5, r7
0802ad42  bff646ae  bge.w        #-884  ; -> 0x0802a9d2
0802ad46  2c9c      ldr          r4, [sp, #176]
0802ad48  2268      ldr          r2, [r4]
0802ad4a  b2f5967f  cmp.w        r2, #300
0802ad4e  3ff740ae  bgt.w        #-896  ; -> 0x0802a9d2
0802ad52  4be6      b            #-874  ; -> 0x0802a9ec
0802ad54  364c      ldr          r4, [pc, #216]  ; [0x0802ae30] = 0x20022074 (f32=1.10221662e-19)
0802ad56  2668      ldr          r6, [r4]
0802ad58  46f48057  orr          r7, r6, #4096
0802ad5c  2760      str          r7, [r4]
0802ad5e  fdf796bd  b.w          #-9428  ; -> 0x0802888e
0802ad62  21f00303  bic          r3, r1, #3
0802ad66  fdf772be  b.w          #-8988  ; -> 0x08028a4e
0802ad6a  c42b      cmp          r3, #196
0802ad6c  3df77dae  bgt.w        #-8966  ; -> 0x08028a6a
0802ad70  2a4d      ldr          r5, [pc, #168]  ; [0x0802ae1c] = 0x080462b0 (f32=3.98383117e-34)
0802ad72  2099      ldr          r1, [sp, #128]
0802ad74  c3f1c404  rsb.w        r4, r3, #196
0802ad78  05eb8406  add.w        r6, r5, r4, lsl #2
0802ad7c  d6ed00ca  vldr         s25, [r6]
0802ad80  b1ee6cda  vneg.f32     s26, s25
0802ad84  81ed00da  vstr         s26, [r1]
0802ad88  fdf76fbe  b.w          #-8994  ; -> 0x08028a6a
0802ad8c  294a      ldr          r2, [pc, #164]  ; [0x0802ae34] = 0x20021de4 (f32=1.10213183e-19)
0802ad8e  1068      ldr          r0, [r2]
0802ad90  18b1      cbz          r0, #6  ; -> 0x0802ad9a
0802ad92  2949      ldr          r1, [pc, #164]  ; [0x0802ae38] = 0x20021e14 (f32=1.10213803e-19)
0802ad94  0c68      ldr          r4, [r1]
0802ad96  012c      cmp          r4, #1
0802ad98  0ad0      beq          #20  ; -> 0x0802adb0
0802ad9a  451e      subs         r5, r0, #1
0802ad9c  204b      ldr          r3, [pc, #128]  ; [0x0802ae20] = 0x2002215c (f32=1.1022466e-19)
0802ad9e  234e      ldr          r6, [pc, #140]  ; [0x0802ae2c] = 0x2002210c (f32=1.10223626e-19)
0802ada0  53f82550  ldr.w        r5, [r3, r5, lsl #2]
0802ada4  3560      str          r5, [r6]
0802ada6  cbe7      b            #-106  ; -> 0x0802ad40
0802ada8  dfed24fa  vldr         s31, [pc, #144]  ; [0x0802ae3c] = 0x3db4c251 (f32=0.0882612541)
0802adac  fcf78cbe  b.w          #-13032  ; -> 0x08027ac8
0802adb0  f7ee009a  vmov.f32     s19, #1.000000e+00
0802adb4  b4ee695a  vcmp.f32     s10, s19
0802adb8  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802adbc  edd1      bne          #-38  ; -> 0x0802ad9a
0802adbe  0c9a      ldr          r2, [sp, #48]
0802adc0  1f49      ldr          r1, [pc, #124]  ; [0x0802ae40] = 0x20021f28 (f32=1.10217371e-19)
0802adc2  204c      ldr          r4, [pc, #128]  ; [0x0802ae44] = 0x20021ca0 (f32=1.10208995e-19)
0802adc4  51f82530  ldr.w        r3, [r1, r5, lsl #2]
0802adc8  154e      ldr          r6, [pc, #84]  ; [0x0802ae20] = 0x2002215c (f32=1.1022466e-19)
0802adca  2568      ldr          r5, [r4]
0802adcc  56f82010  ldr.w        r1, [r6, r0, lsl #2]
0802add0  164c      ldr          r4, [pc, #88]  ; [0x0802ae2c] = 0x2002210c (f32=1.10223626e-19)
0802add2  02eb800b  add.w        r11, r2, r0, lsl #2
0802add6  dbed001a  vldr         s3, [r11]
0802adda  61eea7aa  vmul.f32     s21, s3, s15
0802adde  bdeeea9a  vcvt.s32.f32 s18, s21
0802ade2  19ee102a  vmov         r2, s18
0802ade6  1a44      add          r2, r3
0802ade8  2a40      ands         r2, r5
0802adea  8a42      cmp          r2, r1
0802adec  2260      str          r2, [r4]
0802adee  09dd      ble          #18  ; -> 0x0802ae04
0802adf0  0bee102a  vmov         s22, r2
0802adf4  f8eecbba  vcvt.f32.s32 s23, s22
0802adf8  3beee1ca  vsub.f32     s24, s23, s3
0802adfc  bdeecc8a  vcvt.s32.f32 s16, s24
0802ae00  84ed008a  vstr         s16, [r4]
0802ae04  0138      subs         r0, #1
0802ae06  2568      ldr          r5, [r4]
0802ae08  56f82030  ldr.w        r3, [r6, r0, lsl #2]
0802ae0c  ee1a      subs         r6, r5, r3
0802ae0e  b6f5fa6f  cmp.w        r6, #2000
0802ae12  95da      bge          #-214  ; -> 0x0802ad40
0802ae14  2360      str          r3, [r4]
0802ae16  1d46      mov          r5, r3
0802ae18  92e7      b            #-220  ; -> 0x0802ad40
0802b208  2de9f043  push.w       {r4, r5, r6, r7, r8, r9, lr}
0802b20c  d0f828c0  ldr.w        r12, [r0, #40]
0802b210  4368      ldr          r3, [r0, #4]
0802b212  dcf80820  ldr.w        r2, [r12, #8]
0802b216  d0f81ce0  ldr.w        lr, [r0, #28]
0802b21a  dcf80410  ldr.w        r1, [r12, #4]
0802b21e  d0f82080  ldr.w        r8, [r0, #32]
0802b222  d0f80090  ldr.w        r9, [r0]
0802b226  83b0      sub          sp, #12
0802b228  d0e90376  ldrd         r7, r6, [r0, #12]
0802b22c  0192      str          r2, [sp, #4]
0802b22e  8268      ldr          r2, [r0, #8]
0802b230  d0e90554  ldrd         r5, r4, [r0, #20]
0802b234  1343      orrs         r3, r2
0802b236  3b43      orrs         r3, r7
0802b238  3343      orrs         r3, r6
0802b23a  2b43      orrs         r3, r5
0802b23c  019a      ldr          r2, [sp, #4]
0802b23e  476a      ldr          r7, [r0, #36]
0802b240  dcf80060  ldr.w        r6, [r12]
0802b244  dcf81400  ldr.w        r0, [r12, #20]
0802b248  2343      orrs         r3, r4
0802b24a  dce90345  ldrd         r4, r5, [r12, #12]
0802b24e  013a      subs         r2, #1
0802b250  0139      subs         r1, #1
0802b252  43ea0e03  orr.w        r3, r3, lr
0802b256  1202      lsls         r2, r2, #8
0802b258  42ea0112  orr.w        r2, r2, r1, lsl #4
0802b25c  013d      subs         r5, #1
0802b25e  013c      subs         r4, #1
0802b260  43ea0803  orr.w        r3, r3, r8
0802b264  013e      subs         r6, #1
0802b266  0138      subs         r0, #1
0802b268  2904      lsls         r1, r5, #16
0802b26a  3b43      orrs         r3, r7
0802b26c  2503      lsls         r5, r4, #12
0802b26e  1643      orrs         r6, r2
0802b270  0405      lsls         r4, r0, #20
0802b272  b9f1000f  cmp.w        r9, #0
0802b276  0ed1      bne          #28  ; -> 0x0802b296
0802b278  114a      ldr          r2, [pc, #68]  ; [0x0802b2c0] = 0xa0000140 (f32=-1.08424353e-19)
0802b27a  1360      str          r3, [r2]
0802b27c  dcf81830  ldr.w        r3, [r12, #24]
0802b280  0d43      orrs         r5, r1
0802b282  2543      orrs         r5, r4
0802b284  03f1ff38  add.w        r8, r3, #4294967295
0802b288  45ea0860  orr.w        r0, r5, r8, lsl #24
0802b28c  0643      orrs         r6, r0
0802b28e  9660      str          r6, [r2, #8]
0802b290  03b0      add          sp, #12
0802b292  bde8f083  pop.w        {r4, r5, r6, r7, r8, r9, pc}
0802b296  4fea890c  lsl.w        r12, r9, #2
0802b29a  0cf12049  add.w        r9, r12, #2684354560
0802b29e  084a      ldr          r2, [pc, #32]  ; [0x0802b2c0] = 0xa0000140 (f32=-1.08424353e-19)
0802b2a0  4eea080e  orr.w        lr, lr, r8
0802b2a4  4eea0707  orr.w        r7, lr, r7
0802b2a8  2543      orrs         r5, r4
0802b2aa  3143      orrs         r1, r6
0802b2ac  1760      str          r7, [r2]
0802b2ae  c9f84031  str.w        r3, [r9, #320]
0802b2b2  9560      str          r5, [r2, #8]
0802b2b4  c9f84811  str.w        r1, [r9, #328]
0802b2b8  03b0      add          sp, #12
0802b2ba  bde8f083  pop.w        {r4, r5, r6, r7, r8, r9, pc}
0802b2fc  30b4      push         {r4, r5}
0802b2fe  d0e90035  ldrd         r3, r5, [r0]
0802b302  d0e90224  ldrd         r2, r4, [r0, #8]
0802b306  2b43      orrs         r3, r5
0802b308  43ea4420  orr.w        r0, r3, r4, lsl #9
0802b30c  0349      ldr          r1, [pc, #12]  ; [0x0802b31c] = 0xa0000140 (f32=-1.08424353e-19)
0802b30e  531e      subs         r3, r2, #1
0802b310  40ea4315  orr.w        r5, r0, r3, lsl #5
0802b314  0d61      str          r5, [r1, #16]
0802b316  30bc      pop          {r4, r5}
0802b318  7047      bx           lr
0802b33c  024a      ldr          r2, [pc, #8]  ; [0x0802b348] = 0xa0000140 (f32=-1.08424353e-19)
0802b33e  5369      ldr          r3, [r2, #20]
0802b340  43ea4000  orr.w        r0, r3, r0, lsl #1
0802b344  5061      str          r0, [r2, #20]
0802b346  7047      bx           lr
0802b35c  8000      lsls         r0, r0, #2
0802b35e  00f1204c  add.w        r12, r0, #2684354560
0802b362  31b1      cbz          r1, #12  ; -> 0x0802b372
0802b364  dcf84011  ldr.w        r1, [r12, #320]
0802b368  41f40070  orr          r0, r1, #512
0802b36c  ccf84001  str.w        r0, [r12, #320]
0802b370  7047      bx           lr
0802b372  dcf84031  ldr.w        r3, [r12, #320]
0802b376  47f6ff52  movw         r2, #32255
0802b37a  1340      ands         r3, r2
0802b37c  ccf84031  str.w        r3, [r12, #320]
0802b380  7047      bx           lr
0802b40c  1028      cmp          r0, #16
0802b40e  12d0      beq          #36  ; -> 0x0802b436
0802b410  b0f5807f  cmp.w        r0, #256
0802b414  0cd0      beq          #24  ; -> 0x0802b430
0802b416  b0f5805f  cmp.w        r0, #4096
0802b41a  0bbf      itete        eq
0802b41c  074b      ldreq        r3, [pc, #28]  ; [0x0802b43c] = 0xa00000a0 (f32=-1.08422285e-19)
0802b41e  084b      ldrne        r3, [pc, #32]  ; [0x0802b440] = 0xa0000140 (f32=-1.08424353e-19)
0802b420  5b68      ldreq        r3, [r3, #4]
0802b422  9b69      ldrne        r3, [r3, #24]
0802b424  31ea0303  bics.w       r3, r1, r3
0802b428  0cbf      ite          eq
0802b42a  0120      moveq        r0, #1
0802b42c  0020      movne        r0, #0
0802b42e  7047      bx           lr
0802b430  044b      ldr          r3, [pc, #16]  ; [0x0802b444] = 0xa0000080 (f32=-1.08421872e-19)
0802b432  5b68      ldr          r3, [r3, #4]
0802b434  f6e7      b            #-20  ; -> 0x0802b424
0802b436  044a      ldr          r2, [pc, #16]  ; [0x0802b448] = 0xa0000060 (f32=-1.08421458e-19)
0802b438  5368      ldr          r3, [r2, #4]
0802b43a  f3e7      b            #-26  ; -> 0x0802b424
