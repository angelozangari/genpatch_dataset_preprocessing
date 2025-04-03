}
void vdp_destroy_x11(vdp_t *vdp)
{
    dlclose(vdp->handle);
    free(vdp);
}
