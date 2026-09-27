0802697c  38b5      push         {r3, r4, r5, lr}
0802697e  474d      ldr          r5, [pc, #284]  ; [0x08026a9c] = 0x20001adc (f32=1.08509087e-19)
08026980  0422      movs         r2, #4
08026982  dfed477a  vldr         s15, [pc, #284]  ; [0x08026aa0] = 0x300a3d6d (f32=5.02913988e-10)
08026986  95ed007a  vldr         s14, [r5]
0802698a  464b      ldr          r3, [pc, #280]  ; [0x08026aa4] = 0x20004d7c (f32=1.08676592e-19)
0802698c  b8eec77a  vcvt.f32.s32 s14, s14
08026990  454c      ldr          r4, [pc, #276]  ; [0x08026aa8] = 0x200038a4 (f32=1.08607625e-19)
08026992  1846      mov          r0, r3
08026994  2146      mov          r1, r4
08026996  27ee277a  vmul.f32     s14, s14, s15
0802699a  83ed007a  vstr         s14, [r3]
0802699e  95ed017a  vldr         s14, [r5, #4]
080269a2  b8eec77a  vcvt.f32.s32 s14, s14
080269a6  27ee277a  vmul.f32     s14, s14, s15
080269aa  83ed017a  vstr         s14, [r3, #4]
080269ae  95ed027a  vldr         s14, [r5, #8]
080269b2  b8eec77a  vcvt.f32.s32 s14, s14
080269b6  27ee277a  vmul.f32     s14, s14, s15
080269ba  83ed027a  vstr         s14, [r3, #8]
080269be  95ed037a  vldr         s14, [r5, #12]
080269c2  b8eec77a  vcvt.f32.s32 s14, s14
080269c6  27ee277a  vmul.f32     s14, s14, s15
080269ca  83ed037a  vstr         s14, [r3, #12]
080269ce  95ed047a  vldr         s14, [r5, #16]
080269d2  b8eec77a  vcvt.f32.s32 s14, s14
080269d6  27ee277a  vmul.f32     s14, s14, s15
080269da  83ed047a  vstr         s14, [r3, #16]
080269de  95ed057a  vldr         s14, [r5, #20]
080269e2  b8eec77a  vcvt.f32.s32 s14, s14
080269e6  27ee277a  vmul.f32     s14, s14, s15
080269ea  83ed057a  vstr         s14, [r3, #20]
080269ee  95ed067a  vldr         s14, [r5, #24]
080269f2  b8eec77a  vcvt.f32.s32 s14, s14
080269f6  27ee277a  vmul.f32     s14, s14, s15
080269fa  83ed067a  vstr         s14, [r3, #24]
080269fe  95ed077a  vldr         s14, [r5, #28]
08026a02  b8eec77a  vcvt.f32.s32 s14, s14
08026a06  67ee277a  vmul.f32     s15, s14, s15
08026a0a  c3ed077a  vstr         s15, [r3, #28]
08026a0e  fcf7d1ff  bl           #-12382  ; -> 0x080239b4
08026a12  94ed007a  vldr         s14, [r4]
08026a16  dfed257a  vldr         s15, [pc, #148]  ; [0x08026aac] = 0x4e7ffc00 (f32=1.07367629e+09)
08026a1a  254b      ldr          r3, [pc, #148]  ; [0x08026ab0] = 0x20002ae8 (f32=1.08562182e-19)
08026a1c  27ee277a  vmul.f32     s14, s14, s15
08026a20  bdeec77a  vcvt.s32.f32 s14, s14
08026a24  83ed087a  vstr         s14, [r3, #32]
08026a28  94ed017a  vldr         s14, [r4, #4]
08026a2c  27ee277a  vmul.f32     s14, s14, s15
08026a30  bdeec77a  vcvt.s32.f32 s14, s14
08026a34  83ed097a  vstr         s14, [r3, #36]
08026a38  94ed027a  vldr         s14, [r4, #8]
08026a3c  27ee277a  vmul.f32     s14, s14, s15
08026a40  bdeec77a  vcvt.s32.f32 s14, s14
08026a44  83ed0a7a  vstr         s14, [r3, #40]
08026a48  94ed037a  vldr         s14, [r4, #12]
08026a4c  27ee277a  vmul.f32     s14, s14, s15
08026a50  bdeec77a  vcvt.s32.f32 s14, s14
08026a54  83ed0b7a  vstr         s14, [r3, #44]
08026a58  94ed047a  vldr         s14, [r4, #16]
08026a5c  27ee277a  vmul.f32     s14, s14, s15
08026a60  bdeec77a  vcvt.s32.f32 s14, s14
08026a64  83ed0c7a  vstr         s14, [r3, #48]
08026a68  94ed057a  vldr         s14, [r4, #20]
08026a6c  27ee277a  vmul.f32     s14, s14, s15
08026a70  bdeec77a  vcvt.s32.f32 s14, s14
08026a74  83ed0d7a  vstr         s14, [r3, #52]
08026a78  94ed067a  vldr         s14, [r4, #24]
08026a7c  27ee277a  vmul.f32     s14, s14, s15
08026a80  bdeec77a  vcvt.s32.f32 s14, s14
08026a84  83ed0e7a  vstr         s14, [r3, #56]
08026a88  94ed077a  vldr         s14, [r4, #28]
08026a8c  67ee277a  vmul.f32     s15, s14, s15
08026a90  fdeee77a  vcvt.s32.f32 s15, s15
08026a94  c3ed0f7a  vstr         s15, [r3, #60]
08026a98  38bd      pop          {r3, r4, r5, pc}
08026ab4  38b5      push         {r3, r4, r5, lr}
08026ab6  474d      ldr          r5, [pc, #284]  ; [0x08026bd4] = 0x20001adc (f32=1.08509087e-19)
08026ab8  0422      movs         r2, #4
08026aba  dfed477a  vldr         s15, [pc, #284]  ; [0x08026bd8] = 0x300a3d6d (f32=5.02913988e-10)
08026abe  95ed087a  vldr         s14, [r5, #32]
08026ac2  464b      ldr          r3, [pc, #280]  ; [0x08026bdc] = 0x20004d7c (f32=1.08676592e-19)
08026ac4  b8eec77a  vcvt.f32.s32 s14, s14
08026ac8  454c      ldr          r4, [pc, #276]  ; [0x08026be0] = 0x200038a4 (f32=1.08607625e-19)
08026aca  1846      mov          r0, r3
08026acc  2146      mov          r1, r4
08026ace  27ee277a  vmul.f32     s14, s14, s15
08026ad2  83ed007a  vstr         s14, [r3]
08026ad6  95ed097a  vldr         s14, [r5, #36]
08026ada  b8eec77a  vcvt.f32.s32 s14, s14
08026ade  27ee277a  vmul.f32     s14, s14, s15
08026ae2  83ed017a  vstr         s14, [r3, #4]
08026ae6  95ed0a7a  vldr         s14, [r5, #40]
08026aea  b8eec77a  vcvt.f32.s32 s14, s14
08026aee  27ee277a  vmul.f32     s14, s14, s15
08026af2  83ed027a  vstr         s14, [r3, #8]
08026af6  95ed0b7a  vldr         s14, [r5, #44]
08026afa  b8eec77a  vcvt.f32.s32 s14, s14
08026afe  27ee277a  vmul.f32     s14, s14, s15
08026b02  83ed037a  vstr         s14, [r3, #12]
08026b06  95ed0c7a  vldr         s14, [r5, #48]
08026b0a  b8eec77a  vcvt.f32.s32 s14, s14
08026b0e  27ee277a  vmul.f32     s14, s14, s15
08026b12  83ed047a  vstr         s14, [r3, #16]
08026b16  95ed0d7a  vldr         s14, [r5, #52]
08026b1a  b8eec77a  vcvt.f32.s32 s14, s14
08026b1e  27ee277a  vmul.f32     s14, s14, s15
08026b22  83ed057a  vstr         s14, [r3, #20]
08026b26  95ed0e7a  vldr         s14, [r5, #56]
08026b2a  b8eec77a  vcvt.f32.s32 s14, s14
08026b2e  27ee277a  vmul.f32     s14, s14, s15
08026b32  83ed067a  vstr         s14, [r3, #24]
08026b36  95ed0f7a  vldr         s14, [r5, #60]
08026b3a  b8eec77a  vcvt.f32.s32 s14, s14
08026b3e  67ee277a  vmul.f32     s15, s14, s15
08026b42  c3ed077a  vstr         s15, [r3, #28]
08026b46  fcf735ff  bl           #-12694  ; -> 0x080239b4
08026b4a  94ed007a  vldr         s14, [r4]
08026b4e  dfed257a  vldr         s15, [pc, #148]  ; [0x08026be4] = 0x4e7ffc00 (f32=1.07367629e+09)
08026b52  254b      ldr          r3, [pc, #148]  ; [0x08026be8] = 0x20002ae8 (f32=1.08562182e-19)
08026b54  27ee277a  vmul.f32     s14, s14, s15
08026b58  bdeec77a  vcvt.s32.f32 s14, s14
08026b5c  83ed007a  vstr         s14, [r3]
08026b60  94ed017a  vldr         s14, [r4, #4]
08026b64  27ee277a  vmul.f32     s14, s14, s15
08026b68  bdeec77a  vcvt.s32.f32 s14, s14
08026b6c  83ed017a  vstr         s14, [r3, #4]
08026b70  94ed027a  vldr         s14, [r4, #8]
08026b74  27ee277a  vmul.f32     s14, s14, s15
08026b78  bdeec77a  vcvt.s32.f32 s14, s14
08026b7c  83ed027a  vstr         s14, [r3, #8]
08026b80  94ed037a  vldr         s14, [r4, #12]
08026b84  27ee277a  vmul.f32     s14, s14, s15
08026b88  bdeec77a  vcvt.s32.f32 s14, s14
08026b8c  83ed037a  vstr         s14, [r3, #12]
08026b90  94ed047a  vldr         s14, [r4, #16]
08026b94  27ee277a  vmul.f32     s14, s14, s15
08026b98  bdeec77a  vcvt.s32.f32 s14, s14
08026b9c  83ed047a  vstr         s14, [r3, #16]
08026ba0  94ed057a  vldr         s14, [r4, #20]
08026ba4  27ee277a  vmul.f32     s14, s14, s15
08026ba8  bdeec77a  vcvt.s32.f32 s14, s14
08026bac  83ed057a  vstr         s14, [r3, #20]
08026bb0  94ed067a  vldr         s14, [r4, #24]
08026bb4  27ee277a  vmul.f32     s14, s14, s15
08026bb8  bdeec77a  vcvt.s32.f32 s14, s14
08026bbc  83ed067a  vstr         s14, [r3, #24]
08026bc0  94ed077a  vldr         s14, [r4, #28]
08026bc4  67ee277a  vmul.f32     s15, s14, s15
08026bc8  fdeee77a  vcvt.s32.f32 s15, s15
08026bcc  c3ed077a  vstr         s15, [r3, #28]
08026bd0  38bd      pop          {r3, r4, r5, pc}
