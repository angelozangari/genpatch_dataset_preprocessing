}
static subpicture_region_t * vout_BuildOSDEpg(vlc_epg_t *epg,
                                              int x, int y,
                                              int visible_width,
                                              int visible_height)
{
    subpicture_region_t *head;
    subpicture_region_t **last_ptr = &head;
    time_t current_time = time(NULL);
    /* Display the name of the channel. */
    *last_ptr = vout_OSDEpgText(epg->psz_name,
                                x + visible_width  * EPG_LEFT,
                                y + visible_height * EPG_TOP,
                                visible_height * EPG_NAME_SIZE,
                                0x00ffffff);
    if (!*last_ptr)
        return head;
    /* Display the name of the current program. */
    last_ptr = &(*last_ptr)->p_next;
    *last_ptr = vout_OSDEpgText(epg->p_current->psz_name,
                                x + visible_width  * (EPG_LEFT + 0.025),
                                y + visible_height * (EPG_TOP + 0.05),
                                visible_height * EPG_PROGRAM_SIZE,
                                0x00ffffff);
    if (!*last_ptr)
        return head;
    /* Display the current program time slider. */
    last_ptr = &(*last_ptr)->p_next;
    *last_ptr = vout_OSDEpgSlider(x + visible_width  * EPG_LEFT,
                                  y + visible_height * (EPG_TOP + 0.1),
                                  visible_width  * (1 - 2 * EPG_LEFT),
                                  visible_height * 0.05,
                                  (current_time - epg->p_current->i_start)
                                  / (float)epg->p_current->i_duration);
    if (!*last_ptr)
        return head;
    /* Format the hours of the beginning and the end of the current program. */
    struct tm tm_start, tm_end;
    time_t t_start = epg->p_current->i_start;
    time_t t_end = epg->p_current->i_start + epg->p_current->i_duration;
    localtime_r(&t_start, &tm_start);
    localtime_r(&t_end, &tm_end);
    char text_start[128];
    char text_end[128];
    snprintf(text_start, sizeof(text_start), "%2.2d:%2.2d",
             tm_start.tm_hour, tm_start.tm_min);
    snprintf(text_end, sizeof(text_end), "%2.2d:%2.2d",
             tm_end.tm_hour, tm_end.tm_min);
    /* Display those hours. */
    last_ptr = &(*last_ptr)->p_next;
    *last_ptr = vout_OSDEpgText(text_start,
                                x + visible_width  * (EPG_LEFT + 0.02),
                                y + visible_height * (EPG_TOP + 0.15),
                                visible_height * EPG_PROGRAM_SIZE,
                                0x00ffffff);
    if (!*last_ptr)
        return head;
    last_ptr = &(*last_ptr)->p_next;
    *last_ptr = vout_OSDEpgText(text_end,
                                x + visible_width  * (1 - EPG_LEFT - 0.085),
                                y + visible_height * (EPG_TOP + 0.15),
                                visible_height * EPG_PROGRAM_SIZE,
                                0x00ffffff);
    return head;
}
