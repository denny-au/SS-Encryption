# Assignment 5 - Public Key Cryptography

The program will produce public and private keys from the program keygen. It can also encrypts data into an unreadable jargon using the public key. With the private key, it can decrypt the message into its original form.

## Building
```
$ make clean
$ make
```
Cleans the folder and compiles keygen, encrypt, and decrypt.

## Example File
```
$ vim a.txt
Hello World!

$ vim b.txt
(wq)

$ vim c.txt
(wq)
```
Creates three text files. Text file "a.txt" will contain the message while the files "b.txt" and "c.txt" will showcase the encrypted and decrypted text respectively.

## Running
```
$ ./keygen
$ ./encrypt -i a.txt -o b.txt
$ ./decrypt -i b.txt -o c.txt
$ vim c.txt
```

## Results
1. Makes the public and private key.
2. Encrypts a.txt into b.txt.
3. Decrypts b.txt into c.txt.
4. The file should display "Hello World!" as its decrypted message.

## Keygen Options
```
-h              Display program help and usage.
-v              Display verbose program output.
-b bits         Minimum bits needed for public key n (default: 256).
-i iterations   Miller-Rabin iterations for testing primes (default: 50).
-n pbfile       Public key file (default: ss.pub).
-d pvfile       Private key file (default: ss.priv).
-s seed         Random seed for testing.
```

## Encrypt Options
```
-h              Display program help and usage.
-v              Display verbose program output.
-i infile       Input file of data to encrypt (default: stdin).
-o outfile      Output file for encrypted data (default: stdout).
-n pbfile       Public key file (default: ss.pub).
```

## Decrypt Options
```
-h              Display program help and usage.
-v              Display verbose program output.
-i infile       Input file of data to decrypt (default: stdin).
-o outfile      Output file for decrypted data (default: stdout).
-n pvfile       Private key file (default: ss.priv).
```
