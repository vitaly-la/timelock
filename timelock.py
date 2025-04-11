#!/usr/bin/env python

import base64
import hashlib
import os
import subprocess
import sys
import time

from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
from Crypto.Util.number import getPrime
from Crypto.Util.Padding import pad, unpad
from progress.bar import Bar

HALF_KEY_SIZE = 1536


def gen_puzzle(squarings: int) -> tuple[int, int]:
    p = getPrime(HALF_KEY_SIZE)
    q = getPrime(HALF_KEY_SIZE)
    modulo = p * q
    phi = (p - 1) * (q - 1)
    secret_key = pow(2, pow(2, squarings, phi), modulo)
    return secret_key, modulo


def solve_puzzle(squarings: int, modulo: str) -> int:
    proc = subprocess.run(
        [os.environ["SOLVER"], "2", str(squarings % 100), modulo],
        stdout=subprocess.PIPE,
    )
    base = proc.stdout.decode().strip()
    for _ in Bar("Solving time-lock puzzle").iter(range(100)):
        proc = subprocess.run(
            [os.environ["SOLVER"], base, str(squarings // 100), modulo],
            stdout=subprocess.PIPE,
        )
        base = proc.stdout.decode().strip()
    secret_key = int(base)
    return secret_key


def encrypt(message: bytes, secret_key: int) -> bytes:
    key_bytes = hashlib.sha256(secret_key.to_bytes(2 * HALF_KEY_SIZE)).digest()
    iv = get_random_bytes(AES.block_size)
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    padded_message = pad(message, AES.block_size)
    encrypted_message = cipher.encrypt(padded_message)
    return iv + encrypted_message


def decrypt(encrypted_message: bytes, secret_key: int) -> bytes:
    key_bytes = hashlib.sha256(secret_key.to_bytes(2 * HALF_KEY_SIZE)).digest()
    iv = encrypted_message[: AES.block_size]
    cipher = AES.new(key_bytes, AES.MODE_CBC, iv)
    encrypted_bytes = encrypted_message[AES.block_size :]
    decrypted_padded = cipher.decrypt(encrypted_bytes)
    decrypted_message = unpad(decrypted_padded, AES.block_size)
    return decrypted_message


def lock_file(filename: str, squarings: int) -> None:
    with open(filename, "rb") as file:
        message = file.read()
    secret_key, modulo = gen_puzzle(squarings)
    encrypted_message = encrypt(message, secret_key)
    with open(f"{filename}.enc", "w") as enc:
        enc.write(
            "\n".join(
                f"{k} = {v}"
                for k, v in dict(
                    t=squarings,
                    n=modulo,
                    msg=base64.b64encode(encrypted_message).decode(),
                ).items()
            )
        )
        enc.write("\n")


def unlock_file(filename: str) -> None:
    with open(filename, "r") as dec:
        puzzle = {
            k: v.strip()
            for k, v in (line.split(" = ") for line in dec.readlines() if " = " in line)
        }
    squarings, modulo, encrypted_message = (
        int(puzzle["t"]),
        puzzle["n"],
        base64.b64decode(puzzle["msg"]),
    )
    secret_key = solve_puzzle(squarings, modulo)
    decrypted_message = decrypt(encrypted_message, secret_key)
    with open(f"{filename}.dec", "wb") as file:
        file.write(decrypted_message)


def benchmark_ns(squarings: str) -> int:
    modulo = str(gen_puzzle(0)[1])
    start = time.perf_counter_ns()
    subprocess.run(
        [os.environ["SOLVER"], "2", squarings, modulo],
        stdout=subprocess.DEVNULL,
    )
    return time.perf_counter_ns() - start


def usage() -> None:
    print("Usage: timelock lock <squarings>|<seconds>s <filename>")
    print("       timelock unlock <filename>")
    sys.exit(1)


def main() -> None:
    if len(sys.argv) < 2:
        usage()
    match sys.argv[1]:
        case "lock":
            if len(sys.argv) != 4:
                usage()
            if sys.argv[2].endswith("s"):
                print("Running benchmark")
                test_squarings = 1000000
                elapsed_ns = benchmark_ns(str(test_squarings))
                squarings = test_squarings * int(sys.argv[2][:-1]) * 10**9 // elapsed_ns
                rate = test_squarings * 10**9 // elapsed_ns
                print(
                    f"{squarings} squarings take approximately {sys.argv[2]} with rate {rate}/s"
                )
            else:
                squarings = int(sys.argv[2])
            filename = sys.argv[3]
            print(f"Locking {filename}")
            lock_file(filename, squarings)
            print(f"Time-locked message written to {filename}.enc")
        case "unlock":
            if len(sys.argv) != 3:
                usage()
            filename = sys.argv[2]
            print(f"Unlocking {filename}")
            unlock_file(filename)
            print(f"Unlocked message written to {filename}.dec")
        case _:
            usage()


if __name__ == "__main__":
    main()
