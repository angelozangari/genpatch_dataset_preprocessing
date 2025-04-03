#include <vdpau/vdpau_x11.h>
VdpStatus vdp_presentation_queue_target_create_x11(const vdp_t *vdp,
    VdpDevice device, uint32_t drawable, VdpPresentationQueueTarget *target)
{
    void *ptr;
    VdpStatus err = vdp_get_proc_address(vdp, device,
                       VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_CREATE_X11, &ptr);
    if (err != VDP_STATUS_OK)
        return err;
    VdpPresentationQueueTargetCreateX11 *f = ptr;
    return f(device, drawable, target);
}
