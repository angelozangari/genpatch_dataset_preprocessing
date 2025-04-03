#ifndef __ANDROID__
float strtof (const char *str, char **end)
{
    return strtod (str, end);
}
