PurpleValue *
purple_value_dup(const PurpleValue *value)
{
	PurpleValue *new_value;
	PurpleType type;
	g_return_val_if_fail(value != NULL, NULL);
	type = purple_value_get_type(value);
	if (type == PURPLE_TYPE_SUBTYPE)
	{
		new_value = purple_value_new(PURPLE_TYPE_SUBTYPE,
								   purple_value_get_subtype(value));
	}
	else if (type == PURPLE_TYPE_BOXED)
	{
		new_value = purple_value_new(PURPLE_TYPE_BOXED,
								   purple_value_get_specific_type(value));
	}
	else
		new_value = purple_value_new(type);
	new_value->flags = value->flags;
	switch (type)
	{
		case PURPLE_TYPE_CHAR:
			purple_value_set_char(new_value, purple_value_get_char(value));
			break;
		case PURPLE_TYPE_UCHAR:
			purple_value_set_uchar(new_value, purple_value_get_uchar(value));
			break;
		case PURPLE_TYPE_BOOLEAN:
			purple_value_set_boolean(new_value, purple_value_get_boolean(value));
			break;
		case PURPLE_TYPE_SHORT:
			purple_value_set_short(new_value, purple_value_get_short(value));
			break;
		case PURPLE_TYPE_USHORT:
			purple_value_set_ushort(new_value, purple_value_get_ushort(value));
			break;
		case PURPLE_TYPE_INT:
			purple_value_set_int(new_value, purple_value_get_int(value));
			break;
		case PURPLE_TYPE_UINT:
			purple_value_set_uint(new_value, purple_value_get_uint(value));
			break;
		case PURPLE_TYPE_LONG:
			purple_value_set_long(new_value, purple_value_get_long(value));
			break;
		case PURPLE_TYPE_ULONG:
			purple_value_set_ulong(new_value, purple_value_get_ulong(value));
			break;
		case PURPLE_TYPE_INT64:
			purple_value_set_int64(new_value, purple_value_get_int64(value));
			break;
		case PURPLE_TYPE_UINT64:
			purple_value_set_uint64(new_value, purple_value_get_uint64(value));
			break;
		case PURPLE_TYPE_STRING:
			purple_value_set_string(new_value, purple_value_get_string(value));
			break;
		case PURPLE_TYPE_OBJECT:
			purple_value_set_object(new_value, purple_value_get_object(value));
			break;
		case PURPLE_TYPE_POINTER:
			purple_value_set_pointer(new_value, purple_value_get_pointer(value));
			break;
		case PURPLE_TYPE_ENUM:
			purple_value_set_enum(new_value, purple_value_get_enum(value));
			break;
		case PURPLE_TYPE_BOXED:
			purple_value_set_boxed(new_value, purple_value_get_boxed(value));
			break;
		default:
			break;
	}
	return new_value;
}
