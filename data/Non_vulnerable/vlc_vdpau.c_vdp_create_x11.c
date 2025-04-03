}
VdpStatus vdp_create_x11(void *dpy, int snum,
                         vdp_t **restrict vdpp, VdpDevice *restrict devp)
{
    vdp_t *vdp = malloc(sizeof (*vdp));
    if (unlikely(vdp == NULL))
        return VDP_STATUS_RESOURCES;
    *vdpp = vdp;
    VdpStatus err = VDP_STATUS_NO_IMPLEMENTATION;
    vdp->handle = dlopen("libvdpau.so.1", RTLD_LAZY|RTLD_LOCAL);
    if (vdp->handle == NULL)
    {
        free(vdp);
        return err;
    }
    VdpDeviceCreateX11 *create = dlsym(vdp->handle, "vdp_device_create_x11");
    if (create == NULL)
        goto error;
    VdpGetProcAddress *gpa;
    err = create(dpy, snum, devp, &gpa);
    if (err != VDP_STATUS_OK)
        goto error;
    for (VdpFuncId i = 0; i < sizeof (vdp->vt) / sizeof (void *); i++)
        if (gpa(*devp, i, vdp->funcs + i) != VDP_STATUS_OK)
        {
            void *fallback = vdp_fallback;
            if (unlikely(i == VDP_FUNC_ID_GET_ERROR_STRING))
                fallback = vdp_get_error_string_fallback;
            vdp->funcs[i] = fallback;
        }
    return VDP_STATUS_OK;
error:
    vdp_destroy_x11(vdp);
    return err;
}
