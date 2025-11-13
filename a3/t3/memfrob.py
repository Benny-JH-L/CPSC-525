'''
Encrypts a string in argv[1] by xoring every character with 170 (0xAA).

Outputs the result as a string of \\xHH byte escape sequences, directly usable
in C source code as a string.

You may need to use this to generate the encrypted password for task3-vuln.c.
'''

import sys

def main(s:str) -> None:
    # xor every character with 170
    result = [ord(c) ^ 170 for c in s]
    # output as \xHH byte escape sequences
    print("".join(f"\\x{b:02x}" for b in result))


if __name__ == "__main__":
    main(sys.argv[1])
