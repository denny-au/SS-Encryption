#include <stdio.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    uint64_t bits;
    mpz_t p_copy, q_copy, temp;
    mpz_inits(p_copy, q_copy, temp, NULL);

    do {
        // Implementation found from GeekForGeeks Random Range
        bits = (random() % (nbits / 5) + (nbits / 5));
        make_prime(p, bits, iters);
        bits = nbits - (bits * 2);
        make_prime(q, bits, iters);
        mpz_sub_ui(p_copy, p, 1);
        mpz_sub_ui(q_copy, q, 1);
        mpz_set(temp, p_copy);
        mpz_mod(p_copy, p, q_copy);
        mpz_mod(q_copy, q, temp);
    } while (mpz_cmp_ui(p_copy, 0) == 0 || mpz_cmp_ui(q_copy, 0) == 0);
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
    mpz_clears(p_copy, q_copy, temp, NULL);
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    //ss_make_pub's Step 2
    mpz_t p_copy, q_copy, temp1, temp2, lcm, n_priv;
    mpz_inits(p_copy, q_copy, temp1, temp2, lcm, n_priv, NULL);

    mpz_sub_ui(p_copy, p, 1);
    mpz_sub_ui(q_copy, q, 1);
    mpz_mul(temp1, p_copy, q_copy);
    gcd(temp2, p_copy, q_copy);
    mpz_fdiv_q(lcm, temp1, temp2); // LCM

    mpz_mul(n_priv, p, p);
    mpz_mul(n_priv, n_priv, q);

    mpz_mul(pq, p, q);
    mod_inverse(d, n_priv, lcm);

    mpz_clears(p_copy, q_copy, temp1, temp2, lcm, n_priv, NULL);
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%s\n", username);
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", pq);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%s\n", username);
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", pq);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    // Step 1
    mpz_t c, m;
    mpz_inits(c, m, NULL);

    size_t k = mpz_sizeinbase(n, 2);

    k = (k * 0.5 - 1) / 8;
    // Step 2
    uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
    // Step 3
    arr[0] = 0xFF;

    // Step 4
    while (!feof(infile)) {
        // Step 4a
        uint64_t j = fread(arr + 1, sizeof(uint8_t), k - 1, infile);
        // Step 4b
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, arr);
        // Step 4c
        ss_encrypt(c, m, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    free(arr);
    mpz_clears(c, m, NULL);
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    // Step 1
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    size_t k = mpz_sizeinbase(pq, 2); //bits into numbers
    k = (k - 1) / 8;
    uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
    // Step 2
    while (!feof(infile)) {
        // Step 2a
        gmp_fscanf(infile, "%Zx\n", c);
        // Step 2b
        ss_decrypt(m, c, d, pq);
        mpz_export(arr, &k, 1, sizeof(uint8_t), 1, 0, m);
        // Step 2c
        fwrite(arr + 1, k - 1, sizeof(uint8_t), outfile);
    }
    free(arr);
    mpz_clears(c, m, NULL);
}
