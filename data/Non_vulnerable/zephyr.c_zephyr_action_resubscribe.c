}
static void zephyr_action_resubscribe(PurplePluginAction *action)
{
	PurpleConnection *gc = (PurpleConnection *) action->context;
	zephyr_resubscribe(gc);
}
