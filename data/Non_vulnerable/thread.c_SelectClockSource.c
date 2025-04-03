}
static void SelectClockSource (vlc_object_t *obj)
{
    EnterCriticalSection (&clock_lock);
    if (mdate_selected != mdate_giveup)
    {
        LeaveCriticalSection (&clock_lock);
        return;
    }
#if VLC_WINSTORE_APP
    const char *name = "perf";
#else
    const char *name = "multimedia";
#endif
    char *str = var_InheritString (obj, "clock-source");
    if (str != NULL)
        name = str;
    if (!strcmp (name, "interrupt"))
    {
        msg_Dbg (obj, "using interrupt time as clock source");
#if (_WIN32_WINNT < 0x0601)
        HANDLE h = GetModuleHandle (_T("kernel32.dll"));
        if (unlikely(h == NULL))
            abort ();
        clk.interrupt.query = (void *)GetProcAddress (h,
                                                      "QueryUnbiasedInterruptTime");
        if (unlikely(clk.interrupt.query == NULL))
            abort ();
#endif
        mdate_selected = mdate_interrupt;
    }
    else
    if (!strcmp (name, "tick"))
    {
        msg_Dbg (obj, "using Windows time as clock source");
#if (_WIN32_WINNT < 0x0600)
        HANDLE h = GetModuleHandle (_T("kernel32.dll"));
        if (unlikely(h == NULL))
            abort ();
        clk.tick.get = (void *)GetProcAddress (h, "GetTickCount64");
        if (unlikely(clk.tick.get == NULL))
            abort ();
#endif
        mdate_selected = mdate_tick;
    }
#if !VLC_WINSTORE_APP
    else
    if (!strcmp (name, "multimedia"))
    {
        TIMECAPS caps;
        HMODULE hWinmm = GetModuleHandle(TEXT("winmm.dll"));
        if (!hWinmm)
            goto perf;
        clk.multimedia.timeGetDevCaps = (void*)GetProcAddress(hWinmm, "timeGetDevCaps");
        clk.multimedia.timeGetTime = (void*)GetProcAddress(hWinmm, "timeGetTime");
        if (!clk.multimedia.timeGetDevCaps || !clk.multimedia.timeGetTime)
            goto perf;
        msg_Dbg (obj, "using multimedia timers as clock source");
        if (clk.multimedia.timeGetDevCaps (&caps, sizeof (caps)) != MMSYSERR_NOERROR)
            goto perf;
        msg_Dbg (obj, " min period: %u ms, max period: %u ms",
                 caps.wPeriodMin, caps.wPeriodMax);
        mdate_selected = mdate_multimedia;
    }
#endif
    else
    if (!strcmp (name, "perf"))
    {
    perf:
        msg_Dbg (obj, "using performance counters as clock source");
        if (!QueryPerformanceFrequency (&clk.perf.freq))
            abort ();
        msg_Dbg (obj, " frequency: %llu Hz", clk.perf.freq.QuadPart);
        mdate_selected = mdate_perf;
    }
    else
    if (!strcmp (name, "wall"))
    {
        msg_Dbg (obj, "using system time as clock source");
        mdate_selected = mdate_wall;
    }
    else
    {
        msg_Err (obj, "invalid clock source \"%s\"", name);
        abort ();
    }
    LeaveCriticalSection (&clock_lock);
    free (str);
}
