# Hex2Int

We can use some simple bit mangling to distinguish between possible hex digits.

It is probably not the fastest method because on modern processors the branch prediction is really effective. But it
may be useful if you absolutely want to be economic on prediction units and cache lines.

#### The idea

To decide whether the byte is a valid hexadecimal digit, we have to determine whether it has one of the following patterns:
```
P1: 00110xxx [0..7]
P2: 0011100x [8..9]
P3: 01x000xx [A..C,a..c] except 01000000,01100000
P4: 01x001xx [D..F,d..f] except 01000111,01100111
```
The letters are messy, so subtract one from the ASCII value (for P3 and P4)
```
P1  : 00110xxx [0..7]
P2  : 0011100x [8..9]
P3-1: 01x000xx [@..C,`..c]
P4-1: 01x0010x [D..E,d..e]
```
Distribute the value over 32 bits as [P1][P2][P3-1][P4-1]:

> uint64_t v = (hex << 24) | (hex << 16) | ((hex - 1) << 8) | (hex - 1);

Now we have to test whether any 8-bit block has corresponding bit pattern.

First clear the variable bits of each pattern:

> v &= 0b011111000111111101101110011011110;

Then xor it with anti-pattern

> v ^= 0b011001111110001111011111110111011;

Now, if any 8-bit block has the correct pattern, it has all bits set.

Add one to each pattern.

If the pattern is 11111111, one will be carried over to the next position. It will be preserved
regardless of the next pattern value because the lowest bit of the next pattern is always set and
we also add one to the next pattern as well.

> v += 0b000000001000000010000000100000001;

Pick out the carry bits:

> v &= 0b100000001000000010000000100000000;

If the result is nonzero a pattern was present so we can use straightforward hex value calculation:

return (hex & 0b1111) + 9 * ((hex & 0b01000000) >> 6);

The straightforward algorithm requires 64 bits because four 8-bit patterns cover 32 bits and the addition may
carry over to bit 32. But it can be done in 32 bits too, if we discard the rightmost bit of P1 and/or P3-1
(i.e. use 7-bit pattern for these).
