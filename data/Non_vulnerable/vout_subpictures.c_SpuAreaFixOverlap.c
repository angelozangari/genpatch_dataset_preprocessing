 */
static void SpuAreaFixOverlap(spu_area_t *dst,
                              const spu_area_t *sub_array, int sub_count, int align)
{
    spu_area_t a = spu_area_scaled(*dst);
    bool is_moved = false;
    bool is_ok;
    /* Check for overlap
     * XXX It is not fast O(n^2) but we should not have a lot of region */
    do {
        is_ok = true;
        for (int i = 0; i < sub_count; i++) {
            spu_area_t sub = spu_area_scaled(sub_array[i]);
            if (!spu_area_overlap(a, sub))
                continue;
            if (align & SUBPICTURE_ALIGN_TOP) {
                /* We go down */
                int i_y = sub.y + sub.height;
                a.y = i_y;
                is_moved = true;
            } else if (align & SUBPICTURE_ALIGN_BOTTOM) {
                /* We go up */
                int i_y = sub.y - a.height;
                a.y = i_y;
                is_moved = true;
            } else {
                /* TODO what to do in this case? */
                //fprintf(stderr, "Overlap with unsupported alignment\n");
                break;
            }
            is_ok = false;
            break;
        }
    } while (!is_ok);
    if (is_moved)
        *dst = spu_area_unscaled(a, dst->scale);
}
