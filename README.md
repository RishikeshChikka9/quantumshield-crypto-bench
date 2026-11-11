# QuantumShield Crypto Benchmark Lab

A C++20 research prototype for measuring selected standardized post-quantum cryptographic operations alongside classical signature baselines.

## Scope

- **ML-KEM-768 (NIST FIPS 203):** key generation, encapsulation, decapsulation, and shared-secret equality check.
- **ML-DSA-65 (NIST FIPS 204):** key generation, signing, verification, and negative verification test.
- **RSA-3072 + SHA-256:** classical signature baseline.
- **ECDSA P-256 + SHA-256:** classical signature baseline.

The implementation delegates cryptographic primitives to OpenSSL 3.5+; it does not reimplement cryptographic mathematics. This is intentional: production cryptographic primitives should come from maintained, reviewed implementations.

## Requirements

- C++20 compiler
- CMake 3.20+
- OpenSSL 3.5+ with default provider algorithms available

Check algorithm availability:

```bash
openssl version
openssl list -kem-algorithms | grep -i ML-KEM
openssl list -signature-algorithms | grep -i ML-DSA
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Run

```bash
./build/qs-crypto-bench --iterations 20 --csv results.csv
```

On Windows, run the executable from the generated build directory (for example `build\\Release\\qs-crypto-bench.exe`).

## What is measured

- Median and arithmetic mean operation latency in milliseconds.
- Encapsulation ciphertext length, shared-secret length, and signature length where available.
- A CSV row for every measured operation.
- Correctness checks: ML-KEM sender/receiver shared secrets must match; signatures must verify; tampered messages must fail verification.

Key generation is included in each iteration. Results are therefore prototype-level measurements and must not be compared across machines without documenting CPU, OS, compiler, OpenSSL version/build, power profile, iteration count, and background load.

## Important limitations

1. This project is a benchmark and API-integration prototype, not a certified cryptographic module.
2. It does not claim FIPS 140 validation merely because it uses NIST-standardized algorithms.
3. It does not implement TLS, certificates, hybrid key exchange, side-channel testing, formal verification, or production key management.
4. RSA/ECDSA are signature baselines here; ML-KEM is evaluated as a KEM. These are different cryptographic functions and their timings should not be compared as if they performed the same task.
5. ML-KEM output sizes are recorded from the API operation; private/public key byte sizes are not yet exported by this prototype.
6. Benchmark results are machine- and provider-dependent. Do not publish results until the environment and raw CSV are included.

## Research integrity

Do not add invented measurements or claim security proofs. Run the experiment, preserve raw CSV files, record the environment, repeat runs, and discuss limitations. Treat this as a learning/research prototype, not a security product.

## References

- NIST FIPS 203: https://csrc.nist.gov/pubs/fips/203/final
- NIST FIPS 204: https://csrc.nist.gov/pubs/fips/204/final
- NIST FIPS 205: https://csrc.nist.gov/pubs/fips/205/final
- OpenSSL 3.5 release: https://openssl-library.org/post/2025-04-08-openssl-35-final-release/
