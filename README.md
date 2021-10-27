# nmproj

A (work in progress) proof of concept of
[Load+Reload](https://mlq.me/download/takeaway.pdf).
This side channel attack takes advantage
of how accessing two virtual addresses
mapped to the same physical address
in succession makes the processor
fallback to the L2 cache
in recent AMD architectures.

`victim.c` and `attacker.c` already work
and can be run together to detect
whether the victim is accessing
a certain memory address.
The PoC of how this can be used
to extract information isn't ready yet.
