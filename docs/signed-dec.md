
# Story behind signed decimals via complementary binaries
```c
/*
    can't init undefined because CPU doesn't clear stack memory
    because 00000000ing a stack slot would be redundant when they are mostly
    overwritten by new vals anyways.
    therefore prev vals from prev calls will be used instead.
    Safer to init as -1 (0000 0001 -> 1111 1110 + 1 = 1111 1111) || '0x' =
    base16 (i.e., 4 bits each) + 'FF' [1 bit complement] than 0 because 0 byte is
    also when stack slot is cleared (just marked differently).
    ^ Segmenting opposite sets of 5 bit pos and neg ints
    example given by Intro to Computer Systems:
    - compiler flags as signed, MSB = 1 means negative.
    - 1s complement: 01111 -> 10000 = -15, 2s complement: 10000 + 1 -> 10001 =
    -15, 10000 = -16 instead
    - ^ 2s complement is 'superior' because it has no redundant -0 (zero is
    unsigned lol)
  */
  int start = -1, end = -1;
```