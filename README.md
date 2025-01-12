# INTENT

1. Recording my solution to Thorsten Ball's Writing An Interpreter In Go... but doing it in C
2. Why?
    1. Practicing my C
    2. Sharpening memory management intuitions
    3. Because it's fun
    4. To make me think more deeply about what I'm writing ie. resist just copying the code examples
3. What this is not:
    1. The most performant safest C code you can write - this is a playground for me, I am deliberately trying different things that could be inconsistent with each other just because I can.
    2. It's not even good - there are mallocs hanging out everywhere because it helps me follow along the book (ie. I don't really know what the appropriate lifetimes are so I just heap allocate when in doubt)
4. Notes to myself:
    1. Clean up the TODOS
    2. Come up with an actual memory management scheme
    3. Use macros to implement generics instead of void *
    4. Implement garbage collection, this isn't Goland
