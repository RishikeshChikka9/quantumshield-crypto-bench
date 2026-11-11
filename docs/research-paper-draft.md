# A Comparative Experimental Study of Standardized Post-Quantum Cryptographic Operations and Classical Signature Baselines

**Author:** Rishikesh Chikka  
**Status:** Working research draft — experiments pending  
**Affiliation:** Independent research / PQC startup work (affiliation and startup name to be confirmed by author)

## Abstract

The standardization of post-quantum cryptography (PQC) creates a practical need to understand not only the security goals of new algorithms but also their implementation characteristics and integration requirements. This study develops a C++20 experimental benchmark using OpenSSL 3.5+ to exercise ML-KEM-768 (FIPS 203), ML-DSA-65 (FIPS 204), and classical digital-signature baselines based on RSA-3072 and ECDSA P-256. The implementation measures key-generation, encapsulation, decapsulation, signing, and verification latency, records output lengths where available, and performs basic correctness checks. The final paper will report results only after repeatable experiments are run on a documented hardware and software environment. This work is an implementation and performance study; it is not a cryptanalysis, security proof, side-channel evaluation, or FIPS 140 validation.

**Keywords:** post-quantum cryptography, ML-KEM, ML-DSA, RSA, ECDSA, OpenSSL, benchmarking, cryptographic migration

## 1. Introduction

Public-key cryptography is widely used for key establishment, authentication, and digital signatures. The security of commonly deployed RSA and elliptic-curve cryptography depends on mathematical problems for which sufficiently capable quantum computers would change the computational threat model. Shor's algorithm is relevant to integer factorization and discrete logarithm problems, motivating migration planning for public-key cryptographic systems.

NIST has standardized a first set of post-quantum algorithms. FIPS 203 specifies ML-KEM, a key-encapsulation mechanism based on the Module Learning With Errors problem. FIPS 204 specifies ML-DSA, a lattice-based digital-signature scheme. FIPS 205 specifies SLH-DSA, a stateless hash-based digital-signature scheme. These standards provide concrete algorithm specifications for implementation and interoperability work.

The transition to PQC is not only an algorithm-selection exercise. Implementers must consider API availability, parameter sets, key and signature sizes, performance, protocol compatibility, operational constraints, and cryptographic agility. This project studies a limited subset of those engineering considerations through a reproducible C++ benchmark.

## 2. Research objectives and questions

The objectives are to (1) integrate selected standardized PQC algorithms through a maintained cryptographic library, (2) measure selected operation costs under a documented environment, (3) compare ML-DSA signing and verification with classical signature baselines, and (4) identify implementation and migration considerations.

Research questions:

- RQ1: What latency and output-size profiles are observed for ML-DSA-65, RSA-3072, and ECDSA P-256 on the same host and library build?
- RQ2: What key-generation, encapsulation, and decapsulation costs are observed for ML-KEM-768?
- RQ3: How stable are measurements across repeated runs, and which environment details materially affect reproducibility?
- RQ4: What practical integration considerations arise when using standardized PQC algorithms through OpenSSL's EVP interface?

## 3. Standards and algorithm selection

### 3.1 ML-KEM-768

ML-KEM is standardized in NIST FIPS 203. A KEM enables two parties to establish a shared secret over a public channel; that shared secret is then typically used with symmetric cryptography. ML-KEM-768 is the middle parameter set among ML-KEM-512, ML-KEM-768, and ML-KEM-1024.

### 3.2 ML-DSA-65

ML-DSA is standardized in NIST FIPS 204 and provides digital signatures. ML-DSA-65 is selected for this experiment as a representative parameter set. The implementation uses the library's standardized algorithm interface rather than implementing the lattice scheme directly.

### 3.3 Classical signature baselines

RSA-3072 with SHA-256 and ECDSA over P-256 with SHA-256 are included as classical signature baselines. They are measured for key generation, signing, and verification. These baselines do not perform the same function as ML-KEM; therefore, their signature timings must not be compared directly with KEM timings.

## 4. Implementation architecture

The prototype is written in C++20 and uses OpenSSL 3.5+ EVP interfaces. It contains:

- ML-KEM-768 key generation, encapsulation, decapsulation, and shared-secret equality validation.
- ML-DSA-65 key generation, signing, verification, and a negative test using a modified message.
- RSA-3072/SHA-256 and ECDSA P-256/SHA-256 signature baselines.
- Per-operation latency capture, median and mean summaries, and CSV export.

The project deliberately delegates cryptographic primitives to OpenSSL rather than introducing custom cryptographic code. It is an experimental benchmark, not a production security product.

## 5. Experimental methodology

The final experiment should use at least 100 repetitions per operation after a smoke test. The report must record CPU model, core count, memory, operating system and kernel, compiler and flags, OpenSSL version and build configuration, provider information, date, iteration count, and relevant background-load conditions.

The benchmark records individual operation times. The final analysis should report median, arithmetic mean, standard deviation, interquartile range, and output sizes where the API exposes them. Raw CSV output must be retained with the paper. Where results are highly variable, additional repetitions and controlled runs should be performed.

Correctness checks include equality of the sender and receiver ML-KEM shared secrets, successful verification of valid signatures, and rejection of a modified message. These checks validate basic API usage and round-trip behavior; they do not establish cryptographic security.

## 6. Results

**Pending.** No performance conclusion should be written until the planned experiment has been run on the author's documented environment. Insert the raw CSV-derived table and plots here. Do not substitute smoke-test values for the final experiment.

## 7. Discussion

The discussion should distinguish algorithmic properties from implementation-specific observations. Performance depends on hardware, compiler optimizations, library version, provider implementation, and system load. Signature sizes and key/ciphertext sizes should be discussed separately from operation latency. The paper should avoid treating benchmark speed as a proxy for security strength.

## 8. Limitations

This study does not perform cryptanalysis, formal verification, side-channel testing, fault-injection testing, interoperability testing across independent implementations, TLS deployment evaluation, certificate-chain analysis, or FIPS 140 module validation. The current prototype does not measure raw public/private key encodings for every algorithm. Results from one host cannot be generalized to all deployment environments.

## 9. Conclusion

To be completed after the experiment. The conclusion should answer the research questions using only measured evidence, state the limitations, and identify next steps such as hybrid key exchange experiments, TLS integration, key/certificate lifecycle analysis, and migration verification.

## References

1. NIST, *FIPS 203: Module-Lattice-Based Key-Encapsulation Mechanism Standard*, 2024. https://csrc.nist.gov/pubs/fips/203/final
2. NIST, *FIPS 204: Module-Lattice-Based Digital Signature Standard*, 2024. https://csrc.nist.gov/pubs/fips/204/final
3. NIST, *FIPS 205: Stateless Hash-Based Digital Signature Standard*, 2024. https://csrc.nist.gov/pubs/fips/205/final
4. OpenSSL, *OpenSSL 3.5 Final Release*, 2025. https://openssl-library.org/post/2025-04-08-openssl-35-final-release/
5. Open Quantum Safe, *liboqs documentation*. https://openquantumsafe.org/liboqs/
