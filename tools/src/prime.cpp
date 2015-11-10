#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prime.h"

#define	PRIME_KNOWN_NUMBER_MAX	130000
#if 1
#include "prime_known.cpp"
#else
static int prime_known[PRIME_KNOWN_NUMBER_MAX] = {2};
static int prime_known_number = 1;
#endif

void factorize(int n, factor_t * factors)
{
	int p, pmax = n / 2, m = n, pi = 0;
	int factors_number = 0;
	for (int i = 0; i < PRIME_FACTORS_MAX; i++)
	{
		factors[i].prime = 1;
		factors[i].power = 0;
	}
	while (1)
	{
		p = prime_known[pi];
		if (p > pmax)
			return;
		while(m % p == 0)
		{
			if (factors_number == PRIME_FACTORS_MAX-1)
			{
				printf("ERROR: PRIME_FACTORS_LIMIT_REACHED: factorize(%d)\n", n);
				exit(-1);
			}
			factors[factors_number].prime = p;
			factors[factors_number].power++;
			m = m / p;
		}
		if (factors[factors_number].power != 0)
		{
			factors_number++;
		}
		if (++pi == PRIME_KNOWN_NUMBER_MAX)
		{
			printf("ERROR: PRIME_LIMIT_REACHED: factorize(%d)\n", n);
			exit(-1);
		}
	}
}

bool is_prime(int n)
{
	for (int i = 0, k = prime_known[0], kmax = n / 2;
				 k <= kmax && i < prime_known_number;
				 k = prime_known[++i])
	{
		if (n == k)
			return true;
		if (n % k == 0)
			return false;		
	}
}

static prime_known_find(void)
{
	for (int n = prime_known[prime_known_number - 1] + 1;
			 prime_known_number < PRIME_KNOWN_NUMBER_MAX; n++)
	{
		if (is_prime(n))
		{
			prime_known[prime_known_number++] = n;
		}
	}
}

void prime_init(void)
{
	if (prime_known_number < PRIME_KNOWN_NUMBER_MAX)
		prime_known_find();
}

#define PRIME_KNOWN_PRINT_COLNUM	13

void prime_known_print(void)
{
	printf("static int prime_known_number = %d;\n", prime_known_number);
	printf("static int prime_known[%d] = {\n", prime_known_number);
	for (int i = 0; i < prime_known_number; i++)
	{
		printf("%7d,", prime_known[i]);
		if ((i + 1) % PRIME_KNOWN_PRINT_COLNUM == 0)
			printf("\n");
	}
	printf("};");
}

void factors_reset(factor_t * factors)
{
	for (int i = 0; i < PRIME_FACTORS_MAX; i++)
	{
		factors[i].prime = 1;
		factors[i].power = 0;
	}
}

void factors_cleanup(factor_t * factors)
{
	factor_t tmp[PRIME_FACTORS_MAX];
	factors_reset(tmp);
	for (int i = 0, k = 0; i < PRIME_FACTORS_MAX; i++)
	{
		if (factors[i].power != 0)
			tmp[k++] = factors[i];
	}
	factors_copy(factors, tmp);
}

void factors_copy(factor_t * dst, factor_t * src)
{
	memcpy(dst, src, sizeof(factor_t) * PRIME_FACTORS_MAX);
}

int factors_mul(factor_t * factors)
{
	int mul = 1;
	for (int i = 0; factors[i].power != 0; i++)
		for (int k = 0; k < factors[i].power; k++)
			mul *= factors[i].prime;
	return mul;
}

int nod(int n1, int n2)
{
	int a, b, c;
	if (n1 == n2)
		return n1;
	if (n1 < n2) {
		a = n2; b = n1;
	} else {
		a = n1; b = n2;
	}
	if (b ==0)
		return a;
	while (1) {
		c = a % b;
		if (c == 0)
			return b;
		a = b;
		b = c;
	}
}

int nok(int n1, int n2)
{
	return n1 * n2 / nod(n1, n2);
}
