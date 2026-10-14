extern unsigned int g_20021540;
extern unsigned int g_20021640;
extern unsigned int g_20021740;
extern unsigned int g_20021840;
extern unsigned int g_20021940;
extern unsigned int g_20021a40;
extern unsigned short g_20021c40;
extern unsigned short g_20021c42;
extern unsigned short g_20021c44;
extern unsigned int g_20021c48;
extern unsigned int g_20021c4c;
extern unsigned int g_20021c50;
extern unsigned int g_20021c54;
extern unsigned int g_20021c58;
extern unsigned int g_20021c5c;
extern unsigned int g_20021c68;
extern unsigned int g_20021c6c;
extern unsigned int g_20021c70;
extern unsigned int g_20021c74;
extern unsigned int g_20021c78;
extern unsigned int g_20021c7c;

unsigned int sub_80263e5(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v1;  // r11
    unsigned int v2;  // r7
    unsigned int v11;  // r4
    unsigned int v12;  // r2
    unsigned int v3;  // r7
    unsigned int v4;  // r11
    unsigned int v5;  // r7
    unsigned int v6;  // r10
    unsigned int v7;  // r0
    unsigned int v8;  // r0
    unsigned int v9;  // r1
    unsigned short v10;  // r7
    unsigned int v0;  // [bp-0x28]

    v0 = a3;
    if (!sub_802c905(1073898512, 268435488))
        return 0;
    v1 = (&g_20021540)[g_20021c40];
    v2 = (&g_20021640)[g_20021c40];
    (&g_20021540)[g_20021c40] = *((short *)0x20000ff0);
    (&g_20021640)[g_20021c40] = *((short *)0x20000ff2);
    v3 = (&g_20021740)[g_20021c44];
    v4 = g_20021c48 - v1 + *((short *)0x20000ff0);
    v5 = (&g_20021840)[g_20021c44];
    (&g_20021740)[g_20021c44] = *((short *)0x20000ff4);
    v6 = g_20021c50 - v3 + *((short *)0x20000ff4);
    g_20021c4c = g_20021c4c - v2 + *((short *)0x20000ff2);
    g_20021c6c = (int)(g_20021c4c) >> 4;
    v7 = (&g_20021940)[g_20021c42];
    g_20021c48 = v4;
    (&g_20021840)[g_20021c44] = *((short *)0x20000ff6);
    g_20021c50 = v6;
    g_20021c68 = (int)(g_20021c48) >> 4;
    v8 = g_20021c58 - v7 + *((short *)0x20000ff8);
    g_20021c54 = g_20021c54 - v5 + *((short *)0x20000ff6);
    g_20021c58 = v8;
    g_20021c74 = (int)(g_20021c54) >> 6;
    (&g_20021940)[g_20021c42] = *((short *)0x20000ff8);
    g_20021c70 = (int)(g_20021c50) >> 6;
    g_20021c78 = (int)(g_20021c58) >> 2;
    v9 = g_20021c5c - (&g_20021a40)[g_20021c44] + *((short *)0x20000ffa);
    v10 = g_20021c44 + 1;
    v11 = g_20021c42 + 1;
    (&g_20021a40)[g_20021c44] = *((short *)0x20000ffa);
    g_20021c5c = v9;
    v12 = g_20021c40 + 1;
    g_20021c40 = g_20021c40 + 1;
    g_20021c44 = v10;
    g_20021c7c = (int)(g_20021c5c) >> 6;
    if (3 >= v11)
        g_20021c42 = v11;
    else
        g_20021c42 = 0;
    if (g_20021c40 > 15)
        g_20021c40 = 0;
    if (g_20021c44 <= 63)
        return sub_802c941(1073898512, 268435488);
    g_20021c44 = 0;
    return sub_802c941(1073898512, 268435488);
}
