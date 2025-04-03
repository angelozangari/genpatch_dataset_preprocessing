const char *
purple_primitive_get_id_from_type(PurpleStatusPrimitive type)
{
    int i;
    for (i = 0; i < PURPLE_STATUS_NUM_PRIMITIVES; i++)
    {
		if (type == status_primitive_map[i].type)
			return status_primitive_map[i].id;
    }
    return status_primitive_map[0].id;
}
