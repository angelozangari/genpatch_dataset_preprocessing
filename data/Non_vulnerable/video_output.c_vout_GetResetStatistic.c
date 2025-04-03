}
void vout_GetResetStatistic(vout_thread_t *vout, int *displayed, int *lost)
{
    vout_statistic_GetReset( &vout->p->statistic, displayed, lost );
}
