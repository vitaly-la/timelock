#!/usr/bin/env python

import base64
import os
import subprocess
import sys

from Crypto.Cipher import AES
from Crypto.Protocol.KDF import scrypt
from Crypto.Random import get_random_bytes
from Crypto.Util.number import getPrime
from Crypto.Util.Padding import pad, unpad

HALF_KEY_SIZE = 1024


def gen_puzzle(half_key_size: int, iterations: int) -> tuple[int, int]:
    p = getPrime(half_key_size)
    q = getPrime(half_key_size)
    modulo = p * q
    phi = (p - 1) * (q - 1)
    secret_key = pow(2, pow(2, iterations, phi), modulo)
    return modulo, secret_key


def solve_puzzle(iterations: str, modulo: str) -> int:
    proc = subprocess.run(
        [os.environ["SOLVER"], iterations, modulo], stdout=subprocess.PIPE, check=True
    )
    secret_key = int(proc.stdout.decode().strip())
    return secret_key


def encrypt(message: bytes, secret_key: int) -> bytes:
    salt = get_random_bytes(16)
    key_bytes = scrypt(
        secret_key.to_bytes(2 * HALF_KEY_SIZE), salt, 32, N=2**20, r=8, p=1
    )
    iv = get_random_bytes(AES.block_size)
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    padded_message = pad(message, AES.block_size)
    encrypted_message = cipher.encrypt(padded_message)
    return salt + iv + encrypted_message


def decrypt(encrypted_message: bytes, secret_key: int) -> bytes:
    salt = encrypted_message[:16]
    iv = encrypted_message[16 : 16 + AES.block_size]
    encrypted_bytes = encrypted_message[16 + AES.block_size :]
    key_bytes = scrypt(
        secret_key.to_bytes(2 * HALF_KEY_SIZE), salt, 32, N=2**20, r=8, p=1
    )
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    decrypted_padded = cipher.decrypt(encrypted_bytes)
    decrypted_message = unpad(decrypted_padded, AES.block_size)
    return decrypted_message


def encrypt_file(filename: str, iterations: int) -> None:
    modulo, secret_key = gen_puzzle(HALF_KEY_SIZE, iterations)
    with open(filename, "rb") as file:
        message = file.read()
    encrypted_message = encrypt(message, secret_key)
    with open(f"{filename}.enc", "w") as enc:
        enc.write(
            "\n".join(
                f"{k} = {v}"
                for k, v in dict(
                    t=iterations,
                    n=modulo,
                    msg=base64.b64encode(encrypted_message).decode(),
                ).items()
            )
        )
        enc.write("\n")


def decrypt_file(filename: str) -> None:
    with open(filename, "r") as dec:
        puzzle = {
            k: v.strip()
            for k, v in (line.split(" = ") for line in dec.readlines() if " = " in line)
        }
    iterations, modulo, encrypted_message = (
        puzzle["t"],
        puzzle["n"],
        base64.b64decode(puzzle["msg"]),
    )
    secret_key = solve_puzzle(iterations, modulo)
    decrypted_message = decrypt(encrypted_message, secret_key)
    with open(f"{filename}.dec", "wb") as file:
        file.write(decrypted_message)


def usage() -> None:
    print("Usage: timelock encrypt <iterations> <filename>")
    print("       timelock decrypt <filename>")
    sys.exit(1)


def main() -> None:
    if len(sys.argv) < 2:
        usage()
    match sys.argv[1]:
        case "encrypt":
            if len(sys.argv) != 4:
                usage()
            iterations = int(sys.argv[2])
            filename = sys.argv[3]
            print(f"Encrypting {filename}")
            encrypt_file(filename, iterations)
            print(f"Encrypted message written to {filename}.enc")
        case "decrypt":
            if len(sys.argv) != 3:
                usage()
            filename = sys.argv[2]
            print(f"Decrypting {filename}")
            decrypt_file(filename)
            print(f"Decrypted message written to {filename}.dec")
        case _:
            usage()


if __name__ == "__main__":
    main()
