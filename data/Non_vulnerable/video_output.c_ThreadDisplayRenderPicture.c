}
static int ThreadDisplayRenderPicture(vout_thread_t *vout, bool is_forced)
{
    vout_thread_sys_t *sys = vout->p;
    vout_display_t *vd = vout->p->display.vd;
    picture_t *torender = picture_Hold(vout->p->displayed.current);
    vout_chrono_Start(&vout->p->render);
    vlc_mutex_lock(&vout->p->filter.lock);
    picture_t *filtered = filter_chain_VideoFilter(vout->p->filter.chain_interactive, torender);
    vlc_mutex_unlock(&vout->p->filter.lock);
    if (!filtered)
        return VLC_EGENERIC;
    if (filtered->date != vout->p->displayed.current->date)
        msg_Warn(vout, "Unsupported timestamp modifications done by chain_interactive");
    /*
     * Get the subpicture to be displayed
     */
    const bool do_snapshot = vout_snapshot_IsRequested(&vout->p->snapshot);
    mtime_t render_subtitle_date;
    if (vout->p->pause.is_on)
        render_subtitle_date = vout->p->pause.date;
    else
        render_subtitle_date = filtered->date > 1 ? filtered->date : mdate();
    mtime_t render_osd_date = mdate(); /* FIXME wrong */
    /*
     * Get the subpicture to be displayed
     */
    const bool do_dr_spu = !do_snapshot &&
                           vd->info.subpicture_chromas &&
                           *vd->info.subpicture_chromas != 0;
    //FIXME: Denying do_early_spu if vd->source.orientation != ORIENT_NORMAL
    //will have the effect that snapshots miss the subpictures. We do this
    //because there is currently no way to transform subpictures to match
    //the source format.
    const bool do_early_spu = !do_dr_spu &&
                               vd->source.orientation == ORIENT_NORMAL &&
                              (vd->info.is_slow ||
                               sys->display.use_dr ||
                               do_snapshot ||
                               !vout_IsDisplayFiltered(vd) ||
                               vd->fmt.i_width * vd->fmt.i_height <= vd->source.i_width * vd->source.i_height);
    const vlc_fourcc_t *subpicture_chromas;
    video_format_t fmt_spu;
    if (do_dr_spu) {
        vout_display_place_t place;
        vout_display_PlacePicture(&place, &vd->source, vd->cfg, false);
        fmt_spu = vd->source;
        if (fmt_spu.i_width * fmt_spu.i_height < place.width * place.height) {
            fmt_spu.i_sar_num = vd->cfg->display.sar.num;
            fmt_spu.i_sar_den = vd->cfg->display.sar.den;
            fmt_spu.i_width          =
            fmt_spu.i_visible_width  = place.width;
            fmt_spu.i_height         =
            fmt_spu.i_visible_height = place.height;
        }
        subpicture_chromas = vd->info.subpicture_chromas;
    } else {
        if (do_early_spu) {
            fmt_spu = vd->source;
        } else {
            fmt_spu = vd->fmt;
            fmt_spu.i_sar_num = vd->cfg->display.sar.num;
            fmt_spu.i_sar_den = vd->cfg->display.sar.den;
        }
        subpicture_chromas = NULL;
        if (vout->p->spu_blend &&
            vout->p->spu_blend->fmt_out.video.i_chroma != fmt_spu.i_chroma) {
            filter_DeleteBlend(vout->p->spu_blend);
            vout->p->spu_blend = NULL;
            vout->p->spu_blend_chroma = 0;
        }
        if (!vout->p->spu_blend && vout->p->spu_blend_chroma != fmt_spu.i_chroma) {
            vout->p->spu_blend_chroma = fmt_spu.i_chroma;
            vout->p->spu_blend = filter_NewBlend(VLC_OBJECT(vout), &fmt_spu);
            if (!vout->p->spu_blend)
                msg_Err(vout, "Failed to create blending filter, OSD/Subtitles will not work");
        }
    }
    video_format_t fmt_spu_rot;
    video_format_ApplyRotation(&fmt_spu_rot, &fmt_spu);
    subpicture_t *subpic = spu_Render(vout->p->spu,
                                      subpicture_chromas, &fmt_spu_rot,
                                      &vd->source,
                                      render_subtitle_date, render_osd_date,
                                      do_snapshot);
    /*
     * Perform rendering
     *
     * We have to:
     * - be sure to end up with a direct buffer.
     * - blend subtitles, and in a fast access buffer
     */
    bool is_direct = vout->p->decoder_pool == vout->p->display_pool;
    picture_t *todisplay = filtered;
    if (do_early_spu && subpic) {
        picture_t *blent = picture_pool_Get(vout->p->private_pool);
        if (blent) {
            VideoFormatCopyCropAr(&blent->format, &filtered->format);
            picture_Copy(blent, filtered);
            if (vout->p->spu_blend
             && picture_BlendSubpicture(blent, vout->p->spu_blend, subpic)) {
                picture_Release(todisplay);
                todisplay = blent;
            } else
                picture_Release(blent);
        }
        subpicture_Delete(subpic);
        subpic = NULL;
    }
    assert(vout_IsDisplayFiltered(vd) == !sys->display.use_dr);
    if (sys->display.use_dr && !is_direct) {
        picture_t *direct = picture_pool_Get(vout->p->display_pool);
        if (!direct) {
            picture_Release(todisplay);
            if (subpic)
                subpicture_Delete(subpic);
            return VLC_EGENERIC;
        }
        /* The display uses direct rendering (no conversion), but its pool of
         * pictures is not usable by the decoder (too few, too slow or
         * subject to invalidation...). Since there are no filters, copying
         * pictures from the decoder to the output is unavoidable. */
        VideoFormatCopyCropAr(&direct->format, &todisplay->format);
        picture_Copy(direct, todisplay);
        picture_Release(todisplay);
        todisplay = direct;
    }
    /*
     * Take a snapshot if requested
     */
    if (do_snapshot)
        vout_snapshot_Set(&vout->p->snapshot, &vd->source, todisplay);
    /* Render the direct buffer */
    vout_UpdateDisplaySourceProperties(vd, &todisplay->format);
    if (sys->display.use_dr) {
        vout_display_Prepare(vd, todisplay, subpic);
    } else {
        sys->display.filtered = vout_FilterDisplay(vd, todisplay);
        if (sys->display.filtered) {
            if (!do_dr_spu && !do_early_spu && vout->p->spu_blend && subpic)
                picture_BlendSubpicture(sys->display.filtered, vout->p->spu_blend, subpic);
            vout_display_Prepare(vd, sys->display.filtered, do_dr_spu ? subpic : NULL);
        }
        if (!do_dr_spu && subpic)
        {
            subpicture_Delete(subpic);
            subpic = NULL;
        }
        if (!sys->display.filtered)
        {
            if (subpic != NULL)
                subpicture_Delete(subpic);
            return VLC_EGENERIC;
        }
        todisplay = sys->display.filtered;
    }
    vout_chrono_Stop(&vout->p->render);
#if 0
        {
        static int i = 0;
        if (((i++)%10) == 0)
            msg_Info(vout, "render: avg %d ms var %d ms",
                     (int)(vout->p->render.avg/1000), (int)(vout->p->render.var/1000));
        }
#endif
    /* Wait the real date (for rendering jitter) */
#if 0
    mtime_t delay = direct->date - mdate();
    if (delay < 1000)
        msg_Warn(vout, "picture is late (%lld ms)", delay / 1000);
#endif
    if (!is_forced)
        mwait(todisplay->date);
    /* Display the direct buffer returned by vout_RenderPicture */
    vout->p->displayed.date = mdate();
    vout_display_Display(vd, todisplay, subpic);
    sys->display.filtered = NULL;
    vout_statistic_AddDisplayed(&vout->p->statistic, 1);
    return VLC_SUCCESS;
}
