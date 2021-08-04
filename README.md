# PSEUDOC

My favorite programming language can only be the one crafted by myself, time to be a tryhard, LET'S GO!!!

Be happy everyday!

## Status

Running the memory management down.
-[x] Pointer type of pseudoc: `atom`
-[x] allocate and free
-[ ] get and set value

Testing new ideas.

Finish UT at the same time.

## Code Style
- indent: 2 whitespaces
- naming:
  - lower cases with underscores
  - suffix `_t` for `typedef` types
  - prefix `r_` for pointer type in the parameter used as a c++ reference type
  - prefix `i` sometime might be representing a indirect pointer
- pointer type: * is after the type without a whitespace
  `int* nums`
- brace: start with a whitespace and does not switch to new lines
  ```c
  int foo() {
    return 0;
  }
  ```
- short statement: keep them two line with indent
  ```c
  if (is_new)
    break;
  ```
- no implementations in header file
- use include guard instead of `#pragma once`
