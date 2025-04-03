static void
signed_off_cb(PurpleConnection *pc, gpointer unused)
{
	/* Deal with any dialogs */
	pidgin_disco_signed_off_cb(pc);
	/* Remove all the IQ callbacks for this connection */
	g_hash_table_foreach_remove(iq_callbacks, remove_iq_callbacks_by_pc, pc);
}
