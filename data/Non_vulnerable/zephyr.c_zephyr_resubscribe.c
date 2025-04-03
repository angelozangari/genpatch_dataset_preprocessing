}
static int zephyr_resubscribe(PurpleConnection *gc)
{
	/* Resubscribe to the in-memory list of subscriptions and also
	   unsubscriptions*/
	zephyr_account *zephyr = gc->proto_data;
	GSList *s = zephyr->subscrips;
	zephyr_triple *zt;
	while (s) {
		zt = s->data;
		/* XXX We really should care if this fails */
		zephyr_subscribe_to(zephyr,zt->class,zt->instance,zt->recipient,NULL);
		s = s->next;
	}
	/* XXX handle unsubscriptions */
	return 1;
}
