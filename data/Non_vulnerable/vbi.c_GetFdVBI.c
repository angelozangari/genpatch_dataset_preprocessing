}
int GetFdVBI (vlc_v4l2_vbi_t *vbi)
{
    return vbi_capture_fd(vbi->cap);
}
