#pragma once
// Define the struct
struct KeyValuePair {
    PCWSTR key;
    PCWSTR value;
};

// Define an array of key-value pairs
KeyValuePair alg_id[] = {
    {L"6603", L"Triple DES"},
    {L"6609", L"Two-Key Triple DES"},
    {L"6611", L"AES"},
    {L"660e", L"128-bit AES"},
    {L"660f", L"192-bit AES"},
    {L"6610", L"256-bit AES"},
    {L"6601", L"DES"},
    {L"6604", L"DESX"},
    {L"aa05", L"Elliptic curve Diffie-Hellman"},
    {L"ae06", L"Ephemeral elliptic curve Diffie-Hellman"},
    {L"6602", L"RC2 block"},
    {L"6801", L"RC4 stream"},
    {L"800c", L"SHA256"},
    {L"800d", L"SHA384"},
    {L"800e", L"SHA512"}
};

// Get the size of the array
constexpr size_t alg_count = sizeof(alg_id) / sizeof(alg_id[0]);

// Function to find a value by key
PCWSTR findValue(PCWSTR searchKey) {
    for (size_t i = 0; i < alg_count; ++i) {
        if (wcscmp(alg_id[i].key, searchKey) == 0) { // Compare wide strings
            return alg_id[i].value;
        }
    }
    return L"Key not found";
}
