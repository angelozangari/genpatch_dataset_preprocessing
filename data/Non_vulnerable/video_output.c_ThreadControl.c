}
static int ThreadControl(vout_thread_t *vout, vout_control_cmd_t cmd)
{
    switch(cmd.type) {
    case VOUT_CONTROL_INIT:
        ThreadInit(vout);
        if (!ThreadStart(vout, NULL))
            break;
    case VOUT_CONTROL_CLEAN:
        ThreadStop(vout, NULL);
        ThreadClean(vout);
        return 1;
    case VOUT_CONTROL_REINIT:
        if (ThreadReinit(vout, cmd.u.cfg))
            return 1;
        break;
    case VOUT_CONTROL_SUBPICTURE:
        ThreadDisplaySubpicture(vout, cmd.u.subpicture);
        cmd.u.subpicture = NULL;
        break;
    case VOUT_CONTROL_FLUSH_SUBPICTURE:
        ThreadFlushSubpicture(vout, cmd.u.integer);
        break;
    case VOUT_CONTROL_OSD_TITLE:
        ThreadDisplayOsdTitle(vout, cmd.u.string);
        break;
    case VOUT_CONTROL_CHANGE_FILTERS:
        ThreadChangeFilters(vout, NULL, cmd.u.string, false);
        break;
    case VOUT_CONTROL_CHANGE_SUB_SOURCES:
        ThreadChangeSubSources(vout, cmd.u.string);
        break;
    case VOUT_CONTROL_CHANGE_SUB_FILTERS:
        ThreadChangeSubFilters(vout, cmd.u.string);
        break;
    case VOUT_CONTROL_CHANGE_SUB_MARGIN:
        ThreadChangeSubMargin(vout, cmd.u.integer);
        break;
    case VOUT_CONTROL_PAUSE:
        ThreadChangePause(vout, cmd.u.pause.is_on, cmd.u.pause.date);
        break;
    case VOUT_CONTROL_FLUSH:
        ThreadFlush(vout, false, cmd.u.time);
        break;
    case VOUT_CONTROL_RESET:
        ThreadReset(vout);
        break;
    case VOUT_CONTROL_STEP:
        ThreadStep(vout, cmd.u.time_ptr);
        break;
    case VOUT_CONTROL_FULLSCREEN:
        ThreadChangeFullscreen(vout, cmd.u.boolean);
        break;
    case VOUT_CONTROL_WINDOW_STATE:
        ThreadChangeWindowState(vout, cmd.u.integer);
        break;
    case VOUT_CONTROL_DISPLAY_FILLED:
        ThreadChangeDisplayFilled(vout, cmd.u.boolean);
        break;
    case VOUT_CONTROL_ZOOM:
        ThreadChangeZoom(vout, cmd.u.pair.a, cmd.u.pair.b);
        break;
    case VOUT_CONTROL_ASPECT_RATIO:
        ThreadChangeAspectRatio(vout, cmd.u.pair.a, cmd.u.pair.b);
        break;
    case VOUT_CONTROL_CROP_RATIO:
        ThreadExecuteCropRatio(vout, cmd.u.pair.a, cmd.u.pair.b);
        break;
    case VOUT_CONTROL_CROP_WINDOW:
        ThreadExecuteCropWindow(vout,
                cmd.u.window.x, cmd.u.window.y,
                cmd.u.window.width, cmd.u.window.height);
        break;
    case VOUT_CONTROL_CROP_BORDER:
        ThreadExecuteCropBorder(vout,
                cmd.u.border.left,  cmd.u.border.top,
                cmd.u.border.right, cmd.u.border.bottom);
        break;
    default:
        break;
    }
    vout_control_cmd_Clean(&cmd);
    return 0;
}
