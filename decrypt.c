#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

void helper_function(void) {
    printf("SYNOPSIS\n"
           "   Decrypts data using SS decryption.\n"
           "   Decrypted data is encrypted by the encrypt program.\n\n"
           "USAGE\n"
           "   ./decrypt [OPTIONS]\n\n"
           "OPTIONS\n"
           "   -h              Display program help and usage.\n"
           "   -v              Display verbose program output.\n"
           "   -i infile       Input file of data to decrypt (default: stdin).\n"
           "   -o outfile      Output file for decrypted data (default: stdout).\n"
           "   -n pvfile       Private key file (default: ss.priv).\n");
}

int main(int argc, char **argv) {
    //Step 1
    int opt = 0;
    bool v = false;
    char *infile_name = NULL;
    FILE *infile;
    char *outfile_name = NULL;
    FILE *outfile;
    char *pvfile_name = "ss.priv";
    FILE *pvfile;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': infile_name = optarg; break;
        case 'o': outfile_name = optarg; break;
        case 'n': pvfile_name = optarg; break;
        case 'v': v = true; break;
        case 'h': helper_function(); return 0;
        default: helper_function(); return -1;
        }
    }

    //Step 2
    if (!(pvfile = fopen(pvfile_name, "r"))) {
        printf("NULL detected!\n Please add a name for pvfile.\n");
        return -1;
    }

    if (infile_name == NULL) {
        infile = stdin;
    } else {
        infile = fopen(infile_name, "r");
    }

    if (!infile) {
        printf("Input file cannot open. Problem can be due to file or program.\n");
        return -1;
    }

    if (outfile_name == NULL) {
        outfile = stdin;
    } else {
        outfile = fopen(outfile_name, "w");
    }

    if (!outfile) {
        printf("Output file cannot open. Problem can be due to file or program.\n");
        return -1;
    }

    //Step 3
    mpz_t pq, d;
    mpz_inits(pq, d, NULL);
    ss_read_priv(pq, d, pvfile);

    //Step 4.1 (Bits)
    size_t pqbits = mpz_sizeinbase(pq, 2);
    size_t dbits = mpz_sizeinbase(d, 2);

    //Step 4.2 (Verbose)
    if (v) {
        gmp_printf("pq  (%lu bits) = %Zd\n", pqbits, pq);
        gmp_printf("d  (%lu bits) = %Zd\n", dbits, d);
    }

    //Step 5
    ss_decrypt_file(infile, outfile, d, pq);

    //Step 6
    mpz_clears(pq, d, NULL);
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);

    return 0;
}
