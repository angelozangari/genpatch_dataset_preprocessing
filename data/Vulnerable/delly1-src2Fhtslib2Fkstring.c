/* The MIT License

   Copyright (C) 2011 by Attractive Chaos <attractor@live.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <config.h>

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "htslib/kstring.h"

int kputd(double d, kstring_t *s) {
	int len = 0;
	char buf[21], *cp = buf+20, *ep;
	if (d == 0) {
		if (signbit(d)) {
			kputsn("-0",2,s);
			return 2;
		} else {
			kputsn("0",1,s);
			return 1;
		}
	}

	if (d < 0) {
		kputc('-',s);
		len = 1;
		d=-d;
	}
	if (!(d >= 0.0001 && d <= 999999)) {
		if (ks_resize(s, s->l + 50) < 0)
			return EOF;
		// We let stdio handle the exponent cases
		int s2 = sprintf(s->s + s->l, "%g", d);
		len += s2;
		s->l += s2;
		return len;
	}

	uint64_t i = d*10000000000;
	// Correction for rounding - rather ugly

	// Optimised for small numbers.
	// Better still would be __builtin_clz on hi/lo 32 and get the
	// starting point very rapidly.
	if (d<.0001)
		i+=0;
	else if (d<0.001)
		i+=5;
	else if (d < 0.01)
		i+=50;
	else if (d < 0.1)
		i+=500;
	else if (d < 1)
		i+=5000;
	else if (d < 10)
		i+=50000;
	else if (d < 100)
		i+=500000;
	else if (d < 1000)
		i+=5000000;
	else if (d < 10000)
		i+=50000000;
	else if (d < 100000)
		i+=500000000;
	else
		i+=5000000000;

	do {
		*--cp = '0' + i%10;
		i /= 10;
	} while (i >= 1);
	buf[20] = 0;
	int p = buf+20-cp;
	if (p <= 10) { // d < 1
		//assert(d/1);
		cp[6] = 0; ep = cp+5;// 6 precision
		while (p < 10) {
			*--cp = '0';
			p++;
		}
		*--cp = '.';
		*--cp = '0';
	} else {
		char *xp = --cp;
		while (p > 10) {
			xp[0] = xp[1];
			p--;
			xp++;
		}
		xp[0] = '.';
		cp[7] = 0; ep=cp+6;
		if (cp[6] == '.') cp[6] = 0;
	}

	// Cull trailing zeros
	while (*ep == '0' && ep > cp)
		ep--;
	char *z = ep+1;
	while (ep > cp) {
		if (*ep == '.') {
			if (z[-1] == '.')
				z[-1] = 0;
			else
				z[0] = 0;
			break;
		}
		ep--;
	}

	int sl = strlen(cp);
	len += sl;
	kputsn(cp, sl, s);
	return len;
}

int kvsprintf(kstring_t *s, const char *fmt, va_list ap)
{
	va_list args;
	int l;
	va_copy(args, ap);

	if (fmt[0] == '%' && fmt[1] == 'g' && fmt[2] == 0) {
		double d = va_arg(args, double);
		l = kputd(d, s);
		va_end(args);
		return l;
	}

	l = vsnprintf(s->s + s->l, s->m - s->l, fmt, args); // This line does not work with glibc 2.0. See `man snprintf'.
	va_end(args);
	if (l + 1 > s->m - s->l) {
		if (ks_resize(s, s->l + l + 2) < 0)
			return -1;
		va_copy(args, ap);
		l = vsnprintf(s->s + s->l, s->m - s->l, fmt, args);
		va_end(args);
	}
	s->l += l;
	return l;
}

int ksprintf(kstring_t *s, const char *fmt, ...)
{
	va_list ap;
	int l;
	va_start(ap, fmt);
	l = kvsprintf(s, fmt, ap);
	va_end(ap);
	return l;
}

char *kstrtok(const char *str, const char *sep, ks_tokaux_t *aux)
{
	const char *p, *start;
	if (sep) { // set up the table
		if (str == 0 && (aux->tab[0]&1)) return 0; // no need to set up if we have finished
		aux->finished = 0;
		if (sep[1]) {
			aux->sep = -1;
			aux->tab[0] = aux->tab[1] = aux->tab[2] = aux->tab[3] = 0;
			for (p = sep; *p; ++p) aux->tab[*p>>6] |= 1ull<<(*p&0x3f);
		} else aux->sep = sep[0];
	}
	if (aux->finished) return 0;
	else if (str) aux->p = str - 1, aux->finished = 0;
	if (aux->sep < 0) {
		for (p = start = aux->p + 1; *p; ++p)
			if (aux->tab[*p>>6]>>(*p&0x3f)&1) break;
	} else {
		for (p = start = aux->p + 1; *p; ++p)
			if (*p == aux->sep) break;
	}
	aux->p = p; // end of token
	if (*p == 0) aux->finished = 1; // no more tokens
	return (char*)start;
}

// s MUST BE a null terminated string; l = strlen(s)
int ksplit_core(char *s, int delimiter, int *_max, int **_offsets)
{
	int i, n, max, last_char, last_start, *offsets, l;
	n = 0; max = *_max; offsets = *_offsets;
	l = strlen(s);

#define __ksplit_aux do {						\
		if (_offsets) {						\
			s[i] = 0;					\
			if (n == max) {					\
				int *tmp;				\
				max = max? max<<1 : 2;			\
				if ((tmp = (int*)realloc(offsets, sizeof(int) * max))) {  \
					offsets = tmp;			\
				} else	{				\
					free(offsets);			\
					*_offsets = NULL;		\
					return 0;			\
				}					\
			}						\
			offsets[n++] = last_start;			\
		} else ++n;						\
	} while (0)

	for (i = 0, last_char = last_start = 0; i <= l; ++i) {
		if (delimiter == 0) {
			if (isspace(s[i]) || s[i] == 0) {
				if (isgraph(last_char)) __ksplit_aux; // the end of a field
			} else {
				if (isspace(last_char) || last_char == 0) last_start = i;
			}
		} else {
			if (s[i] == delimiter || s[i] == 0) {
				if (last_char != 0 && last_char != delimiter) __ksplit_aux; // the end of a field
			} else {
				if (last_char == delimiter || last_char == 0) last_start = i;
			}
		}
		last_char = s[i];
	}
	*_max = max; *_offsets = offsets;
	return n;
}

int kgetline(kstring_t *s, kgets_func *fgets_fn, void *fp)
{
	size_t l0 = s->l;

	while (s->l == l0 || s->s[s->l-1] != '\n') {
		if (s->m - s->l < 200) {
			if (ks_resize(s, s->m + 200) < 0)
				return EOF;
		}
		if (fgets_fn(s->s + s->l, s->m - s->l, fp) == NULL) break;
		s->l += strlen(s->s + s->l);
	}

	if (s->l == l0) return EOF;

	if (s->l > l0 && s->s[s->l-1] == '\n') {
		s->l--;
		if (s->l > l0 && s->s[s->l-1] == '\r') s->l--;
	}
	s->s[s->l] = '\0';
	return 0;
}

/**********************
 * Boyer-Moore search *
 **********************/

typedef unsigned char ubyte_t;

// reference: http://www-igm.univ-mlv.fr/~lecroq/string/node14.html
static int *ksBM_prep(const ubyte_t *pat, int m)
{
	int i, *suff, *prep, *bmGs, *bmBc;
	prep = (int*)calloc(m + 256, sizeof(int));
	bmGs = prep; bmBc = prep + m;
	{ // preBmBc()
		for (i = 0; i < 256; ++i) bmBc[i] = m;
		for (i = 0; i < m - 1; ++i) bmBc[pat[i]] = m - i - 1;
	}
	suff = (int*)calloc(m, sizeof(int));
	{ // suffixes()
		int f = 0, g;
		suff[m - 1] = m;
		g = m - 1;
		for (i = m - 2; i >= 0; --i) {
			if (i > g && suff[i + m - 1 - f] < i - g)
				suff[i] = suff[i + m - 1 - f];
			else {
				if (i < g) g = i;
				f = i;
				while (g >= 0 && pat[g] == pat[g + m - 1 - f]) --g;
				suff[i] = f - g;
			}
		}
	}
	{ // preBmGs()
		int j = 0;
		for (i = 0; i < m; ++i) bmGs[i] = m;
		for (i = m - 1; i >= 0; --i)
			if (suff[i] == i + 1)
				for (; j < m - 1 - i; ++j)
					if (bmGs[j] == m)
						bmGs[j] = m - 1 - i;
		for (i = 0; i <= m - 2; ++i)
			bmGs[m - 1 - suff[i]] = m - 1 - i;
	}
	free(suff);
	return prep;
}

void *kmemmem(const void *_str, int n, const void *_pat, int m, int **_prep)
{
	int i, j, *prep = 0, *bmGs, *bmBc;
	const ubyte_t *str, *pat;
	str = (const ubyte_t*)_str; pat = (const ubyte_t*)_pat;
	prep = (_prep == 0 || *_prep == 0)? ksBM_prep(pat, m) : *_prep;
	if (_prep && *_prep == 0) *_prep = prep;
	bmGs = prep; bmBc = prep + m;
	j = 0;
	while (j <= n - m) {
		for (i = m - 1; i >= 0 && pat[i] == str[i+j]; --i);
		if (i >= 0) {
			int max = bmBc[str[i+j]] - m + 1 + i;
			if (max < bmGs[i]) max = bmGs[i];
			j += max;
		} else return (void*)(str + j);
	}
	if (_prep == 0) free(prep);
	return 0;
}

char *kstrstr(const char *str, const char *pat, int **_prep)
{
	return (char*)kmemmem(str, strlen(str), pat, strlen(pat), _prep);
}

char *kstrnstr(const char *str, const char *pat, int n, int **_prep)
{
	return (char*)kmemmem(str, n, pat, strlen(pat), _prep);
}

/***********************
 * The main() function *
 ***********************/

#ifdef KSTRING_MAIN
#include <stdio.h>
int main()
{
	kstring_t *s;
	int *fields, n, i;
	ks_tokaux_t aux;
	char *p;
	s = (kstring_t*)calloc(1, sizeof(kstring_t));
	// test ksprintf()
	ksprintf(s, " abcdefg:    %d ", 100);
	printf("'%s'\n", s->s);
	// test ksplit()
	fields = ksplit(s, 0, &n);
	for (i = 0; i < n; ++i)
		printf("field[%d] = '%s'\n", i, s->s + fields[i]);
	// test kstrtok()
	s->l = 0;
	for (p = kstrtok("ab:cde:fg/hij::k", ":/", &aux); p; p = kstrtok(0, 0, &aux)) {
		kputsn(p, aux.p - p, s);
		kputc('\n', s);
	}
	printf("%s", s->s);
	// free
	free(s->s); free(s); free(fields);

	{
		static char *str = "abcdefgcdgcagtcakcdcd";
		static char *pat = "cd";
		char *ret, *s = str;
		int *prep = 0;
		while ((ret = kstrstr(s, pat, &prep)) != 0) {
			printf("match: %s\n", ret);
			s = ret + prep[0];
		}
		free(prep);
	}
	return 0;
}
#endif

//						↓↓↓VULNERABLE LINES↓↓↓

// 58,11;58,18

