}
static void FixParameters( int *pi_fmt, bool *pb_has_a, bool *pb_swap_uv, vlc_fourcc_t fmt )
{
    switch( fmt )
    {
    case VLC_CODEC_YUV422A:
        *pi_fmt = AV_PIX_FMT_YUV422P;
        *pb_has_a = true;
        break;
    case VLC_CODEC_YUV420A:
        *pi_fmt = AV_PIX_FMT_YUV420P;
        *pb_has_a = true;
        break;
    case VLC_CODEC_YUVA:
        *pi_fmt = AV_PIX_FMT_YUV444P;
        *pb_has_a = true;
        break;
    case VLC_CODEC_RGBA:
        *pi_fmt = AV_PIX_FMT_BGR32;
        *pb_has_a = true;
        break;
    case VLC_CODEC_ARGB:
        *pi_fmt = AV_PIX_FMT_BGR32_1;
        *pb_has_a = true;
        break;
    case VLC_CODEC_BGRA:
        *pi_fmt = AV_PIX_FMT_RGB32;
        *pb_has_a = true;
        break;
    case VLC_CODEC_YV12:
        *pi_fmt = AV_PIX_FMT_YUV420P;
        *pb_swap_uv = true;
        break;
    case VLC_CODEC_YV9:
        *pi_fmt = AV_PIX_FMT_YUV410P;
        *pb_swap_uv = true;
        break;
    default:
        break;
    }
}
