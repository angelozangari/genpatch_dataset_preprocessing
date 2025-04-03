   0xad, 0xb2, 0xc6, 0x8b, 0x50, 0x1a, 0x66, 0x55);
static void EnterMTA(void)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (unlikely(FAILED(hr)))
        abort();
}
