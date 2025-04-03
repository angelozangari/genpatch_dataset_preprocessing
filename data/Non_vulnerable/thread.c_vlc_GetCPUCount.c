/*** CPU ***/
unsigned vlc_GetCPUCount (void)
{
    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    return systemInfo.dwNumberOfProcessors;
}
