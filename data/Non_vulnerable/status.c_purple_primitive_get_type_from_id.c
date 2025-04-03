PurpleStatusPrimitive
purple_primitive_get_type_from_id(const char *id)
{
    int i;
    g_return_val_if_fail(id != NULL, PURPLE_STATUS_UNSET);
    for (i = 0; i < PURPLE_STATUS_NUM_PRIMITIVES; i++)
    {
		if (purple_strequal(id, status_primitive_map[i].id))
            return status_primitive_map[i].type;
    }
    return status_primitive_map[0].type;
}
