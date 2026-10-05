; lubadh::TapManager::num_engines() const
; VA 0x4ddd0 size 40

   4ddd0: f2c02050     	vmov.i32	q9, #0x0
   4ddd4: e2800eaa     	add	r0, r0, #2720
   4ddd8: f2c04051     	vmov.i32	q10, #0x1
   4dddc: f4600a8f     	vld1.32	{d16, d17}, [r0]
   4dde0: f36008f2     	vceq.i32	q8, q8, q9
   4dde4: f35201f4     	vbsl	q8, q9, q10
   4dde8: f26008a1     	vadd.i32	d16, d16, d17
   4ddec: f2600bb0     	vpadd.i32	d16, d16, d16
   4ddf0: ee100b90     	vmov.32	r0, d16[0]
   4ddf4: e12fff1e     	bx	lr
