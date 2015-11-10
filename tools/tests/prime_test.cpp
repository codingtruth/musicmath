#include <stdio.h>
#include <stdlib.h>
#include "prime.h"
#include "frac.h"

int main(void)
{
	prime_init();

	frac_t f, f1 = {8,9}, f2 = {3,5};
	f = frac_add(f1,f2);
	printf("%d/%d + %d/%d = %d/%d\n", F(f1), F(f2), F(f));
	f = frac_mul(f1,f2);
	printf("%d/%d * %d/%d = %d/%d\n", F(f1), F(f2), F(f));

	frac_t frac1 = {10080*13*17, 5040*17}, frac2 = {5040, 10080*17};
	frac_reduce(&frac1);
	frac_reduce(&frac2);
	printf("10080*13*17/5040*17 = %d/%d 5040*19/10080*17 = %d/%d\n", frac1.n, frac1.d, frac2.n, frac2.d);
	
	factor_t factors[PRIME_FACTORS_MAX];
	int check_multiple = 1;
	factorize(10080, factors);
	printf("10080 == ");
	for (int i = 0; factors[i].power != 0; i++)
	{
		if (i != 0)
			printf(" * ");
		if (factors[i].power != 1)
			printf("%d^%d", factors[i].prime, factors[i].power);
		else
			printf("%d", factors[i].prime);
		for (int k = 0; k < factors[i].power; k++)
			check_multiple *= factors[i].prime;
	}
	printf(" == %d", check_multiple);
	if (check_multiple == 10080)
		printf("[OK]\n");
	else
		printf("[ERROR]\n");
#if 0
	prime_known_print();
#endif
	return 0;
}