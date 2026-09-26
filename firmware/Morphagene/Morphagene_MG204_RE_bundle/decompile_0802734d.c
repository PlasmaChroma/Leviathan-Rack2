extern unsigned int g_20001054;
extern unsigned int g_20021314;
extern unsigned int g_20021320;
extern unsigned int g_20021328;
extern unsigned int g_20021c68;
extern unsigned int g_20021c6c;
extern unsigned int g_20021c70;
extern unsigned int g_20021c74;
extern unsigned int g_20021c78;
extern unsigned int g_20021c7c;
extern unsigned int g_20021c9c;
extern unsigned int g_20022074;
extern unsigned int g_200220a4;
extern unsigned int g_200220a8;
extern unsigned int g_200220ac;
extern unsigned int g_200220b0;
extern unsigned int g_200220b4;
extern unsigned int g_200220b8;
extern unsigned int g_200220bc;
extern unsigned int g_200220c0;
extern unsigned int g_20022108;
extern unsigned int g_20024128;

void sub_802734d(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v0;  // [bp-0x18]

    v0 = a3;
    g_20021314 = *((int *)0x2002210c);
    g_20021320 = 0;
    g_20022108 = 0;
    g_20021c9c = 0;
    if (a0 == 1)
    {
        sub_8026619(*(0x2002210c));
        g_20001054 = 1;
        if (*((int *)0x20001290) == 1)
            goto LABEL_80273d7;
LABEL_802737d:
        g_20021328 = 1;
    }
    else if (*((int *)0x20001290) == 1)
    {
LABEL_80273d7:
        g_20022074 = g_20022074 | 2176;
    }
    else if (!(a0 == 3))
    {
        goto LABEL_802737d;
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
