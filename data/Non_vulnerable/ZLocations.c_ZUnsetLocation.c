}
Code_t ZUnsetLocation()
{
    return (Z_SendLocation(LOGIN_CLASS, LOGIN_USER_LOGOUT, ZNOAUTH,
			   "$sender logged out of $1 on $3 at $2"));
}
