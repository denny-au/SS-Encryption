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
           "   Encrypts data using SS encryption.\n"
           "   Encrypted data is decrypted by the decrypt program.\n\n"
           "USAGE\n"
           "   ./encrypt [OPTIONS]\n\n"
           "OPTIONS\n"
           "   -h              Display program help and usage.\n"
           "   -v              Display verbose program output.\n"
           "   -i infile       Input file of data to encrypt (default: stdin).\n"
           "   -o outfile      Output file for encrypted data (default: stdout).\n"
           "   -n pbfile       Public key file (default: ss.pub).\n");
}

int main(int argc, char **argv) {
    //Step 1
    int opt = 0;
    bool v = false;
    char *infile_name = NULL;
    FILE *infile;
    char *outfile_name = NULL;
    FILE *outfile;
    char *pbfile_name = "ss.pub";
    FILE *pbfile;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': infile_name = optarg; break;
        case 'o': outfile_name = optarg; break;
        case 'n': pbfile_name = optarg; break;
        case 'v': v = true; break;
        case 'h': helper_function(); return 0;
        default: helper_function(); return -1;
        }
    }

    //Step 2
    if (!(pbfile = fopen(pbfile_name, "r"))) {
        printf("NULL detected!\n Please add a name for pbfile.\n");
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
        outfile = stdout;
    } else {
        outfile = fopen(outfile_name, "w");
    }
    if (!outfile) {
        printf("Output file cannot open. Problem can be due to file or program.\n");
        return -1;
    }

    //Step 3
    mpz_t n;
    mpz_init(n);
    char username[LOGIN_NAME_MAX];
    ss_read_pub(n, username, pbfile);

    //Step 4.1 (Bits)
    size_t nbits = mpz_sizeinbase(n, 2);

    //Step 4.2 (Verbose)
    if (v) {
        gmp_printf("user = %s\n", username);
        gmp_printf("n  (%lu bits) = %Zd\n", nbits, n);
    }

    //Step 5
    ss_encrypt_file(infile, outfile, n);

    //Step 6
    mpz_clear(n);
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);
    return 0;
}
