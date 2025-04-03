}
VdpStatus vdp_generate_csc_matrix(const vdp_t *vdp, const VdpProcamp *procamp,
    VdpColorStandard standard, VdpCSCMatrix *csc_matrix)
{
    VdpProcamp buf, *copy = NULL;
    if (procamp != NULL)
    {
        buf = *procamp;
        copy = &buf;
    }
    return vdp->vt.generate_csc_matrix(copy, standard, csc_matrix);
}
