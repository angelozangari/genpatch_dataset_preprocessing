 *****************************************************************************/
static int GetSwsCpuMask(void)
{
    int i_sws_cpu = 0;
#if LIBSWSCALE_VERSION_MAJOR < 4
#if defined(__i386__) || defined(__x86_64__)
    if( vlc_CPU_MMX() )
        i_sws_cpu |= SWS_CPU_CAPS_MMX;
#if (LIBSWSCALE_VERSION_INT >= ((0<<16)+(5<<8)+0))
    if( vlc_CPU_MMXEXT() )
        i_sws_cpu |= SWS_CPU_CAPS_MMX2;
#endif
    if( vlc_CPU_3dNOW() )
        i_sws_cpu |= SWS_CPU_CAPS_3DNOW;
#elif defined(__ppc__) || defined(__ppc64__) || defined(__powerpc__)
    if( vlc_CPU_ALTIVEC() )
        i_sws_cpu |= SWS_CPU_CAPS_ALTIVEC;
#endif
#endif
    return i_sws_cpu;
}
