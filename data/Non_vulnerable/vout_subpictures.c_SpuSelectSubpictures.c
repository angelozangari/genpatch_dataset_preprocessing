 *****************************************************************************/
static void SpuSelectSubpictures(spu_t *spu,
                                 unsigned int *subpicture_count,
                                 subpicture_t **subpicture_array,
                                 mtime_t render_subtitle_date,
                                 mtime_t render_osd_date,
                                 bool ignore_osd)
{
    spu_private_t *sys = spu->p;
    /* */
    *subpicture_count = 0;
    /* Create a list of channels */
    int channel[VOUT_MAX_SUBPICTURES];
    int channel_count = 0;
    for (int index = 0; index < VOUT_MAX_SUBPICTURES; index++) {
        spu_heap_entry_t *entry = &sys->heap.entry[index];
        if (!entry->subpicture || entry->reject)
            continue;
        const int i_channel = entry->subpicture->i_channel;
        int i;
        for (i = 0; i < channel_count; i++) {
            if (channel[i] == i_channel)
                break;
        }
        if (channel_count <= i)
            channel[channel_count++] = i_channel;
    }
    /* Fill up the subpicture_array arrays with relevent pictures */
    for (int i = 0; i < channel_count; i++) {
        subpicture_t *available_subpic[VOUT_MAX_SUBPICTURES];
        bool         is_available_late[VOUT_MAX_SUBPICTURES];
        int          available_count = 0;
        mtime_t      start_date = render_subtitle_date;
        mtime_t      ephemer_subtitle_date = 0;
        mtime_t      ephemer_osd_date = 0;
        int64_t      ephemer_subtitle_order = INT64_MIN;
        int64_t      ephemer_system_order = INT64_MIN;
        /* Select available pictures */
        for (int index = 0; index < VOUT_MAX_SUBPICTURES; index++) {
            spu_heap_entry_t *entry = &sys->heap.entry[index];
            subpicture_t *current = entry->subpicture;
            bool is_stop_valid;
            bool is_late;
            if (!current || entry->reject) {
                if (entry->reject)
                    SpuHeapDeleteAt(&sys->heap, index);
                continue;
            }
            if (current->i_channel != channel[i] ||
               (ignore_osd && !current->b_subtitle))
                continue;
            const mtime_t render_date = current->b_subtitle ? render_subtitle_date : render_osd_date;
            if (render_date &&
                render_date < current->i_start) {
                /* Too early, come back next monday */
                continue;
            }
            mtime_t *ephemer_date_ptr  = current->b_subtitle ? &ephemer_subtitle_date  : &ephemer_osd_date;
            int64_t *ephemer_order_ptr = current->b_subtitle ? &ephemer_subtitle_order : &ephemer_system_order;
            if (current->i_start >= *ephemer_date_ptr) {
                *ephemer_date_ptr = current->i_start;
                if (current->i_order > *ephemer_order_ptr)
                    *ephemer_order_ptr = current->i_order;
            }
            is_stop_valid = !current->b_ephemer || current->i_stop > current->i_start;
            is_late = is_stop_valid && current->i_stop <= render_date;
            /* start_date will be used for correct automatic overlap support
             * in case picture that should not be displayed anymore (display_time)
             * overlap with a picture to be displayed (current->i_start)  */
            if (current->b_subtitle && !is_late && !current->b_ephemer)
                start_date = current->i_start;
            /* */
            available_subpic[available_count] = current;
            is_available_late[available_count] = is_late;
            available_count++;
        }
        /* Only forced old picture display at the transition */
        if (start_date < sys->last_sort_date)
            start_date = sys->last_sort_date;
        if (start_date <= 0)
            start_date = INT64_MAX;
        /* Select pictures to be displayed */
        for (int index = 0; index < available_count; index++) {
            subpicture_t *current = available_subpic[index];
            bool is_late = is_available_late[index];
            const mtime_t stop_date = current->b_subtitle ? __MAX(start_date, sys->last_sort_date) : render_osd_date;
            const mtime_t ephemer_date  = current->b_subtitle ? ephemer_subtitle_date  : ephemer_osd_date;
            const int64_t ephemer_order = current->b_subtitle ? ephemer_subtitle_order : ephemer_system_order;
            /* Destroy late and obsolete ephemer subpictures */
            bool is_rejeted = is_late && current->i_stop <= stop_date;
            if (current->b_ephemer) {
                if (current->i_start < ephemer_date)
                    is_rejeted = true;
                else if (current->i_start == ephemer_date &&
                         current->i_order < ephemer_order)
                    is_rejeted = true;
            }
            if (is_rejeted)
                SpuHeapDeleteSubpicture(&sys->heap, current);
            else
                subpicture_array[(*subpicture_count)++] = current;
        }
    }
    sys->last_sort_date = render_subtitle_date;
}
