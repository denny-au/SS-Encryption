#include <stdio.h>
#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define OPTIONS "b:i:n:d:s:vh"

void helper_function(void) {
    printf("SYNOPSIS\n"
           "   Generates an SS public/private key pair.\n\n"
           "USAGE\n"
           "   ./keygen [OPTIONS]\n\n"
           "OPTIONS\n"
           "   -h              Display program help and usage.\n"
           "   -v              Display verbose program output.\n"
           "   -b bits         Minimum bits needed for public key n (default: 256).\n"
           "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
           "   -n pbfile       Public key file (default: ss.pub).\n"
           "   -d pvfile       Private key file (default: ss.priv).\n"
           "   -s seed         Random seed for testing.\n");
}

int main(int argc, char **argv) {
    // Step 1
    int opt = 0;
    uint64_t b = 256;
    uint64_t i = 50;
    char *pbfile_name = "ss.pub";
    FILE *pbfile;
    char *pvfile_name = "ss.priv";
    FILE *pvfile;
    uint64_t s = time(NULL);
    bool v = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': b = strtoul(optarg, NULL, 10); break;
        case 'i': i = strtoul(optarg, NULL, 10); break;
        case 'n': pbfile_name = optarg; break;
        case 'd': pvfile_name = optarg; break;
        case 's': s = strtoul(optarg, NULL, 10); break;
        case 'v': v = true; break;
        case 'h': helper_function(); return 0;
        default: helper_function(); return -1;
        }
    }

    // Step 2
    if (!(pbfile = fopen(pbfile_name, "w"))) {
        printf("NULL detected!\n Please add a name for pbfile.\n");
        return -1;
    }
    if (!(pvfile = fopen(pvfile_name, "w"))) {
        printf("NULL detected!\n Please add a name for pvfile.\n");
        return -1;
    }

    // Step 3
    fchmod(fileno(pvfile), 0600);

    // Step 4
    randstate_init(s);

    // Step 5
    mpz_t p, q, n, d, pq;
    mpz_inits(p, q, n, d, pq, NULL);
    ss_make_pub(p, q, n, b, i);
    ss_make_priv(d, pq, p, q);

    // Step 6
    char *username = getenv("USERNAME");

    // Step 7
    ss_write_pub(n, username, pbfile);
    ss_write_priv(pq, d, pvfile);

    //Step 8.1 (Representing Bits)
    size_t pbits, qbits, nbits, pqbits, dbits;
    pbits = mpz_sizeinbase(p, 2);
    qbits = mpz_sizeinbase(q, 2);
    nbits = mpz_sizeinbase(n, 2);
    pqbits = mpz_sizeinbase(pq, 2);
    dbits = mpz_sizeinbase(d, 2);

    //Step 8.2 (Printing)
    if (v) {
        gmp_printf("user = %s\n", username);
        gmp_printf("p  (%lu bits) = %Zd\n", pbits, p);
        gmp_printf("q  (%lu bits) = %Zd\n", qbits, q);
        gmp_printf("n  (%lu bits) = %Zd\n", nbits, n);
        gmp_printf("pq (%lu bits) = %Zd\n", pqbits, pq);
        gmp_printf("d  (%lu bits) = %Zd\n", dbits, d);
    }

    randstate_clear();
    mpz_clears(p, q, n, d, pq, NULL);
    fclose(pbfile);
    fclose(pvfile);
    return 0;
}
