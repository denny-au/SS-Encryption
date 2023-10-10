#include "numtheory.h"
#include "randstate.h"

//Test Header Files:
#include <stdio.h>
#include <time.h>

extern gmp_randstate_t state;

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t t, a_copy, b_copy;
    mpz_inits(t, a_copy, b_copy, NULL);
    mpz_set(a_copy, a);
    mpz_set(b_copy, b);

    while (mpz_cmp_ui(b_copy, 0) != 0) { // Step 1
        mpz_set(t, b_copy); // Step 2
        mpz_mod(b_copy, a_copy, b_copy); // Step 3
        mpz_set(a_copy, t); // Step 4
    }
    mpz_set(g, a_copy); // Step 5
    mpz_clears(t, a_copy, b_copy, NULL);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r, r_prime, t, t_prime, q, temp;
    mpz_inits(r, r_prime, t, t_prime, q, temp, NULL);
    mpz_set(r, n); // Step 1
    mpz_set(r_prime, a);
    mpz_set_ui(t, 0); // Step 2
    mpz_set_ui(t_prime, 1);

    while (mpz_cmp_ui(r_prime, 0) != 0) { // Step 3
        mpz_fdiv_q(q, r, r_prime); // Step 4

        mpz_set(temp, r); // Step 5
        mpz_set(r, r_prime);
        mpz_mul(r_prime, q, r_prime);
        mpz_sub(r_prime, temp, r_prime);

        mpz_set(temp, t); // Step 6
        mpz_set(t, t_prime);
        mpz_mul(t_prime, q, t_prime);
        mpz_sub(t_prime, temp, t_prime);
    }

    if (mpz_cmp_ui(r, 1) > 0) { // Step 7
        mpz_set_ui(o, 0); // Step 8
        mpz_clears(r, r_prime, t, t_prime, q, temp, NULL);
        return;
    }

    if (mpz_cmp_ui(t, 0) < 0) { // Step 9
        mpz_add(t, t, n); // Step 10
    }

    mpz_set(o, t); // Step 11
    mpz_clears(r, r_prime, t, t_prime, q, temp, NULL);
}

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, d_copy;
    mpz_inits(v, p, d_copy, NULL);
    mpz_set_ui(v, 1); // Step 1
    mpz_set(p, a); // Step 2
    mpz_set(d_copy, d);

    while (mpz_cmp_ui(d_copy, 0) > 0) { // Step 3
        if (mpz_odd_p(d_copy)) { // Step 4
            mpz_mul(v, v, p); // Step 5
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p); // Step 6
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(d_copy, d_copy, 2); // Step 7
    }
    mpz_set(o, v); // Step 8
    mpz_clears(v, p, d_copy, NULL);
}

bool is_prime(const mpz_t n, uint64_t iters) {

    if (mpz_even_p(n) && mpz_cmp_ui(n, 2) != 0) {
        return false;
    }

    if (mpz_cmp_ui(n, 1) < 1) {
        return false;
    }

    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        return true;
    }

    mpz_t r, s, a, n_copy, y, s1, j, two, n1;
    mpz_inits(r, s, a, n_copy, y, s1, j, two, n1, NULL);
    mpz_set_ui(two, 2);

    mpz_sub_ui(r, n, 1); // Step 1
    mpz_set_ui(s, 0);

    while (mpz_odd_p(r)) { // Making sure that it is ODD
        mpz_fdiv_q_ui(r, r, 2);
        mpz_add_ui(s, s, 1);
    }

    // n - 1 and s - 1
    mpz_sub_ui(n1, n, 1);
    mpz_sub_ui(s1, s, 1);

    for (uint64_t i = 1; i <= iters; i++) { // Step 2
        mpz_sub_ui(n_copy, n, 3); // Step 3
        mpz_urandomm(a, state,
            n_copy); // Using randomm instead of randomb because I am not dealing with bits
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n); // Step 4

        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n1) != 0)) { // Step 5
            mpz_set_ui(j, 1); // Step 6

            while (((mpz_cmp(j, s1) < 0) || (mpz_cmp(j, s1) == 0))
                   && (mpz_cmp(y, n1) != 0)) { // Step 7
                pow_mod(y, y, two, n); // Step 8

                if (mpz_cmp_ui(y, 1) == 0) { // Step 9
                    mpz_clears(r, s, a, n_copy, y, s1, j, two, n1, NULL);
                    return false; // Step 10
                }

                mpz_add_ui(j, j, 1); // Step 11
            }

            if (mpz_cmp(y, n1) != 0) { // Step 12
                mpz_clears(r, s, a, n_copy, y, s1, j, two, n1, NULL);
                return false; // Step 13
            }
        }
    }

    mpz_clears(r, s, a, n_copy, y, s1, j, two, n1, NULL);
    return true; // Step 14
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    do {
        mpz_urandomb(p, state, bits); // Step 1
        mpz_setbit(p, bits);
    } while (!is_prime(p, iters)); // Step 2
}
