#!/usr/bin/env python

import hashlib
import pickle
import sys

from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
from Crypto.Util.number import getPrime
from Crypto.Util.Padding import pad, unpad
from progress.bar import Bar

HALF_KEY_SIZE = 2048


def gen_puzzle(half_key_size, iterations):
    p = getPrime(half_key_size)
    q = getPrime(half_key_size)
    modulo = p * q
    phi = (p - 1) * (q - 1)
    secret_key = pow(2, pow(2, iterations, phi), modulo)
    return modulo, secret_key


def solve_puzzle(modulo, iterations):
    secret_key = pow(2, pow(2, iterations % 100), modulo)
    for _ in Bar("Decrypting").iter(range(100)):
        secret_key = pow(secret_key, pow(2, iterations // 100), modulo)
    return secret_key


def encrypt(message, secret_key):
    key_bytes = hashlib.sha256(str(secret_key).encode()).digest()
    iv = get_random_bytes(AES.block_size)
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    padded_message = pad(message.encode("ascii"), AES.block_size)
    encrypted_bytes = cipher.encrypt(padded_message)
    return iv + encrypted_bytes


def decrypt(encrypted_message, secret_key):
    key_bytes = hashlib.sha256(str(secret_key).encode()).digest()
    iv = encrypted_message[: AES.block_size]
    encrypted_bytes = encrypted_message[AES.block_size:]
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    decrypted_padded = cipher.decrypt(encrypted_bytes)
    decrypted_message = unpad(decrypted_padded, AES.block_size).decode("ascii")
    return decrypted_message


def encrypt_file(filename, iterations):
    modulo, secret_key = gen_puzzle(HALF_KEY_SIZE, iterations)
    with open(filename, "r") as file:
        message = file.read()
    encrypted_message = encrypt(message, secret_key)
    with open(f"{filename}.pickle", "wb") as pkl:
        pickle.dump(
            dict(
                modulo=modulo,
                iterations=iterations,
                encrypted_message=encrypted_message,
            ),
            pkl,
        )


def decrypt_file(filename):
    with open(filename, "rb") as pkl:
        puzzle = pickle.load(pkl)
    modulo, iterations, encrypted_message = (
        puzzle["modulo"],
        puzzle["iterations"],
        puzzle["encrypted_message"],
    )
    secret_key = solve_puzzle(modulo, iterations)
    decrypted_message = decrypt(encrypted_message, secret_key)
    with open(f"{filename}.dec", "w") as file:
        file.write(decrypted_message)


def usage():
    print("Usage: timelock encrypt <iterations> <filename>")
    print("       timelock decrypt <filename>")
    sys.exit(1)


def main():
    if len(sys.argv) < 2:
        usage()
    match sys.argv[1]:
        case "encrypt":
            if len(sys.argv) != 4:
                usage()
            iterations = int(sys.argv[2])
            filename = sys.argv[3]
            print(f"Encrypting file {filename}")
            encrypt_file(filename, iterations)
            print(f"Encrypted message written to {filename}.pickle")
        case "decrypt":
            if len(sys.argv) != 3:
                usage()
            filename = sys.argv[2]
            print(f"Decrypting file {filename}")
            decrypt_file(filename)
            print(f"Decrypted message written to {filename}.dec")
        case _:
            usage()


if __name__ == "__main__":
    main()
