# Research Protocol (Results Pending)

## Working title
A Comparative Experimental Study of Standardized Post-Quantum Cryptographic Operations and Classical Signature Baselines

## Research questions

1. What operation latency and output-size differences are observed between ML-DSA-65, RSA-3072, and ECDSA P-256 on the same host and OpenSSL build?
2. What are the observed key-generation, encapsulation, and decapsulation costs of ML-KEM-768?
3. How reproducible are these measurements across repeated runs and documented environments?
4. What implementation and migration considerations arise when integrating standardized PQC primitives into C++ applications?

## Hypotheses (to test, not assume)

- H1: The measured latency and output-size profiles of ML-DSA-65 differ materially from RSA-3072 and ECDSA P-256.
- H2: ML-KEM-768 can be integrated through OpenSSL's EVP provider interface with successful shared-secret agreement.
- H3: Performance conclusions are sensitive to implementation version, hardware, compiler settings, and system load.

## Experimental design

- Algorithms: ML-KEM-768, ML-DSA-65, RSA-3072/SHA-256, ECDSA P-256/SHA-256.
- Implementation: C++20 using OpenSSL 3.5+ EVP APIs.
- Repetitions: at least 100 per operation for a report-quality run; use 20 only for smoke testing.
- Record: CPU model, core count, RAM, OS/kernel, compiler/version/flags, OpenSSL version/build/provider, iteration count, date, and background-load notes.
- Report: raw CSV, median, mean, standard deviation, interquartile range, and output lengths.
- Correctness: verify ML-KEM shared-secret equality; verify valid signatures; confirm tampered messages fail verification.

## Scope boundaries

This is not a cryptanalysis, security proof, FIPS validation, side-channel evaluation, or complete TLS migration study. RSA/ECDSA signature timings must not be directly compared with ML-KEM KEM timings because they perform different functions.

## Results table (fill only after running)

| Algorithm | Operation | Median (ms) | Mean (ms) | Output bytes | Notes |
|---|---|---:|---:|---:|---|
| ML-KEM-768 | Key generation | TBD | TBD | TBD | |
| ML-KEM-768 | Encapsulation | TBD | TBD | TBD | |
| ML-KEM-768 | Decapsulation | TBD | TBD | TBD | |
| ML-DSA-65 | Key generation | TBD | TBD | TBD | |
| ML-DSA-65 | Signing | TBD | TBD | TBD | |
| ML-DSA-65 | Verification | TBD | TBD | TBD | |
| RSA-3072 | Key generation | TBD | TBD | TBD | |
| RSA-3072 | Signing | TBD | TBD | TBD | |
| RSA-3072 | Verification | TBD | TBD | TBD | |
| ECDSA P-256 | Key generation | TBD | TBD | TBD | |
| ECDSA P-256 | Signing | TBD | TBD | TBD | |
| ECDSA P-256 | Verification | TBD | TBD | TBD | |

## Abstract template (complete after experiments)

This study implements selected standardized post-quantum cryptographic operations and classical digital-signature baselines using C++20 and OpenSSL 3.5+. It evaluates [insert measured scope] on [insert documented environment] using [insert repetitions and statistics]. The experiments observed [insert results supported by raw data]. The study discusses integration considerations and limitations, including [insert limitations]. The implementation is an experimental prototype and does not constitute a cryptographic security proof or validated module.
