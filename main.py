#!/usr/bin/env python

from base64 import b64decode, b64encode
from hashlib import sha256
from pprint import pprint
from time import time

from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
from Crypto.Util.number import getPrime
from Crypto.Util.Padding import pad, unpad
from progress.bar import Bar

HALF_KEY_SIZE = 1024
T = 100000
SECRET_MESSAGE = "Hello, World!"


def gen_puzzle(half_key_size, t):
    p = getPrime(half_key_size)
    q = getPrime(half_key_size)
    n = p * q
    phi = (p - 1) * (q - 1)
    puzzle = pow(2, pow(2, t, phi), n)
    return n, puzzle


def solve_puzzle(t, n):
    solution = 2
    for _ in Bar("Decrypting").iter(range(100)):
        solution = pow(solution, pow(2, t // 100), n)
    return solution


def encrypt(msg, key):
    key_bytes = sha256(str(key).encode()).digest()
    iv = get_random_bytes(AES.block_size)
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    padded_msg = pad(msg.encode("ascii"), AES.block_size)
    encrypted_bytes = cipher.encrypt(padded_msg)
    result = b64encode(iv + encrypted_bytes).decode("ascii")
    return result


def decrypt(msg, key):
    key_bytes = sha256(str(key).encode()).digest()
    encrypted_data = b64decode(msg.encode("ascii"))
    iv = encrypted_data[: AES.block_size]
    ciphertext = encrypted_data[AES.block_size:]
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    decrypted_padded = cipher.decrypt(ciphertext)
    decrypted_msg = unpad(decrypted_padded, AES.block_size).decode("ascii")
    return decrypted_msg


def main():
    n, puzzle = gen_puzzle(HALF_KEY_SIZE, T)
    encrypted_message = encrypt(SECRET_MESSAGE, puzzle)
    pprint(dict(n=n, T=T, encrypted_message=encrypted_message))

    print(
        "\n================================================================================\n"
    )

    start = time()
    solution = solve_puzzle(T, n)
    elapsed = time() - start
    decrypted_message = decrypt(encrypted_message, solution)
    pprint(dict(elapsed=elapsed, decrypted_message=decrypted_message))


if __name__ == "__main__":
    main()
