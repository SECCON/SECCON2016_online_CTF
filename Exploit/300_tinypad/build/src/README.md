# tinypad

__keywords__: heap exploitation, glibc, Use After Free, malloc\_consolidate, poisoned NUL byte, House of Einherjar

## What's This?
"tinypad" is a pwnable challenge for SECCON 2016 Online CTF as pwn300.  

## My Intended Solution
After analysis, we can get two vulnerabilities, Use After Free(leak only) and Non-NUL terminated string.  
A fastbin-sized free()'d chunk and one smallbin-sized `malloc()` lead a fastbin into `unsorted_chunks` so we can leak the address of `main_arena` and calculate the base address for libc.   
Now, we can corrupt a chunk size by writing into Non-NULL terminated string but we are not able to use House of Force due to limited request size for an allocation. House of Einherjar is suitable in this case.   
Now, we can forge the list of pads and get an arbitrary memory read and a partial write.   

It's enough to write up. See the detail in "exploit\_tinypad.py".  

___Good pwn time,___  
@hhc0null
