# RISC-V Bitmanip (Bit Manipulation) Extension

This is the RISC-V Bitmanip Extension draft spec. The spec aims to
extend the RISC-V instruction set to enable efficent bit manipulation.
This includes operations like;
 * counting bits, leading zeros, etc
 * bit extraction
 * rotations, shifting and reversing

Originally it was the B-Extension draft spec, but the work group got
dissolved for burocratic reasons in November 2017.

It is currently an independently maintained document. We'd happily donate
it to the RISC-V foundation as starting point for a new B-Extension work
group, if there will be one.

See [bitmanip-draft.pdf](bitmanip-draft.pdf) for a pre-built version
of the PDF spec document.

Building this document from the LaTeX source:

    bash bitmanip.sh 5
