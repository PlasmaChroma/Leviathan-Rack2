extern unsigned int g_20021c68;
extern unsigned int g_20021c6c;
extern unsigned int g_20021c70;
extern unsigned int g_20021c74;
extern unsigned int g_20021c78;
extern unsigned int g_20021c7c;
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

void sub_8026f41(void)
{
    unsigned int index;  // r4
    unsigned int v1;  // cc_dep1
    unsigned int v10;  // r7
    unsigned int *v11;  // r6
    unsigned int *v12;  // r3
    unsigned int v13;  // r8
    unsigned int v14;  // r1
    unsigned int v15;  // r4
    unsigned int v16;  // r7
    unsigned int v17;  // r0
    unsigned int v18;  // r2
    unsigned int v2;  // r12
    unsigned int *v3;  // r2
    unsigned int *j;  // r3
    unsigned int v5;  // r0
    unsigned int *idx;  // r8
    unsigned int *iter;  // r3
    unsigned int v8;  // r7
    unsigned int *v9;  // r8

    if (!g_2002208c)
    {
        return;
    }
    else if (g_2002208c == *((int *)537009340))
    {
        return;
    }
    else if (*((int *)0x20021cbc))
    {
        index = *((int *)0x20021cbc) + 1;
        v1 = g_2002208c;
        g_2002215c[*((int *)0x20021cbc)] = g_2002215c[index];
        if (g_2002208c < index)
        {
            v2 = g_2002208c - 1;
            if (v1 >= index)
                goto LABEL_8026f6f;
        }
        else
        {
            v2 = g_2002208c - 1;
            if (v1 >= index)
            {
LABEL_8026f6f:
                v3 = (*((int *)0x20021cbc) + 2) * 4;
                j = v3 + 134252630;
                v2 = g_2002208c - 1;
                if (!((v3 + 134252631 | j) * 0x20000000) && v2 - *((int *)0x20021cbc) > 10)
                {
                    v5 = g_2002208c - *((int *)0x20021cbc);
                    idx = v3 + 134252629;
                    iter = j;
                    do
                    {
                        v8 = idx[3];
                        v9 = idx + 2;
                        *(iter) = idx[2];
                        iter[1] = v8;
                        iter += 2;
                        idx = v9;
                    } while (iter != &j[2 * (v5 >> 1)]);
                    v10 = v5 & 0xfffffffe;
                    if (v5 != v10)
                        g_2002215c[v10 + index] = g_2002215c[1 + v10 + index];
                }
                else
                {
                    v11 = &(&g_20022160)[g_2002208c];
                    do
                    {
                        v12 = j + 1;
                        *(j) = j[1];
                        j = v12;
                    } while (j != v11);
                }
            }
        }
        g_2002208c = v2;
        sub_8026d99();
        g_20022074 = g_20022074 | 32;
        v13 = g_20021c7c + 100;
        v14 = *((int *)0x20021c80) + 100;
        v15 = g_20021c70 + 100;
        g_200220a4 = g_20021c68 + 100;
        g_200220b8 = v13;
        g_200220bc = v14;
        v16 = g_20021c74 + 100;
        v17 = g_20021c78 + 100;
        v18 = *((int *)0x20021c84) + 100;
        g_200220a8 = g_20021c6c + 100;
        g_200220ac = v15;
        g_200220b0 = v16;
        g_200220b4 = v17;
        g_200220c0 = v18;
        g_20024128 = 480;
        return;
    }
    else
    {
        return;
    }
}
