}
static int zephyr_send_im(PurpleConnection * gc, const char *who, const char *im, PurpleMessageFlags flags)
{
	const char *sig;
	zephyr_account *zephyr = gc->proto_data;
	if (flags & PURPLE_MESSAGE_AUTO_RESP)
		sig = "Automated reply:";
	else {
		sig = zephyr_get_signature();
	}
	zephyr_send_message(zephyr,"MESSAGE","PERSONAL",local_zephyr_normalize(zephyr,who),im,sig,"");
	return 1;
}
