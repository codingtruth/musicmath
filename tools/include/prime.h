#ifndef __PRIME_H__
#define __PRIME_H__

typedef struct{
	int prime;
	int power;
} factor_t;

void prime_init(void);
void prime_known_print(void);
bool is_prime(int n);
void factorize(int n, factor_t * factors);
void factors_reset(factor_t * factors);
void factors_cleanup(factor_t * factors);
void factors_copy(factor_t * dst, factor_t * src);
int factors_mul(factor_t * factors);
int nod(int n1, int n2);
int nok(int n1, int n2);

#define	PRIME_FACTORS_MAX	80

#endif//__PRIME_H__
