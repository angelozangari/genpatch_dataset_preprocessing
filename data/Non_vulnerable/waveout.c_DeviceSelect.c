}
static int DeviceSelect (audio_output_t *aout, const char *id)
{
    var_SetString(aout, "waveout-audio-device", (id != NULL) ? id : "");
    aout_DeviceReport (aout, id);
    aout_RestartRequest (aout, AOUT_RESTART_OUTPUT);
    return 0;
}
