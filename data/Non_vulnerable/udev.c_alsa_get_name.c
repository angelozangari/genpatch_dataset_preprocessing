}
static char *alsa_get_name (struct udev_device *dev)
{
    char *name = NULL;
    unsigned card, device;
    if (alsa_get_device (dev, &card, &device))
        return NULL;
    char card_name[4 + 3 * sizeof (int)];
    snprintf (card_name, sizeof (card_name), "hw:%u", card);
    snd_ctl_t *ctl;
    if (snd_ctl_open (&ctl, card_name, 0))
        return NULL;
    snd_pcm_info_t *pcm_info;
    snd_pcm_info_alloca (&pcm_info);
    snd_pcm_info_set_device (pcm_info, device);
    snd_pcm_info_set_subdevice (pcm_info, 0);
    snd_pcm_info_set_stream (pcm_info, SND_PCM_STREAM_CAPTURE);
    if (snd_ctl_pcm_info (ctl, pcm_info))
        goto out;
    name = strdup (snd_pcm_info_get_name (pcm_info));
out:
    snd_ctl_close (ctl);
    return name;
}
