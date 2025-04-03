*/
static int vhs_sliding_effect( filter_t *p_filter, picture_t *p_pic_out ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    /**
    * one shot offset section
    */
#define OFFSET_AVERAGE_PERIOD   (10 * TIME_UNIT_PER_S)
    /* start trigger to be (re)initialized */
    if ( p_sys->i_offset_trigger == 0
         || p_sys->i_sliding_speed != 0 ) { /* do not mix sliding and offset */
        /* random trigger for offset effect */
        p_sys->i_offset_trigger = p_sys->i_cur_time
                                + ((uint64_t) vlc_mrand48() ) % OFFSET_AVERAGE_PERIOD
                                + OFFSET_AVERAGE_PERIOD / 2;
        p_sys->i_offset_ofs = 0;
    } else if (p_sys->i_offset_trigger <= p_sys->i_cur_time) {
        /* trigger for offset effect occurs */
        p_sys->i_offset_trigger = 0;
        p_sys->i_offset_ofs = (uint32_t)vlc_mrand48()
                            % p_sys->i_height[Y_PLANE];
    }
    else
        p_sys->i_offset_ofs = 0;
    /**
    * phase section
    */
#define MAX_PHASE_OFS (p_sys->i_height[Y_PLANE]*100/15)
    p_sys->i_phase_speed += MOD( (int32_t)vlc_mrand48(), 3) - 1;
    p_sys->i_phase_ofs   += p_sys->i_phase_speed;
    p_sys->i_phase_ofs    = VLC_CLIP( p_sys->i_phase_ofs, -MAX_PHASE_OFS, +MAX_PHASE_OFS);
    if ( abs( p_sys->i_phase_ofs ) >= MAX_PHASE_OFS )
        p_sys->i_phase_speed = 0;
    /**
    * sliding section
    */
#define SLIDING_AVERAGE_PERIOD   (20 * TIME_UNIT_PER_S)
#define SLIDING_AVERAGE_DURATION ( 3 * TIME_UNIT_PER_S)
    /* start trigger to be (re)initialized */
    if ( ( p_sys->i_sliding_stop_trig  == 0 ) &&
         ( p_sys->i_sliding_trigger    == 0 ) &&
         ( p_sys->i_sliding_speed      == 0 ) ) {
        /* random trigger which enable sliding effect */
        p_sys->i_sliding_trigger = p_sys->i_cur_time
                                 + (uint64_t)vlc_mrand48() % SLIDING_AVERAGE_PERIOD
                                 + SLIDING_AVERAGE_PERIOD / 2;
    }
    /* start trigger just occurs */
    else if ( ( p_sys->i_sliding_stop_trig  == 0 ) &&
              ( p_sys->i_sliding_trigger    <= p_sys->i_cur_time ) &&
              ( p_sys->i_sliding_speed      == 0 ) ) {
        /* init sliding parameters */
        p_sys->i_sliding_trigger = 0;
        p_sys->i_sliding_stop_trig = p_sys->i_cur_time
                                   + (uint64_t)vlc_mrand48() % SLIDING_AVERAGE_DURATION
                                   + SLIDING_AVERAGE_DURATION / 2;
        p_sys->i_sliding_ofs = 0;
        /* note: sliding speed unit = image per 100 s */
        p_sys->i_sliding_speed = MOD( (int32_t)vlc_mrand48(), 1001 ) - 500;
        p_sys->i_sliding_type_duplicate = (unsigned)vlc_mrand48() & 0x01;
    }
    /* stop trigger disabling sliding effect occurs */
    else if ( ( p_sys->i_sliding_stop_trig  <= p_sys->i_cur_time )
              && ( p_sys->i_sliding_trigger == 0 ) ) {
        /* first increase speed to ensure we will stop sliding on plain pict */
        if ( abs( p_sys->i_sliding_speed ) < 5 )
            p_sys->i_sliding_speed += 1;
        /* check if offset is close to 0 and then ready to stop */
        if ( abs( p_sys->i_sliding_ofs ) < abs( p_sys->i_sliding_speed
             * p_sys->i_height[Y_PLANE]
             * ( p_sys->i_cur_time - p_sys->i_last_time ) / TIME_UNIT_PER_S )
             || abs( p_sys->i_sliding_ofs ) < p_sys->i_height[Y_PLANE] * 100 / 20 ) {
            /* reset sliding parameters */
            p_sys->i_sliding_ofs = p_sys->i_sliding_speed = 0;
            p_sys->i_sliding_trigger = p_sys->i_sliding_stop_trig = 0;
            p_sys->i_sliding_type_duplicate = false;
        }
    }
    /* update offset */
    p_sys->i_sliding_ofs = MOD( p_sys->i_sliding_ofs
                                + p_sys->i_sliding_speed * p_sys->i_height[Y_PLANE]
                                * ( p_sys->i_cur_time - p_sys->i_last_time)
                                / TIME_UNIT_PER_S,
                                p_sys->i_height[Y_PLANE] * 100 );
    return vhs_sliding_effect_apply( p_filter, p_pic_out );
}
