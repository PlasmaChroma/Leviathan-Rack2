typedef struct st_80274c1_0 {
    char padding_0[32];
    unsigned short field_20;
} st_80274c1_0;

typedef struct st_80274c1_1 {
    char padding_0[52];
    unsigned int field_34;
} st_80274c1_1;

void sub_80274c1(void)
{
    st_80274c1_0 *v0;  // r6
    st_80274c1_1 *v1;  // r5

    v0->field_20 = (unsigned short)v1;
    v1->field_34 = v0 >> 14;
    sub_80274c9();
    return;
}
