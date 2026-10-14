00003470 <map>:
    3470: ee300a60     	vsub.f32	s0, s0, s1
    3474: ee322a61     	vsub.f32	s4, s4, s3
    3478: ee602a02     	vmul.f32	s5, s0, s4
    347c: ee311a60     	vsub.f32	s2, s2, s1
    3480: eec20a81     	vdiv.f32	s1, s5, s2
    3484: ee300aa1     	vadd.f32	s0, s1, s3
    3488: e12fff1e     	bx	lr

