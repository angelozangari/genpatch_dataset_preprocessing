}
static HRESULT ActivateDevice(void *opaque, REFIID iid, PROPVARIANT *actparms,
                              void **restrict pv)
{
    aout_sys_t *sys = opaque;
    (void)iid; (void)actparms;
    *pv = sys->client;
    return S_OK;
}
