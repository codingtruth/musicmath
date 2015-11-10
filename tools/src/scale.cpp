#include <stdlib.h>
#include <stdio.h>
#include "prime.h"
#include "frac.h"

static const frac_t scale_base[7] = {
	{9,10}, {25,27}, {9,10}, {8,9}, {15,16}, {8,9}, {9,10}
};

void scale_base_test_rotate(void)
{
	frac_t sum, f, s[7];
	int k, kr, j;
	for (int i = 0; i < 7; i++)
	{
		for (k = 0, kr = 1, f.n = 1, f.d = 1, sum.n = 0, sum.d = 1; k < 6; k++)
		{
			j = (i + k) % 7;
			f = frac_mul(f, scale_base[j]);
			//s[k] = f;
			sum = frac_add(sum, f);
			kr = nok(kr, f.d);
			printf("  %d/%d", F(f));
		}
		printf("   :  %d/%d   %d  \t%s %s\n", F(sum), kr, 
				is_prime(sum.n) ? " PRIME " : " NOT_PRIME",
				is_prime(sum.d * 6 - sum.n) ? " PRIME " : " NOT_PRIME");
	}
}

int main(void)
{
	scale_base_test_rotate();
	return 0;
}