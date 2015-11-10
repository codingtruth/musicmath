#ifndef __FRAC_H__
#define __FRAC_H__

#define F(f)	f.n, f.d

typedef struct
{
	int n;
	int d;
} frac_t;

void frac_reduce(frac_t * frac);
frac_t frac_mul(frac_t f1, frac_t f2);
frac_t frac_div(frac_t f1, frac_t f2);
frac_t frac_add(frac_t f1, frac_t f2);

#endif//__FRAC_H__