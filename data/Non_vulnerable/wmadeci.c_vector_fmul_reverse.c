}
static inline void vector_fmul_reverse(int32_t *dst, const int32_t *src0, const int32_t *src1, int len){
    int i;
    src1 += len-1;
    for(i=0; i<len; i++)
        dst[i] = fixmul32b(src0[i], src1[-i]);
}
