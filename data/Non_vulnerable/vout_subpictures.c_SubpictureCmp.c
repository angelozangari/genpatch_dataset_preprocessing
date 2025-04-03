 */
static int SubpictureCmp(const void *s0, const void *s1)
{
    subpicture_t *subpic0 = *(subpicture_t**)s0;
    subpicture_t *subpic1 = *(subpicture_t**)s1;
    int r;
    r = IntegerCmp(!subpic0->b_absolute, !subpic1->b_absolute);
    if (!r)
        r = IntegerCmp(subpic0->i_start, subpic1->i_start);
    if (!r)
        r = IntegerCmp(subpic0->i_channel, subpic1->i_channel);
    if (!r)
        r = IntegerCmp(subpic0->i_order, subpic1->i_order);
    return r;
}
