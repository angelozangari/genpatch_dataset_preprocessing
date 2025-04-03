}
void vout_ControlChangeSubFilters(vout_thread_t *vout, const char *filters)
{
    vout_control_PushString(&vout->p->control, VOUT_CONTROL_CHANGE_SUB_FILTERS,
                            filters);
}
