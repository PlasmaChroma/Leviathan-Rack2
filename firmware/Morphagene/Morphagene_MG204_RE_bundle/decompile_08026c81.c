extern unsigned int g_20021c68;
extern unsigned int g_20021c6c;
extern unsigned int g_20021c70;
extern unsigned int g_20021c74;
extern unsigned int g_20021c78;
extern unsigned int g_20021c7c;
extern unsigned int g_20021df4;
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
extern unsigned int g_2002215c[4];
extern unsigned int g_20022160;
extern unsigned int g_20024128;

int sub_8026c81(int a0)
{
    unsigned int v0;  // r3
    unsigned int v1;  // r5
    unsigned int v10;  // r1
    unsigned int v11;  // r6
    unsigned int v2;  // r7
    int v3;  // r2
    unsigned int v4;  // r12
    unsigned int v5;  // r1
    unsigned int idx;  // r3
    unsigned int v7;  // r7
    unsigned int v8;  // r1
    unsigned int v9;  // r7

    v0 = g_2002208c;
    if (300 < g_2002208c)
    {
        return a0;
    }
    else if (a0 <= 0)
    {
        return a0;
    }
    else if (*((int *)537009316) >= a0)
    {
        if (g_2002208c)
        {
            if (g_2002208c >= 0)
            {
                v1 = &g_2002215c[0];
                if (a0 == g_2002215c)
                    return a0;
                do
                {
                    v2 = v1 + 4;
                    if (*((int *)(v1 + 4)) == a0)
                        return a0;
                } while ((v1 = v2, g_2002208c * 4 + 537010524 != v1));
                v3 = g_2002215c[g_2002208c];
                v4 = g_2002208c + 1;
                if (g_2002215c[g_2002208c] >= a0)
                {
                    v5 = &g_2002215c[0x3fffffff + g_2002208c];
                    idx = v4;
                    while (1)
                    {
                        idx = v0;
                        g_2002215c[lr] = v3;
                        v7 = idx - 1;
                        if (idx == 1 || !(v3 = *((int *)v5), v8 = v5 - 4, *((int *)v5) >= a0))
                            break;
                        v0 = v7;
                        v5 -= 4;
                    }
                }
                else
                {
                    idx = v4;
                }
            }
            else
            {
                idx = g_2002208c + 1;
                v4 = idx;
            }
            g_2002215c[idx] = a0;
            g_2002208c = v4;
            g_20021df4 = 1066192077;
        }
        else
        {
            g_2002215c[0] = 0;
            g_20022160 = a0;
            g_20021df4 = 1066192077;
            g_2002208c = 1;
        }
        g_200220a4 = g_20021c68 + 100;
        g_200220a8 = g_20021c6c + 100;
        g_200220b0 = g_20021c74 + 100;
        g_200220ac = g_20021c70 + 100;
        lr = g_20021c7c + 100;
        g_200220b4 = g_20021c78 + 100;
        v9 = *((int *)0x20021c80) + 100;
        v10 = *((int *)0x20021c84) + 100;
        v11 = g_20022074 | 32;
        g_200220b8 = lr;
        g_200220bc = v9;
        g_200220c0 = v10;
        g_20022074 = v11;
        g_20024128 = 480;
        return g_20021c7c;
    }
    else
    {
        return a0;
    }
}
