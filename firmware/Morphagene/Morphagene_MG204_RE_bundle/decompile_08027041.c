extern unsigned int g_20021c68;
extern unsigned int g_20021c6c;
extern unsigned int g_20021c70;
extern unsigned int g_20021c74;
extern unsigned int g_20021c78;
extern unsigned int g_20021c7c;
extern unsigned int g_20021df4;
extern unsigned int g_20021e64;
extern unsigned int g_20021e68;
extern unsigned int g_20021e6c;
extern unsigned int g_20021e70;
extern unsigned int g_20021e74;
extern unsigned int g_20021e78;
extern unsigned int g_20021e7c;
extern unsigned int g_20021e80;
extern unsigned int g_20021f4c;
extern unsigned int g_20021f50;
extern unsigned int g_20021f54;
extern unsigned int g_20021f58;
extern unsigned int g_20021f5c;
extern unsigned int g_20021f60;
extern unsigned int g_20021f64;
extern unsigned int g_20021f68;
extern unsigned int g_20021f70;
extern unsigned int g_20021f74;
extern unsigned int g_20021f78;
extern unsigned int g_20021f7c;
extern unsigned int g_20021f80;
extern unsigned int g_20021f84;
extern unsigned int g_20021f88;
extern unsigned int g_20021f8c;
extern unsigned int g_20021fb8;
extern unsigned int g_20021fbc;
extern unsigned int g_20021fc0;
extern unsigned int g_20021fc4;
extern unsigned int g_20021fc8;
extern unsigned int g_20021fcc;
extern unsigned int g_20021fd0;
extern unsigned int g_20021fd4;
extern unsigned int g_20021fd8;
extern unsigned int g_20021fdc;
extern unsigned int g_20021fe0;
extern unsigned int g_20021fe4;
extern unsigned int g_20021fe8;
extern unsigned int g_20021fec;
extern unsigned int g_20021ff0;
extern unsigned int g_20021ff4;
extern unsigned int g_20021ff8;
extern unsigned int g_20021ffc;
extern unsigned int g_20022000;
extern unsigned int g_20022004;
extern unsigned int g_20022008;
extern unsigned int g_2002200c;
extern unsigned int g_20022010;
extern unsigned int g_20022014;
extern unsigned int g_20022074;
extern unsigned int g_2002208c;
extern unsigned int g_200220a4;
extern unsigned int g_200220a8;
extern unsigned int g_200220ac;
extern unsigned int g_200220b0;
extern unsigned int g_200220b4;
extern unsigned int g_200220b8;
extern unsigned int g_200220bc;
extern unsigned int g_200220c0;
extern unsigned int g_2002215c;
extern unsigned int g_20022160;
extern unsigned int g_20024128;

void sub_8027041(void)
{
    unsigned int v8;  // r2
    unsigned int v9;  // lr
    unsigned int v10;  // r10
    unsigned int v11;  // r9
    unsigned int v12;  // r8
    unsigned int v13;  // r7
    unsigned int v14;  // r6
    unsigned int v15;  // r5
    unsigned int v16;  // r4
    unsigned int iter;  // r8
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v8 = g_2002208c;
    if (!g_2002208c)
        return;
    v7 = v9;
    v6 = v10;
    v5 = v11;
    v4 = v12;
    v3 = v13;
    v2 = v14;
    v1 = v15;
    v0 = v16;
    if (g_2002208c != 1)
    {
        g_20022074 = g_20022074 | 0x4000;
    }
    else
    {
        g_2002215c = 0;
        iter = &g_20022160;
        do
        {
            *((unsigned int *)iter) = 0;
            *((unsigned int *)(iter + 4)) = 0;
            iter += 8;
        } while (iter != 537014520);
        (&g_2002215c)[998 + v8] = 0;
        g_20021df4 = 1066192077;
        g_2002208c = 0;
        if (*(0x20022088) > 0)
        {
            g_20021e64 = 3192704205;
            g_20021f70 = 0;
            g_20021f4c = 0;
            g_20021fb8 = 1;
            g_20021ff8 = 0;
            g_20021fd8 = 0;
            if (*((int *)537010312) != 1)
            {
                g_20021e68 = 3192704205;
                g_20021f74 = 0;
                g_20021f50 = 0;
                g_20021fbc = 1;
                g_20021ffc = 0;
                g_20021fdc = 0;
                if (*((int *)0x20022088) != 2)
                {
                    g_20021e6c = 3192704205;
                    g_20021f78 = 0;
                    g_20021f54 = 0;
                    g_20021fc0 = 1;
                    g_20022000 = 0;
                    g_20021fe0 = 0;
                    if (*((int *)0x20022088) != 3)
                    {
                        g_20021e70 = 3192704205;
                        g_20021f7c = 0;
                        g_20021f58 = 0;
                        g_20021fc4 = 1;
                        g_20022004 = 0;
                        g_20021fe4 = 0;
                        if (*((int *)0x20022088) != 4)
                        {
                            g_20021e74 = 3192704205;
                            g_20021f80 = 0;
                            g_20021f5c = 0;
                            g_20021fc8 = 1;
                            g_20022008 = 0;
                            g_20021fe8 = 0;
                            if (*((int *)0x20022088) != 5)
                            {
                                g_20021e78 = 3192704205;
                                g_20021f84 = 0;
                                g_20021f60 = 0;
                                g_20021fcc = 1;
                                g_2002200c = 0;
                                g_20021fec = 0;
                                if (*((int *)0x20022088) != 6)
                                {
                                    g_20021e7c = 3192704205;
                                    g_20021f88 = 0;
                                    g_20021f64 = 0;
                                    g_20021fd0 = 1;
                                    g_20022010 = 0;
                                    g_20021ff0 = 0;
                                    if (*((int *)0x20022088) != 7)
                                    {
                                        g_20021e80 = 3192704205;
                                        g_20021f8c = 0;
                                        g_20021f68 = 0;
                                        g_20021fd4 = 1;
                                        g_20022014 = 0;
                                        g_20021ff4 = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        g_20022074 = g_20022074 | 0x2000 | 32;
    }
    g_200220a4 = g_20021c68 + 100;
    g_200220a8 = g_20021c6c + 100;
    g_200220ac = g_20021c70 + 100;
    g_200220b0 = g_20021c74 + 100;
    g_200220b4 = g_20021c78 + 100;
    g_200220b8 = g_20021c7c + 100;
    g_200220bc = *((int *)0x20021c80) + 100;
    g_200220c0 = *((int *)0x20021c84) + 100;
    g_20024128 = 480;
    return;
}
