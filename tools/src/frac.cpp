#include "prime.h"
#include "frac.h"

void frac_reduce(frac_t * frac)
{
#if 1
	int a = nod(frac->n, frac->d);
	if (a == 0)
		return;
	frac->n /= a;
	frac->d /= a;
#else
	factor_t fact_n[PRIME_FACTORS_MAX];
	factor_t fact_d[PRIME_FACTORS_MAX];
	factorize(frac->n, fact_n);
	factorize(frac->d, fact_d);
	for (int i = 0, k = 0; fact_n[i].power != 0; i++)
	{
		for (; fact_d[k].power != 0; k++)
			if (fact_d[k].prime >= fact_n[i].prime)
				break;
		if (fact_d[k].power == 0)
			break;
		if (fact_n[i].prime == fact_d[k].prime)
		{
			if (fact_n[i].power >= fact_d[k].power)
			{
				fact_n[i].power -= fact_d[k].power;
				fact_d[k].power = 0;
			}
			else
			{
				fact_d[k].power -= fact_n[i].power;
				fact_n[i].power = 0;
			}
			k++;
		}
	}
	factors_cleanup(fact_n);
	factors_cleanup(fact_d);
	frac->n = factors_mul(fact_n);
	frac->d = factors_mul(fact_d);
#endif
}

frac_t frac_mul(frac_t f1, frac_t f2)
{
	frac_t f;
	f.n = f1.n * f2.n;
	f.d = f1.d * f2.d;
	frac_reduce(&f);
	return f;
}

frac_t frac_div(frac_t f1, frac_t f2)
{
	frac_t f3, f;
	f3.n = f2.d;
	f3.d = f2.n;
	f = frac_mul(f1, f3);
	return f;
}

frac_t frac_add(frac_t f1, frac_t f2)
{
	frac_t f;
	f.d = nok(f1.d, f2.d);
	f.n = f1.n * (f.d / f1.d) + f2.n * (f.d / f2.d);
	frac_reduce(&f);
	return f;
}