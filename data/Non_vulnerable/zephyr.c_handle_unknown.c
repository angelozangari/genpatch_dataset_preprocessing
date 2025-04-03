/* just for debugging */
static void handle_unknown(ZNotice_t *notice)
{
	purple_debug_error("zephyr","z_packet: %s\n", notice->z_packet);
	purple_debug_error("zephyr","z_version: %s\n", notice->z_version);
	purple_debug_error("zephyr","z_kind: %d\n", (int)(notice->z_kind));
	purple_debug_error("zephyr","z_class: %s\n", notice->z_class);
	purple_debug_error("zephyr","z_class_inst: %s\n", notice->z_class_inst);
	purple_debug_error("zephyr","z_opcode: %s\n", notice->z_opcode);
	purple_debug_error("zephyr","z_sender: %s\n", notice->z_sender);
	purple_debug_error("zephyr","z_recipient: %s\n", notice->z_recipient);
	purple_debug_error("zephyr","z_message: %s\n", notice->z_message);
	purple_debug_error("zephyr","z_message_len: %d\n", notice->z_message_len);
}
