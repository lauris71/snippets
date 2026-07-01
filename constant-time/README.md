# Constant time comparison of values

Constant-time comparison functions that prevent timing attacks by ensuring the execution time does not depend on the input values.

These depend on bit-shuffling instead of branching. While most compilers should leave these as-is, it is always good idea to check the assembly output and disable optimizations. They are not very fast, but in normal use-cases, the security benefit far outweighs the performance cost.

## Functions

### `ct_zero8`
Checks if an 8-bit value is zero in constant time. Returns `0xff` if the value is zero, `0x00` otherwise.

### `ct_zero64`
Checks if a 64-bit value is zero in constant time. Returns `0xffffffffffffffff` if the value is zero, `0x00` otherwise.

### `ct_eq8`
Checks if two 8-bit values are equal in constant time. Returns `0xff` if the values are equal, `0x00` otherwise.

### `ct_eq32`
Checks if two 32-bit values are equal in constant time. Returns `0xff` if the values are equal, `0x00` otherwise.

### `ct_eq64`
Checks if two 64-bit values are equal in constant time. Returns `0xff` if the values are equal, `0x00` otherwise.

### `ge_64`
Compares two 64-bit unsigned integers. Returns `0xff` if the first is greater than or equal to the second, `0x00` otherwise.

### `ct_strncmp`
Compares two strings for equality in constant time. Returns `0xff` if the strings are equal, `0x00` otherwise.

Always perform `lhs_len` steps (and reads all bytes from `lhs` regardless of the length of `rhs`. So if used to compare a known string to a secret string, use `lhs` for the known string.

There is still a slight possibility of leaking the length of the second string (`rhs`) through cache latency. This cannot be solved at this specific point but should be handled by enforcing specific rules to the secret string storage and length.