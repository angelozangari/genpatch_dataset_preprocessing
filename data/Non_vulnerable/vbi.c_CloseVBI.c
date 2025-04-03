}
void CloseVBI (vlc_v4l2_vbi_t *vbi)
{
    close (vbi_capture_fd (vbi->cap));
    vbi_capture_delete (vbi->cap);
    free (vbi);
}
