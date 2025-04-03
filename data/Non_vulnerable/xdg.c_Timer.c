extern char **environ;
static void Timer (void *data)
{
    vlc_inhibit_t *ih = data;
    vlc_inhibit_sys_t *sys = ih->p_sys;
    char *argv[3] = {
        (char *)"xdg-screensaver", (char *)"reset", NULL
    };
    pid_t pid;
    int err = posix_spawnp (&pid, "xdg-screensaver", NULL, &sys->attr,
                            argv, environ);
    if (err == 0)
    {
        int status;
        while (waitpid (pid, &status, 0) == -1);
    }
    else
        msg_Warn (ih, "error starting xdg-screensaver: %s",
                  vlc_strerror_c(err));
}
