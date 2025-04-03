};
static bool dsc_is_rotated(const transform_description_t *dsc)
{
    return dsc->plane32 != dsc->yuyv;
}
