}
static void ThreadClean(vout_thread_t *vout)
{
    if (vout->p->window.object) {
        assert(vout->p->window.is_unused);
        vout_window_Delete(vout->p->window.object);
    }
    vout_chrono_Clean(&vout->p->render);
    vout->p->dead = true;
    vout_control_Dead(&vout->p->control);
}
