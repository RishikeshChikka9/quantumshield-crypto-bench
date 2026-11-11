#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/core_names.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/crypto.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

using Clock = std::chrono::steady_clock;
using PKeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using PKeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
using MdCtxPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

static void fail(const std::string& what) {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error(what);
}

static PKeyPtr generate_key(const std::string& algorithm) {
    PKeyCtxPtr ctx(EVP_PKEY_CTX_new_from_name(nullptr, algorithm.c_str(), nullptr), EVP_PKEY_CTX_free);
    if (!ctx || EVP_PKEY_keygen_init(ctx.get()) <= 0) fail("keygen init failed for " + algorithm);

    if (algorithm == "RSA") {
        if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(), 3072) <= 0) fail("RSA key size setup failed");
    } else if (algorithm == "EC") {
        if (EVP_PKEY_CTX_set_group_name(ctx.get(), "prime256v1") <= 0) fail("EC curve setup failed");
    }

    EVP_PKEY* raw = nullptr;
    if (EVP_PKEY_generate(ctx.get(), &raw) <= 0) fail("key generation failed for " + algorithm);
    return PKeyPtr(raw, EVP_PKEY_free);
}

struct Sample {
    std::string algorithm;
    std::string operation;
    double milliseconds;
    std::size_t output_bytes;
};

static double elapsed_ms(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

static void benchmark_kem(int iterations, std::vector<Sample>& samples) {
    const std::string alg = "ML-KEM-768";
    for (int i = 0; i < iterations; ++i) {
        auto start = Clock::now();
        auto key = generate_key(alg);
        samples.push_back({alg, "keygen", elapsed_ms(start), 0});

        PKeyCtxPtr enc(EVP_PKEY_CTX_new(key.get(), nullptr), EVP_PKEY_CTX_free);
        if (!enc || EVP_PKEY_encapsulate_init(enc.get(), nullptr) <= 0) fail("ML-KEM encapsulate init failed");
        std::size_t ct_len = 0, ss_len = 0;
        if (EVP_PKEY_encapsulate(enc.get(), nullptr, &ct_len, nullptr, &ss_len) <= 0) fail("ML-KEM output sizing failed");
        std::vector<unsigned char> ciphertext(ct_len), sender_secret(ss_len);
        start = Clock::now();
        if (EVP_PKEY_encapsulate(enc.get(), ciphertext.data(), &ct_len, sender_secret.data(), &ss_len) <= 0) fail("ML-KEM encapsulation failed");
        samples.push_back({alg, "encapsulate", elapsed_ms(start), ct_len});

        PKeyCtxPtr dec(EVP_PKEY_CTX_new(key.get(), nullptr), EVP_PKEY_CTX_free);
        if (!dec || EVP_PKEY_decapsulate_init(dec.get(), nullptr) <= 0) fail("ML-KEM decapsulate init failed");
        std::size_t recovered_len = 0;
        if (EVP_PKEY_decapsulate(dec.get(), nullptr, &recovered_len, ciphertext.data(), ct_len) <= 0) fail("ML-KEM decapsulation sizing failed");
        std::vector<unsigned char> recovered(recovered_len);
        start = Clock::now();
        if (EVP_PKEY_decapsulate(dec.get(), recovered.data(), &recovered_len, ciphertext.data(), ct_len) <= 0) fail("ML-KEM decapsulation failed");
        samples.push_back({alg, "decapsulate", elapsed_ms(start), recovered_len});
        if (ss_len != recovered_len || CRYPTO_memcmp(sender_secret.data(), recovered.data(), ss_len) != 0)
            fail("ML-KEM shared-secret round-trip check failed");
    }
}

static std::vector<unsigned char> sign_message(EVP_PKEY* key, const std::string& algorithm,
                                                const std::vector<unsigned char>& message) {
    MdCtxPtr ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    const char* digest = (algorithm == "ML-DSA-65") ? nullptr : "SHA256";
    if (!ctx || EVP_DigestSignInit_ex(ctx.get(), nullptr, digest, nullptr, nullptr, key, nullptr) <= 0)
        fail("signature init failed for " + algorithm);
    std::size_t sig_len = 0;
    if (EVP_DigestSign(ctx.get(), nullptr, &sig_len, message.data(), message.size()) <= 0)
        fail("signature sizing failed for " + algorithm);
    std::vector<unsigned char> signature(sig_len);
    if (EVP_DigestSign(ctx.get(), signature.data(), &sig_len, message.data(), message.size()) <= 0)
        fail("signing failed for " + algorithm);
    signature.resize(sig_len);
    return signature;
}

static bool verify_message(EVP_PKEY* key, const std::string& algorithm,
                           const std::vector<unsigned char>& message,
                           const std::vector<unsigned char>& signature) {
    MdCtxPtr ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    const char* digest = (algorithm == "ML-DSA-65") ? nullptr : "SHA256";
    if (!ctx || EVP_DigestVerifyInit_ex(ctx.get(), nullptr, digest, nullptr, nullptr, key, nullptr) <= 0)
        fail("verification init failed for " + algorithm);
    const int result = EVP_DigestVerify(ctx.get(), signature.data(), signature.size(), message.data(), message.size());
    if (result < 0) fail("signature verification operation failed for " + algorithm);
    return result == 1;
}

static void benchmark_signature(const std::string& alg, int iterations, std::vector<Sample>& samples) {
    const std::vector<unsigned char> message = {
        'Q','u','a','n','t','u','m','S','h','i','e','l','d',' ','r','e','s','e','a','r','c','h',' ','b','e','n','c','h','m','a','r','k'
    };
    for (int i = 0; i < iterations; ++i) {
        auto start = Clock::now();
        auto key = generate_key(alg);
        samples.push_back({alg, "keygen", elapsed_ms(start), 0});

        start = Clock::now();
        auto signature = sign_message(key.get(), alg, message);
        samples.push_back({alg, "sign", elapsed_ms(start), signature.size()});

        start = Clock::now();
        const bool valid = verify_message(key.get(), alg, message, signature);
        samples.push_back({alg, "verify", elapsed_ms(start), signature.size()});
        if (!valid) fail("signature round-trip verification failed for " + alg);

        auto tampered = message;
        tampered[0] ^= 0x01;
        if (verify_message(key.get(), alg, tampered, signature)) fail("tampered message unexpectedly verified for " + alg);
    }
}

static double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    const auto n = values.size();
    if (n == 0) return 0.0;
    return n % 2 ? values[n / 2] : (values[n / 2 - 1] + values[n / 2]) / 2.0;
}

static void write_csv(const std::string& path, const std::vector<Sample>& samples) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("cannot open CSV output: " + path);
    out << "algorithm,operation,latency_ms,output_bytes\n";
    out << std::fixed << std::setprecision(6);
    for (const auto& s : samples)
        out << s.algorithm << ',' << s.operation << ',' << s.milliseconds << ',' << s.output_bytes << '\n';
}

static void print_summary(const std::vector<Sample>& samples) {
    std::cout << "\nAlgorithm, operation, median ms, mean ms, output bytes (last sample)\n";
    std::vector<std::pair<std::string, std::string>> keys;
    for (const auto& s : samples) {
        auto k = std::make_pair(s.algorithm, s.operation);
        if (std::find(keys.begin(), keys.end(), k) == keys.end()) keys.push_back(k);
    }
    for (const auto& k : keys) {
        std::vector<double> times;
        std::size_t bytes = 0;
        for (const auto& s : samples) if (s.algorithm == k.first && s.operation == k.second) {
            times.push_back(s.milliseconds); bytes = s.output_bytes;
        }
        const double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        std::cout << k.first << ", " << k.second << ", " << std::fixed << std::setprecision(4)
                  << median(times) << ", " << mean << ", " << bytes << '\n';
    }
}

int main(int argc, char** argv) {
    try {
        int iterations = 20;
        std::string csv = "results.csv";
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--iterations" && i + 1 < argc) iterations = std::stoi(argv[++i]);
            else if (arg == "--csv" && i + 1 < argc) csv = argv[++i];
            else if (arg == "--help") {
                std::cout << "Usage: qs-crypto-bench [--iterations N] [--csv results.csv]\n";
                return 0;
            } else throw std::runtime_error("unknown or incomplete argument: " + arg);
        }
        if (iterations < 1 || iterations > 10000) throw std::runtime_error("iterations must be between 1 and 10000");

        std::cout << "QuantumShield Crypto Benchmark Lab\nOpenSSL: " << OpenSSL_version(OPENSSL_VERSION)
                  << "\nIterations per operation: " << iterations << '\n';
        std::vector<Sample> samples;
        benchmark_kem(iterations, samples);
        benchmark_signature("ML-DSA-65", iterations, samples);
        benchmark_signature("RSA", iterations, samples);
        benchmark_signature("EC", iterations, samples);
        write_csv(csv, samples);
        print_summary(samples);
        std::cout << "\nCSV written to: " << csv << '\n'
                  << "Note: timings are environment-specific research measurements, not security proofs.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
}
