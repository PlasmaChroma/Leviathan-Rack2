080239b4  2de9f04f  push.w       {r4, r5, r6, r7, r8, r9, r10, r11, lr}
080239b8  b14b      ldr          r3, [pc, #708]  ; [0x08023c80] = 0xe0001000 (f32=-3.69115025e+19)
080239ba  0026      movs         r6, #0
080239bc  b14c      ldr          r4, [pc, #708]  ; [0x08023c84] = 0x20005878 (f32=1.08712936e-19)
080239be  0125      movs         r5, #1
080239c0  2ded108b  vpush        {d8, d9, d10, d11, d12, d13, d14, d15}
080239c4  a3b0      sub          sp, #140
080239c6  1290      str          r0, [sp, #72]
080239c8  5e60      str          r6, [r3, #4]
080239ca  2368      ldr          r3, [r4]
080239cc  1192      str          r2, [sp, #68]
080239ce  2b44      add          r3, r5
080239d0  ad4a      ldr          r2, [pc, #692]  ; [0x08023c88] = 0x20004de0 (f32=1.08677884e-19)
080239d2  1391      str          r1, [sp, #76]
080239d4  9268      ldr          r2, [r2, #8]
080239d6  2360      str          r3, [r4]
080239d8  ac4b      ldr          r3, [pc, #688]  ; [0x08023c8c] = 0x20004e80 (f32=1.08679952e-19)
080239da  2a44      add          r2, r5
080239dc  aa49      ldr          r1, [pc, #680]  ; [0x08023c88] = 0x20004de0 (f32=1.08677884e-19)
080239de  1b68      ldr          r3, [r3]
080239e0  8a60      str          r2, [r1, #8]
080239e2  b342      cmp          r3, r6
080239e4  aa4a      ldr          r2, [pc, #680]  ; [0x08023c90] = 0x20002ab8 (f32=1.08561562e-19)
080239e6  1560      str          r5, [r2]
080239e8  02dd      ble          #4  ; -> 0x080239f0
080239ea  013b      subs         r3, #1
080239ec  a74a      ldr          r2, [pc, #668]  ; [0x08023c8c] = 0x20004e80 (f32=1.08679952e-19)
080239ee  1360      str          r3, [r2]
080239f0  a84b      ldr          r3, [pc, #672]  ; [0x08023c94] = 0x200055d4 (f32=1.08704199e-19)
080239f2  1b68      ldr          r3, [r3]
080239f4  002b      cmp          r3, #0
080239f6  02dd      ble          #4  ; -> 0x080239fe
080239f8  013b      subs         r3, #1
080239fa  a64a      ldr          r2, [pc, #664]  ; [0x08023c94] = 0x200055d4 (f32=1.08704199e-19)
080239fc  1360      str          r3, [r2]
080239fe  a64a      ldr          r2, [pc, #664]  ; [0x08023c98] = 0x200055dc (f32=1.08704302e-19)
08023a00  1368      ldr          r3, [r2]
08023a02  002b      cmp          r3, #0
08023a04  01dd      ble          #2  ; -> 0x08023a0a
08023a06  013b      subs         r3, #1
08023a08  1360      str          r3, [r2]
08023a0a  a44a      ldr          r2, [pc, #656]  ; [0x08023c9c] = 0x2000565c (f32=1.08705956e-19)
08023a0c  1368      ldr          r3, [r2]
08023a0e  002b      cmp          r3, #0
08023a10  01dd      ble          #2  ; -> 0x08023a16
08023a12  013b      subs         r3, #1
08023a14  1360      str          r3, [r2]
08023a16  a24a      ldr          r2, [pc, #648]  ; [0x08023ca0] = 0x20005874 (f32=1.08712884e-19)
08023a18  1368      ldr          r3, [r2]
08023a1a  002b      cmp          r3, #0
08023a1c  01dd      ble          #2  ; -> 0x08023a22
08023a1e  013b      subs         r3, #1
08023a20  1360      str          r3, [r2]
08023a22  a04b      ldr          r3, [pc, #640]  ; [0x08023ca4] = 0x20004f98 (f32=1.08683571e-19)
08023a24  f2ee006a  vmov.f32     s13, #8.000000e+00
08023a28  9f4a      ldr          r2, [pc, #636]  ; [0x08023ca8] = 0x20004c48 (f32=1.08672611e-19)
08023a2a  93ed067a  vldr         s14, [r3, #24]
08023a2e  d2ed007a  vldr         s15, [r2]
08023a32  baeece7a  vcvt.f32.s32 s14, s14, #4
08023a36  9d4e      ldr          r6, [pc, #628]  ; [0x08023cac] = 0x20001b3c (f32=1.08510328e-19)
08023a38  5d69      ldr          r5, [r3, #20]
08023a3a  dff800c3  ldr.w        r12, [pc, #768]  ; [0x08023d3c] = 0x20002ad0 (f32=1.08561872e-19)
08023a3e  77ee677a  vsub.f32     s15, s14, s15
08023a42  dcf80000  ldr.w        r0, [r12]
08023a46  f0eee77a  vabs.f32     s15, s15
08023a4a  f4eee67a  vcmpe.f32    s15, s13
08023a4e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023a52  c8bf      it           gt
08023a54  82ed007a  vstrgt       s14, [r2]
08023a58  0428      cmp          r0, #4
08023a5a  3268      ldr          r2, [r6]
08023a5c  a5eb0201  sub.w        r1, r5, r2
08023a60  81eae172  eor.w        r2, r1, r1, asr #31
08023a64  a2ebe172  sub.w        r2, r2, r1, asr #31
08023a68  41f34184  ble.w        #6274  ; -> 0x080252ee
08023a6c  011f      subs         r1, r0, #4
08023a6e  01eb8101  add.w        r1, r1, r1, lsl #2
08023a72  4fea410e  lsl.w        lr, r1, #1
08023a76  0ef1080e  add.w        lr, lr, #8
08023a7a  8d4f      ldr          r7, [pc, #564]  ; [0x08023cb0] = 0x20004d70 (f32=1.08676436e-19)
08023a7c  9645      cmp          lr, r2
08023a7e  3968      ldr          r1, [r7]
08023a80  01f1ff31  add.w        r1, r1, #4294967295
08023a84  3960      str          r1, [r7]
08023a86  02da      bge          #4  ; -> 0x08023a8e
08023a88  0029      cmp          r1, #0
08023a8a  c2f28383  blt.w        #9990  ; -> 0x08026194
08023a8e  894a      ldr          r2, [pc, #548]  ; [0x08023cb4] = 0x20002b54 (f32=1.08563578e-19)
08023a90  1046      mov          r0, r2
08023a92  1b92      str          r2, [sp, #108]
08023a94  884a      ldr          r2, [pc, #544]  ; [0x08023cb8] = 0x20004dd0 (f32=1.08677677e-19)
08023a96  0168      ldr          r1, [r0]
08023a98  0139      subs         r1, #1
08023a9a  0160      str          r1, [r0]
08023a9c  8749      ldr          r1, [pc, #540]  ; [0x08023cbc] = 0x200037f4 (f32=1.08605351e-19)
08023a9e  0868      ldr          r0, [r1]
08023aa0  0028      cmp          r0, #0
08023aa2  41f00f84  bne.w        #6174  ; -> 0x080252c4
08023aa6  864e      ldr          r6, [pc, #536]  ; [0x08023cc0] = 0x20002b5c (f32=1.08563681e-19)
08023aa8  3168      ldr          r1, [r6]
08023aaa  0129      cmp          r1, #1
08023aac  02f0ae83  beq.w        #10076  ; -> 0x0802620c
08023ab0  1068      ldr          r0, [r2]
08023ab2  814e      ldr          r6, [pc, #516]  ; [0x08023cb8] = 0x20004dd0 (f32=1.08677677e-19)
08023ab4  0128      cmp          r0, #1
08023ab6  01f01e84  beq.w        #6204  ; -> 0x080252f6
08023aba  0028      cmp          r0, #0
08023abc  41f02084  bne.w        #6208  ; -> 0x08025300
08023ac0  804d      ldr          r5, [pc, #512]  ; [0x08023cc4] = 0x20001b54 (f32=1.08510638e-19)
08023ac2  8149      ldr          r1, [pc, #516]  ; [0x08023cc8] = 0x200055c8 (f32=1.08704044e-19)
08023ac4  2868      ldr          r0, [r5]
08023ac6  0968      ldr          r1, [r1]
08023ac8  0128      cmp          r0, #1
08023aca  02f04e84  beq.w        #10396  ; -> 0x0802636a
08023ace  0029      cmp          r1, #0
08023ad0  41f31b84  ble.w        #6198  ; -> 0x0802530a
08023ad4  7d48      ldr          r0, [pc, #500]  ; [0x08023ccc] = 0x20000274 (f32=1.08428334e-19)
08023ad6  42f6e065  movw         r5, #12000
08023ada  0168      ldr          r1, [r0]
08023adc  0131      adds         r1, #1
08023ade  a942      cmp          r1, r5
08023ae0  0160      str          r1, [r0]
08023ae2  09dd      ble          #18  ; -> 0x08023af8
08023ae4  7749      ldr          r1, [pc, #476]  ; [0x08023cc4] = 0x20001b54 (f32=1.08510638e-19)
08023ae6  7a4d      ldr          r5, [pc, #488]  ; [0x08023cd0] = 0x20002b50 (f32=1.08563526e-19)
08023ae8  0846      mov          r0, r1
08023aea  0121      movs         r1, #1
08023aec  0068      ldr          r0, [r0]
08023aee  2960      str          r1, [r5]
08023af0  0030      adds         r0, #0
08023af2  18bf      it           ne
08023af4  0120      movne        r0, #1
08023af6  1060      str          r0, [r2]
08023af8  764a      ldr          r2, [pc, #472]  ; [0x08023cd4] = 0x40021800 (f32=2.03271484)
08023afa  7749      ldr          r1, [pc, #476]  ; [0x08023cd8] = 0x20004c5c (f32=1.08672869e-19)
08023afc  1269      ldr          r2, [r2, #16]
08023afe  12f4806f  tst.w        r2, #1024
08023b02  0a68      ldr          r2, [r1]
08023b04  02d0      beq          #4  ; -> 0x08023b0c
08023b06  052a      cmp          r2, #5
08023b08  02f04782  beq.w        #9358  ; -> 0x08025f9a
08023b0c  0132      adds         r2, #1
08023b0e  7348      ldr          r0, [pc, #460]  ; [0x08023cdc] = 0x200056c4 (f32=1.08707301e-19)
08023b10  0025      movs         r5, #0
08023b12  052a      cmp          r2, #5
08023b14  0a60      str          r2, [r1]
08023b16  0560      str          r5, [r0]
08023b18  6f49      ldr          r1, [pc, #444]  ; [0x08023cd8] = 0x20004c5c (f32=1.08672869e-19)
08023b1a  01dd      ble          #2  ; -> 0x08023b20
08023b1c  0522      movs         r2, #5
08023b1e  0a60      str          r2, [r1]
08023b20  6f4d      ldr          r5, [pc, #444]  ; [0x08023ce0] = 0x20002b48 (f32=1.08563423e-19)
08023b22  4bf68031  movw         r1, #48000
08023b26  2268      ldr          r2, [r4]
08023b28  0020      movs         r0, #0
08023b2a  2c68      ldr          r4, [r5]
08023b2c  121b      subs         r2, r2, r4
08023b2e  6d4c      ldr          r4, [pc, #436]  ; [0x08023ce4] = 0x20003864 (f32=1.08606798e-19)
08023b30  8a42      cmp          r2, r1
08023b32  2060      str          r0, [r4]
08023b34  6c4a      ldr          r2, [pc, #432]  ; [0x08023ce8] = 0x20004dd4 (f32=1.08677729e-19)
08023b36  42f33f82  ble.w        #9342  ; -> 0x08025fb8
08023b3a  1046      mov          r0, r2
08023b3c  1492      str          r2, [sp, #80]
08023b3e  0121      movs         r1, #1
08023b40  6a4a      ldr          r2, [pc, #424]  ; [0x08023cec] = 0x20003898 (f32=1.0860747e-19)
08023b42  0668      ldr          r6, [r0]
08023b44  1b98      ldr          r0, [sp, #108]
08023b46  1160      str          r1, [r2]
08023b48  d0f80080  ldr.w        r8, [r0]
08023b4c  684a      ldr          r2, [pc, #416]  ; [0x08023cf0] = 0x20004f94 (f32=1.08683519e-19)
08023b4e  0592      str          r2, [sp, #20]
08023b50  684c      ldr          r4, [pc, #416]  ; [0x08023cf4] = 0x20002b28 (f32=1.08563009e-19)
08023b52  5868      ldr          r0, [r3, #4]
08023b54  2268      ldr          r2, [r4]
08023b56  811a      subs         r1, r0, r2
08023b58  01f58071  add.w        r1, r1, #256
08023b5c  b1f5007f  cmp.w        r1, #512
08023b60  04d9      bls          #8  ; -> 0x08023b6c
08023b62  0246      mov          r2, r0
08023b64  6449      ldr          r1, [pc, #400]  ; [0x08023cf8] = 0x2000383c (f32=1.08606281e-19)
08023b66  2060      str          r0, [r4]
08023b68  0020      movs         r0, #0
08023b6a  0860      str          r0, [r1]
08023b6c  07ee902a  vmov         s15, r2
08023b70  ccf80060  str.w        r6, [r12]
08023b74  614a      ldr          r2, [pc, #388]  ; [0x08023cfc] = 0x20002ac0 (f32=1.08561665e-19)
08023b76  faeeeb7a  vcvt.f32.s32 s15, s15, #9
08023b7a  6148      ldr          r0, [pc, #388]  ; [0x08023d00] = 0x20000008 (f32=1.08420321e-19)
08023b7c  92ed007a  vldr         s14, [r2]
08023b80  6049      ldr          r1, [pc, #384]  ; [0x08023d04] = 0x200000e4 (f32=1.08423164e-19)
08023b82  fdeee77a  vcvt.s32.f32 s15, s15
08023b86  17ee90ca  vmov         r12, s15
08023b8a  4fea8c02  lsl.w        r2, r12, #2
08023b8e  1044      add          r0, r2
08023b90  1144      add          r1, r2
08023b92  d0ed007a  vldr         s15, [r0]
08023b96  91ed00ea  vldr         s28, [r1]
08023b9a  f4ee477a  vcmp.f32     s15, s14
08023b9e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023ba2  09d0      beq          #18  ; -> 0x08023bb8
08023ba4  5848      ldr          r0, [pc, #352]  ; [0x08023d08] = 0x20002ae4 (f32=1.0856213e-19)
08023ba6  0168      ldr          r1, [r0]
08023ba8  31b9      cbnz         r1, #12  ; -> 0x08023bb8
08023baa  dff874b1  ldr.w        r11, [pc, #372]  ; [0x08023d20] = 0x20002aa4 (f32=1.08561303e-19)
08023bae  dbf80010  ldr.w        r1, [r11]
08023bb2  0029      cmp          r1, #0
08023bb4  02f07683  beq.w        #9964  ; -> 0x080262a4
08023bb8  b8f1000f  cmp.w        r8, #0
08023bbc  45dd      ble          #138  ; -> 0x08023c4a
08023bbe  5a69      ldr          r2, [r3, #20]
08023bc0  0021      movs         r1, #0
08023bc2  9fed527a  vldr         s14, [pc, #328]  ; [0x08023d0c] = 0x3f888889 (f32=1.06666672)
08023bc6  803a      subs         r2, #128
08023bc8  4b48      ldr          r0, [pc, #300]  ; [0x08023cf8] = 0x2000383c (f32=1.08606281e-19)
08023bca  07ee902a  vmov         s15, r2
08023bce  0160      str          r1, [r0]
08023bd0  f8eee77a  vcvt.f32.s32 s15, s15
08023bd4  67ee877a  vmul.f32     s15, s15, s14
08023bd8  fdeee77a  vcvt.s32.f32 s15, s15
08023bdc  17ee902a  vmov         r2, s15
08023be0  8a42      cmp          r2, r1
08023be2  c2f2d282  blt.w        #9636  ; -> 0x0802618a
08023be6  b2f5805f  cmp.w        r2, #4096
08023bea  c2f2a883  blt.w        #10064  ; -> 0x0802633e
08023bee  dfed487a  vldr         s15, [pc, #288]  ; [0x08023d10] = 0x407fe9d3 (f32=3.9986465)
08023bf2  40f6ff72  movw         r2, #4095
08023bf6  4749      ldr          r1, [pc, #284]  ; [0x08023d14] = 0x20002ac4 (f32=1.08561717e-19)
08023bf8  0968      ldr          r1, [r1]
08023bfa  0229      cmp          r1, #2
08023bfc  02f0c182  beq.w        #9602  ; -> 0x08026182
08023c00  4548      ldr          r0, [pc, #276]  ; [0x08023d18] = 0x20001ac4 (f32=1.08508777e-19)
08023c02  0068      ldr          r0, [r0]
08023c04  0028      cmp          r0, #0
08023c06  02f0eb81  beq.w        #9174  ; -> 0x08025fe0
08023c0a  2e48      ldr          r0, [pc, #184]  ; [0x08023cc4] = 0x20001b54 (f32=1.08510638e-19)
08023c0c  0446      mov          r4, r0
08023c0e  4348      ldr          r0, [pc, #268]  ; [0x08023d1c] = 0x20004ddc (f32=1.08677832e-19)
08023c10  2468      ldr          r4, [r4]
08023c12  c0ed007a  vstr         s15, [r0]
08023c16  012c      cmp          r4, #1
08023c18  02f0fd83  beq.w        #10234  ; -> 0x08026416
08023c1c  0029      cmp          r1, #0
08023c1e  01dc      bgt          #2  ; -> 0x08023c24
08023c20  072e      cmp          r6, #7
08023c22  12d1      bne          #36  ; -> 0x08023c4a
08023c24  3849      ldr          r1, [pc, #224]  ; [0x08023d08] = 0x20002ae4 (f32=1.0856213e-19)
08023c26  0a68      ldr          r2, [r1]
08023c28  22b9      cbnz         r2, #8  ; -> 0x08023c34
08023c2a  3d4c      ldr          r4, [pc, #244]  ; [0x08023d20] = 0x20002aa4 (f32=1.08561303e-19)
08023c2c  2268      ldr          r2, [r4]
08023c2e  002a      cmp          r2, #0
08023c30  02f0db82  beq.w        #9654  ; -> 0x080261ea
08023c34  3b4a      ldr          r2, [pc, #236]  ; [0x08023d24] = 0x20001b64 (f32=1.08510845e-19)
08023c36  3c49      ldr          r1, [pc, #240]  ; [0x08023d28] = 0x20003870 (f32=1.08606953e-19)
08023c38  1268      ldr          r2, [r2]
08023c3a  0968      ldr          r1, [r1]
08023c3c  0260      str          r2, [r0]
08023c3e  3b48      ldr          r0, [pc, #236]  ; [0x08023d2c] = 0x20004c40 (f32=1.08672507e-19)
08023c40  3b4c      ldr          r4, [pc, #236]  ; [0x08023d30] = 0x20001b30 (f32=1.08510173e-19)
08023c42  0260      str          r2, [r0]
08023c44  3b4a      ldr          r2, [pc, #236]  ; [0x08023d34] = 0x20003838 (f32=1.08606229e-19)
08023c46  2160      str          r1, [r4]
08023c48  1160      str          r1, [r2]
08023c4a  9a68      ldr          r2, [r3, #8]
08023c4c  9fed3a7a  vldr         s14, [pc, #232]  ; [0x08023d38] = 0x3f8a3d71 (f32=1.08000004)
08023c50  c83a      subs         r2, #200
08023c52  07ee902a  vmov         s15, r2
08023c56  f8eee77a  vcvt.f32.s32 s15, s15
08023c5a  67ee877a  vmul.f32     s15, s15, s14
08023c5e  fdeee77a  vcvt.s32.f32 s15, s15
08023c62  17ee902a  vmov         r2, s15
08023c66  b2f5805f  cmp.w        r2, #4096
08023c6a  82f28781  bge.w        #8974  ; -> 0x08025f7c
08023c6e  002a      cmp          r2, #0
08023c70  82f20a82  bge.w        #9236  ; -> 0x08026088
08023c74  0021      movs         r1, #0
08023c76  0120      movs         r0, #1
08023c78  dfed325a  vldr         s11, [pc, #200]  ; [0x08023d44] = 0x00000000 (f32=0)
08023c7c  64e0      b            #200  ; -> 0x08023d48
08023d48  1a68      ldr          r2, [r3]
08023d4a  f7ee007a  vmov.f32     s15, #1.000000e+00
08023d4e  1fed045a  vldr         s10, [pc, #-16]  ; [0x08023d40] = 0x398a697b (f32=0.000264000002)
08023d52  c83a      subs         r2, #200
08023d54  5fed054a  vldr         s9, [pc, #-20]  ; [0x08023d44] = 0x00000000 (f32=0)
08023d58  07ee102a  vmov         s14, r2
08023d5c  cd4a      ldr          r2, [pc, #820]  ; [0x08024094] = 0x20003874 (f32=1.08607005e-19)
08023d5e  b8eec77a  vcvt.f32.s32 s14, s14
08023d62  d2ed006a  vldr         s13, [r2]
08023d66  cc4a      ldr          r2, [pc, #816]  ; [0x08024098] = 0x20001b58 (f32=1.0851069e-19)
08023d68  92ed006a  vldr         s12, [r2]
08023d6c  27ee057a  vmul.f32     s14, s14, s10
08023d70  9fedca5a  vldr         s10, [pc, #808]  ; [0x0802409c] = 0x3a83126f (f32=0.00100000005)
08023d74  75eec65a  vsub.f32     s11, s11, s12
08023d78  c7fe677a  vminnm.f32   s15, s14, s15
08023d7c  c7fea47a  vmaxnm.f32   s15, s15, s9
08023d80  37eee67a  vsub.f32     s14, s15, s13
08023d84  a5ee856a  vfma.f32     s12, s11, s10
08023d88  f4ee647a  vcmp.f32     s15, s9
08023d8c  e7ee056a  vfma.f32     s13, s14, s10
08023d90  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023d94  82ed006a  vstr         s12, [r2]
08023d98  be4a      ldr          r2, [pc, #760]  ; [0x08024094] = 0x20003874 (f32=1.08607005e-19)
08023d9a  c2ed006a  vstr         s13, [r2]
08023d9e  05d1      bne          #10  ; -> 0x08023dac
08023da0  20b1      cbz          r0, #8  ; -> 0x08023dac
08023da2  bf48      ldr          r0, [pc, #764]  ; [0x080240a0] = 0x20004dbc (f32=1.08677419e-19)
08023da4  0268      ldr          r2, [r0]
08023da6  022a      cmp          r2, #2
08023da8  02f00583  beq.w        #9738  ; -> 0x080263b6
08023dac  f5ee407a  vcmp.f32     s15, #0
08023db0  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023db4  42f0e880  bne.w        #8656  ; -> 0x08025f88
08023db8  0029      cmp          r1, #0
08023dba  42f0e580  bne.w        #8650  ; -> 0x08025f88
08023dbe  1a69      ldr          r2, [r3, #16]
08023dc0  9fedb87a  vldr         s14, [pc, #736]  ; [0x080240a4] = 0x3f8a3d71 (f32=1.08000004)
08023dc4  c83a      subs         r2, #200
08023dc6  07ee902a  vmov         s15, r2
08023dca  f8eee77a  vcvt.f32.s32 s15, s15
08023dce  67ee877a  vmul.f32     s15, s15, s14
08023dd2  fdeee77a  vcvt.s32.f32 s15, s15
08023dd6  17ee900a  vmov         r0, s15
08023dda  b0f5805f  cmp.w        r0, #4096
08023dde  82f2c180  bge.w        #8578  ; -> 0x08025f64
08023de2  0028      cmp          r0, #0
08023de4  82f2ae81  bge.w        #9052  ; -> 0x08026144
08023de8  dfedaf5a  vldr         s11, [pc, #700]  ; [0x080240a8] = 0x3e3504f4 (f32=0.176776707)
08023dec  af4a      ldr          r2, [pc, #700]  ; [0x080240ac] = 0x20004c4c (f32=1.08672662e-19)
08023dee  9fedb07a  vldr         s14, [pc, #704]  ; [0x080240b0] = 0x00000000 (f32=0)
08023df2  1092      str          r2, [sp, #64]
08023df4  82ed027a  vstr         s14, [r2, #8]
08023df8  db68      ldr          r3, [r3, #12]
08023dfa  ae4f      ldr          r7, [pc, #696]  ; [0x080240b4] = 0x20004d24 (f32=1.08675454e-19)
08023dfc  c83b      subs         r3, #200
08023dfe  9feda94a  vldr         s8, [pc, #676]  ; [0x080240a4] = 0x3f8a3d71 (f32=1.08000004)
08023e02  d7ed006a  vldr         s13, [r7]
08023e06  07ee903a  vmov         s15, r3
08023e0a  ab4b      ldr          r3, [pc, #684]  ; [0x080240b8] = 0x20003888 (f32=1.08607263e-19)
08023e0c  75eee64a  vsub.f32     s9, s11, s13
08023e10  9feda25a  vldr         s10, [pc, #648]  ; [0x0802409c] = 0x3a83126f (f32=0.00100000005)
08023e14  f8eee77a  vcvt.f32.s32 s15, s15
08023e18  93ed006a  vldr         s12, [r3]
08023e1c  109b      ldr          r3, [sp, #64]
08023e1e  37ee467a  vsub.f32     s14, s14, s12
08023e22  a54a      ldr          r2, [pc, #660]  ; [0x080240b8] = 0x20003888 (f32=1.08607263e-19)
08023e24  67ee847a  vmul.f32     s15, s15, s8
08023e28  c3ed015a  vstr         s11, [r3, #4]
08023e2c  e4ee856a  vfma.f32     s13, s9, s10
08023e30  a7ee056a  vfma.f32     s12, s14, s10
08023e34  fdeee77a  vcvt.s32.f32 s15, s15
08023e38  17ee903a  vmov         r3, s15
08023e3c  c7ed006a  vstr         s13, [r7]
08023e40  b3f5805f  cmp.w        r3, #4096
08023e44  82ed006a  vstr         s12, [r2]
08023e48  82f27780  bge.w        #8430  ; -> 0x08025f3a
08023e4c  002b      cmp          r3, #0
08023e4e  82f2f980  bge.w        #8690  ; -> 0x08026044
08023e52  fe22      movs         r2, #254
08023e54  9949      ldr          r1, [pc, #612]  ; [0x080240bc] = 0x3d9720ca (f32=0.0737930089)
08023e56  9fed9ada  vldr         s26, [pc, #616]  ; [0x080240c0] = 0xbf7ff972 (f32=-0.999899983)
08023e5a  059b      ldr          r3, [sp, #20]
08023e5c  dff8e882  ldr.w        r8, [pc, #744]  ; [0x08024148] = 0x20004c74 (f32=1.08673179e-19)
08023e60  1b68      ldr          r3, [r3]
08023e62  1098      ldr          r0, [sp, #64]
08023e64  0493      str          r3, [sp, #16]
08023e66  0160      str          r1, [r0]
08023e68  c8f82c20  str.w        r2, [r8, #44]
08023e6c  c8f80020  str.w        r2, [r8]
08023e70  002b      cmp          r3, #0
08023e72  42f05380  bne.w        #8358  ; -> 0x08025f1c
08023e76  934b      ldr          r3, [pc, #588]  ; [0x080240c4] = 0x200037f4 (f32=1.08605351e-19)
08023e78  9349      ldr          r1, [pc, #588]  ; [0x080240c8] = 0x0802b9c4 (f32=3.93388184e-34)
08023e7a  1b68      ldr          r3, [r3]
08023e7c  01eb8202  add.w        r2, r1, r2, lsl #2
08023e80  012b      cmp          r3, #1
08023e82  d2ed00da  vldr         s27, [r2]
08023e86  02f09082  beq.w        #9504  ; -> 0x080263aa
08023e8a  9fed89ca  vldr         s24, [pc, #548]  ; [0x080240b0] = 0x00000000 (f32=0)
08023e8e  119b      ldr          r3, [sp, #68]
08023e90  002b      cmp          r3, #0
08023e92  41f3d881  ble.w        #5040  ; -> 0x08025246
08023e96  0023      movs         r3, #0
08023e98  b6ee006a  vmov.f32     s12, #5.000000e-01
08023e9c  b0ee4dfa  vmov.f32     s30, s26
08023ea0  dff8a8b2  ldr.w        r11, [pc, #680]  ; [0x0802414c] = 0x20001a14 (f32=1.08506503e-19)
08023ea4  0193      str          r3, [sp, #4]
08023ea6  894b      ldr          r3, [pc, #548]  ; [0x080240cc] = 0x20004d78 (f32=1.0867654e-19)
08023ea8  dff8a492  ldr.w        r9, [pc, #676]  ; [0x08024150] = 0x20004e00 (f32=1.08678298e-19)
08023eac  1693      str          r3, [sp, #88]
08023eae  884b      ldr          r3, [pc, #544]  ; [0x080240d0] = 0x20001b30 (f32=1.08510173e-19)
08023eb0  dfed88ca  vldr         s25, [pc, #544]  ; [0x080240d4] = 0x3c23d70a (f32=0.00999999978)
08023eb4  1a93      str          r3, [sp, #104]
08023eb6  884b      ldr          r3, [pc, #544]  ; [0x080240d8] = 0x20002aa4 (f32=1.08561303e-19)
08023eb8  dfed78aa  vldr         s21, [pc, #480]  ; [0x0802409c] = 0x3a83126f (f32=0.00100000005)
08023ebc  0393      str          r3, [sp, #12]
08023ebe  874b      ldr          r3, [pc, #540]  ; [0x080240dc] = 0x20002ae4 (f32=1.0856213e-19)
08023ec0  dfed871a  vldr         s3, [pc, #540]  ; [0x080240e0] = 0x3f333333 (f32=0.699999988)
08023ec4  0293      str          r3, [sp, #8]
08023ec6  874b      ldr          r3, [pc, #540]  ; [0x080240e4] = 0x20001b54 (f32=1.08510638e-19)
08023ec8  dfed87ea  vldr         s29, [pc, #540]  ; [0x080240e8] = 0x43300000 (f32=176)
08023ecc  0e93      str          r3, [sp, #56]
08023ece  874b      ldr          r3, [pc, #540]  ; [0x080240ec] = 0x20002b50 (f32=1.08563526e-19)
08023ed0  dfed87ba  vldr         s23, [pc, #540]  ; [0x080240f0] = 0x3f926e98 (f32=1.14400005)
08023ed4  1993      str          r3, [sp, #100]
08023ed6  874b      ldr          r3, [pc, #540]  ; [0x080240f4] = 0x20003870 (f32=1.08606953e-19)
08023ed8  dfed87fa  vldr         s31, [pc, #540]  ; [0x080240f8] = 0x3caaaa99 (f32=0.0208333004)
08023edc  0793      str          r3, [sp, #28]
08023ede  874b      ldr          r3, [pc, #540]  ; [0x080240fc] = 0x20001b64 (f32=1.08510845e-19)
08023ee0  9fed87ba  vldr         s22, [pc, #540]  ; [0x08024100] = 0x3ecccccd (f32=0.400000006)
08023ee4  0893      str          r3, [sp, #32]
08023ee6  874b      ldr          r3, [pc, #540]  ; [0x08024104] = 0x20003868 (f32=1.0860685e-19)
08023ee8  0f93      str          r3, [sp, #60]
08023eea  874b      ldr          r3, [pc, #540]  ; [0x08024108] = 0x20002aac (f32=1.08561407e-19)
08023eec  0d93      str          r3, [sp, #52]
08023eee  874b      ldr          r3, [pc, #540]  ; [0x0802410c] = 0x20002b44 (f32=1.08563371e-19)
08023ef0  0093      str          r3, [sp]
08023ef2  874b      ldr          r3, [pc, #540]  ; [0x08024110] = 0x20002ad4 (f32=1.08561924e-19)
08023ef4  1593      str          r3, [sp, #84]
08023ef6  874b      ldr          r3, [pc, #540]  ; [0x08024114] = 0x20001ac4 (f32=1.08508777e-19)
08023ef8  1793      str          r3, [sp, #92]
08023efa  874b      ldr          r3, [pc, #540]  ; [0x08024118] = 0x20004c40 (f32=1.08672507e-19)
08023efc  1893      str          r3, [sp, #96]
08023efe  019b      ldr          r3, [sp, #4]
08023f00  1299      ldr          r1, [sp, #72]
08023f02  4feac30c  lsl.w        r12, r3, #3
08023f06  dfed856a  vldr         s13, [pc, #532]  ; [0x0802411c] = 0x3a03126f (f32=0.000500000024)
08023f0a  0a46      mov          r2, r1
08023f0c  0cf10403  add.w        r3, r12, #4
08023f10  6244      add          r2, r12
08023f12  0993      str          r3, [sp, #36]
08023f14  cb18      adds         r3, r1, r3
08023f16  92ed002a  vldr         s4, [r2]
08023f1a  d3ed003a  vldr         s7, [r3]
08023f1e  b0eec27a  vabs.f32     s14, s4
08023f22  7f4b      ldr          r3, [pc, #508]  ; [0x08024120] = 0x20003884 (f32=1.08607212e-19)
08023f24  f5eec03a  vcmpe.f32    s7, #0
08023f28  d3ed007a  vldr         s15, [r3]
08023f2c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023f30  4cbf      ite          mi
08023f32  37ee637a  vsubmi.f32   s14, s14, s7
08023f36  37ee237a  vaddpl.f32   s14, s14, s7
08023f3a  37ee677a  vsub.f32     s14, s14, s15
08023f3e  e7ee2c7a  vfma.f32     s15, s14, s25
08023f42  f4eee67a  vcmpe.f32    s15, s13
08023f46  c3ed007a  vstr         s15, [r3]
08023f4a  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023f4e  09d5      bpl          #18  ; -> 0x08023f64
08023f50  67eea77a  vmul.f32     s15, s15, s15
08023f54  9fed737a  vldr         s14, [pc, #460]  ; [0x08024124] = 0x4a742400 (f32=4000000)
08023f58  67ee877a  vmul.f32     s15, s15, s14
08023f5c  22ee272a  vmul.f32     s4, s4, s15
08023f60  63eea73a  vmul.f32     s7, s7, s15
08023f64  0799      ldr          r1, [sp, #28]
08023f66  f7ee007a  vmov.f32     s15, #1.000000e+00
08023f6a  0f9a      ldr          r2, [sp, #60]
08023f6c  91ed004a  vldr         s8, [r1]
08023f70  1368      ldr          r3, [r2]
08023f72  b4ee674a  vcmp.f32     s8, s15
08023f76  591e      subs         r1, r3, #1
08023f78  f1ee10fa  vmrs         APSR_nzcv, fpscr
08023f7c  1160      str          r1, [r2]
08023f7e  01f01d82  beq.w        #5178  ; -> 0x080253bc
08023f82  0d9b      ldr          r3, [sp, #52]
08023f84  1a68      ldr          r2, [r3]
08023f86  531e      subs         r3, r2, #1
08023f88  0d9a      ldr          r2, [sp, #52]
08023f8a  0029      cmp          r1, #0
08023f8c  1360      str          r3, [r2]
08023f8e  664a      ldr          r2, [pc, #408]  ; [0x08024128] = 0x2000388c (f32=1.08607315e-19)
08023f90  92ed005a  vldr         s10, [r2]
08023f94  c1f2cb85  blt.w        #7062  ; -> 0x08025b2e
08023f98  169a      ldr          r2, [sp, #88]
08023f9a  0026      movs         r6, #0
08023f9c  1268      ldr          r2, [r2]
08023f9e  002b      cmp          r3, #0
08023fa0  c1f27d85  blt.w        #6906  ; -> 0x08025a9e
08023fa4  0025      movs         r5, #0
08023fa6  002a      cmp          r2, #0
08023fa8  41f04982  bne.w        #5266  ; -> 0x0802543e
08023fac  049b      ldr          r3, [sp, #16]
08023fae  002b      cmp          r3, #0
08023fb0  01f09f82  beq.w        #5438  ; -> 0x080254f2
08023fb4  1b9b      ldr          r3, [sp, #108]
08023fb6  1b68      ldr          r3, [r3]
08023fb8  b3f5617f  cmp.w        r3, #900
08023fbc  c1f29982  blt.w        #5426  ; -> 0x080254f2
08023fc0  189b      ldr          r3, [sp, #96]
08023fc2  93ed008a  vldr         s16, [r3]
08023fc6  089b      ldr          r3, [sp, #32]
08023fc8  d3ed006a  vldr         s13, [r3]
08023fcc  1a9b      ldr          r3, [sp, #104]
08023fce  76eec86a  vsub.f32     s13, s13, s16
08023fd2  93ed003a  vldr         s6, [r3]
08023fd6  0e9b      ldr          r3, [sp, #56]
08023fd8  34ee434a  vsub.f32     s8, s8, s6
08023fdc  66ee8e6a  vmul.f32     s13, s13, s28
08023fe0  1968      ldr          r1, [r3]
08023fe2  24ee0e4a  vmul.f32     s8, s8, s28
08023fe6  514b      ldr          r3, [pc, #324]  ; [0x0802412c] = 0x20003840 (f32=1.08606333e-19)
08023fe8  75ee6d4a  vsub.f32     s9, s10, s27
08023fec  b0ee6d9a  vmov.f32     s18, s27
08023ff0  dfed4f8a  vldr         s17, [pc, #316]  ; [0x08024130] = 0x3f7fbe77 (f32=0.999000013)
08023ff4  1b68      ldr          r3, [r3]
08023ff6  b7ee000a  vmov.f32     s0, #1.000000e+00
08023ffa  dfed4e9a  vldr         s19, [pc, #312]  ; [0x08024134] = 0x3d4ccccd (f32=0.0500000007)
08023ffe  78ee266a  vadd.f32     s13, s16, s13
08024002  0693      str          r3, [sp, #24]
08024004  a4eea89a  vfma.f32     s18, s9, s17
08024008  9ded067a  vldr         s14, [sp, #24]
0802400c  33ee043a  vadd.f32     s6, s6, s8
08024010  494b      ldr          r3, [pc, #292]  ; [0x08024138] = 0x20004c48 (f32=1.08672611e-19)
08024012  f8eec75a  vcvt.f32.s32 s11, s14
08024016  9ded045a  vldr         s10, [sp, #16]
0802401a  d3ed007a  vldr         s15, [r3]
0802401e  474b      ldr          r3, [pc, #284]  ; [0x0802413c] = 0x20002abc (f32=1.08561613e-19)
08024020  b8eec55a  vcvt.f32.s32 s10, s10
08024024  77eee57a  vsub.f32     s15, s15, s11
08024028  93ed007a  vldr         s14, [r3]
0802402c  f0ee498a  vmov.f32     s17, s18
08024030  434b      ldr          r3, [pc, #268]  ; [0x08024140] = 0x20004c64 (f32=1.08672973e-19)
08024032  77eec77a  vsub.f32     s15, s15, s14
08024036  d3ed000a  vldr         s1, [r3]
0802403a  424b      ldr          r3, [pc, #264]  ; [0x08024144] = 0x20004dc0 (f32=1.0867747e-19)
0802403c  a7eea97a  vfma.f32     s14, s15, s19
08024040  93ed001a  vldr         s2, [r3]
08024044  75ee602a  vsub.f32     s5, s10, s1
08024048  134b      ldr          r3, [pc, #76]  ; [0x08024098] = 0x20001b58 (f32=1.0851069e-19)
0802404a  05ee101a  vmov         s10, r1
0802404e  f0ee607a  vmov.f32     s15, s1
08024052  d3ed005a  vldr         s11, [r3]
08024056  b8eec55a  vcvt.f32.s32 s10, s10
0802405a  334b      ldr          r3, [pc, #204]  ; [0x08024128] = 0x2000388c (f32=1.08607315e-19)
0802405c  f0ee474a  vmov.f32     s9, s14
08024060  35eee19a  vsub.f32     s18, s11, s3
08024064  c3ed008a  vstr         s17, [r3]
08024068  b0ee407a  vmov.f32     s14, s0
0802406c  334b      ldr          r3, [pc, #204]  ; [0x0802413c] = 0x20002abc (f32=1.08561613e-19)
0802406e  35ee415a  vsub.f32     s10, s10, s2
08024072  c3ed004a  vstr         s9, [r3]
08024076  e2eeaa7a  vfma.f32     s15, s5, s21
0802407a  a9ee217a  vfma.f32     s14, s18, s3
0802407e  089b      ldr          r3, [sp, #32]
08024080  a5ee2a1a  vfma.f32     s2, s10, s21
08024084  c3ed006a  vstr         s13, [r3]
08024088  079b      ldr          r3, [sp, #28]
0802408a  83ed003a  vstr         s6, [r3]
0802408e  b4eec07a  vcmpe.f32    s14, s0
08024092  5fe0      b            #190  ; -> 0x08024154
08024154  dc4b      ldr          r3, [pc, #880]  ; [0x080244c8] = 0x20004c64 (f32=1.08672973e-19)
08024156  c3ed007a  vstr         s15, [r3]
0802415a  dc4b      ldr          r3, [pc, #880]  ; [0x080244cc] = 0x20004dc0 (f32=1.0867747e-19)
0802415c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024160  83ed001a  vstr         s2, [r3]
08024164  149b      ldr          r3, [sp, #80]
08024166  1b68      ldr          r3, [r3]
08024168  01f18981  bmi.w        #4882  ; -> 0x0802547e
0802416c  012b      cmp          r3, #1
0802416e  41f38681  ble.w        #4876  ; -> 0x0802547e
08024172  b7ee085a  vmov.f32     s10, #1.500000e+00
08024176  27ee055a  vmul.f32     s10, s14, s10
0802417a  dfedd57a  vldr         s15, [pc, #852]  ; [0x080244d0] = 0x3dcccccd (f32=0.100000001)
0802417e  b7ee007a  vmov.f32     s14, #1.000000e+00
08024182  64eea74a  vmul.f32     s9, s9, s15
08024186  74eea47a  vadd.f32     s15, s9, s9
0802418a  b0ee644a  vmov.f32     s8, s9
0802418e  a1ee674a  vfms.f32     s8, s2, s15
08024192  d04b      ldr          r3, [pc, #832]  ; [0x080244d4] = 0x2000386c (f32=1.08606902e-19)
08024194  d3ed007a  vldr         s15, [r3]
08024198  77eec77a  vsub.f32     s15, s15, s14
0802419c  aeee277a  vfma.f32     s14, s28, s15
080241a0  83ed007a  vstr         s14, [r3]
080241a4  049b      ldr          r3, [sp, #16]
080241a6  002b      cmp          r3, #0
080241a8  41f02781  bne.w        #4686  ; -> 0x080253fa
080241ac  ca4b      ldr          r3, [pc, #808]  ; [0x080244d8] = 0x20004c60 (f32=1.08672921e-19)
080241ae  cb4a      ldr          r2, [pc, #812]  ; [0x080244dc] = 0x20001b20 (f32=1.08509966e-19)
080241b0  1b68      ldr          r3, [r3]
080241b2  cb48      ldr          r0, [pc, #812]  ; [0x080244e0] = 0x20004dcc (f32=1.08677626e-19)
080241b4  013b      subs         r3, #1
080241b6  1268      ldr          r2, [r2]
080241b8  0068      ldr          r0, [r0]
080241ba  1a40      ands         r2, r3
080241bc  c64c      ldr          r4, [pc, #792]  ; [0x080244d8] = 0x20004c60 (f32=1.08672921e-19)
080241be  0128      cmp          r0, #1
080241c0  c74b      ldr          r3, [pc, #796]  ; [0x080244e0] = 0x20004dcc (f32=1.08677626e-19)
080241c2  2260      str          r2, [r4]
080241c4  01f0de85  beq.w        #7100  ; -> 0x08025d84
080241c8  c64b      ldr          r3, [pc, #792]  ; [0x080244e4] = 0x20002ac0 (f32=1.08561665e-19)
080241ca  009a      ldr          r2, [sp]
080241cc  d3ed002a  vldr         s5, [r3]
080241d0  179b      ldr          r3, [sp, #92]
080241d2  66eea27a  vmul.f32     s15, s13, s5
080241d6  1b68      ldr          r3, [r3]
080241d8  c2ed007a  vstr         s15, [r2]
080241dc  0bb1      cbz          r3, #2  ; -> 0x080241e2
080241de  67eea67a  vmul.f32     s15, s15, s13
080241e2  67ee277a  vmul.f32     s15, s14, s15
080241e6  c2ed007a  vstr         s15, [r2]
080241ea  0029      cmp          r1, #0
080241ec  41f0fc80  bne.w        #4600  ; -> 0x080253e8
080241f0  23ee277a  vmul.f32     s14, s6, s15
080241f4  bc4a      ldr          r2, [pc, #752]  ; [0x080244e8] = 0x20001ad0 (f32=1.08508932e-19)
080241f6  82ed007a  vstr         s14, [r2]
080241fa  23b1      cbz          r3, #8  ; -> 0x08024206
080241fc  27ee037a  vmul.f32     s14, s14, s6
08024200  b94b      ldr          r3, [pc, #740]  ; [0x080244e8] = 0x20001ad0 (f32=1.08508932e-19)
08024202  83ed007a  vstr         s14, [r3]
08024206  dfedb96a  vldr         s13, [pc, #740]  ; [0x080244ec] = 0x41f4e69c (f32=30.6126022)
0802420a  b4eee67a  vcmpe.f32    s14, s13
0802420e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024212  41f17f80  bpl.w        #4350  ; -> 0x08025314
08024216  f4eee67a  vcmpe.f32    s15, s13
0802421a  b34b      ldr          r3, [pc, #716]  ; [0x080244e8] = 0x20001ad0 (f32=1.08508932e-19)
0802421c  c3ed006a  vstr         s13, [r3]
08024220  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024224  41f18c80  bpl.w        #4376  ; -> 0x08025340
08024228  f0ee667a  vmov.f32     s15, s13
0802422c  009b      ldr          r3, [sp]
0802422e  c3ed006a  vstr         s13, [r3]
08024232  af4b      ldr          r3, [pc, #700]  ; [0x080244f0] = 0x2000388c (f32=1.08607315e-19)
08024234  d3ed006a  vldr         s13, [r3]
08024238  f5eec06a  vcmpe.f32    s13, #0
0802423c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024240  19dd      ble          #50  ; -> 0x08024276
08024242  b1ee007a  vmov.f32     s14, #4.000000e+00
08024246  36ee877a  vadd.f32     s14, s13, s14
0802424a  b4eee77a  vcmpe.f32    s14, s15
0802424e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024252  04d5      bpl          #8  ; -> 0x0802425e
08024254  77eee67a  vsub.f32     s15, s15, s13
08024258  009b      ldr          r3, [sp]
0802425a  c3ed007a  vstr         s15, [r3]
0802425e  a24b      ldr          r3, [pc, #648]  ; [0x080244e8] = 0x20001ad0 (f32=1.08508932e-19)
08024260  d3ed007a  vldr         s15, [r3]
08024264  b4eee77a  vcmpe.f32    s14, s15
08024268  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802426c  03d5      bpl          #6  ; -> 0x08024276
0802426e  77eee66a  vsub.f32     s13, s15, s13
08024272  c3ed006a  vstr         s13, [r3]
08024276  059b      ldr          r3, [sp, #20]
08024278  1b68      ldr          r3, [r3]
0802427a  012b      cmp          r3, #1
0802427c  01f0b583  beq.w        #5994  ; -> 0x080259ea
08024280  029b      ldr          r3, [sp, #8]
08024282  039a      ldr          r2, [sp, #12]
08024284  1b68      ldr          r3, [r3]
08024286  1268      ldr          r2, [r2]
08024288  012b      cmp          r3, #1
0802428a  01f09b83  beq.w        #5942  ; -> 0x080259c4
0802428e  dbf80030  ldr.w        r3, [r11]
08024292  012a      cmp          r2, #1
08024294  01f08383  beq.w        #5894  ; -> 0x0802599e
08024298  012b      cmp          r3, #1
0802429a  f0ee006a  vmov.f32     s13, #2.000000e+00
0802429e  01f06f83  beq.w        #5854  ; -> 0x08025980
080242a2  9bed027a  vldr         s14, [r11, #8]
080242a6  0221      movs         r1, #2
080242a8  dbed017a  vldr         s15, [r11, #4]
080242ac  37ee267a  vadd.f32     s14, s14, s13
080242b0  77ee8c7a  vadd.f32     s15, s15, s24
080242b4  8bed027a  vstr         s14, [r11, #8]
080242b8  cbed017a  vstr         s15, [r11, #4]
080242bc  dbf82c20  ldr.w        r2, [r11, #44]
080242c0  f0ee006a  vmov.f32     s13, #2.000000e+00
080242c4  8b4b      ldr          r3, [pc, #556]  ; [0x080244f4] = 0x20001a14 (f32=1.08506503e-19)
080242c6  012a      cmp          r2, #1
080242c8  01f04c83  beq.w        #5784  ; -> 0x08025964
080242cc  93ed0d7a  vldr         s14, [r3, #52]
080242d0  0322      movs         r2, #3
080242d2  d3ed0c7a  vldr         s15, [r3, #48]
080242d6  37ee267a  vadd.f32     s14, s14, s13
080242da  77ee8c7a  vadd.f32     s15, s15, s24
080242de  83ed0d7a  vstr         s14, [r3, #52]
080242e2  c3ed0c7a  vstr         s15, [r3, #48]
080242e6  059b      ldr          r3, [sp, #20]
080242e8  1b68      ldr          r3, [r3]
080242ea  012b      cmp          r3, #1
080242ec  01f01d83  beq.w        #5690  ; -> 0x0802592a
080242f0  814b      ldr          r3, [pc, #516]  ; [0x080244f8] = 0x200037f4 (f32=1.08605351e-19)
080242f2  1b68      ldr          r3, [r3]
080242f4  012b      cmp          r3, #1
080242f6  01f0c384  beq.w        #6534  ; -> 0x08025c80
080242fa  2c23      movs         r3, #44
080242fc  7a48      ldr          r0, [pc, #488]  ; [0x080244e8] = 0x20001ad0 (f32=1.08508932e-19)
080242fe  dfed7f5a  vldr         s11, [pc, #508]  ; [0x080244fc] = 0x42800000 (f32=64)
08024302  03fb01b3  mla          r3, r3, r1, r11
08024306  90ed007a  vldr         s14, [r0]
0802430a  0020      movs         r0, #0
0802430c  d3ed016a  vldr         s13, [r3, #4]
08024310  1861      str          r0, [r3, #16]
08024312  77ee667a  vsub.f32     s15, s14, s13
08024316  f4eee57a  vcmpe.f32    s15, s11
0802431a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802431e  01f34484  bgt.w        #6280  ; -> 0x08025baa
08024322  9fed777a  vldr         s14, [pc, #476]  ; [0x08024500] = 0xc2800000 (f32=-64)
08024326  f4eec77a  vcmpe.f32    s15, s14
0802432a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802432e  01f16e85  bmi.w        #6876  ; -> 0x08025e0e
08024332  f4eeea7a  vcmpe.f32    s15, s21
08024336  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802433a  02f3b582  bgt.w        #9578  ; -> 0x080268a8
0802433e  9fed717a  vldr         s14, [pc, #452]  ; [0x08024504] = 0xba83126f (f32=-0.00100000005)
08024342  f4eec77a  vcmpe.f32    s15, s14
08024346  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802434a  02f1ad82  bmi.w        #9562  ; -> 0x080268a8
0802434e  2c23      movs         r3, #44
08024350  0098      ldr          r0, [sp]
08024352  77eea67a  vadd.f32     s15, s15, s13
08024356  dfed695a  vldr         s11, [pc, #420]  ; [0x080244fc] = 0x42800000 (f32=64)
0802435a  90ed007a  vldr         s14, [r0]
0802435e  03fb01b0  mla          r0, r3, r1, r11
08024362  03fb02b3  mla          r3, r3, r2, r11
08024366  0024      movs         r4, #0
08024368  c0ed017a  vstr         s15, [r0, #4]
0802436c  d3ed016a  vldr         s13, [r3, #4]
08024370  1c61      str          r4, [r3, #16]
08024372  77ee667a  vsub.f32     s15, s14, s13
08024376  f4eee57a  vcmpe.f32    s15, s11
0802437a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802437e  01f31884  bgt.w        #6192  ; -> 0x08025bb2
08024382  9fed5f7a  vldr         s14, [pc, #380]  ; [0x08024500] = 0xc2800000 (f32=-64)
08024386  f4eec77a  vcmpe.f32    s15, s14
0802438a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802438e  01f14285  bmi.w        #6788  ; -> 0x08025e16
08024392  f4eeea7a  vcmpe.f32    s15, s21
08024396  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802439a  02f39582  bgt.w        #9514  ; -> 0x080268c8
0802439e  9fed597a  vldr         s14, [pc, #356]  ; [0x08024504] = 0xba83126f (f32=-0.00100000005)
080243a2  f4eec77a  vcmpe.f32    s15, s14
080243a6  f1ee10fa  vmrs         APSR_nzcv, fpscr
080243aa  02f18d82  bmi.w        #9498  ; -> 0x080268c8
080243ae  2c23      movs         r3, #44
080243b0  77eea67a  vadd.f32     s15, s15, s13
080243b4  03fb02b3  mla          r3, r3, r2, r11
080243b8  c3ed017a  vstr         s15, [r3, #4]
080243bc  029b      ldr          r3, [sp, #8]
080243be  1b68      ldr          r3, [r3]
080243c0  022b      cmp          r3, #2
080243c2  01f0ef80  beq.w        #4574  ; -> 0x080255a4
080243c6  2c20      movs         r0, #44
080243c8  00fb01b0  mla          r0, r0, r1, r11
080243cc  0369      ldr          r3, [r0, #16]
080243ce  002b      cmp          r3, #0
080243d0  01f0b080  beq.w        #4448  ; -> 0x08025534
080243d4  d0ed027a  vldr         s15, [r0, #8]
080243d8  34eea77a  vadd.f32     s14, s9, s15
080243dc  3e4b      ldr          r3, [pc, #248]  ; [0x080244d8] = 0x20004c60 (f32=1.08672921e-19)
080243de  f0ee004a  vmov.f32     s9, #2.000000e+00
080243e2  2c20      movs         r0, #44
080243e4  b7ee081a  vmov.f32     s2, #1.500000e+00
080243e8  1b68      ldr          r3, [r3]
080243ea  87fe247a  vmaxnm.f32   s14, s14, s9
080243ee  00fb01b1  mla          r1, r0, r1, r11
080243f2  1c46      mov          r4, r3
080243f4  0a93      str          r3, [sp, #40]
080243f6  394b      ldr          r3, [pc, #228]  ; [0x080244dc] = 0x20001b20 (f32=1.08509966e-19)
080243f8  07ee904a  vmov         s15, r4
080243fc  4c6a      ldr          r4, [r1, #36]
080243fe  1b68      ldr          r3, [r3]
08024400  b8eee73a  vcvt.f32.s32 s6, s15
08024404  d1ed082a  vldr         s5, [r1, #32]
08024408  dfed3f5a  vldr         s11, [pc, #252]  ; [0x08024508] = 0x42c80000 (f32=100)
0802440c  77ee034a  vadd.f32     s9, s14, s6
08024410  bdeee47a  vcvt.s32.f32 s14, s9
08024414  c1ed034a  vstr         s9, [r1, #12]
08024418  17ee100a  vmov         r0, s14
0802441c  b8eec77a  vcvt.f32.s32 s14, s14
08024420  1840      ands         r0, r3
08024422  34eec77a  vsub.f32     s14, s9, s14
08024426  451e      subs         r5, r0, #1
08024428  861c      adds         r6, r0, #2
0802442a  4861      str          r0, [r1, #20]
0802442c  1d40      ands         r5, r3
0802442e  1e40      ands         r6, r3
08024430  81ed077a  vstr         s14, [r1, #28]
08024434  411c      adds         r1, r0, #1
08024436  04eb8505  add.w        r5, r4, r5, lsl #2
0802443a  27ee060a  vmul.f32     s0, s14, s12
0802443e  04eb8606  add.w        r6, r4, r6, lsl #2
08024442  1940      ands         r1, r3
08024444  d5ed007a  vldr         s15, [r5]
08024448  04eb8000  add.w        r0, r4, r0, lsl #2
0802444c  d6ed004a  vldr         s9, [r6]
08024450  04eb8101  add.w        r1, r4, r1, lsl #2
08024454  d0ed006a  vldr         s13, [r0]
08024458  77eea47a  vadd.f32     s15, s15, s9
0802445c  d1ed000a  vldr         s1, [r1]
08024460  77eee64a  vsub.f32     s9, s15, s13
08024464  76eea77a  vadd.f32     s15, s13, s15
08024468  74eee04a  vsub.f32     s9, s9, s1
0802446c  64ee804a  vmul.f32     s9, s9, s0
08024470  e7eec64a  vfms.f32     s9, s15, s12
08024474  e0ee814a  vfma.f32     s9, s1, s2
08024478  e7ee246a  vfma.f32     s13, s14, s9
0802447c  16ee901a  vmov         r1, s13
08024480  21f00041  bic          r1, r1, #2147483648
08024484  b1f1ff4f  cmp.w        r1, #2139095040
08024488  88bf      it           hi
0802448a  f0ee656a  vmovhi.f32   s13, s11
0802448e  66eea26a  vmul.f32     s13, s13, s5
08024492  0399      ldr          r1, [sp, #12]
08024494  0968      ldr          r1, [r1]
08024496  0229      cmp          r1, #2
08024498  01f06b81  beq.w        #4822  ; -> 0x08025772
0802449c  2c20      movs         r0, #44
0802449e  00fb02b0  mla          r0, r0, r2, r11
080244a2  0169      ldr          r1, [r0, #16]
080244a4  0029      cmp          r1, #0
080244a6  01f03f80  beq.w        #4222  ; -> 0x08025528
080244aa  d0ed024a  vldr         s9, [r0, #8]
080244ae  34ee247a  vadd.f32     s14, s8, s9
080244b2  f0ee007a  vmov.f32     s15, #2.000000e+00
080244b6  2c21      movs         r1, #44
080244b8  f7ee082a  vmov.f32     s5, #1.500000e+00
080244bc  9fed124a  vldr         s8, [pc, #72]  ; [0x08024508] = 0x42c80000 (f32=100)
080244c0  01fb02b2  mla          r2, r1, r2, r11
080244c4  22e0      b            #68  ; -> 0x0802450c
0802450c  dbf85010  ldr.w        r1, [r11, #80]
08024510  c7fe277a  vmaxnm.f32   s15, s14, s15
08024514  77ee835a  vadd.f32     s11, s15, s6
08024518  0691      str          r1, [sp, #24]
0802451a  506a      ldr          r0, [r2, #36]
0802451c  fdeee57a  vcvt.s32.f32 s15, s11
08024520  c2ed035a  vstr         s11, [r2, #12]
08024524  92ed083a  vldr         s6, [r2, #32]
08024528  17ee901a  vmov         r1, s15
0802452c  f8eee77a  vcvt.f32.s32 s15, s15
08024530  1940      ands         r1, r3
08024532  75eee77a  vsub.f32     s15, s11, s15
08024536  4d1e      subs         r5, r1, #1
08024538  8c1c      adds         r4, r1, #2
0802453a  5161      str          r1, [r2, #20]
0802453c  1d40      ands         r5, r3
0802453e  1c40      ands         r4, r3
08024540  c2ed077a  vstr         s15, [r2, #28]
08024544  4a1c      adds         r2, r1, #1
08024546  00eb8505  add.w        r5, r0, r5, lsl #2
0802454a  67ee860a  vmul.f32     s1, s15, s12
0802454e  00eb8404  add.w        r4, r0, r4, lsl #2
08024552  1340      ands         r3, r2
08024554  d5ed004a  vldr         s9, [r5]
08024558  00eb8101  add.w        r1, r0, r1, lsl #2
0802455c  94ed007a  vldr         s14, [r4]
08024560  00eb8303  add.w        r3, r0, r3, lsl #2
08024564  74ee874a  vadd.f32     s9, s9, s14
08024568  91ed007a  vldr         s14, [r1]
0802456c  93ed001a  vldr         s2, [r3]
08024570  74eec75a  vsub.f32     s11, s9, s14
08024574  77ee244a  vadd.f32     s9, s14, s9
08024578  75eec15a  vsub.f32     s11, s11, s2
0802457c  65eea05a  vmul.f32     s11, s11, s1
08024580  e4eec65a  vfms.f32     s11, s9, s12
08024584  e1ee225a  vfma.f32     s11, s2, s5
08024588  a7eea57a  vfma.f32     s14, s15, s11
0802458c  17ee103a  vmov         r3, s14
08024590  23f00043  bic          r3, r3, #2147483648
08024594  b3f1ff4f  cmp.w        r3, #2139095040
08024598  88bf      it           hi
0802459a  b0ee447a  vmovhi.f32   s14, s8
0802459e  27ee037a  vmul.f32     s14, s14, s6
080245a2  16ee903a  vmov         r3, s13
080245a6  23f00043  bic          r3, r3, #2147483648
080245aa  b3f1ff4f  cmp.w        r3, #2139095040
080245ae  17ee103a  vmov         r3, s14
080245b2  23f00043  bic          r3, r3, #2147483648
080245b6  00f20487  bhi.w        #3592  ; -> 0x080253c2
080245ba  f2ee047a  vmov.f32     s15, #1.000000e+01
080245be  b3f1ff4f  cmp.w        r3, #2139095040
080245c2  f4eee76a  vcmpe.f32    s13, s15
080245c6  01f20283  bhi.w        #5636  ; -> 0x08025bce
080245ca  f1ee10fa  vmrs         APSR_nzcv, fpscr
080245ce  07dc      bgt          #14  ; -> 0x080245e0
080245d0  faee047a  vmov.f32     s15, #-1.000000e+01
080245d4  f4eee76a  vcmpe.f32    s13, s15
080245d8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080245dc  41f10a83  bpl.w        #5652  ; -> 0x08025bf4
080245e0  f2ee046a  vmov.f32     s13, #1.000000e+01
080245e4  faee047a  vmov.f32     s15, #-1.000000e+01
080245e8  b4eee67a  vcmpe.f32    s14, s13
080245ec  f1ee10fa  vmrs         APSR_nzcv, fpscr
080245f0  b4eee77a  vcmpe.f32    s14, s15
080245f4  ccbf      ite          gt
080245f6  0123      movgt        r3, #1
080245f8  0023      movle        r3, #0
080245fa  f1ee10fa  vmrs         APSR_nzcv, fpscr
080245fe  48bf      it           mi
08024600  43f00103  orrmi        r3, r3, #1
08024604  e84a      ldr          r2, [pc, #928]  ; [0x080249a8] = 0x20001b68 (f32=1.08510897e-19)
08024606  d2ed006a  vldr         s13, [r2]
0802460a  e74a      ldr          r2, [pc, #924]  ; [0x080249a8] = 0x20001b68 (f32=1.08510897e-19)
0802460c  c2ed006a  vstr         s13, [r2]
08024610  13b1      cbz          r3, #4  ; -> 0x08024618
08024612  e64b      ldr          r3, [pc, #920]  ; [0x080249ac] = 0x20003858 (f32=1.08606643e-19)
08024614  93ed007a  vldr         s14, [r3]
08024618  199b      ldr          r3, [sp, #100]
0802461a  e44a      ldr          r2, [pc, #912]  ; [0x080249ac] = 0x20003858 (f32=1.08606643e-19)
0802461c  1b68      ldr          r3, [r3]
0802461e  82ed007a  vstr         s14, [r2]
08024622  002b      cmp          r3, #0
08024624  40f0b486  bne.w        #3432  ; -> 0x08025390
08024628  e14b      ldr          r3, [pc, #900]  ; [0x080249b0] = 0x20001b58 (f32=1.0851069e-19)
0802462a  f0ee634a  vmov.f32     s9, s7
0802462e  f0ee425a  vmov.f32     s11, s4
08024632  d3ed007a  vldr         s15, [r3]
08024636  e7ee274a  vfma.f32     s9, s14, s15
0802463a  e7eea65a  vfma.f32     s11, s15, s13
0802463e  f0ee647a  vmov.f32     s15, s9
08024642  dc4b      ldr          r3, [pc, #880]  ; [0x080249b4] = 0x20003888 (f32=1.08607263e-19)
08024644  dfeddc4a  vldr         s9, [pc, #880]  ; [0x080249b8] = 0x3e6147ae (f32=0.219999999)
08024648  93ed003a  vldr         s6, [r3]
0802464c  b4eee43a  vcmpe.f32    s6, s9
08024650  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024654  11dd      ble          #34  ; -> 0x0802467a
08024656  9fedd94a  vldr         s8, [pc, #868]  ; [0x080249bc] = 0x3f9c28f6 (f32=1.22000003)
0802465a  73ee644a  vsub.f32     s9, s6, s9
0802465e  d9ed182a  vldr         s5, [r9, #96]
08024662  34ee434a  vsub.f32     s8, s8, s6
08024666  99ed193a  vldr         s6, [r9, #100]
0802466a  65ee845a  vmul.f32     s11, s11, s8
0802466e  67ee847a  vmul.f32     s15, s15, s8
08024672  e2eea45a  vfma.f32     s11, s5, s9
08024676  e4ee837a  vfma.f32     s15, s9, s6
0802467a  d14b      ldr          r3, [pc, #836]  ; [0x080249c0] = 0x20004c64 (f32=1.08672973e-19)
0802467c  36eee54a  vsub.f32     s8, s13, s11
08024680  f2ee044a  vmov.f32     s9, #1.000000e+01
08024684  93ed001a  vldr         s2, [r3]
08024688  e4ee015a  vfma.f32     s11, s8, s2
0802468c  f4eee45a  vcmpe.f32    s11, s9
08024690  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024694  06dc      bgt          #12  ; -> 0x080246a4
08024696  faee044a  vmov.f32     s9, #-1.000000e+01
0802469a  f4eee45a  vcmpe.f32    s11, s9
0802469e  f1ee10fa  vmrs         APSR_nzcv, fpscr
080246a2  02d5      bpl          #4  ; -> 0x080246aa
080246a4  c74b      ldr          r3, [pc, #796]  ; [0x080249c4] = 0x20001b5c (f32=1.08510742e-19)
080246a6  d3ed005a  vldr         s11, [r3]
080246aa  c74b      ldr          r3, [pc, #796]  ; [0x080249c8] = 0x20004d28 (f32=1.08675506e-19)
080246ac  c64a      ldr          r2, [pc, #792]  ; [0x080249c8] = 0x20004d28 (f32=1.08675506e-19)
080246ae  93ed004a  vldr         s8, [r3]
080246b2  c64b      ldr          r3, [pc, #792]  ; [0x080249cc] = 0x20002b3c (f32=1.08563268e-19)
080246b4  35eec44a  vsub.f32     s8, s11, s8
080246b8  d3ed004a  vldr         s9, [r3]
080246bc  c14b      ldr          r3, [pc, #772]  ; [0x080249c4] = 0x20001b5c (f32=1.08510742e-19)
080246be  efee044a  vfma.f32     s9, s30, s8
080246c2  c3ed005a  vstr         s11, [r3]
080246c6  c14b      ldr          r3, [pc, #772]  ; [0x080249cc] = 0x20002b3c (f32=1.08563268e-19)
080246c8  c3ed005a  vstr         s11, [r3]
080246cc  14ee903a  vmov         r3, s9
080246d0  c2ed004a  vstr         s9, [r2]
080246d4  23f00043  bic          r3, r3, #2147483648
080246d8  b3f1ff4f  cmp.w        r3, #2139095040
080246dc  05d9      bls          #10  ; -> 0x080246ea
080246de  9fedbc4a  vldr         s8, [pc, #752]  ; [0x080249d0] = 0x00000000 (f32=0)
080246e2  f0ee444a  vmov.f32     s9, s8
080246e6  82ed004a  vstr         s8, [r2]
080246ea  15ee903a  vmov         r3, s11
080246ee  23f00043  bic          r3, r3, #2147483648
080246f2  b3f1ff4f  cmp.w        r3, #2139095040
080246f6  02d9      bls          #4  ; -> 0x080246fe
080246f8  0023      movs         r3, #0
080246fa  b44a      ldr          r2, [pc, #720]  ; [0x080249cc] = 0x20002b3c (f32=1.08563268e-19)
080246fc  1360      str          r3, [r2]
080246fe  98ed004a  vldr         s8, [r8]
08024702  75eee45a  vsub.f32     s11, s11, s9
08024706  d8ed014a  vldr         s9, [r8, #4]
0802470a  b8eec44a  vcvt.f32.s32 s8, s8
0802470e  65ee865a  vmul.f32     s11, s11, s12
08024712  34ee644a  vsub.f32     s8, s8, s9
08024716  e4ee2a4a  vfma.f32     s9, s8, s21
0802471a  f4eeee4a  vcmpe.f32    s9, s29
0802471e  c8ed014a  vstr         s9, [r8, #4]
08024722  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024726  40f35986  ble.w        #3250  ; -> 0x080253dc
0802472a  9fedaa4a  vldr         s8, [pc, #680]  ; [0x080249d4] = 0x43d50000 (f32=426)
0802472e  9fedaa0a  vldr         s0, [pc, #680]  ; [0x080249d8] = 0x3b03126f (f32=0.00200000009)
08024732  34eec44a  vsub.f32     s8, s9, s8
08024736  24ee000a  vmul.f32     s0, s8, s0
0802473a  bdeee44a  vcvt.s32.f32 s8, s9
0802473e  98ed0a3a  vldr         s6, [r8, #40]
08024742  a64a      ldr          r2, [pc, #664]  ; [0x080249dc] = 0x0802b5c4 (f32=3.93341165e-34)
08024744  64eeaf4a  vmul.f32     s9, s9, s31
08024748  dfedaa8a  vldr         s17, [pc, #680]  ; [0x080249f4] = 0x3e99999a (f32=0.300000012)
0802474c  f1ee002a  vmov.f32     s5, #4.000000e+00
08024750  14ee103a  vmov         r3, s8
08024754  98ed064a  vldr         s8, [r8, #24]
08024758  d8ed020a  vldr         s1, [r8, #8]
0802475c  33ee049a  vadd.f32     s18, s6, s8
08024760  02eb8303  add.w        r3, r2, r3, lsl #2
08024764  98ed078a  vldr         s16, [r8, #28]
08024768  f4eee24a  vcmpe.f32    s9, s5
0802476c  29ee289a  vmul.f32     s18, s18, s17
08024770  d3ed008a  vldr         s17, [r3]
08024774  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024778  78eee08a  vsub.f32     s17, s17, s1
0802477c  e0ee095a  vfma.f32     s11, s0, s18
08024780  98ed030a  vldr         s0, [r8, #12]
08024784  98ed089a  vldr         s18, [r8, #32]
08024788  e8eeac0a  vfma.f32     s1, s17, s25
0802478c  d8ed048a  vldr         s17, [r8, #16]
08024790  35eec88a  vsub.f32     s16, s11, s16
08024794  c8ed035a  vstr         s11, [r8, #12]
08024798  25eeabaa  vmul.f32     s20, s11, s23
0802479c  c8ed020a  vstr         s1, [r8, #8]
080247a0  a0ee880a  vfma.f32     s0, s1, s16
080247a4  98ed098a  vldr         s16, [r8, #36]
080247a8  8ded1daa  vstr         s20, [sp, #116]
080247ac  f0ee409a  vmov.f32     s19, s0
080247b0  98ed050a  vldr         s0, [r8, #20]
080247b4  75eea95a  vadd.f32     s11, s11, s19
080247b8  c8ed079a  vstr         s19, [r8, #28]
080247bc  65ee865a  vmul.f32     s11, s11, s12
080247c0  35eec99a  vsub.f32     s18, s11, s18
080247c4  c8ed045a  vstr         s11, [r8, #16]
080247c8  e0ee898a  vfma.f32     s17, s1, s18
080247cc  9fed8a9a  vldr         s18, [pc, #552]  ; [0x080249f8] = 0x3f818937 (f32=1.01199996)
080247d0  25ee899a  vmul.f32     s18, s11, s18
080247d4  8ded1e9a  vstr         s18, [sp, #120]
080247d8  75eea85a  vadd.f32     s11, s11, s17
080247dc  c8ed088a  vstr         s17, [r8, #32]
080247e0  65ee865a  vmul.f32     s11, s11, s12
080247e4  35eec88a  vsub.f32     s16, s11, s16
080247e8  c8ed055a  vstr         s11, [r8, #20]
080247ec  65eeab8a  vmul.f32     s17, s11, s23
080247f0  a0ee880a  vfma.f32     s0, s1, s16
080247f4  cded1f8a  vstr         s17, [sp, #124]
080247f8  75ee805a  vadd.f32     s11, s11, s0
080247fc  88ed090a  vstr         s0, [r8, #36]
08024800  65ee865a  vmul.f32     s11, s11, s12
08024804  35eec30a  vsub.f32     s0, s11, s6
08024808  9fed7c3a  vldr         s6, [pc, #496]  ; [0x080249fc] = 0x3fa374bc (f32=1.27699995)
0802480c  c8ed065a  vstr         s11, [r8, #24]
08024810  25ee833a  vmul.f32     s6, s11, s6
08024814  a0ee804a  vfma.f32     s8, s1, s0
08024818  8ded203a  vstr         s6, [sp, #128]
0802481c  75ee845a  vadd.f32     s11, s11, s8
08024820  88ed0a4a  vstr         s8, [r8, #40]
08024824  9fed764a  vldr         s8, [pc, #472]  ; [0x08024a00] = 0x3f2a3d71 (f32=0.665000021)
08024828  65ee845a  vmul.f32     s11, s11, s8
0802482c  cded215a  vstr         s11, [sp, #132]
08024830  80f2c085  bge.w        #2944  ; -> 0x080253b4
08024834  fdeee45a  vcvt.s32.f32 s11, s9
08024838  22aa      add          r2, sp, #136
0802483a  15ee903a  vmov         r3, s11
0802483e  b8eee54a  vcvt.f32.s32 s8, s11
08024842  02eb8302  add.w        r2, r2, r3, lsl #2
08024846  74eec44a  vsub.f32     s9, s9, s8
0802484a  52ed045a  vldr         s11, [r2, #-16]
0802484e  12ed053a  vldr         s6, [r2, #-20]
08024852  75eec35a  vsub.f32     s11, s11, s6
08024856  624b      ldr          r3, [pc, #392]  ; [0x080249e0] = 0x2000385c (f32=1.08606695e-19)
08024858  93ed004a  vldr         s8, [r3]
0802485c  a5eea43a  vfma.f32     s6, s11, s9
08024860  f0eec35a  vabs.f32     s11, s6
08024864  b4eee54a  vcmpe.f32    s8, s11
08024868  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802486c  40f18a85  bpl.w        #2836  ; -> 0x08025384
08024870  75eec45a  vsub.f32     s11, s11, s8
08024874  dfed5b4a  vldr         s9, [pc, #364]  ; [0x080249e4] = 0x3f666666 (f32=0.899999976)
08024878  a5eea44a  vfma.f32     s8, s11, s9
0802487c  159a      ldr          r2, [sp, #84]
0802487e  64ee055a  vmul.f32     s11, s8, s10
08024882  f1ee044a  vmov.f32     s9, #5.000000e+00
08024886  564b      ldr          r3, [pc, #344]  ; [0x080249e0] = 0x2000385c (f32=1.08606695e-19)
08024888  92ed038a  vldr         s16, [r2, #12]
0802488c  77ee678a  vsub.f32     s17, s14, s15
08024890  92ed020a  vldr         s0, [r2, #8]
08024894  c5fe865a  vmaxnm.f32   s11, s11, s12
08024898  b1ee488a  vneg.f32     s16, s16
0802489c  d2ed012a  vldr         s5, [r2, #4]
080248a0  f0ee400a  vmov.f32     s1, s0
080248a4  c5fee45a  vminnm.f32   s11, s11, s9
080248a8  f1ee622a  vneg.f32     s5, s5
080248ac  d2ed004a  vldr         s9, [r2]
080248b0  e1ee287a  vfma.f32     s15, s2, s17
080248b4  83ed004a  vstr         s8, [r3]
080248b8  e8ee250a  vfma.f32     s1, s16, s11
080248bc  f0ee628a  vmov.f32     s17, s5
080248c0  b2ee044a  vmov.f32     s8, #1.000000e+01
080248c4  f4eec47a  vcmpe.f32    s15, s8
080248c8  e0eea58a  vfma.f32     s17, s1, s11
080248cc  f1ee10fa  vmrs         APSR_nzcv, fpscr
080248d0  f0ee680a  vmov.f32     s1, s17
080248d4  f0ee648a  vmov.f32     s17, s9
080248d8  e0eea58a  vfma.f32     s17, s1, s11
080248dc  28ee833a  vmul.f32     s6, s17, s6
080248e0  06dc      bgt          #12  ; -> 0x080248f0
080248e2  faee045a  vmov.f32     s11, #-1.000000e+01
080248e6  f4eee57a  vcmpe.f32    s15, s11
080248ea  f1ee10fa  vmrs         APSR_nzcv, fpscr
080248ee  02d5      bpl          #4  ; -> 0x080248f6
080248f0  3d4b      ldr          r3, [pc, #244]  ; [0x080249e8] = 0x20001ad4 (f32=1.08508984e-19)
080248f2  d3ed007a  vldr         s15, [r3]
080248f6  3d4b      ldr          r3, [pc, #244]  ; [0x080249ec] = 0x20004c44 (f32=1.08672559e-19)
080248f8  3c4a      ldr          r2, [pc, #240]  ; [0x080249ec] = 0x20004c44 (f32=1.08672559e-19)
080248fa  93ed004a  vldr         s8, [r3]
080248fe  3c4b      ldr          r3, [pc, #240]  ; [0x080249f0] = 0x20002ac8 (f32=1.08561769e-19)
08024900  37eec44a  vsub.f32     s8, s15, s8
08024904  d3ed005a  vldr         s11, [r3]
08024908  374b      ldr          r3, [pc, #220]  ; [0x080249e8] = 0x20001ad4 (f32=1.08508984e-19)
0802490a  efee045a  vfma.f32     s11, s30, s8
0802490e  c3ed007a  vstr         s15, [r3]
08024912  374b      ldr          r3, [pc, #220]  ; [0x080249f0] = 0x20002ac8 (f32=1.08561769e-19)
08024914  c3ed007a  vstr         s15, [r3]
08024918  15ee903a  vmov         r3, s11
0802491c  c2ed005a  vstr         s11, [r2]
08024920  23f00043  bic          r3, r3, #2147483648
08024924  b3f1ff4f  cmp.w        r3, #2139095040
08024928  05d9      bls          #10  ; -> 0x08024936
0802492a  9fed294a  vldr         s8, [pc, #164]  ; [0x080249d0] = 0x00000000 (f32=0)
0802492e  f0ee445a  vmov.f32     s11, s8
08024932  82ed004a  vstr         s8, [r2]
08024936  17ee903a  vmov         r3, s15
0802493a  23f00043  bic          r3, r3, #2147483648
0802493e  b3f1ff4f  cmp.w        r3, #2139095040
08024942  02d9      bls          #4  ; -> 0x0802494a
08024944  0023      movs         r3, #0
08024946  2a4a      ldr          r2, [pc, #168]  ; [0x080249f0] = 0x20002ac8 (f32=1.08561769e-19)
08024948  1360      str          r3, [r2]
0802494a  98ed0b4a  vldr         s8, [r8, #44]
0802494e  77eee57a  vsub.f32     s15, s15, s11
08024952  d8ed0c5a  vldr         s11, [r8, #48]
08024956  b8eec44a  vcvt.f32.s32 s8, s8
0802495a  67ee867a  vmul.f32     s15, s15, s12
0802495e  34ee654a  vsub.f32     s8, s8, s11
08024962  e4ee2a5a  vfma.f32     s11, s8, s21
08024966  f4eeee5a  vcmpe.f32    s11, s29
0802496a  c8ed0c5a  vstr         s11, [r8, #48]
0802496e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024972  40f3fb84  ble.w        #2550  ; -> 0x0802536c
08024976  9fed174a  vldr         s8, [pc, #92]  ; [0x080249d4] = 0x43d50000 (f32=426)
0802497a  dfed179a  vldr         s19, [pc, #92]  ; [0x080249d8] = 0x3b03126f (f32=0.00200000009)
0802497e  35eec44a  vsub.f32     s8, s11, s8
08024982  64ee299a  vmul.f32     s19, s8, s19
08024986  bdeee54a  vcvt.s32.f32 s8, s11
0802498a  d8ed110a  vldr         s1, [r8, #68]
0802498e  d8ed158a  vldr         s17, [r8, #84]
08024992  65eeaf5a  vmul.f32     s11, s11, s31
08024996  114a      ldr          r2, [pc, #68]  ; [0x080249dc] = 0x0802b5c4 (f32=3.93341165e-34)
08024998  b1ee00da  vmov.f32     s26, #4.000000e+00
0802499c  38eea0aa  vadd.f32     s20, s17, s1
080249a0  14ee103a  vmov         r3, s8
080249a4  2ee0      b            #92  ; -> 0x08024a04
08024a04  1fed059a  vldr         s18, [pc, #-20]  ; [0x080249f4] = 0x3e99999a (f32=0.300000012)
08024a08  02eb8303  add.w        r3, r2, r3, lsl #2
08024a0c  98ed0d4a  vldr         s8, [r8, #52]
08024a10  2aee09aa  vmul.f32     s20, s20, s18
08024a14  d8ed128a  vldr         s17, [r8, #72]
08024a18  93ed009a  vldr         s18, [r3]
08024a1c  f4eecd5a  vcmpe.f32    s11, s26
08024a20  e9ee8a7a  vfma.f32     s15, s19, s20
08024a24  98ed0eaa  vldr         s20, [r8, #56]
08024a28  39ee449a  vsub.f32     s18, s18, s8
08024a2c  d8ed0f9a  vldr         s19, [r8, #60]
08024a30  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024a34  a9ee2c4a  vfma.f32     s8, s18, s25
08024a38  37eee89a  vsub.f32     s18, s15, s17
08024a3c  c8ed0e7a  vstr         s15, [r8, #56]
08024a40  67eeab8a  vmul.f32     s17, s15, s23
08024a44  a4ee09aa  vfma.f32     s20, s8, s18
08024a48  cded1d8a  vstr         s17, [sp, #116]
08024a4c  d8ed138a  vldr         s17, [r8, #76]
08024a50  98ed109a  vldr         s18, [r8, #64]
08024a54  88ed0d4a  vstr         s8, [r8, #52]
08024a58  77ee8a7a  vadd.f32     s15, s15, s20
08024a5c  88ed12aa  vstr         s20, [r8, #72]
08024a60  67ee867a  vmul.f32     s15, s15, s12
08024a64  37eee8aa  vsub.f32     s20, s15, s17
08024a68  5fed1d8a  vldr         s17, [pc, #-116]  ; [0x080249f8] = 0x3f818937 (f32=1.01199996)
08024a6c  c8ed0f7a  vstr         s15, [r8, #60]
08024a70  e4ee0a9a  vfma.f32     s19, s8, s20
08024a74  27eea8aa  vmul.f32     s20, s15, s17
08024a78  d8ed148a  vldr         s17, [r8, #80]
08024a7c  8ded1eaa  vstr         s20, [sp, #120]
08024a80  77eea97a  vadd.f32     s15, s15, s19
08024a84  c8ed139a  vstr         s19, [r8, #76]
08024a88  67ee867a  vmul.f32     s15, s15, s12
08024a8c  77eee89a  vsub.f32     s19, s15, s17
08024a90  c8ed107a  vstr         s15, [r8, #64]
08024a94  27eeabaa  vmul.f32     s20, s15, s23
08024a98  d8ed158a  vldr         s17, [r8, #84]
08024a9c  a4ee299a  vfma.f32     s18, s8, s19
08024aa0  8ded1faa  vstr         s20, [sp, #124]
08024aa4  77ee897a  vadd.f32     s15, s15, s18
08024aa8  88ed149a  vstr         s18, [r8, #80]
08024aac  1fed2d9a  vldr         s18, [pc, #-180]  ; [0x080249fc] = 0x3fa374bc (f32=1.27699995)
08024ab0  67ee867a  vmul.f32     s15, s15, s12
08024ab4  77eee88a  vsub.f32     s17, s15, s17
08024ab8  c8ed117a  vstr         s15, [r8, #68]
08024abc  27ee899a  vmul.f32     s18, s15, s18
08024ac0  e4ee280a  vfma.f32     s1, s8, s17
08024ac4  1fed324a  vldr         s8, [pc, #-200]  ; [0x08024a00] = 0x3f2a3d71 (f32=0.665000021)
08024ac8  8ded209a  vstr         s18, [sp, #128]
08024acc  77eea07a  vadd.f32     s15, s15, s1
08024ad0  c8ed150a  vstr         s1, [r8, #84]
08024ad4  67ee847a  vmul.f32     s15, s15, s8
08024ad8  cded217a  vstr         s15, [sp, #132]
08024adc  80f24284  bge.w        #2180  ; -> 0x08025364
08024ae0  fdeee57a  vcvt.s32.f32 s15, s11
08024ae4  22aa      add          r2, sp, #136
08024ae6  17ee903a  vmov         r3, s15
08024aea  b8eee74a  vcvt.f32.s32 s8, s15
08024aee  02eb8302  add.w        r2, r2, r3, lsl #2
08024af2  75eec45a  vsub.f32     s11, s11, s8
08024af6  52ed047a  vldr         s15, [r2, #-16]
08024afa  12ed059a  vldr         s18, [r2, #-20]
08024afe  77eec97a  vsub.f32     s15, s15, s18
08024b02  e64b      ldr          r3, [pc, #920]  ; [0x08024e9c] = 0x200037fc (f32=1.08605454e-19)
08024b04  93ed004a  vldr         s8, [r3]
08024b08  a7eea59a  vfma.f32     s18, s15, s11
08024b0c  f0eec95a  vabs.f32     s11, s18
08024b10  b4eee54a  vcmpe.f32    s8, s11
08024b14  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024b18  40f12e84  bpl.w        #2140  ; -> 0x08025378
08024b1c  75eec45a  vsub.f32     s11, s11, s8
08024b20  dfeddf7a  vldr         s15, [pc, #892]  ; [0x08024ea0] = 0x3f666666 (f32=0.899999976)
08024b24  a5eea74a  vfma.f32     s8, s11, s15
08024b28  f0ee447a  vmov.f32     s15, s8
08024b2c  27ee855a  vmul.f32     s10, s15, s10
08024b30  059b      ldr          r3, [sp, #20]
08024b32  f1ee045a  vmov.f32     s11, #5.000000e+00
08024b36  d94a      ldr          r2, [pc, #868]  ; [0x08024e9c] = 0x200037fc (f32=1.08605454e-19)
08024b38  1b68      ldr          r3, [r3]
08024b3a  85fe065a  vmaxnm.f32   s10, s10, s12
08024b3e  c2ed007a  vstr         s15, [r2]
08024b42  85fe655a  vminnm.f32   s10, s10, s11
08024b46  a8ee050a  vfma.f32     s0, s16, s10
08024b4a  0493      str          r3, [sp, #16]
08024b4c  e0ee052a  vfma.f32     s5, s0, s10
08024b50  e2ee854a  vfma.f32     s9, s5, s10
08024b54  64ee894a  vmul.f32     s9, s9, s18
08024b58  cbb9      cbnz         r3, #50  ; -> 0x08024b8e
08024b5a  d24b      ldr          r3, [pc, #840]  ; [0x08024ea4] = 0x20002ab0 (f32=1.08561458e-19)
08024b5c  b0ee435a  vmov.f32     s10, s6
08024b60  d3ed005a  vldr         s11, [r3]
08024b64  d04b      ldr          r3, [pc, #832]  ; [0x08024ea8] = 0x20001ad8 (f32=1.08509036e-19)
08024b66  75eec35a  vsub.f32     s11, s11, s6
08024b6a  d3ed007a  vldr         s15, [r3]
08024b6e  cd4b      ldr          r3, [pc, #820]  ; [0x08024ea4] = 0x20002ab0 (f32=1.08561458e-19)
08024b70  a1ee255a  vfma.f32     s10, s2, s11
08024b74  77eee47a  vsub.f32     s15, s15, s9
08024b78  f0ee455a  vmov.f32     s11, s10
08024b7c  b0ee645a  vmov.f32     s10, s9
08024b80  c3ed005a  vstr         s11, [r3]
08024b84  a1ee275a  vfma.f32     s10, s2, s15
08024b88  c74b      ldr          r3, [pc, #796]  ; [0x08024ea8] = 0x20001ad8 (f32=1.08509036e-19)
08024b8a  83ed005a  vstr         s10, [r3]
08024b8e  c74b      ldr          r3, [pc, #796]  ; [0x08024eac] = 0x20002a90 (f32=1.08561045e-19)
08024b90  33ee663a  vsub.f32     s6, s6, s13
08024b94  c64a      ldr          r2, [pc, #792]  ; [0x08024eb0] = 0x20001b4c (f32=1.08510535e-19)
08024b96  41f29b00  movw         r0, #4251
08024b9a  1b68      ldr          r3, [r3]
08024b9c  74eec74a  vsub.f32     s9, s9, s14
08024ba0  d2f800a0  ldr.w        r10, [r2]
08024ba4  e1ee036a  vfma.f32     s13, s2, s6
08024ba8  013b      subs         r3, #1
08024baa  c24a      ldr          r2, [pc, #776]  ; [0x08024eb4] = 0x20002a9c (f32=1.085612e-19)
08024bac  bf49      ldr          r1, [pc, #764]  ; [0x08024eac] = 0x20002a90 (f32=1.08561045e-19)
08024bae  a1ee247a  vfma.f32     s14, s2, s9
08024bb2  03ea0a03  and.w        r3, r3, r10
08024bb6  d2f800e0  ldr.w        lr, [r2]
08024bba  dfedbf2a  vldr         s5, [pc, #764]  ; [0x08024eb8] = 0x3f266666 (f32=0.649999976)
08024bbe  b5ee009a  vmov.f32     s18, #2.500000e-01
08024bc2  03f22e72  addw         r2, r3, #1838
08024bc6  03f6c104  addw         r4, r3, #2241
08024bca  0b60      str          r3, [r1]
08024bcc  03f66621  addw         r1, r3, #2662
08024bd0  02ea0a02  and.w        r2, r2, r10
08024bd4  04ea0a04  and.w        r4, r4, r10
08024bd8  01ea0a01  and.w        r1, r1, r10
08024bdc  1844      add          r0, r3
08024bde  0eeb8202  add.w        r2, lr, r2, lsl #2
08024be2  66eea26a  vmul.f32     s13, s13, s5
08024be6  0eeb8404  add.w        r4, lr, r4, lsl #2
08024bea  00ea0a00  and.w        r0, r0, r10
08024bee  92ed003a  vldr         s6, [r2]
08024bf2  03f63842  addw         r2, r3, #3128
08024bf6  0eeb8101  add.w        r1, lr, r1, lsl #2
08024bfa  27ee227a  vmul.f32     s14, s14, s5
08024bfe  89ed003a  vstr         s6, [r9]
08024c02  02ea0a02  and.w        r2, r2, r10
08024c06  94ed004a  vldr         s8, [r4]
08024c0a  03f66864  addw         r4, r3, #3688
08024c0e  0eeb8202  add.w        r2, lr, r2, lsl #2
08024c12  dfedaa9a  vldr         s19, [pc, #680]  ; [0x08024ebc] = 0x3f2aaaab (f32=0.666666687)
08024c16  89ed014a  vstr         s8, [r9, #4]
08024c1a  04ea0a04  and.w        r4, r4, r10
08024c1e  91ed005a  vldr         s10, [r1]
08024c22  41f2fd21  movw         r1, #4861
08024c26  0eeb8404  add.w        r4, lr, r4, lsl #2
08024c2a  33ee04da  vadd.f32     s26, s6, s8
08024c2e  89ed025a  vstr         s10, [r9, #8]
08024c32  1944      add          r1, r3
08024c34  d2ed007a  vldr         s15, [r2]
08024c38  41f2a352  movw         r2, #5539
08024c3c  0eeb8000  add.w        r0, lr, r0, lsl #2
08024c40  01ea0a01  and.w        r1, r1, r10
08024c44  c9ed037a  vstr         s15, [r9, #12]
08024c48  1a44      add          r2, r3
08024c4a  d4ed005a  vldr         s11, [r4]
08024c4e  33ee443a  vsub.f32     s6, s6, s8
08024c52  0eeb8101  add.w        r1, lr, r1, lsl #2
08024c56  02ea0a02  and.w        r2, r2, r10
08024c5a  c9ed045a  vstr         s11, [r9, #16]
08024c5e  75ee274a  vadd.f32     s9, s10, s15
08024c62  90ed004a  vldr         s8, [r0]
08024c66  0eeb8202  add.w        r2, lr, r2, lsl #2
08024c6a  75ee677a  vsub.f32     s15, s10, s15
08024c6e  dfed948a  vldr         s17, [pc, #592]  ; [0x08024ec0] = 0xbf2aaaab (f32=-0.666666687)
08024c72  89ed054a  vstr         s8, [r9, #20]
08024c76  35ee84aa  vadd.f32     s20, s11, s8
08024c7a  d1ed002a  vldr         s5, [r1]
08024c7e  35eec44a  vsub.f32     s8, s11, s8
08024c82  3dee241a  vadd.f32     s2, s26, s9
08024c86  9fed8f0a  vldr         s0, [pc, #572]  ; [0x08024ec4] = 0x3eaaaaab (f32=0.333333343)
08024c8a  c9ed062a  vstr         s5, [r9, #24]
08024c8e  0aea0301  and.w        r1, r10, r3
08024c92  d2ed005a  vldr         s11, [r2]
08024c96  03f22f72  addw         r2, r3, #1839
08024c9a  c9ed0a4a  vstr         s9, [r9, #40]
08024c9e  7dee644a  vsub.f32     s9, s26, s9
08024ca2  32eea55a  vadd.f32     s10, s5, s11
08024ca6  c9ed075a  vstr         s11, [r9, #28]
08024caa  c9ed0b7a  vstr         s15, [r9, #44]
08024cae  72eee55a  vsub.f32     s11, s5, s11
08024cb2  73ee272a  vadd.f32     s5, s6, s15
08024cb6  89ed093a  vstr         s6, [r9, #36]
08024cba  3aee058a  vadd.f32     s16, s20, s10
08024cbe  89ed0caa  vstr         s20, [r9, #48]
08024cc2  73ee677a  vsub.f32     s15, s6, s15
08024cc6  89ed0d4a  vstr         s8, [r9, #52]
08024cca  89ed0e5a  vstr         s10, [r9, #56]
08024cce  3aee455a  vsub.f32     s10, s20, s10
08024cd2  31ee083a  vadd.f32     s6, s2, s16
08024cd6  c9ed0f5a  vstr         s11, [r9, #60]
08024cda  89ed08da  vstr         s26, [r9, #32]
08024cde  34ee25da  vadd.f32     s26, s8, s11
08024ce2  89ed101a  vstr         s2, [r9, #64]
08024ce6  31ee481a  vsub.f32     s2, s2, s16
08024cea  c9ed114a  vstr         s9, [r9, #68]
08024cee  74ee655a  vsub.f32     s11, s8, s11
08024cf2  c9ed122a  vstr         s5, [r9, #72]
08024cf6  34ee854a  vadd.f32     s8, s9, s10
08024cfa  c9ed137a  vstr         s15, [r9, #76]
08024cfe  0eeb8101  add.w        r1, lr, r1, lsl #2
08024d02  89ed148a  vstr         s16, [r9, #80]
08024d06  02ea0a02  and.w        r2, r2, r10
08024d0a  97ed008a  vldr         s16, [r7]
08024d0e  89ed155a  vstr         s10, [r9, #84]
08024d12  34eec55a  vsub.f32     s10, s9, s10
08024d16  23ee088a  vmul.f32     s16, s6, s16
08024d1a  6b48      ldr          r0, [pc, #428]  ; [0x08024ec8] = 0x20004d74 (f32=1.08676488e-19)
08024d1c  72ee8d4a  vadd.f32     s9, s5, s26
08024d20  89ed183a  vstr         s6, [r9, #96]
08024d24  90ed003a  vldr         s6, [r0]
08024d28  72eecd2a  vsub.f32     s5, s5, s26
08024d2c  a6ee898a  vfma.f32     s16, s13, s18
08024d30  6648      ldr          r0, [pc, #408]  ; [0x08024ecc] = 0x20002b2c (f32=1.08563061e-19)
08024d32  c9ed194a  vstr         s9, [r9, #100]
08024d36  0eeb8202  add.w        r2, lr, r2, lsl #2
08024d3a  89ed1c1a  vstr         s2, [r9, #112]
08024d3e  37eea51a  vadd.f32     s2, s15, s11
08024d42  89ed1a4a  vstr         s8, [r9, #104]
08024d46  37eee54a  vsub.f32     s8, s15, s11
08024d4a  c9ed175a  vstr         s11, [r9, #92]
08024d4e  109d      ldr          r5, [sp, #64]
08024d50  88fe698a  vminnm.f32   s16, s16, s19
08024d54  88fe288a  vmaxnm.f32   s16, s16, s17
08024d58  28ee08aa  vmul.f32     s20, s16, s16
08024d5c  d0ed005a  vldr         s11, [r0]
08024d60  5b48      ldr          r0, [pc, #364]  ; [0x08024ed0] = 0x20003850 (f32=1.0860654e-19)
08024d62  dfed5c0a  vldr         s1, [pc, #368]  ; [0x08024ed4] = 0xbf7e5477 (f32=-0.993476331)
08024d66  60ee4a4a  vnmul.f32    s9, s0, s20
08024d6a  d0ed007a  vldr         s15, [r0]
08024d6e  89ed1e5a  vstr         s10, [r9, #120]
08024d72  95ed005a  vldr         s10, [r5]
08024d76  e5eee07a  vfms.f32     s15, s11, s1
08024d7a  a4ee888a  vfma.f32     s16, s9, s16
08024d7e  524e      ldr          r6, [pc, #328]  ; [0x08024ec8] = 0x20004d74 (f32=1.08676488e-19)
08024d80  89ed1f4a  vstr         s8, [r9, #124]
08024d84  89ed16da  vstr         s26, [r9, #88]
08024d88  c9ed1d2a  vstr         s5, [r9, #116]
08024d8c  89ed1b1a  vstr         s2, [r9, #108]
08024d90  5148      ldr          r0, [pc, #324]  ; [0x08024ed8] = 0x20001b2c (f32=1.08510121e-19)
08024d92  33ee483a  vsub.f32     s6, s6, s16
08024d96  514c      ldr          r4, [pc, #324]  ; [0x08024edc] = 0x20004c6c (f32=1.08673076e-19)
08024d98  dfed515a  vldr         s11, [pc, #324]  ; [0x08024ee0] = 0x3eb33333 (f32=0.349999994)
08024d9c  a3ee058a  vfma.f32     s16, s6, s10
08024da0  e8ee207a  vfma.f32     s15, s16, s1
08024da4  86ed008a  vstr         s16, [r6]
08024da8  494e      ldr          r6, [pc, #292]  ; [0x08024ed0] = 0x20003850 (f32=1.0860654e-19)
08024daa  86ed008a  vstr         s16, [r6]
08024dae  474e      ldr          r6, [pc, #284]  ; [0x08024ecc] = 0x20002b2c (f32=1.08563061e-19)
08024db0  38ee678a  vsub.f32     s16, s16, s15
08024db4  c6ed007a  vstr         s15, [r6]
08024db8  28ee068a  vmul.f32     s16, s16, s12
08024dbc  81ed008a  vstr         s16, [r1]
08024dc0  d7ed004a  vldr         s9, [r7]
08024dc4  d9ed197a  vldr         s15, [r9, #100]
08024dc8  4649      ldr          r1, [pc, #280]  ; [0x08024ee4] = 0x20002b34 (f32=1.08563164e-19)
08024dca  67eea47a  vmul.f32     s15, s15, s9
08024dce  d5ed004a  vldr         s9, [r5]
08024dd2  91ed005a  vldr         s10, [r1]
08024dd6  4449      ldr          r1, [pc, #272]  ; [0x08024ee8] = 0x20002b4c (f32=1.08563475e-19)
08024dd8  e7ee097a  vfma.f32     s15, s14, s18
08024ddc  91ed003a  vldr         s6, [r1]
08024de0  4249      ldr          r1, [pc, #264]  ; [0x08024eec] = 0x20004c68 (f32=1.08673024e-19)
08024de2  91ed004a  vldr         s8, [r1]
08024de6  3f49      ldr          r1, [pc, #252]  ; [0x08024ee4] = 0x20002b34 (f32=1.08563164e-19)
08024de8  c7fee99a  vminnm.f32   s19, s15, s19
08024dec  c9fea88a  vmaxnm.f32   s17, s19, s17
08024df0  68eea87a  vmul.f32     s15, s17, s17
08024df4  a3ee604a  vfms.f32     s8, s6, s1
08024df8  20ee670a  vnmul.f32    s0, s0, s15
08024dfc  e0ee288a  vfma.f32     s17, s0, s17
08024e00  f0ee447a  vmov.f32     s15, s8
08024e04  35ee685a  vsub.f32     s10, s10, s17
08024e08  e5ee248a  vfma.f32     s17, s10, s9
08024e0c  e8eea07a  vfma.f32     s15, s17, s1
08024e10  c1ed008a  vstr         s17, [r1]
08024e14  3549      ldr          r1, [pc, #212]  ; [0x08024eec] = 0x20004c68 (f32=1.08673024e-19)
08024e16  c1ed008a  vstr         s17, [r1]
08024e1a  3349      ldr          r1, [pc, #204]  ; [0x08024ee8] = 0x20002b4c (f32=1.08563475e-19)
08024e1c  78eee78a  vsub.f32     s17, s17, s15
08024e20  c1ed007a  vstr         s15, [r1]
08024e24  68ee868a  vmul.f32     s17, s17, s12
08024e28  c2ed008a  vstr         s17, [r2]
08024e2c  94ed005a  vldr         s10, [r4]
08024e30  d0ed007a  vldr         s15, [r0]
08024e34  75ee277a  vadd.f32     s15, s10, s15
08024e38  f4eee57a  vcmpe.f32    s15, s11
08024e3c  c0ed007a  vstr         s15, [r0]
08024e40  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024e44  06dc      bgt          #12  ; -> 0x08024e54
08024e46  dfed2a5a  vldr         s11, [pc, #168]  ; [0x08024ef0] = 0x3d4ccccd (f32=0.0500000007)
08024e4a  f4eee57a  vcmpe.f32    s15, s11
08024e4e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024e52  09d5      bpl          #18  ; -> 0x08024e68
08024e54  b1ee455a  vneg.f32     s10, s10
08024e58  f0ee007a  vmov.f32     s15, #2.000000e+00
08024e5c  84ed005a  vstr         s10, [r4]
08024e60  e5ee275a  vfma.f32     s11, s10, s15
08024e64  c0ed005a  vstr         s11, [r0]
08024e68  224a      ldr          r2, [pc, #136]  ; [0x08024ef4] = 0x20003854 (f32=1.08606591e-19)
08024e6a  9fed1d5a  vldr         s10, [pc, #116]  ; [0x08024ee0] = 0x3eb33333 (f32=0.349999994)
08024e6e  d2ed005a  vldr         s11, [r2]
08024e72  214a      ldr          r2, [pc, #132]  ; [0x08024ef8] = 0x20002b58 (f32=1.0856363e-19)
08024e74  1f49      ldr          r1, [pc, #124]  ; [0x08024ef4] = 0x20003854 (f32=1.08606591e-19)
08024e76  d2ed007a  vldr         s15, [r2]
08024e7a  75eea77a  vadd.f32     s15, s11, s15
08024e7e  f4eec57a  vcmpe.f32    s15, s10
08024e82  c2ed007a  vstr         s15, [r2]
08024e86  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024e8a  37dc      bgt          #110  ; -> 0x08024efc
08024e8c  9fed185a  vldr         s10, [pc, #96]  ; [0x08024ef0] = 0x3d4ccccd (f32=0.0500000007)
08024e90  f4eec57a  vcmpe.f32    s15, s10
08024e94  f1ee10fa  vmrs         APSR_nzcv, fpscr
08024e98  3ad5      bpl          #116  ; -> 0x08024f10
08024e9a  2fe0      b            #94  ; -> 0x08024efc
08024efc  f1ee655a  vneg.f32     s11, s11
08024f00  f0ee007a  vmov.f32     s15, #2.000000e+00
08024f04  c1ed005a  vstr         s11, [r1]
08024f08  a5eea75a  vfma.f32     s10, s11, s15
08024f0c  82ed005a  vstr         s10, [r2]
08024f10  d94a      ldr          r2, [pc, #868]  ; [0x08025278] = 0x20002b38 (f32=1.08563216e-19)
08024f12  03f6c200  addw         r0, r3, #2242
08024f16  d9ed1a7a  vldr         s15, [r9, #104]
08024f1a  1468      ldr          r4, [r2]
08024f1c  00ea0a00  and.w        r0, r0, r10
08024f20  d7ed005a  vldr         s11, [r7]
08024f24  d54a      ldr          r2, [pc, #852]  ; [0x0802527c] = 0x20001b2c (f32=1.08510121e-19)
08024f26  0eeb8000  add.w        r0, lr, r0, lsl #2
08024f2a  67eea55a  vmul.f32     s11, s15, s11
08024f2e  07ee904a  vmov         s15, r4
08024f32  9fedd35a  vldr         s10, [pc, #844]  ; [0x08025280] = 0x44724000 (f32=969)
08024f36  f8eee77a  vcvt.f32.s32 s15, s15
08024f3a  c0ed005a  vstr         s11, [r0]
08024f3e  d2ed004a  vldr         s9, [r2]
08024f42  e4ee857a  vfma.f32     s15, s9, s10
08024f46  bdeee75a  vcvt.s32.f32 s10, s15
08024f4a  15ee105a  vmov         r5, s10
08024f4e  f8eec54a  vcvt.f32.s32 s9, s10
08024f52  b5f5727f  cmp.w        r5, #968
08024f56  05f10102  add.w        r2, r5, #1
08024f5a  77eee44a  vsub.f32     s9, s15, s9
08024f5e  40f3fe81  ble.w        #1020  ; -> 0x0802535e
08024f62  6ff47271  mvn          r1, #968
08024f66  6918      adds         r1, r5, r1
08024f68  b2f5727f  cmp.w        r2, #968
08024f6c  c24e      ldr          r6, [pc, #776]  ; [0x08025278] = 0x20002b38 (f32=1.08563216e-19)
08024f6e  9fedc53a  vldr         s6, [pc, #788]  ; [0x08025284] = 0x4448c000 (f32=803)
08024f72  c8bf      it           gt
08024f74  a5f57272  subgt.w      r2, r5, #968
08024f78  c34d      ldr          r5, [pc, #780]  ; [0x08025288] = 0x20001b6c (f32=1.08510949e-19)
08024f7a  b4f5727f  cmp.w        r4, #968
08024f7e  05eb8101  add.w        r1, r5, r1, lsl #2
08024f82  05eb8202  add.w        r2, r5, r2, lsl #2
08024f86  04f10105  add.w        r5, r4, #1
08024f8a  d1ed007a  vldr         s15, [r1]
08024f8e  08bf      it           eq
08024f90  0025      moveq        r5, #0
08024f92  92ed004a  vldr         s8, [r2]
08024f96  03f66722  addw         r2, r3, #2663
08024f9a  bb49      ldr          r1, [pc, #748]  ; [0x08025288] = 0x20001b6c (f32=1.08510949e-19)
08024f9c  34ee674a  vsub.f32     s8, s8, s15
08024fa0  3560      str          r5, [r6]
08024fa2  01eb8404  add.w        r4, r1, r4, lsl #2
08024fa6  b949      ldr          r1, [pc, #740]  ; [0x0802528c] = 0x20001b50 (f32=1.08510587e-19)
08024fa8  02ea0a02  and.w        r2, r2, r10
08024fac  40f22235  movw         r5, #802
08024fb0  e4ee847a  vfma.f32     s15, s9, s8
08024fb4  0968      ldr          r1, [r1]
08024fb6  0eeb8202  add.w        r2, lr, r2, lsl #2
08024fba  05ee101a  vmov         s10, r1
08024fbe  b8eec55a  vcvt.f32.s32 s10, s10
08024fc2  e7eea15a  vfma.f32     s11, s15, s3
08024fc6  e5eee17a  vfms.f32     s15, s11, s3
08024fca  c4ed005a  vstr         s11, [r4]
08024fce  c0ed007a  vstr         s15, [r0]
08024fd2  d7ed004a  vldr         s9, [r7]
08024fd6  d9ed1b7a  vldr         s15, [r9, #108]
08024fda  ad48      ldr          r0, [pc, #692]  ; [0x08025290] = 0x20002b58 (f32=1.0856363e-19)
08024fdc  67eea44a  vmul.f32     s9, s15, s9
08024fe0  c2ed004a  vstr         s9, [r2]
08024fe4  d0ed007a  vldr         s15, [r0]
08024fe8  a7ee835a  vfma.f32     s10, s15, s6
08024fec  b7ee083a  vmov.f32     s6, #1.500000e+00
08024ff0  fdeec55a  vcvt.s32.f32 s11, s10
08024ff4  15ee906a  vmov         r6, s11
08024ff8  b8eee54a  vcvt.f32.s32 s8, s11
08024ffc  ffee005a  vmov.f32     s11, #-1.000000e+00
08025000  ae42      cmp          r6, r5
08025002  06f10100  add.w        r0, r6, #1
08025006  40f22235  movw         r5, #802
0802500a  35ee444a  vsub.f32     s8, s10, s8
0802500e  ccbf      ite          gt
08025010  a6f22334  subwgt       r4, r6, #803
08025014  3446      movle        r4, r6
08025016  a842      cmp          r0, r5
08025018  9e4d      ldr          r5, [pc, #632]  ; [0x08025294] = 0x20002b64 (f32=1.08563785e-19)
0802501a  b7ee005a  vmov.f32     s10, #1.000000e+00
0802501e  c8bf      it           gt
08025020  a6f22230  subwgt       r0, r6, #802
08025024  05eb8404  add.w        r4, r5, r4, lsl #2
08025028  03f63946  addw         r6, r3, #3129
0802502c  05eb8000  add.w        r0, r5, r0, lsl #2
08025030  d4ed007a  vldr         s15, [r4]
08025034  4c1c      adds         r4, r1, #1
08025036  05eb8101  add.w        r1, r5, r1, lsl #2
0802503a  d0ed002a  vldr         s5, [r0]
0802503e  40f22330  movw         r0, #803
08025042  924d      ldr          r5, [pc, #584]  ; [0x0802528c] = 0x20001b50 (f32=1.08510587e-19)
08025044  06ea0a06  and.w        r6, r6, r10
08025048  72eee72a  vsub.f32     s5, s5, s15
0802504c  8442      cmp          r4, r0
0802504e  14bf      ite          ne
08025050  2046      movne        r0, r4
08025052  0020      moveq        r0, #0
08025054  0a9c      ldr          r4, [sp, #40]
08025056  0eeb8606  add.w        r6, lr, r6, lsl #2
0802505a  2860      str          r0, [r5]
0802505c  03f66965  addw         r5, r3, #3689
08025060  e4ee227a  vfma.f32     s15, s8, s5
08025064  0698      ldr          r0, [sp, #24]
08025066  0b91      str          r1, [sp, #44]
08025068  a400      lsls         r4, r4, #2
0802506a  0146      mov          r1, r0
0802506c  0998      ldr          r0, [sp, #36]
0802506e  0694      str          r4, [sp, #24]
08025070  05ea0a05  and.w        r5, r5, r10
08025074  2144      add          r1, r4
08025076  139c      ldr          r4, [sp, #76]
08025078  0eeb8505  add.w        r5, lr, r5, lsl #2
0802507c  e7eea14a  vfma.f32     s9, s15, s3
08025080  0c91      str          r1, [sp, #48]
08025082  2146      mov          r1, r4
08025084  6144      add          r1, r12
08025086  0a91      str          r1, [sp, #40]
08025088  0146      mov          r1, r0
0802508a  e4eee17a  vfms.f32     s15, s9, s3
0802508e  0198      ldr          r0, [sp, #4]
08025090  2144      add          r1, r4
08025092  814c      ldr          r4, [pc, #516]  ; [0x08025298] = 0x200038e4 (f32=1.08608453e-19)
08025094  0130      adds         r0, #1
08025096  0991      str          r1, [sp, #36]
08025098  0b99      ldr          r1, [sp, #44]
0802509a  2468      ldr          r4, [r4]
0802509c  c1ed004a  vstr         s9, [r1]
080250a0  7e49      ldr          r1, [pc, #504]  ; [0x0802529c] = 0x20004c58 (f32=1.08672818e-19)
080250a2  0190      str          r0, [sp, #4]
080250a4  0968      ldr          r1, [r1]
080250a6  c2ed007a  vstr         s15, [r2]
080250aa  d7ed004a  vldr         s9, [r7]
080250ae  d9ed1c7a  vldr         s15, [r9, #112]
080250b2  7b48      ldr          r0, [pc, #492]  ; [0x080252a0] = 0x200038e8 (f32=1.08608504e-19)
080250b4  67eea47a  vmul.f32     s15, s15, s9
080250b8  7a4a      ldr          r2, [pc, #488]  ; [0x080252a4] = 0x20000278 (f32=1.08428386e-19)
080250ba  00eb840c  add.w        r12, r0, r4, lsl #2
080250be  40f2d440  movw         r0, #1236
080250c2  0134      adds         r4, #1
080250c4  02eb8102  add.w        r2, r2, r1, lsl #2
080250c8  c6ed007a  vstr         s15, [r6]
080250cc  0131      adds         r1, #1
080250ce  dced007a  vldr         s15, [r12]
080250d2  8442      cmp          r4, r0
080250d4  08bf      it           eq
080250d6  0024      moveq        r4, #0
080250d8  d9ed1d2a  vldr         s5, [r9, #116]
080250dc  41f29c00  movw         r0, #4252
080250e0  67ee8b4a  vmul.f32     s9, s15, s22
080250e4  97ed004a  vldr         s8, [r7]
080250e8  6b4e      ldr          r6, [pc, #428]  ; [0x08025298] = 0x200038e4 (f32=1.08608453e-19)
080250ea  1844      add          r0, r3
080250ec  e2ee844a  vfma.f32     s9, s5, s8
080250f0  3460      str          r4, [r6]
080250f2  00ea0a00  and.w        r0, r0, r10
080250f6  41f2fe24  movw         r4, #4862
080250fa  0eeb8000  add.w        r0, lr, r0, lsl #2
080250fe  1c44      add          r4, r3
08025100  40f2e753  movw         r3, #1511
08025104  04ea0a04  and.w        r4, r4, r10
08025108  e4eecb7a  vfms.f32     s15, s9, s22
0802510c  cced004a  vstr         s9, [r12]
08025110  9942      cmp          r1, r3
08025112  08bf      it           eq
08025114  0021      moveq        r1, #0
08025116  0eeb8404  add.w        r4, lr, r4, lsl #2
0802511a  dbf82430  ldr.w        r3, [r11, #36]
0802511e  c5ed007a  vstr         s15, [r5]
08025122  d7ed004a  vldr         s9, [r7]
08025126  d9ed1e7a  vldr         s15, [r9, #120]
0802512a  5c4d      ldr          r5, [pc, #368]  ; [0x0802529c] = 0x20004c58 (f32=1.08672818e-19)
0802512c  67eea47a  vmul.f32     s15, s15, s9
08025130  c0ed007a  vstr         s15, [r0]
08025134  d2ed007a  vldr         s15, [r2]
08025138  d9ed1f2a  vldr         s5, [r9, #124]
0802513c  67ee8b4a  vmul.f32     s9, s15, s22
08025140  97ed004a  vldr         s8, [r7]
08025144  2960      str          r1, [r5]
08025146  5849      ldr          r1, [pc, #352]  ; [0x080252a8] = 0x20002ab0 (f32=1.08561458e-19)
08025148  e2ee844a  vfma.f32     s9, s5, s8
0802514c  e4eecb7a  vfms.f32     s15, s9, s22
08025150  c2ed004a  vstr         s9, [r2]
08025154  c4ed007a  vstr         s15, [r4]
08025158  d9ed1a2a  vldr         s5, [r9, #104]
0802515c  99ed184a  vldr         s8, [r9, #96]
08025160  d9ed197a  vldr         s15, [r9, #100]
08025164  34ee224a  vadd.f32     s8, s8, s5
08025168  d9ed1b4a  vldr         s9, [r9, #108]
0802516c  d9ed1c2a  vldr         s5, [r9, #112]
08025170  069c      ldr          r4, [sp, #24]
08025172  77eea44a  vadd.f32     s9, s15, s9
08025176  34ee224a  vadd.f32     s8, s8, s5
0802517a  0a68      ldr          r2, [r1]
0802517c  d9ed1d7a  vldr         s15, [r9, #116]
08025180  2344      add          r3, r4
08025182  99ed1e1a  vldr         s2, [r9, #120]
08025186  4949      ldr          r1, [pc, #292]  ; [0x080252ac] = 0x20001ad8 (f32=1.08509036e-19)
08025188  74eea77a  vadd.f32     s15, s9, s15
0802518c  d9ed1f2a  vldr         s5, [r9, #124]
08025190  74ee014a  vadd.f32     s9, s8, s2
08025194  1a60      str          r2, [r3]
08025196  0c9a      ldr          r2, [sp, #48]
08025198  77eea27a  vadd.f32     s15, s15, s5
0802519c  0b68      ldr          r3, [r1]
0802519e  74eee64a  vsub.f32     s9, s9, s13
080251a2  1360      str          r3, [r2]
080251a4  77eec77a  vsub.f32     s15, s15, s14
080251a8  414a      ldr          r2, [pc, #260]  ; [0x080252b0] = 0x20003888 (f32=1.08607263e-19)
080251aa  d2ed002a  vldr         s5, [r2]
080251ae  414a      ldr          r2, [pc, #260]  ; [0x080252b4] = 0x20003874 (f32=1.08607005e-19)
080251b0  e4eea26a  vfma.f32     s13, s9, s5
080251b4  a2eea77a  vfma.f32     s14, s5, s15
080251b8  92ed004a  vldr         s8, [r2]
080251bc  0a9b      ldr          r3, [sp, #40]
080251be  74ee042a  vadd.f32     s5, s8, s8
080251c2  0198      ldr          r0, [sp, #4]
080251c4  24ee044a  vmul.f32     s8, s8, s8
080251c8  099c      ldr          r4, [sp, #36]
080251ca  c6fec56a  vminnm.f32   s13, s13, s10
080251ce  c6fea56a  vmaxnm.f32   s13, s13, s11
080251d2  66eea64a  vmul.f32     s9, s13, s13
080251d6  87fe457a  vminnm.f32   s14, s14, s10
080251da  87fe257a  vmaxnm.f32   s14, s14, s11
080251de  67ee077a  vmul.f32     s15, s14, s14
080251e2  64ee864a  vmul.f32     s9, s9, s12
080251e6  67ee867a  vmul.f32     s15, s15, s12
080251ea  66eee44a  vnmul.f32    s9, s13, s9
080251ee  67ee677a  vnmul.f32    s15, s14, s15
080251f2  e6ee834a  vfma.f32     s9, s13, s6
080251f6  e7ee037a  vfma.f32     s15, s14, s6
080251fa  b0ee637a  vmov.f32     s14, s7
080251fe  f0ee646a  vmov.f32     s13, s9
08025202  f0ee424a  vmov.f32     s9, s4
08025206  a7eea27a  vfma.f32     s14, s15, s5
0802520a  32ee262a  vadd.f32     s4, s4, s13
0802520e  e6eea24a  vfma.f32     s9, s13, s5
08025212  73eea73a  vadd.f32     s7, s7, s15
08025216  f0ee477a  vmov.f32     s15, s14
0802521a  f0ee646a  vmov.f32     s13, s9
0802521e  e3eec47a  vfms.f32     s15, s7, s8
08025222  e2ee446a  vfms.f32     s13, s4, s8
08025226  86fec57a  vminnm.f32   s14, s13, s10
0802522a  87fe257a  vmaxnm.f32   s14, s14, s11
0802522e  83ed007a  vstr         s14, [r3]
08025232  87fec55a  vminnm.f32   s10, s15, s10
08025236  119b      ldr          r3, [sp, #68]
08025238  c5fe255a  vmaxnm.f32   s11, s10, s11
0802523c  c4ed005a  vstr         s11, [r4]
08025240  8342      cmp          r3, r0
08025242  7ef45cae  bne.w        #-4936  ; -> 0x08023efe
08025246  1c4b      ldr          r3, [pc, #112]  ; [0x080252b8] = 0x20004de0 (f32=1.08677884e-19)
08025248  0021      movs         r1, #0
0802524a  1c48      ldr          r0, [pc, #112]  ; [0x080252bc] = 0x20002ab8 (f32=1.08561562e-19)
0802524c  db69      ldr          r3, [r3, #28]
0802524e  1c4a      ldr          r2, [pc, #112]  ; [0x080252c0] = 0xe0001000 (f32=-3.69115025e+19)
08025250  0160      str          r1, [r0]
08025252  c3ebc313  rsb          r3, r3, r3, lsl #7
08025256  5268      ldr          r2, [r2, #4]
08025258  1749      ldr          r1, [pc, #92]  ; [0x080252b8] = 0x20004de0 (f32=1.08677884e-19)
0802525a  1344      add          r3, r2
0802525c  1648      ldr          r0, [pc, #88]  ; [0x080252b8] = 0x20004de0 (f32=1.08677884e-19)
0802525e  4968      ldr          r1, [r1, #4]
08025260  db09      lsrs         r3, r3, #7
08025262  0260      str          r2, [r0]
08025264  8a42      cmp          r2, r1
08025266  c361      str          r3, [r0, #28]
08025268  00d9      bls          #0  ; -> 0x0802526c
0802526a  4260      str          r2, [r0, #4]
0802526c  23b0      add          sp, #140
0802526e  bdec108b  vpop         {d8, d9, d10, d11, d12, d13, d14, d15}
08025272  bde8f08f  pop.w        {r4, r5, r6, r7, r8, r9, r10, r11, pc}
080252c4  0128      cmp          r0, #1
080252c6  7ef4f3ab  bne.w        #-6170  ; -> 0x08023ab0
080252ca  9d4d      ldr          r5, [pc, #628]  ; [0x08025540] = 0x20002b5c (f32=1.08563681e-19)
080252cc  2968      ldr          r1, [r5]
080252ce  0029      cmp          r1, #0
080252d0  7ef4eeab  bne.w        #-6180  ; -> 0x08023ab0
080252d4  9b4e      ldr          r6, [pc, #620]  ; [0x08025544] = 0x20001a14 (f32=1.08506503e-19)
080252d6  2860      str          r0, [r5]
080252d8  3168      ldr          r1, [r6]
080252da  0129      cmp          r1, #1
080252dc  4ff08041  mov.w        r1, #1073741824
080252e0  01f0d481  beq.w        #5032  ; -> 0x0802668c
080252e4  3166      str          r1, [r6, #96]
080252e6  c6f88c10  str.w        r1, [r6, #140]
080252ea  00f09dbf  b.w          #3898  ; -> 0x08026228
080252ee  4ff0080e  mov.w        lr, #8
080252f2  fef7c2bb  b.w          #-6268  ; -> 0x08023a7a
080252f6  944d      ldr          r5, [pc, #592]  ; [0x08025548] = 0x20001b54 (f32=1.08510638e-19)
080252f8  2968      ldr          r1, [r5]
080252fa  0029      cmp          r1, #0
080252fc  00f09c87  beq.w        #3896  ; -> 0x08026238
08025300  9249      ldr          r1, [pc, #584]  ; [0x0802554c] = 0x200055c8 (f32=1.08704044e-19)
08025302  0968      ldr          r1, [r1]
08025304  0029      cmp          r1, #0
08025306  3ef7e5ab  bgt.w        #-6198  ; -> 0x08023ad4
0802530a  914a      ldr          r2, [pc, #580]  ; [0x08025550] = 0x20000274 (f32=1.08428334e-19)
0802530c  0021      movs         r1, #0
0802530e  1160      str          r1, [r2]
08025310  fef7f2bb  b.w          #-6172  ; -> 0x08023af8
08025314  f4eee67a  vcmpe.f32    s15, s13
08025318  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802531c  40f14d84  bpl.w        #2202  ; -> 0x08025bba
08025320  dfed8c7a  vldr         s15, [pc, #560]  ; [0x08025554] = 0x49f4e69c (f32=2006227.5)
08025324  009b      ldr          r3, [sp]
08025326  b4eee77a  vcmpe.f32    s14, s15
0802532a  c3ed006a  vstr         s13, [r3]
0802532e  f0ee667a  vmov.f32     s15, s13
08025332  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025336  7ef77caf  ble.w        #-4360  ; -> 0x08024232
0802533a  874b      ldr          r3, [pc, #540]  ; [0x08025558] = 0x49f4e69c (f32=2006227.5)
0802533c  874a      ldr          r2, [pc, #540]  ; [0x0802555c] = 0x20001ad0 (f32=1.08508932e-19)
0802533e  1360      str          r3, [r2]
08025340  9fed847a  vldr         s14, [pc, #528]  ; [0x08025554] = 0x49f4e69c (f32=2006227.5)
08025344  f4eec77a  vcmpe.f32    s15, s14
08025348  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802534c  7ef771af  ble.w        #-4382  ; -> 0x08024232
08025350  009b      ldr          r3, [sp]
08025352  f0ee477a  vmov.f32     s15, s14
08025356  83ed007a  vstr         s14, [r3]
0802535a  fef76abf  b.w          #-4396  ; -> 0x08024232
0802535e  15ee101a  vmov         r1, s10
08025362  01e6      b            #-1022  ; -> 0x08024f68
08025364  dfed7e5a  vldr         s11, [pc, #504]  ; [0x08025560] = 0x3f7fff58 (f32=0.999989986)
08025368  fff7c9bb  b.w          #-2158  ; -> 0x08024afe
0802536c  dfed7d9a  vldr         s19, [pc, #500]  ; [0x08025564] = 0xbb3a2e8c (f32=-0.00284090918)
08025370  65eea99a  vmul.f32     s19, s11, s19
08025374  fff707bb  b.w          #-2546  ; -> 0x08024986
08025378  dfed7b7a  vldr         s15, [pc, #492]  ; [0x08025568] = 0x3f7ff972 (f32=0.999899983)
0802537c  64ee277a  vmul.f32     s15, s8, s15
08025380  fff7d4bb  b.w          #-2136  ; -> 0x08024b2c
08025384  dfed785a  vldr         s11, [pc, #480]  ; [0x08025568] = 0x3f7ff972 (f32=0.999899983)
08025388  24ee254a  vmul.f32     s8, s8, s11
0802538c  fff776ba  b.w          #-2836  ; -> 0x0802487c
08025390  0e9b      ldr          r3, [sp, #56]
08025392  1b68      ldr          r3, [r3]
08025394  012b      cmp          r3, #1
08025396  754b      ldr          r3, [pc, #468]  ; [0x0802556c] = 0x20001b58 (f32=1.0851069e-19)
08025398  d3ed007a  vldr         s15, [r3]
0802539c  00f0e684  beq.w        #2508  ; -> 0x08025d6c
080253a0  67ee275a  vmul.f32     s11, s14, s15
080253a4  72ee234a  vadd.f32     s9, s4, s7
080253a8  67eea67a  vmul.f32     s15, s15, s13
080253ac  e4ee865a  vfma.f32     s11, s9, s12
080253b0  fff747b9  b.w          #-3442  ; -> 0x08024642
080253b4  dfed6a4a  vldr         s9, [pc, #424]  ; [0x08025560] = 0x3f7fff58 (f32=0.999989986)
080253b8  fff74bba  b.w          #-2922  ; -> 0x08024852
080253bc  0b46      mov          r3, r1
080253be  fef7e3bd  b.w          #-5178  ; -> 0x08023f88
080253c2  b3f1ff4f  cmp.w        r3, #2139095040
080253c6  dfed6a6a  vldr         s13, [pc, #424]  ; [0x08025570] = 0x00000000 (f32=0)
080253ca  40f21384  bls.w        #2086  ; -> 0x08025bf4
080253ce  694b      ldr          r3, [pc, #420]  ; [0x08025574] = 0x20001b68 (f32=1.08510897e-19)
080253d0  9fed677a  vldr         s14, [pc, #412]  ; [0x08025570] = 0x00000000 (f32=0)
080253d4  c3ed006a  vstr         s13, [r3]
080253d8  fff71eb9  b.w          #-3524  ; -> 0x08024618
080253dc  9fed610a  vldr         s0, [pc, #388]  ; [0x08025564] = 0xbb3a2e8c (f32=-0.00284090918)
080253e0  24ee800a  vmul.f32     s0, s9, s0
080253e4  fff7a9b9  b.w          #-3246  ; -> 0x0802473a
080253e8  63ee226a  vmul.f32     s13, s6, s5
080253ec  5b4a      ldr          r2, [pc, #364]  ; [0x0802555c] = 0x20001ad0 (f32=1.08508932e-19)
080253ee  26ee877a  vmul.f32     s14, s13, s14
080253f2  82ed007a  vstr         s14, [r2]
080253f6  fef700bf  b.w          #-4608  ; -> 0x080241fa
080253fa  5f48      ldr          r0, [pc, #380]  ; [0x08025578] = 0x20002aa8 (f32=1.08561355e-19)
080253fc  5f4a      ldr          r2, [pc, #380]  ; [0x0802557c] = 0x20004c70 (f32=1.08673128e-19)
080253fe  d0ed002a  vldr         s5, [r0]
08025402  d2ed007a  vldr         s15, [r2]
08025406  5e4b      ldr          r3, [pc, #376]  ; [0x08025580] = 0x20004dcc (f32=1.08677626e-19)
08025408  72ee8c2a  vadd.f32     s5, s5, s24
0802540c  77ee8c7a  vadd.f32     s15, s15, s24
08025410  5b4c      ldr          r4, [pc, #364]  ; [0x08025580] = 0x20004dcc (f32=1.08677626e-19)
08025412  1b68      ldr          r3, [r3]
08025414  c0ed002a  vstr         s5, [r0]
08025418  c2ed007a  vstr         s15, [r2]
0802541c  002b      cmp          r3, #0
0802541e  7ef4d3ae  bne.w        #-4698  ; -> 0x080241c8
08025422  4ff0000e  mov.w        lr, #0
08025426  0123      movs         r3, #1
08025428  c2f800e0  str.w        lr, [r2]
0802542c  039a      ldr          r2, [sp, #12]
0802542e  c0f800e0  str.w        lr, [r0]
08025432  1360      str          r3, [r2]
08025434  029a      ldr          r2, [sp, #8]
08025436  2360      str          r3, [r4]
08025438  1360      str          r3, [r2]
0802543a  fef7c5be  b.w          #-4726  ; -> 0x080241c8
0802543e  039b      ldr          r3, [sp, #12]
08025440  1a68      ldr          r2, [r3]
08025442  012a      cmp          r2, #1
08025444  00f0a684  beq.w        #2380  ; -> 0x08025d94
08025448  089b      ldr          r3, [sp, #32]
0802544a  189a      ldr          r2, [sp, #96]
0802544c  93ed008a  vldr         s16, [r3]
08025450  029b      ldr          r3, [sp, #8]
08025452  82ed008a  vstr         s16, [r2]
08025456  1b68      ldr          r3, [r3]
08025458  012b      cmp          r3, #1
0802545a  0e9b      ldr          r3, [sp, #56]
0802545c  1968      ldr          r1, [r3]
0802545e  00f0ed84  beq.w        #2522  ; -> 0x08025e3c
08025462  b0ee443a  vmov.f32     s6, s8
08025466  1a9b      ldr          r3, [sp, #104]
08025468  83ed004a  vstr         s8, [r3]
0802546c  dfed406a  vldr         s13, [pc, #256]  ; [0x08025570] = 0x00000000 (f32=0)
08025470  0022      movs         r2, #0
08025472  444b      ldr          r3, [pc, #272]  ; [0x08025584] = 0x20003864 (f32=1.08606798e-19)
08025474  b0ee664a  vmov.f32     s8, s13
08025478  1a60      str          r2, [r3]
0802547a  fef7b4bd  b.w          #-5272  ; -> 0x08023fe6
0802547e  002b      cmp          r3, #0
08025480  41f0f781  bne.w        #5102  ; -> 0x08026872
08025484  f5eec04a  vcmpe.f32    s9, #0
08025488  dfed3f7a  vldr         s15, [pc, #252]  ; [0x08025588] = 0x46000000 (f32=8192)
0802548c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025490  4cbf      ite          mi
08025492  3e4b      ldrmi        r3, [pc, #248]  ; [0x0802558c] = 0x20000270 (f32=1.08428282e-19)
08025494  3e4b      ldrpl        r3, [pc, #248]  ; [0x08025590] = 0x20002a98 (f32=1.08561148e-19)
08025496  93ed007a  vldr         s14, [r3]
0802549a  e4ee877a  vfma.f32     s15, s9, s14
0802549e  fdeee77a  vcvt.s32.f32 s15, s15
080254a2  17ee902a  vmov         r2, s15
080254a6  b2f5006f  cmp.w        r2, #2048
080254aa  c0f2a083  blt.w        #1856  ; -> 0x08025bee
080254ae  43f2ff74  movw         r4, #14335
080254b2  3848      ldr          r0, [pc, #224]  ; [0x08025594] = 0x08028dc4 (f32=3.92870967e-34)
080254b4  0123      movs         r3, #1
080254b6  9fed385a  vldr         s10, [pc, #224]  ; [0x08025598] = 0x3d800000 (f32=0.0625)
080254ba  a242      cmp          r2, r4
080254bc  a8bf      it           ge
080254be  2246      movge        r2, r4
080254c0  c2f30a04  ubfx         r4, r2, #0, #11
080254c4  d212      asrs         r2, r2, #11
080254c6  00eb8400  add.w        r0, r0, r4, lsl #2
080254ca  03fa02f2  lsl.w        r2, r3, r2
080254ce  90ed007a  vldr         s14, [r0]
080254d2  07ee902a  vmov         s15, r2
080254d6  27ee057a  vmul.f32     s14, s14, s10
080254da  f8eee77a  vcvt.f32.s32 s15, s15
080254de  27ee877a  vmul.f32     s14, s15, s14
080254e2  9fed234a  vldr         s8, [pc, #140]  ; [0x08025570] = 0x00000000 (f32=0)
080254e6  b7ee085a  vmov.f32     s10, #1.500000e+00
080254ea  f0ee444a  vmov.f32     s9, s8
080254ee  fef750be  b.w          #-4960  ; -> 0x08024192
080254f2  2a4a      ldr          r2, [pc, #168]  ; [0x0802559c] = 0x20004ddc (f32=1.08677832e-19)
080254f4  2a4b      ldr          r3, [pc, #168]  ; [0x080255a0] = 0x20003838 (f32=1.08606229e-19)
080254f6  92ed008a  vldr         s16, [r2]
080254fa  089a      ldr          r2, [sp, #32]
080254fc  93ed003a  vldr         s6, [r3]
08025500  189b      ldr          r3, [sp, #96]
08025502  d2ed006a  vldr         s13, [r2]
08025506  34ee434a  vsub.f32     s8, s8, s6
0802550a  83ed008a  vstr         s16, [r3]
0802550e  1a9b      ldr          r3, [sp, #104]
08025510  76eec86a  vsub.f32     s13, s13, s16
08025514  24ee0e4a  vmul.f32     s8, s8, s28
08025518  83ed003a  vstr         s6, [r3]
0802551c  0e9b      ldr          r3, [sp, #56]
0802551e  66ee8e6a  vmul.f32     s13, s13, s28
08025522  1968      ldr          r1, [r3]
08025524  fef75fbd  b.w          #-5442  ; -> 0x08023fe6
08025528  d0ed014a  vldr         s9, [r0, #4]
0802552c  34ee247a  vadd.f32     s14, s8, s9
08025530  fef7bfbf  b.w          #-4226  ; -> 0x080244b2
08025534  d0ed017a  vldr         s15, [r0, #4]
08025538  34eea77a  vadd.f32     s14, s9, s15
0802553c  fef74ebf  b.w          #-4452  ; -> 0x080243dc
080255a4  dbf81030  ldr.w        r3, [r11, #16]
080255a8  e849      ldr          r1, [pc, #928]  ; [0x0802594c] = 0x20001a14 (f32=1.08506503e-19)
080255aa  002b      cmp          r3, #0
080255ac  00f0d983  beq.w        #1970  ; -> 0x08025d62
080255b0  d1ed027a  vldr         s15, [r1, #8]
080255b4  74eea77a  vadd.f32     s15, s9, s15
080255b8  e54b      ldr          r3, [pc, #916]  ; [0x08025950] = 0x20004c60 (f32=1.08672921e-19)
080255ba  b0ee007a  vmov.f32     s14, #2.000000e+00
080255be  dbf82400  ldr.w        r0, [r11, #36]
080255c2  b7ee081a  vmov.f32     s2, #1.500000e+00
080255c6  1b68      ldr          r3, [r3]
080255c8  c7fe877a  vmaxnm.f32   s15, s15, s14
080255cc  dfede12a  vldr         s5, [pc, #900]  ; [0x08025954] = 0x42c80000 (f32=100)
080255d0  1946      mov          r1, r3
080255d2  0a93      str          r3, [sp, #40]
080255d4  e04b      ldr          r3, [pc, #896]  ; [0x08025958] = 0x20001b20 (f32=1.08509966e-19)
080255d6  06ee901a  vmov         s13, r1
080255da  dbed085a  vldr         s11, [r11, #32]
080255de  1b68      ldr          r3, [r3]
080255e0  b8eee63a  vcvt.f32.s32 s6, s13
080255e4  d94c      ldr          r4, [pc, #868]  ; [0x0802594c] = 0x20001a14 (f32=1.08506503e-19)
080255e6  77ee837a  vadd.f32     s15, s15, s6
080255ea  bdeee77a  vcvt.s32.f32 s14, s15
080255ee  cbed037a  vstr         s15, [r11, #12]
080255f2  17ee101a  vmov         r1, s14
080255f6  b8eec77a  vcvt.f32.s32 s14, s14
080255fa  1940      ands         r1, r3
080255fc  37eec70a  vsub.f32     s0, s15, s14
08025600  4e1e      subs         r6, r1, #1
08025602  8d1c      adds         r5, r1, #2
08025604  cbf81410  str.w        r1, [r11, #20]
08025608  1e40      ands         r6, r3
0802560a  1d40      ands         r5, r3
0802560c  8bed070a  vstr         s0, [r11, #28]
08025610  20ee068a  vmul.f32     s16, s0, s12
08025614  00eb8505  add.w        r5, r0, r5, lsl #2
08025618  00eb8606  add.w        r6, r0, r6, lsl #2
0802561c  d5ed007a  vldr         s15, [r5]
08025620  4d1c      adds         r5, r1, #1
08025622  96ed007a  vldr         s14, [r6]
08025626  00eb8101  add.w        r1, r0, r1, lsl #2
0802562a  1d40      ands         r5, r3
0802562c  37ee277a  vadd.f32     s14, s14, s15
08025630  d1ed006a  vldr         s13, [r1]
08025634  00eb8505  add.w        r5, r0, r5, lsl #2
08025638  77ee667a  vsub.f32     s15, s14, s13
0802563c  d5ed000a  vldr         s1, [r5]
08025640  36ee877a  vadd.f32     s14, s13, s14
08025644  77eee07a  vsub.f32     s15, s15, s1
08025648  67ee887a  vmul.f32     s15, s15, s16
0802564c  e7ee467a  vfms.f32     s15, s14, s12
08025650  e0ee817a  vfma.f32     s15, s1, s2
08025654  e0ee276a  vfma.f32     s13, s0, s15
08025658  16ee901a  vmov         r1, s13
0802565c  21f00041  bic          r1, r1, #2147483648
08025660  b1f1ff4f  cmp.w        r1, #2139095040
08025664  dbf86810  ldr.w        r1, [r11, #104]
08025668  88bf      it           hi
0802566a  f0ee626a  vmovhi.f32   s13, s5
0802566e  65eea66a  vmul.f32     s13, s11, s13
08025672  0029      cmp          r1, #0
08025674  00f07083  beq.w        #1760  ; -> 0x08025d58
08025678  d4ed187a  vldr         s15, [r4, #96]
0802567c  34eea77a  vadd.f32     s14, s9, s15
08025680  f0ee007a  vmov.f32     s15, #2.000000e+00
08025684  dbf87c00  ldr.w        r0, [r11, #124]
08025688  f7ee082a  vmov.f32     s5, #1.500000e+00
0802568c  0024      movs         r4, #0
0802568e  dfedb14a  vldr         s9, [pc, #708]  ; [0x08025954] = 0x42c80000 (f32=100)
08025692  c7fe277a  vmaxnm.f32   s15, s14, s15
08025696  77ee837a  vadd.f32     s15, s15, s6
0802569a  9bed1e8a  vldr         s16, [r11, #120]
0802569e  bdeee77a  vcvt.s32.f32 s14, s15
080256a2  cbed197a  vstr         s15, [r11, #100]
080256a6  17ee101a  vmov         r1, s14
080256aa  b8eec77a  vcvt.f32.s32 s14, s14
080256ae  1940      ands         r1, r3
080256b0  37eec71a  vsub.f32     s2, s15, s14
080256b4  4e1e      subs         r6, r1, #1
080256b6  8d1c      adds         r5, r1, #2
080256b8  cbf86c10  str.w        r1, [r11, #108]
080256bc  1e40      ands         r6, r3
080256be  1d40      ands         r5, r3
080256c0  8bed1d1a  vstr         s2, [r11, #116]
080256c4  61ee068a  vmul.f32     s17, s2, s12
080256c8  00eb8505  add.w        r5, r0, r5, lsl #2
080256cc  00eb8606  add.w        r6, r0, r6, lsl #2
080256d0  d5ed007a  vldr         s15, [r5]
080256d4  4d1c      adds         r5, r1, #1
080256d6  96ed007a  vldr         s14, [r6]
080256da  00eb8101  add.w        r1, r0, r1, lsl #2
080256de  1d40      ands         r5, r3
080256e0  37ee277a  vadd.f32     s14, s14, s15
080256e4  d1ed000a  vldr         s1, [r1]
080256e8  00eb8505  add.w        r5, r0, r5, lsl #2
080256ec  0299      ldr          r1, [sp, #8]
080256ee  77ee607a  vsub.f32     s15, s14, s1
080256f2  95ed000a  vldr         s0, [r5]
080256f6  30ee877a  vadd.f32     s14, s1, s14
080256fa  0c60      str          r4, [r1]
080256fc  77eec07a  vsub.f32     s15, s15, s0
08025700  67eea87a  vmul.f32     s15, s15, s17
08025704  e7ee467a  vfms.f32     s15, s14, s12
08025708  e0ee227a  vfma.f32     s15, s0, s5
0802570c  e1ee270a  vfma.f32     s1, s2, s15
08025710  10ee901a  vmov         r1, s1
08025714  f0ee607a  vmov.f32     s15, s1
08025718  21f00041  bic          r1, r1, #2147483648
0802571c  b1f1ff4f  cmp.w        r1, #2139095040
08025720  dbf80010  ldr.w        r1, [r11]
08025724  88bf      it           hi
08025726  f0ee647a  vmovhi.f32   s15, s9
0802572a  0129      cmp          r1, #1
0802572c  8b49      ldr          r1, [pc, #556]  ; [0x0802595c] = 0x200037f0 (f32=1.08605299e-19)
0802572e  e8ee276a  vfma.f32     s13, s16, s15
08025732  d1ed007a  vldr         s15, [r1]
08025736  4ff00001  mov.w        r1, #0
0802573a  0cbf      ite          eq
0802573c  75eea75a  vaddeq.f32   s11, s11, s15
08025740  75eee75a  vsubne.f32   s11, s11, s15
08025744  f5eec05a  vcmpe.f32    s11, #0
08025748  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802574c  00f1dc83  bmi.w        #1976  ; -> 0x08025f08
08025750  f7ee007a  vmov.f32     s15, #1.000000e+00
08025754  f4eee75a  vcmpe.f32    s11, s15
08025758  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802575c  41f38d80  ble.w        #4378  ; -> 0x0802687a
08025760  cbf87810  str.w        r1, [r11, #120]
08025764  0399      ldr          r1, [sp, #12]
08025766  cbed087a  vstr         s15, [r11, #32]
0802576a  0968      ldr          r1, [r1]
0802576c  0229      cmp          r1, #2
0802576e  7ef495ae  bne.w        #-4822  ; -> 0x0802449c
08025772  dbf83c20  ldr.w        r2, [r11, #60]
08025776  7549      ldr          r1, [pc, #468]  ; [0x0802594c] = 0x20001a14 (f32=1.08506503e-19)
08025778  002a      cmp          r2, #0
0802577a  00f0e882  beq.w        #1488  ; -> 0x08025d4e
0802577e  d1ed0d7a  vldr         s15, [r1, #52]
08025782  74ee277a  vadd.f32     s15, s8, s15
08025786  b0ee007a  vmov.f32     s14, #2.000000e+00
0802578a  dbf85020  ldr.w        r2, [r11, #80]
0802578e  b7ee081a  vmov.f32     s2, #1.500000e+00
08025792  dfed702a  vldr         s5, [pc, #448]  ; [0x08025954] = 0x42c80000 (f32=100)
08025796  1646      mov          r6, r2
08025798  dbed134a  vldr         s9, [r11, #76]
0802579c  c7fe877a  vmaxnm.f32   s15, s15, s14
080257a0  77ee837a  vadd.f32     s15, s15, s6
080257a4  dbf89410  ldr.w        r1, [r11, #148]
080257a8  684d      ldr          r5, [pc, #416]  ; [0x0802594c] = 0x20001a14 (f32=1.08506503e-19)
080257aa  bdeee77a  vcvt.s32.f32 s14, s15
080257ae  cbed0e7a  vstr         s15, [r11, #56]
080257b2  0696      str          r6, [sp, #24]
080257b4  17ee102a  vmov         r2, s14
080257b8  b8eec77a  vcvt.f32.s32 s14, s14
080257bc  1a40      ands         r2, r3
080257be  37eec70a  vsub.f32     s0, s15, s14
080257c2  541e      subs         r4, r2, #1
080257c4  901c      adds         r0, r2, #2
080257c6  cbf84020  str.w        r2, [r11, #64]
080257ca  1c40      ands         r4, r3
080257cc  1840      ands         r0, r3
080257ce  8bed120a  vstr         s0, [r11, #72]
080257d2  20ee068a  vmul.f32     s16, s0, s12
080257d6  06eb8000  add.w        r0, r6, r0, lsl #2
080257da  06eb8404  add.w        r4, r6, r4, lsl #2
080257de  d0ed005a  vldr         s11, [r0]
080257e2  501c      adds         r0, r2, #1
080257e4  94ed007a  vldr         s14, [r4]
080257e8  06eb8202  add.w        r2, r6, r2, lsl #2
080257ec  1840      ands         r0, r3
080257ee  77ee255a  vadd.f32     s11, s14, s11
080257f2  92ed007a  vldr         s14, [r2]
080257f6  06eb8000  add.w        r0, r6, r0, lsl #2
080257fa  75eec77a  vsub.f32     s15, s11, s14
080257fe  d0ed000a  vldr         s1, [r0]
08025802  77ee255a  vadd.f32     s11, s14, s11
08025806  77eee07a  vsub.f32     s15, s15, s1
0802580a  67ee887a  vmul.f32     s15, s15, s16
0802580e  e5eec67a  vfms.f32     s15, s11, s12
08025812  e0ee817a  vfma.f32     s15, s1, s2
08025816  a0ee277a  vfma.f32     s14, s0, s15
0802581a  17ee102a  vmov         r2, s14
0802581e  22f00042  bic          r2, r2, #2147483648
08025822  b2f1ff4f  cmp.w        r2, #2139095040
08025826  88bf      it           hi
08025828  b0ee627a  vmovhi.f32   s14, s5
0802582c  24ee877a  vmul.f32     s14, s9, s14
08025830  0029      cmp          r1, #0
08025832  00f08782  beq.w        #1294  ; -> 0x08025d44
08025836  d5ed235a  vldr         s11, [r5, #140]
0802583a  34ee254a  vadd.f32     s8, s8, s11
0802583e  f0ee005a  vmov.f32     s11, #2.000000e+00
08025842  dbf8a810  ldr.w        r1, [r11, #168]
08025846  f7ee082a  vmov.f32     s5, #1.500000e+00
0802584a  0026      movs         r6, #0
0802584c  9fed411a  vldr         s2, [pc, #260]  ; [0x08025954] = 0x42c80000 (f32=100)
08025850  c4fe255a  vmaxnm.f32   s11, s8, s11
08025854  75ee835a  vadd.f32     s11, s11, s6
08025858  dbf82c00  ldr.w        r0, [r11, #44]
0802585c  9bed290a  vldr         s0, [r11, #164]
08025860  fdeee57a  vcvt.s32.f32 s15, s11
08025864  cbed245a  vstr         s11, [r11, #144]
08025868  17ee902a  vmov         r2, s15
0802586c  f8eee77a  vcvt.f32.s32 s15, s15
08025870  1a40      ands         r2, r3
08025872  75eee75a  vsub.f32     s11, s11, s15
08025876  551e      subs         r5, r2, #1
08025878  941c      adds         r4, r2, #2
0802587a  cbf89820  str.w        r2, [r11, #152]
0802587e  1d40      ands         r5, r3
08025880  1c40      ands         r4, r3
08025882  cbed285a  vstr         s11, [r11, #160]
08025886  25ee868a  vmul.f32     s16, s11, s12
0802588a  01eb8404  add.w        r4, r1, r4, lsl #2
0802588e  01eb8505  add.w        r5, r1, r5, lsl #2
08025892  d4ed007a  vldr         s15, [r4]
08025896  541c      adds         r4, r2, #1
08025898  95ed004a  vldr         s8, [r5]
0802589c  01eb8202  add.w        r2, r1, r2, lsl #2
080258a0  2340      ands         r3, r4
080258a2  34ee274a  vadd.f32     s8, s8, s15
080258a6  92ed003a  vldr         s6, [r2]
080258aa  01eb8303  add.w        r3, r1, r3, lsl #2
080258ae  74ee437a  vsub.f32     s15, s8, s6
080258b2  d3ed000a  vldr         s1, [r3]
080258b6  33ee044a  vadd.f32     s8, s6, s8
080258ba  039b      ldr          r3, [sp, #12]
080258bc  77eee07a  vsub.f32     s15, s15, s1
080258c0  1e60      str          r6, [r3]
080258c2  67ee887a  vmul.f32     s15, s15, s16
080258c6  e4ee467a  vfms.f32     s15, s8, s12
080258ca  e0eea27a  vfma.f32     s15, s1, s5
080258ce  a5eea73a  vfma.f32     s6, s11, s15
080258d2  13ee103a  vmov         r3, s6
080258d6  f0ee437a  vmov.f32     s15, s6
080258da  23f00043  bic          r3, r3, #2147483648
080258de  b3f1ff4f  cmp.w        r3, #2139095040
080258e2  1e4b      ldr          r3, [pc, #120]  ; [0x0802595c] = 0x200037f0 (f32=1.08605299e-19)
080258e4  88bf      it           hi
080258e6  f0ee417a  vmovhi.f32   s15, s2
080258ea  0128      cmp          r0, #1
080258ec  a0ee277a  vfma.f32     s14, s0, s15
080258f0  d3ed007a  vldr         s15, [r3]
080258f4  4ff00003  mov.w        r3, #0
080258f8  0cbf      ite          eq
080258fa  74eea74a  vaddeq.f32   s9, s9, s15
080258fe  74eee74a  vsubne.f32   s9, s9, s15
08025902  f5eec04a  vcmpe.f32    s9, #0
08025906  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802590a  00f1f582  bmi.w        #1514  ; -> 0x08025ef8
0802590e  f7ee007a  vmov.f32     s15, #1.000000e+00
08025912  f4eee74a  vcmpe.f32    s9, s15
08025916  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802591a  41f30180  ble.w        #4098  ; -> 0x08026920
0802591e  cbed137a  vstr         s15, [r11, #76]
08025922  cbf8a430  str.w        r3, [r11, #164]
08025926  fef73cbe  b.w          #-5000  ; -> 0x080245a2
0802592a  2c23      movs         r3, #44
0802592c  0c48      ldr          r0, [pc, #48]  ; [0x08025960] = 0x20001ad0 (f32=1.08508932e-19)
0802592e  009c      ldr          r4, [sp]
08025930  0668      ldr          r6, [r0]
08025932  0020      movs         r0, #0
08025934  2568      ldr          r5, [r4]
08025936  03fb01b4  mla          r4, r3, r1, r11
0802593a  03fb02b3  mla          r3, r3, r2, r11
0802593e  6660      str          r6, [r4, #4]
08025940  2061      str          r0, [r4, #16]
08025942  5d60      str          r5, [r3, #4]
08025944  1861      str          r0, [r3, #16]
08025946  fef739bd  b.w          #-5518  ; -> 0x080243bc
08025964  93ed237a  vldr         s14, [r3, #140]
08025968  d3ed227a  vldr         s15, [r3, #136]
0802596c  37ee267a  vadd.f32     s14, s14, s13
08025970  77ee8c7a  vadd.f32     s15, s15, s24
08025974  83ed237a  vstr         s14, [r3, #140]
08025978  c3ed227a  vstr         s15, [r3, #136]
0802597c  fef7b3bc  b.w          #-5786  ; -> 0x080242e6
08025980  9bed187a  vldr         s14, [r11, #96]
08025984  0021      movs         r1, #0
08025986  dbed177a  vldr         s15, [r11, #92]
0802598a  37ee267a  vadd.f32     s14, s14, s13
0802598e  77ee8c7a  vadd.f32     s15, s15, s24
08025992  8bed187a  vstr         s14, [r11, #96]
08025996  cbed177a  vstr         s15, [r11, #92]
0802599a  fef78fbc  b.w          #-5858  ; -> 0x080242bc
0802599e  dbf82c10  ldr.w        r1, [r11, #44]
080259a2  9e48      ldr          r0, [pc, #632]  ; [0x08025c1c] = 0x20001a14 (f32=1.08506503e-19)
080259a4  0129      cmp          r1, #1
080259a6  00f0a182  beq.w        #1346  ; -> 0x08025eec
080259aa  0099      ldr          r1, [sp]
080259ac  0c68      ldr          r4, [r1]
080259ae  0021      movs         r1, #0
080259b0  0463      str          r4, [r0, #48]
080259b2  0220      movs         r0, #2
080259b4  cbf82c20  str.w        r2, [r11, #44]
080259b8  039a      ldr          r2, [sp, #12]
080259ba  cbf88410  str.w        r1, [r11, #132]
080259be  1060      str          r0, [r2]
080259c0  fef76abc  b.w          #-5932  ; -> 0x08024298
080259c4  dbf80010  ldr.w        r1, [r11]
080259c8  9448      ldr          r0, [pc, #592]  ; [0x08025c1c] = 0x20001a14 (f32=1.08506503e-19)
080259ca  0129      cmp          r1, #1
080259cc  00f08582  beq.w        #1290  ; -> 0x08025eda
080259d0  9349      ldr          r1, [pc, #588]  ; [0x08025c20] = 0x20001ad0 (f32=1.08508932e-19)
080259d2  0c68      ldr          r4, [r1]
080259d4  0021      movs         r1, #0
080259d6  4460      str          r4, [r0, #4]
080259d8  0220      movs         r0, #2
080259da  cbf85810  str.w        r1, [r11, #88]
080259de  0299      ldr          r1, [sp, #8]
080259e0  cbf80030  str.w        r3, [r11]
080259e4  0860      str          r0, [r1]
080259e6  fef754bc  b.w          #-5976  ; -> 0x08024292
080259ea  039a      ldr          r2, [sp, #12]
080259ec  65eea25a  vmul.f32     s11, s11, s5
080259f0  f2ee002a  vmov.f32     s5, #8.000000e+00
080259f4  8b48      ldr          r0, [pc, #556]  ; [0x08025c24] = 0x20002aa4 (f32=1.08561303e-19)
080259f6  1268      ldr          r2, [r2]
080259f8  022a      cmp          r2, #2
080259fa  65eea25a  vmul.f32     s11, s11, s5
080259fe  00f07182  beq.w        #1250  ; -> 0x08025ee4
08025a02  8949      ldr          r1, [pc, #548]  ; [0x08025c28] = 0x200038a0 (f32=1.08607574e-19)
08025a04  009c      ldr          r4, [sp]
08025a06  91ed007a  vldr         s14, [r1]
08025a0a  d4ed007a  vldr         s15, [r4]
08025a0e  8749      ldr          r1, [pc, #540]  ; [0x08025c2c] = 0x20002aa8 (f32=1.08561355e-19)
08025a10  77ee277a  vadd.f32     s15, s14, s15
08025a14  d1ed006a  vldr         s13, [r1]
08025a18  77eea57a  vadd.f32     s15, s15, s11
08025a1c  f4eee67a  vcmpe.f32    s15, s13
08025a20  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025a24  40f12682  bpl.w        #1100  ; -> 0x08025e74
08025a28  75ee876a  vadd.f32     s13, s11, s14
08025a2c  1a46      mov          r2, r3
08025a2e  0360      str          r3, [r0]
08025a30  c1ed006a  vstr         s13, [r1]
08025a34  0299      ldr          r1, [sp, #8]
08025a36  dff80ce2  ldr.w        lr, [pc, #524]  ; [0x08025c44] = 0x20002ae4 (f32=1.0856213e-19)
08025a3a  0868      ldr          r0, [r1]
08025a3c  0228      cmp          r0, #2
08025a3e  00f02e82  beq.w        #1116  ; -> 0x08025e9e
08025a42  7949      ldr          r1, [pc, #484]  ; [0x08025c28] = 0x200038a0 (f32=1.08607574e-19)
08025a44  764c      ldr          r4, [pc, #472]  ; [0x08025c20] = 0x20001ad0 (f32=1.08508932e-19)
08025a46  91ed003a  vldr         s6, [r1]
08025a4a  d4ed007a  vldr         s15, [r4]
08025a4e  7849      ldr          r1, [pc, #480]  ; [0x08025c30] = 0x20004c70 (f32=1.08673128e-19)
08025a50  73ee277a  vadd.f32     s15, s6, s15
08025a54  91ed007a  vldr         s14, [r1]
08025a58  77eea57a  vadd.f32     s15, s15, s11
08025a5c  f4eec77a  vcmpe.f32    s15, s14
08025a60  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025a64  40f12a82  bpl.w        #1108  ; -> 0x08025ebc
08025a68  35ee837a  vadd.f32     s14, s11, s6
08025a6c  0120      movs         r0, #1
08025a6e  cef80000  str.w        r0, [lr]
08025a72  81ed007a  vstr         s14, [r1]
08025a76  022a      cmp          r2, #2
08025a78  09d0      beq          #18  ; -> 0x08025a8e
08025a7a  6e4b      ldr          r3, [pc, #440]  ; [0x08025c34] = 0x20003890 (f32=1.08607367e-19)
08025a7c  91ed007a  vldr         s14, [r1]
08025a80  1868      ldr          r0, [r3]
08025a82  029b      ldr          r3, [sp, #8]
08025a84  6c4c      ldr          r4, [pc, #432]  ; [0x08025c38] = 0x20001b44 (f32=1.08510432e-19)
08025a86  6d49      ldr          r1, [pc, #436]  ; [0x08025c3c] = 0x20001b28 (f32=1.0851007e-19)
08025a88  1b68      ldr          r3, [r3]
08025a8a  2060      str          r0, [r4]
08025a8c  0860      str          r0, [r1]
08025a8e  6449      ldr          r1, [pc, #400]  ; [0x08025c20] = 0x20001ad0 (f32=1.08508932e-19)
08025a90  81ed007a  vstr         s14, [r1]
08025a94  0099      ldr          r1, [sp]
08025a96  c1ed006a  vstr         s13, [r1]
08025a9a  fef7f5bb  b.w          #-6166  ; -> 0x08024288
08025a9e  604b      ldr          r3, [pc, #384]  ; [0x08025c20] = 0x20001ad0 (f32=1.08508932e-19)
08025aa0  4ff43471  mov.w        r1, #720
08025aa4  6648      ldr          r0, [pc, #408]  ; [0x08025c40] = 0x200055d4 (f32=1.08704199e-19)
08025aa6  d3ed007a  vldr         s15, [r3]
08025aaa  029b      ldr          r3, [sp, #8]
08025aac  75ee277a  vadd.f32     s15, s10, s15
08025ab0  0160      str          r1, [r0]
08025ab2  1b68      ldr          r3, [r3]
08025ab4  0d99      ldr          r1, [sp, #52]
08025ab6  fdeee77a  vcvt.s32.f32 s15, s15
08025aba  6248      ldr          r0, [pc, #392]  ; [0x08025c44] = 0x20002ae4 (f32=1.0856213e-19)
08025abc  c1ed007a  vstr         s15, [r1]
08025ac0  002b      cmp          r3, #0
08025ac2  40f08a81  bne.w        #788  ; -> 0x08025dda
08025ac6  012a      cmp          r2, #1
08025ac8  26d0      beq          #76  ; -> 0x08025b18
08025aca  1d46      mov          r5, r3
08025acc  5e48      ldr          r0, [pc, #376]  ; [0x08025c48] = 0x20002acc (f32=1.0856182e-19)
08025ace  beee007a  vmov.f32     s14, #-5.000000e-01
08025ad2  5e4c      ldr          r4, [pc, #376]  ; [0x08025c4c] = 0x0bb38435 (f32=6.9147215e-32)
08025ad4  0368      ldr          r3, [r0]
08025ad6  5e49      ldr          r1, [pc, #376]  ; [0x08025c50] = 0x3619636b (f32=2.28566455e-06)
08025ad8  04fb03f3  mul          r3, r4, r3
08025adc  5d4c      ldr          r4, [pc, #372]  ; [0x08025c54] = 0x2000387c (f32=1.08607108e-19)
08025ade  dfed5e5a  vldr         s11, [pc, #376]  ; [0x08025c58] = 0x360637bd (f32=1.99999999e-06)
08025ae2  1944      add          r1, r3
08025ae4  d4ed004a  vldr         s9, [r4]
08025ae8  5c4b      ldr          r3, [pc, #368]  ; [0x08025c5c] = 0x20003854 (f32=1.08606591e-19)
08025aea  06ee901a  vmov         s13, r1
08025aee  0160      str          r1, [r0]
08025af0  d3ed007a  vldr         s15, [r3]
08025af4  b8ee663a  vcvt.f32.u32 s6, s13
08025af8  dfed596a  vldr         s13, [pc, #356]  ; [0x08025c60] = 0x3649539c (f32=3.00000011e-06)
08025afc  a3ee247a  vfma.f32     s14, s6, s9
08025b00  e7ee257a  vfma.f32     s15, s14, s11
08025b04  f4eee67a  vcmpe.f32    s15, s13
08025b08  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025b0c  40f36781  ble.w        #718  ; -> 0x08025dde
08025b10  5449      ldr          r1, [pc, #336]  ; [0x08025c64] = 0x361a59b3 (f32=2.30000001e-06)
08025b12  1960      str          r1, [r3]
08025b14  fef747ba  b.w          #-7026  ; -> 0x08023fa6
08025b18  4649      ldr          r1, [pc, #280]  ; [0x08025c34] = 0x20003890 (f32=1.08607367e-19)
08025b1a  474c      ldr          r4, [pc, #284]  ; [0x08025c38] = 0x20001b44 (f32=1.08510432e-19)
08025b1c  0d68      ldr          r5, [r1]
08025b1e  2168      ldr          r1, [r4]
08025b20  a942      cmp          r1, r5
08025b22  00f06b84  beq.w        #2262  ; -> 0x080263fc
08025b26  2560      str          r5, [r4]
08025b28  1d46      mov          r5, r3
08025b2a  0260      str          r2, [r0]
08025b2c  cee7      b            #-100  ; -> 0x08025acc
08025b2e  0099      ldr          r1, [sp]
08025b30  4ff43472  mov.w        r2, #720
08025b34  3b4c      ldr          r4, [pc, #236]  ; [0x08025c24] = 0x20002aa4 (f32=1.08561303e-19)
08025b36  d1ed007a  vldr         s15, [r1]
08025b3a  4b49      ldr          r1, [pc, #300]  ; [0x08025c68] = 0x20004e80 (f32=1.08679952e-19)
08025b3c  75ee277a  vadd.f32     s15, s10, s15
08025b40  0a60      str          r2, [r1]
08025b42  039a      ldr          r2, [sp, #12]
08025b44  fdeee77a  vcvt.s32.f32 s15, s15
08025b48  1168      ldr          r1, [r2]
08025b4a  0f9a      ldr          r2, [sp, #60]
08025b4c  c2ed007a  vstr         s15, [r2]
08025b50  169a      ldr          r2, [sp, #88]
08025b52  1268      ldr          r2, [r2]
08025b54  11b9      cbnz         r1, #4  ; -> 0x08025b5c
08025b56  012a      cmp          r2, #1
08025b58  00f03782  beq.w        #1134  ; -> 0x08025fca
08025b5c  0026      movs         r6, #0
08025b5e  3a4c      ldr          r4, [pc, #232]  ; [0x08025c48] = 0x20002acc (f32=1.0856182e-19)
08025b60  beee007a  vmov.f32     s14, #-5.000000e-01
08025b64  394d      ldr          r5, [pc, #228]  ; [0x08025c4c] = 0x0bb38435 (f32=6.9147215e-32)
08025b66  2168      ldr          r1, [r4]
08025b68  3948      ldr          r0, [pc, #228]  ; [0x08025c50] = 0x3619636b (f32=2.28566455e-06)
08025b6a  05fb01f1  mul          r1, r5, r1
08025b6e  394d      ldr          r5, [pc, #228]  ; [0x08025c54] = 0x2000387c (f32=1.08607108e-19)
08025b70  dfed395a  vldr         s11, [pc, #228]  ; [0x08025c58] = 0x360637bd (f32=1.99999999e-06)
08025b74  0844      add          r0, r1
08025b76  d5ed004a  vldr         s9, [r5]
08025b7a  3c49      ldr          r1, [pc, #240]  ; [0x08025c6c] = 0x20004c6c (f32=1.08673076e-19)
08025b7c  06ee900a  vmov         s13, r0
08025b80  2060      str          r0, [r4]
08025b82  d1ed007a  vldr         s15, [r1]
08025b86  b8ee663a  vcvt.f32.u32 s6, s13
08025b8a  dfed356a  vldr         s13, [pc, #212]  ; [0x08025c60] = 0x3649539c (f32=3.00000011e-06)
08025b8e  a3ee247a  vfma.f32     s14, s6, s9
08025b92  e7ee257a  vfma.f32     s15, s14, s11
08025b96  f4eee67a  vcmpe.f32    s15, s13
08025b9a  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025b9e  40f32a81  ble.w        #596  ; -> 0x08025df6
08025ba2  3348      ldr          r0, [pc, #204]  ; [0x08025c70] = 0x35ae7ba9 (f32=1.30000001e-06)
08025ba4  0860      str          r0, [r1]
08025ba6  fef7fab9  b.w          #-7180  ; -> 0x08023f9e
08025baa  dfed327a  vldr         s15, [pc, #200]  ; [0x08025c74] = 0x40e4e34e (f32=7.15274715)
08025bae  fef7cebb  b.w          #-6244  ; -> 0x0802434e
08025bb2  dfed307a  vldr         s15, [pc, #192]  ; [0x08025c74] = 0x40e4e34e (f32=7.15274715)
08025bb6  fef7fabb  b.w          #-6156  ; -> 0x080243ae
08025bba  dfed2f6a  vldr         s13, [pc, #188]  ; [0x08025c78] = 0x49f4e69c (f32=2006227.5)
08025bbe  b4eee67a  vcmpe.f32    s14, s13
08025bc2  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025bc6  3ff7b8ab  bgt.w        #-2192  ; -> 0x0802533a
08025bca  fff7b9bb  b.w          #-2190  ; -> 0x08025340
08025bce  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025bd2  07dc      bgt          #14  ; -> 0x08025be4
08025bd4  faee047a  vmov.f32     s15, #-1.000000e+01
08025bd8  f4eee76a  vcmpe.f32    s13, s15
08025bdc  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025be0  7ff5f5ab  bpl.w        #-2070  ; -> 0x080253ce
08025be4  0023      movs         r3, #0
08025be6  9fed257a  vldr         s14, [pc, #148]  ; [0x08025c7c] = 0x00000000 (f32=0)
08025bea  fef70bbd  b.w          #-5610  ; -> 0x08024604
08025bee  b4ee007a  vmov.f32     s14, #1.250000e-01
08025bf2  76e4      b            #-1812  ; -> 0x080254e2
08025bf4  f2ee045a  vmov.f32     s11, #1.000000e+01
08025bf8  faee047a  vmov.f32     s15, #-1.000000e+01
08025bfc  b4eee57a  vcmpe.f32    s14, s11
08025c00  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025c04  b4eee77a  vcmpe.f32    s14, s15
08025c08  ccbf      ite          gt
08025c0a  0123      movgt        r3, #1
08025c0c  0023      movle        r3, #0
08025c0e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025c12  48bf      it           mi
08025c14  43f00103  orrmi        r3, r3, #1
08025c18  fef7f7bc  b.w          #-5650  ; -> 0x0802460a
08025c80  2c20      movs         r0, #44
08025c82  b0ee007a  vmov.f32     s14, #2.000000e+00
08025c86  00fb01b4  mla          r4, r0, r1, r11
08025c8a  00fb02b0  mla          r0, r0, r2, r11
08025c8e  d4ed027a  vldr         s15, [r4, #8]
08025c92  0361      str          r3, [r0, #16]
08025c94  77ee877a  vadd.f32     s15, s15, s14
08025c98  2361      str          r3, [r4, #16]
08025c9a  f4eec77a  vcmpe.f32    s15, s14
08025c9e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025ca2  00f10181  bmi.w        #514  ; -> 0x08025ea8
08025ca6  c4ed027a  vstr         s15, [r4, #8]
08025caa  2c23      movs         r3, #44
08025cac  f3ee006a  vmov.f32     s13, #1.600000e+01
08025cb0  03fb01b3  mla          r3, r3, r1, r11
08025cb4  93ed017a  vldr         s14, [r3, #4]
08025cb8  37ee077a  vadd.f32     s14, s14, s14
08025cbc  77ee266a  vadd.f32     s13, s14, s13
08025cc0  f4eee76a  vcmpe.f32    s13, s15
08025cc4  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025cc8  00f1b480  bmi.w        #360  ; -> 0x08025e34
08025ccc  f2ee086a  vmov.f32     s13, #1.200000e+01
08025cd0  37ee267a  vadd.f32     s14, s14, s13
08025cd4  b4eee77a  vcmpe.f32    s14, s15
08025cd8  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025cdc  40f1a780  bpl.w        #334  ; -> 0x08025e2e
08025ce0  0123      movs         r3, #1
08025ce2  0298      ldr          r0, [sp, #8]
08025ce4  0360      str          r3, [r0]
08025ce6  2c23      movs         r3, #44
08025ce8  b0ee007a  vmov.f32     s14, #2.000000e+00
08025cec  03fb02b3  mla          r3, r3, r2, r11
08025cf0  d3ed027a  vldr         s15, [r3, #8]
08025cf4  77ee877a  vadd.f32     s15, s15, s14
08025cf8  f4eec77a  vcmpe.f32    s15, s14
08025cfc  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025d00  00f1d780  bmi.w        #430  ; -> 0x08025eb2
08025d04  c3ed027a  vstr         s15, [r3, #8]
08025d08  2c23      movs         r3, #44
08025d0a  f3ee006a  vmov.f32     s13, #1.600000e+01
08025d0e  03fb02b3  mla          r3, r3, r2, r11
08025d12  93ed017a  vldr         s14, [r3, #4]
08025d16  37ee077a  vadd.f32     s14, s14, s14
08025d1a  77ee266a  vadd.f32     s13, s14, s13
08025d1e  f4eee76a  vcmpe.f32    s13, s15
08025d22  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025d26  7dd4      bmi          #250  ; -> 0x08025e24
08025d28  f2ee086a  vmov.f32     s13, #1.200000e+01
08025d2c  37ee267a  vadd.f32     s14, s14, s13
08025d30  b4eee77a  vcmpe.f32    s14, s15
08025d34  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025d38  71d5      bpl          #226  ; -> 0x08025e1e
08025d3a  0123      movs         r3, #1
08025d3c  0398      ldr          r0, [sp, #12]
08025d3e  0360      str          r3, [r0]
08025d40  fef73cbb  b.w          #-6536  ; -> 0x080243bc
08025d44  d5ed225a  vldr         s11, [r5, #136]
08025d48  34ee254a  vadd.f32     s8, s8, s11
08025d4c  77e5      b            #-1298  ; -> 0x0802583e
08025d4e  d1ed0c7a  vldr         s15, [r1, #48]
08025d52  74ee277a  vadd.f32     s15, s8, s15
08025d56  16e5      b            #-1492  ; -> 0x08025786
08025d58  d4ed177a  vldr         s15, [r4, #92]
08025d5c  34eea77a  vadd.f32     s14, s9, s15
08025d60  8ee4      b            #-1764  ; -> 0x08025680
08025d62  d1ed017a  vldr         s15, [r1, #4]
08025d66  74eea77a  vadd.f32     s15, s9, s15
08025d6a  25e4      b            #-1974  ; -> 0x080255b8
08025d6c  f0ee634a  vmov.f32     s9, s7
08025d70  f0ee425a  vmov.f32     s11, s4
08025d74  e7eea64a  vfma.f32     s9, s15, s13
08025d78  e7ee275a  vfma.f32     s11, s14, s15
08025d7c  f0ee647a  vmov.f32     s15, s9
08025d80  fef75fbc  b.w          #-5954  ; -> 0x08024642
08025d84  049a      ldr          r2, [sp, #16]
08025d86  1a60      str          r2, [r3]
08025d88  039b      ldr          r3, [sp, #12]
08025d8a  1860      str          r0, [r3]
08025d8c  029b      ldr          r3, [sp, #8]
08025d8e  1860      str          r0, [r3]
08025d90  fef71aba  b.w          #-7116  ; -> 0x080241c8
08025d94  c54b      ldr          r3, [pc, #788]  ; [0x080260ac] = 0x20001b28 (f32=1.0851007e-19)
08025d96  c649      ldr          r1, [pc, #792]  ; [0x080260b0] = 0x20003804 (f32=1.08605557e-19)
08025d98  1b68      ldr          r3, [r3]
08025d9a  c648      ldr          r0, [pc, #792]  ; [0x080260b4] = 0x20002b60 (f32=1.08563733e-19)
08025d9c  01eb8301  add.w        r1, r1, r3, lsl #2
08025da0  029b      ldr          r3, [sp, #8]
08025da2  d0ed007a  vldr         s15, [r0]
08025da6  d1ed006a  vldr         s13, [r1]
08025daa  0899      ldr          r1, [sp, #32]
08025dac  26eea78a  vmul.f32     s16, s13, s15
08025db0  1b68      ldr          r3, [r3]
08025db2  c148      ldr          r0, [pc, #772]  ; [0x080260b8] = 0x20002ae4 (f32=1.0856213e-19)
08025db4  81ed008a  vstr         s16, [r1]
08025db8  1899      ldr          r1, [sp, #96]
08025dba  81ed008a  vstr         s16, [r1]
08025dbe  002b      cmp          r3, #0
08025dc0  7ff44aab  bne.w        #-2412  ; -> 0x08025458
08025dc4  0e9b      ldr          r3, [sp, #56]
08025dc6  1968      ldr          r1, [r3]
08025dc8  0029      cmp          r1, #0
08025dca  7ff44aab  bne.w        #-2412  ; -> 0x08025462
08025dce  0260      str          r2, [r0]
08025dd0  1a9b      ldr          r3, [sp, #104]
08025dd2  93ed003a  vldr         s6, [r3]
08025dd6  fff749bb  b.w          #-2414  ; -> 0x0802546c
08025dda  0025      movs         r5, #0
08025ddc  76e6      b            #-788  ; -> 0x08025acc
08025dde  9fedb77a  vldr         s14, [pc, #732]  ; [0x080260bc] = 0xb649539c (f32=-3.00000011e-06)
08025de2  f4eec77a  vcmpe.f32    s15, s14
08025de6  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025dea  40f13a85  bpl.w        #2676  ; -> 0x08026862
08025dee  b449      ldr          r1, [pc, #720]  ; [0x080260c0] = 0xb60cedba (f32=-2.10000007e-06)
08025df0  1960      str          r1, [r3]
08025df2  fef7d8b8  b.w          #-7760  ; -> 0x08023fa6
08025df6  9fedb17a  vldr         s14, [pc, #708]  ; [0x080260bc] = 0xb649539c (f32=-3.00000011e-06)
08025dfa  f4eec77a  vcmpe.f32    s15, s14
08025dfe  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025e02  40f13285  bpl.w        #2660  ; -> 0x0802686a
08025e06  af48      ldr          r0, [pc, #700]  ; [0x080260c4] = 0xb5a10fb0 (f32=-1.20000004e-06)
08025e08  0860      str          r0, [r1]
08025e0a  fef7c8b8  b.w          #-7792  ; -> 0x08023f9e
08025e0e  dfedae7a  vldr         s15, [pc, #696]  ; [0x080260c8] = 0xc0e4e34e (f32=-7.15274715)
08025e12  fef79cba  b.w          #-6856  ; -> 0x0802434e
08025e16  dfedac7a  vldr         s15, [pc, #688]  ; [0x080260c8] = 0xc0e4e34e (f32=-7.15274715)
08025e1a  fef7c8ba  b.w          #-6768  ; -> 0x080243ae
08025e1e  012e      cmp          r6, #1
08025e20  7ef4ccaa  bne.w        #-6760  ; -> 0x080243bc
08025e24  4ff08040  mov.w        r0, #1073741824
08025e28  9860      str          r0, [r3, #8]
08025e2a  fef7c7ba  b.w          #-6770  ; -> 0x080243bc
08025e2e  012d      cmp          r5, #1
08025e30  7ff459af  bne.w        #-334  ; -> 0x08025ce6
08025e34  4ff08040  mov.w        r0, #1073741824
08025e38  9860      str          r0, [r3, #8]
08025e3a  54e7      b            #-344  ; -> 0x08025ce6
08025e3c  0129      cmp          r1, #1
08025e3e  c7d1      bne          #-114  ; -> 0x08025dd0
08025e40  a24a      ldr          r2, [pc, #648]  ; [0x080260cc] = 0x20004c38 (f32=1.08672404e-19)
08025e42  a34b      ldr          r3, [pc, #652]  ; [0x080260d0] = 0x20001b44 (f32=1.08510432e-19)
08025e44  1268      ldr          r2, [r2]
08025e46  1b68      ldr          r3, [r3]
08025e48  d3eb4203  rsbs         r3, r3, r2, lsl #1
08025e4c  00f1d482  bmi.w        #1448  ; -> 0x080263f8
08025e50  0c2b      cmp          r3, #12
08025e52  a8bf      it           ge
08025e54  0c23      movge        r3, #12
08025e56  964a      ldr          r2, [pc, #600]  ; [0x080260b0] = 0x20003804 (f32=1.08605557e-19)
08025e58  9648      ldr          r0, [pc, #600]  ; [0x080260b4] = 0x20002b60 (f32=1.08563733e-19)
08025e5a  02eb8303  add.w        r3, r2, r3, lsl #2
08025e5e  d0ed007a  vldr         s15, [r0]
08025e62  93ed003a  vldr         s6, [r3]
08025e66  1a9b      ldr          r3, [sp, #104]
08025e68  23ee273a  vmul.f32     s6, s6, s15
08025e6c  83ed003a  vstr         s6, [r3]
08025e70  fff7fcba  b.w          #-2568  ; -> 0x0802546c
08025e74  35ee877a  vadd.f32     s14, s11, s14
08025e78  f4eec76a  vcmpe.f32    s13, s14
08025e7c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025e80  7ff5d8ad  bpl.w        #-1104  ; -> 0x08025a34
08025e84  c1ed007a  vstr         s15, [r1]
08025e88  f0ee676a  vmov.f32     s13, s15
08025e8c  0299      ldr          r1, [sp, #8]
08025e8e  1a46      mov          r2, r3
08025e90  0360      str          r3, [r0]
08025e92  0868      ldr          r0, [r1]
08025e94  dff820e2  ldr.w        lr, [pc, #544]  ; [0x080260b8] = 0x20002ae4 (f32=1.0856213e-19)
08025e98  0228      cmp          r0, #2
08025e9a  7ff4d2ad  bne.w        #-1116  ; -> 0x08025a42
08025e9e  8d49      ldr          r1, [pc, #564]  ; [0x080260d4] = 0x20004c70 (f32=1.08673128e-19)
08025ea0  0346      mov          r3, r0
08025ea2  91ed007a  vldr         s14, [r1]
08025ea6  f2e5      b            #-1052  ; -> 0x08025a8e
08025ea8  f0ee477a  vmov.f32     s15, s14
08025eac  84ed027a  vstr         s14, [r4, #8]
08025eb0  fbe6      b            #-522  ; -> 0x08025caa
08025eb2  f0ee477a  vmov.f32     s15, s14
08025eb6  83ed027a  vstr         s14, [r3, #8]
08025eba  25e7      b            #-438  ; -> 0x08025d08
08025ebc  75ee835a  vadd.f32     s11, s11, s6
08025ec0  b4eee57a  vcmpe.f32    s14, s11
08025ec4  f1ee10fa  vmrs         APSR_nzcv, fpscr
08025ec8  26d5      bpl          #76  ; -> 0x08025f18
08025eca  0120      movs         r0, #1
08025ecc  c1ed007a  vstr         s15, [r1]
08025ed0  b0ee677a  vmov.f32     s14, s15
08025ed4  cef80000  str.w        r0, [lr]
08025ed8  cde5      b            #-1126  ; -> 0x08025a76
08025eda  7f4b      ldr          r3, [pc, #508]  ; [0x080260d8] = 0x20001ad0 (f32=1.08508932e-19)
08025edc  1c68      ldr          r4, [r3]
08025ede  0023      movs         r3, #0
08025ee0  c465      str          r4, [r0, #92]
08025ee2  79e5      b            #-1294  ; -> 0x080259d8
08025ee4  7d49      ldr          r1, [pc, #500]  ; [0x080260dc] = 0x20002aa8 (f32=1.08561355e-19)
08025ee6  d1ed006a  vldr         s13, [r1]
08025eea  a3e5      b            #-1210  ; -> 0x08025a34
08025eec  009a      ldr          r2, [sp]
08025eee  1468      ldr          r4, [r2]
08025ef0  0022      movs         r2, #0
08025ef2  c0f88840  str.w        r4, [r0, #136]
08025ef6  5ce5      b            #-1352  ; -> 0x080259b2
08025ef8  4ff07e52  mov.w        r2, #1065353216
08025efc  cbf84c30  str.w        r3, [r11, #76]
08025f00  cbf8a420  str.w        r2, [r11, #164]
08025f04  fef74dbb  b.w          #-6502  ; -> 0x080245a2
08025f08  4ff07e50  mov.w        r0, #1065353216
08025f0c  cbf82010  str.w        r1, [r11, #32]
08025f10  cbf87800  str.w        r0, [r11, #120]
08025f14  fef7bdba  b.w          #-6790  ; -> 0x08024492
08025f18  0346      mov          r3, r0
08025f1a  ace5      b            #-1192  ; -> 0x08025a76
08025f1c  704b      ldr          r3, [pc, #448]  ; [0x080260e0] = 0x200037f4 (f32=1.08605351e-19)
08025f1e  1b68      ldr          r3, [r3]
08025f20  012b      cmp          r3, #1
08025f22  00f05082  beq.w        #1184  ; -> 0x080263c6
08025f26  049a      ldr          r2, [sp, #16]
08025f28  dfed6eda  vldr         s27, [pc, #440]  ; [0x080260e4] = 0x00000000 (f32=0)
08025f2c  012a      cmp          r2, #1
08025f2e  00f01582  beq.w        #1066  ; -> 0x0802635c
08025f32  b0ee6dca  vmov.f32     s24, s27
08025f36  fdf7aabf  b.w          #-8364  ; -> 0x08023e8e
08025f3a  6b49      ldr          r1, [pc, #428]  ; [0x080260e8] = 0x3eb34517 (f32=0.350136489)
08025f3c  0022      movs         r2, #0
08025f3e  40f6ff73  movw         r3, #4095
08025f42  002e      cmp          r6, #0
08025f44  40f33b82  ble.w        #1142  ; -> 0x080263be
08025f48  a3f56063  sub.w        r3, r3, #3584
08025f4c  9fed677a  vldr         s14, [pc, #412]  ; [0x080260ec] = 0x38a7c5ac (f32=7.9999998e-05)
08025f50  9fed67da  vldr         s26, [pc, #412]  ; [0x080260f0] = 0xbf7ff972 (f32=-0.999899983)
08025f54  07ee903a  vmov         s15, r3
08025f58  f8eee77a  vcvt.f32.s32 s15, s15
08025f5c  a7ee87da  vfma.f32     s26, s15, s14
08025f60  fdf77bbf  b.w          #-8458  ; -> 0x08023e5a
08025f64  dfed635a  vldr         s11, [pc, #396]  ; [0x080260f4] = 0x3eb29231 (f32=0.348771602)
08025f68  dfed637a  vldr         s15, [pc, #396]  ; [0x080260f8] = 0x3e7ae148 (f32=0.245000005)
08025f6c  634a      ldr          r2, [pc, #396]  ; [0x080260fc] = 0x20004c4c (f32=1.08672662e-19)
08025f6e  b0ee677a  vmov.f32     s14, s15
08025f72  1092      str          r2, [sp, #64]
08025f74  c2ed027a  vstr         s15, [r2, #8]
08025f78  fdf73ebf  b.w          #-8580  ; -> 0x08023df8
08025f7c  0121      movs         r1, #1
08025f7e  0020      movs         r0, #0
08025f80  dfed5f5a  vldr         s11, [pc, #380]  ; [0x08026100] = 0x3fa28f5c (f32=1.26999998)
08025f84  fdf7e0be  b.w          #-8768  ; -> 0x08023d48
08025f88  5e49      ldr          r1, [pc, #376]  ; [0x08026104] = 0x20004dbc (f32=1.08677419e-19)
08025f8a  0a68      ldr          r2, [r1]
08025f8c  002a      cmp          r2, #0
08025f8e  7df416af  bne.w        #-8660  ; -> 0x08023dbe
08025f92  0222      movs         r2, #2
08025f94  0a60      str          r2, [r1]
08025f96  fdf712bf  b.w          #-8668  ; -> 0x08023dbe
08025f9a  5b48      ldr          r0, [pc, #364]  ; [0x08026108] = 0x200056c4 (f32=1.08707301e-19)
08025f9c  0268      ldr          r2, [r0]
08025f9e  0132      adds         r2, #1
08025fa0  012a      cmp          r2, #1
08025fa2  0260      str          r2, [r0]
08025fa4  7df4bcad  bne.w        #-9352  ; -> 0x08023b20
08025fa8  5848      ldr          r0, [pc, #352]  ; [0x0802610c] = 0x20002b48 (f32=1.08563423e-19)
08025faa  2468      ldr          r4, [r4]
08025fac  0268      ldr          r2, [r0]
08025fae  a61a      subs         r6, r4, r2
08025fb0  f02e      cmp          r6, #240
08025fb2  00f39982  bgt.w        #1330  ; -> 0x080264e8
08025fb6  564a      ldr          r2, [pc, #344]  ; [0x08026110] = 0x20004dd4 (f32=1.08677729e-19)
08025fb8  1492      str          r2, [sp, #80]
08025fba  1b99      ldr          r1, [sp, #108]
08025fbc  1668      ldr          r6, [r2]
08025fbe  554a      ldr          r2, [pc, #340]  ; [0x08026114] = 0x20004f94 (f32=1.08683519e-19)
08025fc0  d1f80080  ldr.w        r8, [r1]
08025fc4  0592      str          r2, [sp, #20]
08025fc6  fdf7c3bd  b.w          #-9338  ; -> 0x08023b50
08025fca  5348      ldr          r0, [pc, #332]  ; [0x08026118] = 0x20003890 (f32=1.08607367e-19)
08025fcc  374d      ldr          r5, [pc, #220]  ; [0x080260ac] = 0x20001b28 (f32=1.0851007e-19)
08025fce  0668      ldr          r6, [r0]
08025fd0  2868      ldr          r0, [r5]
08025fd2  b042      cmp          r0, r6
08025fd4  00f00482  beq.w        #1032  ; -> 0x080263e0
08025fd8  2e60      str          r6, [r5]
08025fda  0e46      mov          r6, r1
08025fdc  2260      str          r2, [r4]
08025fde  bee5      b            #-1156  ; -> 0x08025b5e
08025fe0  b7ee007a  vmov.f32     s14, #1.000000e+00
08025fe4  07ee902a  vmov         s15, r2
08025fe8  4c48      ldr          r0, [pc, #304]  ; [0x0802611c] = 0x20001b54 (f32=1.08510638e-19)
08025fea  f8eee76a  vcvt.f32.s32 s13, s15
08025fee  dfed4c7a  vldr         s15, [pc, #304]  ; [0x08026120] = 0x3a400c01 (f32=0.000732600747)
08025ff2  b0ee476a  vmov.f32     s12, s14
08025ff6  0446      mov          r4, r0
08025ff8  4a48      ldr          r0, [pc, #296]  ; [0x08026124] = 0x20004ddc (f32=1.08677832e-19)
08025ffa  2468      ldr          r4, [r4]
08025ffc  a6eea76a  vfma.f32     s12, s13, s15
08026000  012c      cmp          r4, #1
08026002  f0ee467a  vmov.f32     s15, s12
08026006  80ed006a  vstr         s12, [r0]
0802600a  7df407ae  bne.w        #-9202  ; -> 0x08023c1c
0802600e  464c      ldr          r4, [pc, #280]  ; [0x08026128] = 0x20004c3c (f32=1.08672456e-19)
08026010  2468      ldr          r4, [r4]
08026012  d2eb4402  rsbs         r2, r2, r4, lsl #1
08026016  00f1b183  bmi.w        #1890  ; -> 0x0802677c
0802601a  b2f5805f  cmp.w        r2, #4096
0802601e  c0f21a83  blt.w        #1588  ; -> 0x08026656
08026022  b1ee007a  vmov.f32     s14, #4.000000e+00
08026026  a2f58052  sub.w        r2, r2, #4096
0802602a  9fed406a  vldr         s12, [pc, #256]  ; [0x0802612c] = 0x3b400000 (f32=0.0029296875)
0802602e  06ee902a  vmov         s13, r2
08026032  3f4a      ldr          r2, [pc, #252]  ; [0x08026130] = 0x20003838 (f32=1.08606229e-19)
08026034  f8eee66a  vcvt.f32.s32 s13, s13
08026038  a6ee867a  vfma.f32     s14, s13, s12
0802603c  82ed007a  vstr         s14, [r2]
08026040  fdf7ecbd  b.w          #-9256  ; -> 0x08023c1c
08026044  b3f5606f  cmp.w        r3, #3584
08026048  00f34e84  bgt.w        #2204  ; -> 0x080268e8
0802604c  c3f56063  rsb.w        r3, r3, #3584
08026050  9fed387a  vldr         s14, [pc, #224]  ; [0x08026134] = 0x3d924925 (f32=0.0714285746)
08026054  3849      ldr          r1, [pc, #224]  ; [0x08026138] = 0x0802b1c4 (f32=3.93294145e-34)
08026056  07ee903a  vmov         s15, r3
0802605a  9fed25da  vldr         s26, [pc, #148]  ; [0x080260f0] = 0xbf7ff972 (f32=-0.999899983)
0802605e  f8eee77a  vcvt.f32.s32 s15, s15
08026062  67ee877a  vmul.f32     s15, s15, s14
08026066  fdeee77a  vcvt.s32.f32 s15, s15
0802606a  17ee902a  vmov         r2, s15
0802606e  22eae272  bic.w        r2, r2, r2, asr #31
08026072  fe2a      cmp          r2, #254
08026074  a8bf      it           ge
08026076  fe22      movge        r2, #254
08026078  c2f1ff03  rsb.w        r3, r2, #255
0802607c  23f00303  bic          r3, r3, #3
08026080  0b44      add          r3, r1
08026082  1968      ldr          r1, [r3]
08026084  fdf7e9be  b.w          #-8750  ; -> 0x08023e5a
08026088  2c49      ldr          r1, [pc, #176]  ; [0x0802613c] = 0x0802afc4 (f32=3.93270635e-34)
0802608a  5211      asrs         r2, r2, #5
0802608c  01eb8202  add.w        r2, r1, r2, lsl #2
08026090  d2ed005a  vldr         s11, [r2]
08026094  f5ee405a  vcmp.f32     s11, #0
08026098  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802609c  0dbf      iteet        eq
0802609e  0120      moveq        r0, #1
080260a0  0020      movne        r0, #0
080260a2  0121      movne        r1, #1
080260a4  0021      moveq        r1, #0
080260a6  fdf74fbe  b.w          #-9058  ; -> 0x08023d48
08026144  6328      cmp          r0, #99
08026146  40f6ff72  movw         r2, #4095
0802614a  40f3ec80  ble.w        #472  ; -> 0x08026326
0802614e  b0f5fa7f  cmp.w        r0, #500
08026152  80f2fc83  bge.w        #2040  ; -> 0x0802694e
08026156  121a      subs         r2, r2, r0
08026158  f8eee77a  vcvt.f32.s32 s15, s15
0802615c  1fed087a  vldr         s14, [pc, #-32]  ; [0x08026140] = 0x3a007358 (f32=0.000490000006)
08026160  1211      asrs         r2, r2, #4
08026162  b949      ldr          r1, [pc, #740]  ; [0x08026448] = 0x0802adc4 (f32=3.93247125e-34)
08026164  27ee877a  vmul.f32     s14, s15, s14
08026168  b848      ldr          r0, [pc, #736]  ; [0x0802644c] = 0x20004c4c (f32=1.08672662e-19)
0802616a  c2f1ff02  rsb.w        r2, r2, #255
0802616e  1090      str          r0, [sp, #64]
08026170  5210      asrs         r2, r2, #1
08026172  80ed027a  vstr         s14, [r0, #8]
08026176  01eb8202  add.w        r2, r1, r2, lsl #2
0802617a  d2ed005a  vldr         s11, [r2]
0802617e  fdf73bbe  b.w          #-9098  ; -> 0x08023df8
08026182  b348      ldr          r0, [pc, #716]  ; [0x08026450] = 0x20004c3c (f32=1.08672456e-19)
08026184  0260      str          r2, [r0]
08026186  fdf73bbd  b.w          #-9610  ; -> 0x08023c00
0802618a  f7ee007a  vmov.f32     s15, #1.000000e+00
0802618e  0a46      mov          r2, r1
08026190  fdf731bd  b.w          #-9630  ; -> 0x08023bf6
08026194  1022      movs         r2, #16
08026196  af49      ldr          r1, [pc, #700]  ; [0x08026454] = 0x20003898 (f32=1.0860747e-19)
08026198  3a60      str          r2, [r7]
0802619a  0a68      ldr          r2, [r1]
0802619c  012a      cmp          r2, #1
0802619e  00f09a82  beq.w        #1332  ; -> 0x080266d6
080261a2  07ee905a  vmov         s15, r5
080261a6  9fedac7a  vldr         s14, [pc, #688]  ; [0x08026458] = 0x3b500000 (f32=0.00317382812)
080261aa  0021      movs         r1, #0
080261ac  3560      str          r5, [r6]
080261ae  f8eee77a  vcvt.f32.s32 s15, s15
080261b2  aa4a      ldr          r2, [pc, #680]  ; [0x0802645c] = 0x20004dd0 (f32=1.08677677e-19)
080261b4  aa4e      ldr          r6, [pc, #680]  ; [0x08026460] = 0x20002a94 (f32=1.08561096e-19)
080261b6  ab4f      ldr          r7, [pc, #684]  ; [0x08026464] = 0x20003890 (f32=1.08607367e-19)
080261b8  67ee877a  vmul.f32     s15, s15, s14
080261bc  3160      str          r1, [r6]
080261be  1168      ldr          r1, [r2]
080261c0  fdeee77a  vcvt.s32.f32 s15, s15
080261c4  0129      cmp          r1, #1
080261c6  c7ed007a  vstr         s15, [r7]
080261ca  00f06282  beq.w        #1220  ; -> 0x08026692
080261ce  a649      ldr          r1, [pc, #664]  ; [0x08026468] = 0x20002ac4 (f32=1.08561717e-19)
080261d0  0025      movs         r5, #0
080261d2  0728      cmp          r0, #7
080261d4  0d60      str          r5, [r1]
080261d6  a549      ldr          r1, [pc, #660]  ; [0x0802646c] = 0x20002b54 (f32=1.08563578e-19)
080261d8  0846      mov          r0, r1
080261da  1b91      str          r1, [sp, #108]
080261dc  14bf      ite          ne
080261de  4ff47a71  movne.w      r1, #1000
080261e2  3221      moveq        r1, #50
080261e4  0160      str          r1, [r0]
080261e6  fdf759bc  b.w          #-10062  ; -> 0x08023a9c
080261ea  0122      movs         r2, #1
080261ec  a048      ldr          r0, [pc, #640]  ; [0x08026470] = 0x20003838 (f32=1.08606229e-19)
080261ee  a14d      ldr          r5, [pc, #644]  ; [0x08026474] = 0x20003870 (f32=1.08606953e-19)
080261f0  0068      ldr          r0, [r0]
080261f2  2260      str          r2, [r4]
080261f4  a04c      ldr          r4, [pc, #640]  ; [0x08026478] = 0x20001b30 (f32=1.08510173e-19)
080261f6  2860      str          r0, [r5]
080261f8  2060      str          r0, [r4]
080261fa  a04d      ldr          r5, [pc, #640]  ; [0x0802647c] = 0x20001b64 (f32=1.08510845e-19)
080261fc  a048      ldr          r0, [pc, #640]  ; [0x08026480] = 0x20004c40 (f32=1.08672507e-19)
080261fe  c5ed007a  vstr         s15, [r5]
08026202  c0ed007a  vstr         s15, [r0]
08026206  0a60      str          r2, [r1]
08026208  fdf71fbd  b.w          #-9666  ; -> 0x08023c4a
0802620c  9d4d      ldr          r5, [pc, #628]  ; [0x08026484] = 0x20001a14 (f32=1.08506503e-19)
0802620e  3060      str          r0, [r6]
08026210  2968      ldr          r1, [r5]
08026212  0129      cmp          r1, #1
08026214  00f03282  beq.w        #1124  ; -> 0x0802667c
08026218  286e      ldr          r0, [r5, #96]
0802621a  d5f88c10  ldr.w        r1, [r5, #140]
0802621e  6860      str          r0, [r5, #4]
08026220  e865      str          r0, [r5, #92]
08026222  2963      str          r1, [r5, #48]
08026224  c5f88810  str.w        r1, [r5, #136]
08026228  9749      ldr          r1, [pc, #604]  ; [0x08026488] = 0x20002aa4 (f32=1.08561303e-19)
0802622a  984d      ldr          r5, [pc, #608]  ; [0x0802648c] = 0x20002ae4 (f32=1.0856213e-19)
0802622c  0846      mov          r0, r1
0802622e  0121      movs         r1, #1
08026230  0160      str          r1, [r0]
08026232  2960      str          r1, [r5]
08026234  fdf73cbc  b.w          #-10120  ; -> 0x08023ab0
08026238  9549      ldr          r1, [pc, #596]  ; [0x08026490] = 0x200055c8 (f32=1.08704044e-19)
0802623a  0968      ldr          r1, [r1]
0802623c  0029      cmp          r1, #0
0802623e  7df446ac  bne.w        #-10100  ; -> 0x08023ace
08026242  944f      ldr          r7, [pc, #592]  ; [0x08026494] = 0x20002b50 (f32=1.08563526e-19)
08026244  3a68      ldr          r2, [r7]
08026246  012a      cmp          r2, #1
08026248  00f09f82  beq.w        #1342  ; -> 0x0802678a
0802624c  5a69      ldr          r2, [r3, #20]
0802624e  9fed927a  vldr         s14, [pc, #584]  ; [0x08026498] = 0x3f888889 (f32=1.06666672)
08026252  803a      subs         r2, #128
08026254  2860      str          r0, [r5]
08026256  7e48      ldr          r0, [pc, #504]  ; [0x08026450] = 0x20004c3c (f32=1.08672456e-19)
08026258  07ee902a  vmov         s15, r2
0802625c  f8eee77a  vcvt.f32.s32 s15, s15
08026260  67ee877a  vmul.f32     s15, s15, s14
08026264  fdeee77a  vcvt.s32.f32 s15, s15
08026268  17ee902a  vmov         r2, s15
0802626c  c0ed007a  vstr         s15, [r0]
08026270  002a      cmp          r2, #0
08026272  c0f2fd81  blt.w        #1018  ; -> 0x08026670
08026276  b2f5805f  cmp.w        r2, #4096
0802627a  c0f2d782  blt.w        #1454  ; -> 0x0802682c
0802627e  40f6ff72  movw         r2, #4095
08026282  dfed867a  vldr         s15, [pc, #536]  ; [0x0802649c] = 0x407fe9d3 (f32=3.9986465)
08026286  0c21      movs         r1, #12
08026288  854d      ldr          r5, [pc, #532]  ; [0x080264a0] = 0x20003848 (f32=1.08606436e-19)
0802628a  0260      str          r2, [r0]
0802628c  c5ed007a  vstr         s15, [r5]
08026290  844d      ldr          r5, [pc, #528]  ; [0x080264a4] = 0x20004c38 (f32=1.08672404e-19)
08026292  0120      movs         r0, #1
08026294  744a      ldr          r2, [pc, #464]  ; [0x08026468] = 0x20002ac4 (f32=1.08561717e-19)
08026296  2960      str          r1, [r5]
08026298  0221      movs         r1, #2
0802629a  1b9d      ldr          r5, [sp, #108]
0802629c  1160      str          r1, [r2]
0802629e  2860      str          r0, [r5]
080262a0  fff733b8  b.w          #-3994  ; -> 0x0802530a
080262a4  4fea6c01  asr.w        r1, r12, #1
080262a8  dff834e2  ldr.w        lr, [pc, #564]  ; [0x080264e0] = 0x20000048 (f32=1.08421148e-19)
080262ac  7e4d      ldr          r5, [pc, #504]  ; [0x080264a8] = 0x20000078 (f32=1.08421768e-19)
080262ae  bcf1070f  cmp.w        r12, #7
080262b2  4fea8101  lsl.w        r1, r1, #2
080262b6  7d4f      ldr          r7, [pc, #500]  ; [0x080264ac] = 0x20000058 (f32=1.08421355e-19)
080262b8  7d4e      ldr          r6, [pc, #500]  ; [0x080264b0] = 0x20000068 (f32=1.08421561e-19)
080262ba  8e44      add          lr, r1
080262bc  0f44      add          r7, r1
080262be  0e44      add          r6, r1
080262c0  2944      add          r1, r5
080262c2  def800a0  ldr.w        r10, [lr]
080262c6  7b4d      ldr          r5, [pc, #492]  ; [0x080264b4] = 0x20000088 (f32=1.08421975e-19)
080262c8  dff818e2  ldr.w        lr, [pc, #536]  ; [0x080264e4] = 0x20000028 (f32=1.08420734e-19)
080262cc  1544      add          r5, r2
080262ce  7a4c      ldr          r4, [pc, #488]  ; [0x080264b8] = 0x20002ad4 (f32=1.08561924e-19)
080262d0  7244      add          r2, lr
080262d2  d6f800e0  ldr.w        lr, [r6]
080262d6  0968      ldr          r1, [r1]
080262d8  1668      ldr          r6, [r2]
080262da  149a      ldr          r2, [sp, #80]
080262dc  d7f80090  ldr.w        r9, [r7]
080262e0  c2f800c0  str.w        r12, [r2]
080262e4  754a      ldr          r2, [pc, #468]  ; [0x080264bc] = 0x20002ac0 (f32=1.08561665e-19)
080262e6  2f68      ldr          r7, [r5]
080262e8  c2ed007a  vstr         s15, [r2]
080262ec  4ff00102  mov.w        r2, #1
080262f0  e160      str          r1, [r4, #12]
080262f2  734d      ldr          r5, [pc, #460]  ; [0x080264c0] = 0x200037f0 (f32=1.08605299e-19)
080262f4  7349      ldr          r1, [pc, #460]  ; [0x080264c4] = 0x200038a0 (f32=1.08607574e-19)
080262f6  c4f800a0  str.w        r10, [r4]
080262fa  c4f80490  str.w        r9, [r4, #4]
080262fe  c4f808e0  str.w        lr, [r4, #8]
08026302  2f60      str          r7, [r5]
08026304  0e60      str          r6, [r1]
08026306  cbf80020  str.w        r2, [r11]
0802630a  0260      str          r2, [r0]
0802630c  00f09480  beq.w        #296  ; -> 0x08026438
08026310  bcfa8cf2  clz          r2, r12
08026314  5209      lsrs         r2, r2, #5
08026316  002a      cmp          r2, #0
08026318  40f08e80  bne.w        #284  ; -> 0x08026438
0802631c  6a49      ldr          r1, [pc, #424]  ; [0x080264c8] = 0x20001ac4 (f32=1.08508777e-19)
0802631e  6646      mov          r6, r12
08026320  0a60      str          r2, [r1]
08026322  fdf749bc  b.w          #-10094  ; -> 0x08023bb8
08026326  121a      subs         r2, r2, r0
08026328  4749      ldr          r1, [pc, #284]  ; [0x08026448] = 0x0802adc4 (f32=3.93247125e-34)
0802632a  1211      asrs         r2, r2, #4
0802632c  c2f1ff02  rsb.w        r2, r2, #255
08026330  5210      asrs         r2, r2, #1
08026332  01eb8202  add.w        r2, r1, r2, lsl #2
08026336  d2ed005a  vldr         s11, [r2]
0802633a  fdf757bd  b.w          #-9554  ; -> 0x08023dec
0802633e  6349      ldr          r1, [pc, #396]  ; [0x080264cc] = 0x08028dc4 (f32=3.92870967e-34)
08026340  c2f30a00  ubfx         r0, r2, #0, #11
08026344  b2f5006f  cmp.w        r2, #2048
08026348  01eb8001  add.w        r1, r1, r0, lsl #2
0802634c  d1ed007a  vldr         s15, [r1]
08026350  fdf651ac  blt.w        #-10078  ; -> 0x08023bf6
08026354  77eea77a  vadd.f32     s15, s15, s15
08026358  fdf74dbc  b.w          #-10086  ; -> 0x08023bf6
0802635c  002b      cmp          r3, #0
0802635e  7ff4e8ad  bne.w        #-1072  ; -> 0x08025f32
08026362  bfee00ca  vmov.f32     s24, #-1.000000e+00
08026366  fdf792bd  b.w          #-9436  ; -> 0x08023e8e
0802636a  0029      cmp          r1, #0
0802636c  7df4afab  bne.w        #-10402  ; -> 0x08023ace
08026370  484f      ldr          r7, [pc, #288]  ; [0x08026494] = 0x20002b50 (f32=1.08563526e-19)
08026372  3a68      ldr          r2, [r7]
08026374  012a      cmp          r2, #1
08026376  00f01682  beq.w        #1068  ; -> 0x080267a6
0802637a  404e      ldr          r6, [pc, #256]  ; [0x0802647c] = 0x20001b64 (f32=1.08510845e-19)
0802637c  3d4a      ldr          r2, [pc, #244]  ; [0x08026474] = 0x20003870 (f32=1.08606953e-19)
0802637e  3746      mov          r7, r6
08026380  2960      str          r1, [r5]
08026382  d2ed006a  vldr         s13, [r2]
08026386  1646      mov          r6, r2
08026388  97ed007a  vldr         s14, [r7]
0802638c  3a4a      ldr          r2, [pc, #232]  ; [0x08026478] = 0x20001b30 (f32=1.08510173e-19)
0802638e  c6ee877a  vdiv.f32     s15, s13, s14
08026392  3549      ldr          r1, [pc, #212]  ; [0x08026468] = 0x20002ac4 (f32=1.08561717e-19)
08026394  1546      mov          r5, r2
08026396  364a      ldr          r2, [pc, #216]  ; [0x08026470] = 0x20003838 (f32=1.08606229e-19)
08026398  0860      str          r0, [r1]
0802639a  c6ed007a  vstr         s15, [r6]
0802639e  c5ed007a  vstr         s15, [r5]
080263a2  c2ed007a  vstr         s15, [r2]
080263a6  fef7b0bf  b.w          #-4256  ; -> 0x0802530a
080263aa  f1ee6dda  vneg.f32     s27, s27
080263ae  9fed48ca  vldr         s24, [pc, #288]  ; [0x080264d0] = 0x00000000 (f32=0)
080263b2  fdf76cbd  b.w          #-9512  ; -> 0x08023e8e
080263b6  0122      movs         r2, #1
080263b8  0260      str          r2, [r0]
080263ba  fdf700bd  b.w          #-9728  ; -> 0x08023dbe
080263be  9fed45da  vldr         s26, [pc, #276]  ; [0x080264d4] = 0xbf7ff972 (f32=-0.999899983)
080263c2  fdf74abd  b.w          #-9580  ; -> 0x08023e5a
080263c6  049b      ldr          r3, [sp, #16]
080263c8  b7ee00ca  vmov.f32     s24, #1.000000e+00
080263cc  dfed407a  vldr         s15, [pc, #256]  ; [0x080264d0] = 0x00000000 (f32=0)
080263d0  012b      cmp          r3, #1
080263d2  dfed41da  vldr         s27, [pc, #260]  ; [0x080264d8] = 0x80000000 (f32=-0)
080263d6  18bf      it           ne
080263d8  b0ee67ca  vmovne.f32   s24, s15
080263dc  fdf757bd  b.w          #-9554  ; -> 0x08023e8e
080263e0  0498      ldr          r0, [sp, #16]
080263e2  0128      cmp          r0, #1
080263e4  00f08681  beq.w        #780  ; -> 0x080266f4
080263e8  3c48      ldr          r0, [pc, #240]  ; [0x080264dc] = 0x200037f4 (f32=1.08605351e-19)
080263ea  0068      ldr          r0, [r0]
080263ec  0128      cmp          r0, #1
080263ee  00f0de81  beq.w        #956  ; -> 0x080267ae
080263f2  0e46      mov          r6, r1
080263f4  fff7b3bb  b.w          #-2202  ; -> 0x08025b5e
080263f8  0023      movs         r3, #0
080263fa  2ce5      b            #-1448  ; -> 0x08025e56
080263fc  0499      ldr          r1, [sp, #16]
080263fe  0129      cmp          r1, #1
08026400  00f06f81  beq.w        #734  ; -> 0x080266e2
08026404  3549      ldr          r1, [pc, #212]  ; [0x080264dc] = 0x200037f4 (f32=1.08605351e-19)
08026406  0968      ldr          r1, [r1]
08026408  0129      cmp          r1, #1
0802640a  7ff45eab  bne.w        #-2372  ; -> 0x08025aca
0802640e  0260      str          r2, [r0]
08026410  1546      mov          r5, r2
08026412  fff75bbb  b.w          #-2378  ; -> 0x08025acc
08026416  0e4c      ldr          r4, [pc, #56]  ; [0x08026450] = 0x20004c3c (f32=1.08672456e-19)
08026418  2468      ldr          r4, [r4]
0802641a  d2eb4402  rsbs         r2, r2, r4, lsl #1
0802641e  00f1aa81  bmi.w        #852  ; -> 0x08026776
08026422  b2f5805f  cmp.w        r2, #4096
08026426  c0f29881  blt.w        #816  ; -> 0x0802675a
0802642a  9fed1c7a  vldr         s14, [pc, #112]  ; [0x0802649c] = 0x407fe9d3 (f32=3.9986465)
0802642e  104a      ldr          r2, [pc, #64]  ; [0x08026470] = 0x20003838 (f32=1.08606229e-19)
08026430  82ed007a  vstr         s14, [r2]
08026434  fdf7f2bb  b.w          #-10268  ; -> 0x08023c1c
08026438  234a      ldr          r2, [pc, #140]  ; [0x080264c8] = 0x20001ac4 (f32=1.08508777e-19)
0802643a  6646      mov          r6, r12
0802643c  1146      mov          r1, r2
0802643e  0122      movs         r2, #1
08026440  0a60      str          r2, [r1]
08026442  fdf7b9bb  b.w          #-10382  ; -> 0x08023bb8
080264e8  b34a      ldr          r2, [pc, #716]  ; [0x080267b8] = 0x2000383c (f32=1.08606281e-19)
080264ea  0027      movs         r7, #0
080264ec  0460      str          r4, [r0]
080264ee  1568      ldr          r5, [r2]
080264f0  dff830e3  ldr.w        lr, [pc, #816]  ; [0x08026824] = 0x20003864 (f32=1.08606798e-19)
080264f4  701b      subs         r0, r6, r5
080264f6  ed11      asrs         r5, r5, #7
080264f8  0f60      str          r7, [r1]
080264fa  a842      cmp          r0, r5
080264fc  cef80060  str.w        r6, [lr]
08026500  00f30181  bgt.w        #514  ; -> 0x08026706
08026504  6d42      rsbs         r5, r5, #0
08026506  a842      cmp          r0, r5
08026508  c0f2fd80  blt.w        #506  ; -> 0x08026706
0802650c  ab49      ldr          r1, [pc, #684]  ; [0x080267bc] = 0x20003868 (f32=1.0860685e-19)
0802650e  1b98      ldr          r0, [sp, #108]
08026510  0f91      str          r1, [sp, #60]
08026512  1660      str          r6, [r2]
08026514  d0f80080  ldr.w        r8, [r0]
08026518  0a68      ldr          r2, [r1]
0802651a  a949      ldr          r1, [pc, #676]  ; [0x080267c0] = 0x20004f94 (f32=1.08683519e-19)
0802651c  cef80070  str.w        r7, [lr]
08026520  0591      str          r1, [sp, #20]
08026522  a849      ldr          r1, [pc, #672]  ; [0x080267c4] = 0x20002b44 (f32=1.08563371e-19)
08026524  0846      mov          r0, r1
08026526  40f64e21  movw         r1, #2638
0802652a  d0ed007a  vldr         s15, [r0]
0802652e  a648      ldr          r0, [pc, #664]  ; [0x080267c8] = 0x20002a94 (f32=1.08561096e-19)
08026530  fdeee77a  vcvt.s32.f32 s15, s15
08026534  17ee904a  vmov         r4, s15
08026538  121b      subs         r2, r2, r4
0802653a  0260      str          r2, [r0]
0802653c  ef32      adds         r2, #239
0802653e  8a42      cmp          r2, r1
08026540  09d8      bhi          #18  ; -> 0x08026556
08026542  a249      ldr          r1, [pc, #648]  ; [0x080267cc] = 0x20002aac (f32=1.08561407e-19)
08026544  0022      movs         r2, #0
08026546  0f9c      ldr          r4, [sp, #60]
08026548  0846      mov          r0, r1
0802654a  4ff4b471  mov.w        r1, #360
0802654e  2260      str          r2, [r4]
08026550  0260      str          r2, [r0]
08026552  9f4a      ldr          r2, [pc, #636]  ; [0x080267d0] = 0x20004e80 (f32=1.08679952e-19)
08026554  1160      str          r1, [r2]
08026556  711e      subs         r1, r6, #1
08026558  4bf67e32  movw         r2, #47998
0802655c  9142      cmp          r1, r2
0802655e  00f21881  bhi.w        #560  ; -> 0x08026792
08026562  f0ee006a  vmov.f32     s13, #2.000000e+00
08026566  07ee906a  vmov         s15, r6
0802656a  f1ee005a  vmov.f32     s11, #4.000000e+00
0802656e  994a      ldr          r2, [pc, #612]  ; [0x080267d4] = 0x200000a8 (f32=1.08422389e-19)
08026570  b8eee77a  vcvt.f32.s32 s14, s15
08026574  b0ee665a  vmov.f32     s10, s13
08026578  d2ed007a  vldr         s15, [r2]
0802657c  b2ee006a  vmov.f32     s12, #8.000000e+00
08026580  a7ee255a  vfma.f32     s10, s14, s11
08026584  65ee277a  vmul.f32     s15, s10, s15
08026588  b0ee457a  vmov.f32     s14, s10
0802658c  f4eec67a  vcmpe.f32    s15, s12
08026590  f1ee10fa  vmrs         APSR_nzcv, fpscr
08026594  04d8      bhi          #8  ; -> 0x080265a0
08026596  f4eee67a  vcmpe.f32    s15, s13
0802659a  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802659e  39da      bge          #114  ; -> 0x08026614
080265a0  d2ed017a  vldr         s15, [r2, #4]
080265a4  f2ee006a  vmov.f32     s13, #8.000000e+00
080265a8  67ee277a  vmul.f32     s15, s14, s15
080265ac  f4eee67a  vcmpe.f32    s15, s13
080265b0  f1ee10fa  vmrs         APSR_nzcv, fpscr
080265b4  06d8      bhi          #12  ; -> 0x080265c4
080265b6  f0ee006a  vmov.f32     s13, #2.000000e+00
080265ba  f4eee67a  vcmpe.f32    s15, s13
080265be  f1ee10fa  vmrs         APSR_nzcv, fpscr
080265c2  27da      bge          #78  ; -> 0x08026614
080265c4  d2ed027a  vldr         s15, [r2, #8]
080265c8  f2ee006a  vmov.f32     s13, #8.000000e+00
080265cc  67ee277a  vmul.f32     s15, s14, s15
080265d0  f4eee67a  vcmpe.f32    s15, s13
080265d4  f1ee10fa  vmrs         APSR_nzcv, fpscr
080265d8  06d8      bhi          #12  ; -> 0x080265e8
080265da  f0ee006a  vmov.f32     s13, #2.000000e+00
080265de  f4eee67a  vcmpe.f32    s15, s13
080265e2  f1ee10fa  vmrs         APSR_nzcv, fpscr
080265e6  15da      bge          #42  ; -> 0x08026614
080265e8  d2ed037a  vldr         s15, [r2, #12]
080265ec  f2ee006a  vmov.f32     s13, #8.000000e+00
080265f0  67ee277a  vmul.f32     s15, s14, s15
080265f4  f4eee67a  vcmpe.f32    s15, s13
080265f8  f1ee10fa  vmrs         APSR_nzcv, fpscr
080265fc  06d8      bhi          #12  ; -> 0x0802660c
080265fe  f0ee006a  vmov.f32     s13, #2.000000e+00
08026602  f4eee67a  vcmpe.f32    s15, s13
08026606  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802660a  03da      bge          #6  ; -> 0x08026614
0802660c  d2ed047a  vldr         s15, [r2, #16]
08026610  67ee277a  vmul.f32     s15, s14, s15
08026614  7049      ldr          r1, [pc, #448]  ; [0x080267d8] = 0x20004dd4 (f32=1.08677729e-19)
08026616  714a      ldr          r2, [pc, #452]  ; [0x080267dc] = 0x20002b60 (f32=1.08563733e-19)
08026618  0e68      ldr          r6, [r1]
0802661a  c2ed007a  vstr         s15, [r2]
0802661e  1491      str          r1, [sp, #80]
08026620  2eb9      cbnz         r6, #10  ; -> 0x0802662e
08026622  b5ee007a  vmov.f32     s14, #2.500000e-01
08026626  67ee877a  vmul.f32     s15, s15, s14
0802662a  c2ed007a  vstr         s15, [r2]
0802662e  6c4a      ldr          r2, [pc, #432]  ; [0x080267e0] = 0x20004d78 (f32=1.0867654e-19)
08026630  b8f1000f  cmp.w        r8, #0
08026634  1146      mov          r1, r2
08026636  4ff00102  mov.w        r2, #1
0802663a  0a60      str          r2, [r1]
0802663c  05dd      ble          #10  ; -> 0x0802664a
0802663e  0022      movs         r2, #0
08026640  1b99      ldr          r1, [sp, #108]
08026642  9046      mov          r8, r2
08026644  0a60      str          r2, [r1]
08026646  0f99      ldr          r1, [sp, #60]
08026648  0a60      str          r2, [r1]
0802664a  6649      ldr          r1, [pc, #408]  ; [0x080267e4] = 0x20004dc4 (f32=1.08677522e-19)
0802664c  0a68      ldr          r2, [r1]
0802664e  0132      adds         r2, #1
08026650  0a60      str          r2, [r1]
08026652  fdf77dba  b.w          #-11014  ; -> 0x08023b50
08026656  06ee902a  vmov         s13, r2
0802665a  9fed636a  vldr         s12, [pc, #396]  ; [0x080267e8] = 0x3a400000 (f32=0.000732421875)
0802665e  634a      ldr          r2, [pc, #396]  ; [0x080267ec] = 0x20003838 (f32=1.08606229e-19)
08026660  f8eee66a  vcvt.f32.s32 s13, s13
08026664  a6ee867a  vfma.f32     s14, s13, s12
08026668  82ed007a  vstr         s14, [r2]
0802666c  fdf7d6ba  b.w          #-10836  ; -> 0x08023c1c
08026670  4ff07e55  mov.w        r5, #1065353216
08026674  5e4a      ldr          r2, [pc, #376]  ; [0x080267f0] = 0x20003848 (f32=1.08606436e-19)
08026676  0160      str          r1, [r0]
08026678  1560      str          r5, [r2]
0802667a  09e6      b            #-1006  ; -> 0x08026290
0802667c  a868      ldr          r0, [r5, #8]
0802667e  696b      ldr          r1, [r5, #52]
08026680  6860      str          r0, [r5, #4]
08026682  e865      str          r0, [r5, #92]
08026684  2963      str          r1, [r5, #48]
08026686  c5f88810  str.w        r1, [r5, #136]
0802668a  cde5      b            #-1126  ; -> 0x08026228
0802668c  b160      str          r1, [r6, #8]
0802668e  7163      str          r1, [r6, #52]
08026690  cae5      b            #-1132  ; -> 0x08026228
08026692  ee43      mvns         r6, r5
08026694  5749      ldr          r1, [pc, #348]  ; [0x080267f4] = 0x08028dc4 (f32=3.92870967e-34)
08026696  f6ee006a  vmov.f32     s13, #5.000000e-01
0802669a  b5f5006f  cmp.w        r5, #2048
0802669e  c6f30a06  ubfx         r6, r6, #0, #11
080266a2  01eb8601  add.w        r1, r1, r6, lsl #2
080266a6  91ed007a  vldr         s14, [r1]
080266aa  27ee267a  vmul.f32     s14, s14, s13
080266ae  fdeec77a  vcvt.s32.f32 s15, s14
080266b2  05db      blt          #10  ; -> 0x080266c0
080266b4  f8eee77a  vcvt.f32.s32 s15, s15
080266b8  67eea67a  vmul.f32     s15, s15, s13
080266bc  fdeee77a  vcvt.s32.f32 s15, s15
080266c0  4b4d      ldr          r5, [pc, #300]  ; [0x080267f0] = 0x20003848 (f32=1.08606436e-19)
080266c2  f8eee77a  vcvt.f32.s32 s15, s15
080266c6  4c49      ldr          r1, [pc, #304]  ; [0x080267f8] = 0x20001b30 (f32=1.08510173e-19)
080266c8  95ed007a  vldr         s14, [r5]
080266cc  67ee877a  vmul.f32     s15, s15, s14
080266d0  c1ed007a  vstr         s15, [r1]
080266d4  7be5      b            #-1290  ; -> 0x080261ce
080266d6  424a      ldr          r2, [pc, #264]  ; [0x080267e0] = 0x20004d78 (f32=1.0867654e-19)
080266d8  1746      mov          r7, r2
080266da  0022      movs         r2, #0
080266dc  3a60      str          r2, [r7]
080266de  0a60      str          r2, [r1]
080266e0  5fe5      b            #-1346  ; -> 0x080261a2
080266e2  0c46      mov          r4, r1
080266e4  4549      ldr          r1, [pc, #276]  ; [0x080267fc] = 0x200038a0 (f32=1.08607574e-19)
080266e6  1d46      mov          r5, r3
080266e8  454b      ldr          r3, [pc, #276]  ; [0x08026800] = 0x20004c70 (f32=1.08673128e-19)
080266ea  0968      ldr          r1, [r1]
080266ec  0460      str          r4, [r0]
080266ee  1960      str          r1, [r3]
080266f0  fff7ecb9  b.w          #-3112  ; -> 0x08025acc
080266f4  414a      ldr          r2, [pc, #260]  ; [0x080267fc] = 0x200038a0 (f32=1.08607574e-19)
080266f6  0e46      mov          r6, r1
080266f8  4249      ldr          r1, [pc, #264]  ; [0x08026804] = 0x20002aa8 (f32=1.08561355e-19)
080266fa  1068      ldr          r0, [r2]
080266fc  049a      ldr          r2, [sp, #16]
080266fe  0860      str          r0, [r1]
08026700  2260      str          r2, [r4]
08026702  fff72cba  b.w          #-2984  ; -> 0x08025b5e
08026706  4049      ldr          r1, [pc, #256]  ; [0x08026808] = 0x20003890 (f32=1.08607367e-19)
08026708  0020      movs         r0, #0
0802670a  1660      str          r6, [r2]
0802670c  0968      ldr          r1, [r1]
0802670e  3f4a      ldr          r2, [pc, #252]  ; [0x0802680c] = 0x20001b28 (f32=1.0851007e-19)
08026710  3f4c      ldr          r4, [pc, #252]  ; [0x08026810] = 0x20002ae4 (f32=1.0856213e-19)
08026712  1b9d      ldr          r5, [sp, #108]
08026714  1160      str          r1, [r2]
08026716  2860      str          r0, [r5]
08026718  2268      ldr          r2, [r4]
0802671a  3e48      ldr          r0, [pc, #248]  ; [0x08026814] = 0x20001b44 (f32=1.08510432e-19)
0802671c  0160      str          r1, [r0]
0802671e  a2b9      cbnz         r2, #40  ; -> 0x0802674a
08026720  3d49      ldr          r1, [pc, #244]  ; [0x08026818] = 0x20002aa4 (f32=1.08561303e-19)
08026722  0a68      ldr          r2, [r1]
08026724  8ab9      cbnz         r2, #34  ; -> 0x0802674a
08026726  264a      ldr          r2, [pc, #152]  ; [0x080267c0] = 0x20004f94 (f32=1.08683519e-19)
08026728  0592      str          r2, [sp, #20]
0802672a  1268      ldr          r2, [r2]
0802672c  7ab9      cbnz         r2, #30  ; -> 0x0802674e
0802672e  3b4a      ldr          r2, [pc, #236]  ; [0x0802681c] = 0x200037f4 (f32=1.08605351e-19)
08026730  1268      ldr          r2, [r2]
08026732  62b9      cbnz         r2, #24  ; -> 0x0802674e
08026734  2148      ldr          r0, [pc, #132]  ; [0x080267bc] = 0x20003868 (f32=1.0860685e-19)
08026736  9046      mov          r8, r2
08026738  244f      ldr          r7, [pc, #144]  ; [0x080267cc] = 0x20002aac (f32=1.08561407e-19)
0802673a  0546      mov          r5, r0
0802673c  0f90      str          r0, [sp, #60]
0802673e  0120      movs         r0, #1
08026740  3a60      str          r2, [r7]
08026742  2a60      str          r2, [r5]
08026744  0860      str          r0, [r1]
08026746  2060      str          r0, [r4]
08026748  ebe6      b            #-554  ; -> 0x08026522
0802674a  1d4a      ldr          r2, [pc, #116]  ; [0x080267c0] = 0x20004f94 (f32=1.08683519e-19)
0802674c  0592      str          r2, [sp, #20]
0802674e  1b4a      ldr          r2, [pc, #108]  ; [0x080267bc] = 0x20003868 (f32=1.0860685e-19)
08026750  4ff00008  mov.w        r8, #0
08026754  0f92      str          r2, [sp, #60]
08026756  1268      ldr          r2, [r2]
08026758  e3e6      b            #-570  ; -> 0x08026522
0802675a  264c      ldr          r4, [pc, #152]  ; [0x080267f4] = 0x08028dc4 (f32=3.92870967e-34)
0802675c  c2f30a05  ubfx         r5, r2, #0, #11
08026760  b2f5006f  cmp.w        r2, #2048
08026764  04eb8504  add.w        r4, r4, r5, lsl #2
08026768  94ed007a  vldr         s14, [r4]
0802676c  fff65fae  blt.w        #-834  ; -> 0x0802642e
08026770  37ee077a  vadd.f32     s14, s14, s14
08026774  5be6      b            #-842  ; -> 0x0802642e
08026776  b7ee007a  vmov.f32     s14, #1.000000e+00
0802677a  58e6      b            #-848  ; -> 0x0802642e
0802677c  02f58052  add.w        r2, r2, #4096
08026780  b5ee007a  vmov.f32     s14, #2.500000e-01
08026784  9fed266a  vldr         s12, [pc, #152]  ; [0x08026820] = 0x39400000 (f32=0.000183105469)
08026788  51e4      b            #-1886  ; -> 0x0802602e
0802678a  3960      str          r1, [r7]
0802678c  3160      str          r1, [r6]
0802678e  fef7bcbd  b.w          #-5256  ; -> 0x0802530a
08026792  0e49      ldr          r1, [pc, #56]  ; [0x080267cc] = 0x20002aac (f32=1.08561407e-19)
08026794  0022      movs         r2, #0
08026796  0f9c      ldr          r4, [sp, #60]
08026798  0846      mov          r0, r1
0802679a  0f49      ldr          r1, [pc, #60]  ; [0x080267d8] = 0x20004dd4 (f32=1.08677729e-19)
0802679c  2260      str          r2, [r4]
0802679e  1491      str          r1, [sp, #80]
080267a0  0260      str          r2, [r0]
080267a2  0e68      ldr          r6, [r1]
080267a4  51e7      b            #-350  ; -> 0x0802664a
080267a6  3960      str          r1, [r7]
080267a8  3260      str          r2, [r6]
080267aa  fef7aebd  b.w          #-5284  ; -> 0x0802530a
080267ae  2260      str          r2, [r4]
080267b0  1646      mov          r6, r2
080267b2  fff7d4b9  b.w          #-3160  ; -> 0x08025b5e
0802682c  f8eee77a  vcvt.f32.s32 s15, s15
08026830  1fed037a  vldr         s14, [pc, #-12]  ; [0x08026828] = 0x3b500000 (f32=0.00317382812)
08026834  4c49      ldr          r1, [pc, #304]  ; [0x08026968] = 0x08028dc4 (f32=3.92870967e-34)
08026836  c2f30a00  ubfx         r0, r2, #0, #11
0802683a  4c4d      ldr          r5, [pc, #304]  ; [0x0802696c] = 0x20003848 (f32=1.08606436e-19)
0802683c  b2f5006f  cmp.w        r2, #2048
08026840  27ee877a  vmul.f32     s14, s15, s14
08026844  01eb8001  add.w        r1, r1, r0, lsl #2
08026848  d1ed007a  vldr         s15, [r1]
0802684c  bdeec77a  vcvt.s32.f32 s14, s14
08026850  c5ed007a  vstr         s15, [r5]
08026854  17ee101a  vmov         r1, s14
08026858  fff61aad  blt.w        #-1484  ; -> 0x08026290
0802685c  77eea77a  vadd.f32     s15, s15, s15
08026860  14e5      b            #-1496  ; -> 0x0802628c
08026862  c3ed007a  vstr         s15, [r3]
08026866  fdf79ebb  b.w          #-10436  ; -> 0x08023fa6
0802686a  c1ed007a  vstr         s15, [r1]
0802686e  fdf796bb  b.w          #-10452  ; -> 0x08023f9e
08026872  b7ee085a  vmov.f32     s10, #1.500000e+00
08026876  fdf780bc  b.w          #-9984  ; -> 0x0802417a
0802687a  f4ee675a  vcmp.f32     s11, s15
0802687e  cbed085a  vstr         s11, [r11, #32]
08026882  37eee57a  vsub.f32     s14, s15, s11
08026886  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802688a  8bed1e7a  vstr         s14, [r11, #120]
0802688e  3df400ae  beq.w        #-9216  ; -> 0x08024492
08026892  f5ee405a  vcmp.f32     s11, #0
08026896  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802689a  3df4faad  beq.w        #-9228  ; -> 0x08024492
0802689e  0221      movs         r1, #2
080268a0  0298      ldr          r0, [sp, #8]
080268a2  0160      str          r1, [r0]
080268a4  fdf7f5bd  b.w          #-9238  ; -> 0x08024492
080268a8  27eea73a  vmul.f32     s6, s15, s15
080268ac  b3ee0b7a  vmov.f32     s14, #2.700000e+01
080268b0  f2ee022a  vmov.f32     s5, #9.000000e+00
080268b4  73ee075a  vadd.f32     s11, s6, s14
080268b8  a3ee227a  vfma.f32     s14, s6, s5
080268bc  65eea75a  vmul.f32     s11, s11, s15
080268c0  c5ee877a  vdiv.f32     s15, s11, s14
080268c4  fdf743bd  b.w          #-9594  ; -> 0x0802434e
080268c8  27eea73a  vmul.f32     s6, s15, s15
080268cc  b3ee0b7a  vmov.f32     s14, #2.700000e+01
080268d0  f2ee022a  vmov.f32     s5, #9.000000e+00
080268d4  73ee075a  vadd.f32     s11, s6, s14
080268d8  a3ee227a  vfma.f32     s14, s6, s5
080268dc  65eea75a  vmul.f32     s11, s11, s15
080268e0  c5ee877a  vdiv.f32     s15, s11, s14
080268e4  fdf763bd  b.w          #-9530  ; -> 0x080243ae
080268e8  c3f56062  rsb.w        r2, r3, #3584
080268ec  9fed207a  vldr         s14, [pc, #128]  ; [0x08026970] = 0x3d924925 (f32=0.0714285746)
080268f0  2048      ldr          r0, [pc, #128]  ; [0x08026974] = 0x0802b1c4 (f32=3.93294145e-34)
080268f2  07ee902a  vmov         s15, r2
080268f6  f8eee77a  vcvt.f32.s32 s15, s15
080268fa  67ee877a  vmul.f32     s15, s15, s14
080268fe  fdeee77a  vcvt.s32.f32 s15, s15
08026902  17ee902a  vmov         r2, s15
08026906  22eae272  bic.w        r2, r2, r2, asr #31
0802690a  fe2a      cmp          r2, #254
0802690c  a8bf      it           ge
0802690e  fe22      movge        r2, #254
08026910  c2f1ff01  rsb.w        r1, r2, #255
08026914  21f00301  bic          r1, r1, #3
08026918  0144      add          r1, r0
0802691a  0968      ldr          r1, [r1]
0802691c  fff711bb  b.w          #-2526  ; -> 0x08025f42
08026920  f5ee404a  vcmp.f32     s9, #0
08026924  cbed134a  vstr         s9, [r11, #76]
08026928  77eee45a  vsub.f32     s11, s15, s9
0802692c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08026930  cbed295a  vstr         s11, [r11, #164]
08026934  3df435ae  beq.w        #-9110  ; -> 0x080245a2
08026938  f4ee674a  vcmp.f32     s9, s15
0802693c  f1ee10fa  vmrs         APSR_nzcv, fpscr
08026940  3df42fae  beq.w        #-9122  ; -> 0x080245a2
08026944  0223      movs         r3, #2
08026946  039a      ldr          r2, [sp, #12]
08026948  1360      str          r3, [r2]
0802694a  fdf72abe  b.w          #-9132  ; -> 0x080245a2
0802694e  121a      subs         r2, r2, r0
08026950  0949      ldr          r1, [pc, #36]  ; [0x08026978] = 0x0802adc4 (f32=3.93247125e-34)
08026952  1211      asrs         r2, r2, #4
08026954  c2f1ff02  rsb.w        r2, r2, #255
08026958  5210      asrs         r2, r2, #1
0802695a  01eb8202  add.w        r2, r1, r2, lsl #2
0802695e  d2ed005a  vldr         s11, [r2]
08026962  fff701bb  b.w          #-2558  ; -> 0x08025f68
