 */
static void SpuRegionPlace(int *x, int *y,
                           const subpicture_t *subpic,
                           const subpicture_region_t *region)
{
    assert(region->i_x != INT_MAX && region->i_y != INT_MAX);
    if (subpic->b_absolute) {
        *x = region->i_x;
        *y = region->i_y;
    } else {
        if (region->i_align & SUBPICTURE_ALIGN_TOP)
            *y = region->i_y;
        else if (region->i_align & SUBPICTURE_ALIGN_BOTTOM)
            *y = subpic->i_original_picture_height - region->fmt.i_visible_height - region->i_y;
        else
            *y = subpic->i_original_picture_height / 2 - region->fmt.i_visible_height / 2;
        if (region->i_align & SUBPICTURE_ALIGN_LEFT)
            *x = region->i_x;
        else if (region->i_align & SUBPICTURE_ALIGN_RIGHT)
            *x = subpic->i_original_picture_width - region->fmt.i_visible_width - region->i_x;
        else
            *x = subpic->i_original_picture_width / 2 - region->fmt.i_visible_width / 2;
    }
}
