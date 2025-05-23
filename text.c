/* Copyright 2025 Oğuz İsmail Uysal <oguzismailuysal@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#define INDEX(x) ((x) > 10 ? 9 + (x)%8 : (x)-1)
#define SLICE(x, i, n) ((x)>>(i) & (1<<(n))-1)

static int num[6], target;
extern const int answer[];
static int seq, ops;

static int
valid(void) {
	short max[] = {
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1,
	};
	int i;
	for (i = 0; i < 6; i++)
		switch (num[i]) {
		case 1: case 2: case 3: case 4: case 5:
		case 6: case 7: case 8: case 9: case 10:
		case 25: case 50: case 75: case 100:
			if (max[INDEX(num[i])]--)
				break;
		default:
			return 0;
		}
	return target > 100 && target < 1000;
}

static int
r(void) {
	switch (scanf(" %d %d %d %d %d %d %d",
		&num[0], &num[1], &num[2], &num[3],
		&num[4], &num[5], &target)) {
	case 7:
		if (valid())
			return 1;
	default:
		fputs("bad input\n", stderr);
		exit(1);
	case EOF:
		return 0;
	}
}

static int
binom(int n, int k) {
	int x, i;
	if (k > n)
		return 0;
	x = 1;
	for (i = 0; i < k; i++) {
		x *= n-i;
		x /= i+1;
	}
	return x;
}

static int
rank(const int *c, int k) {
	int x, i;
	x = 0;
	for (i = 0; i < k; i++)
		x += binom(c[i], i+1);
	return x;
}

static int
section(const int *c) {
	static const int off[] = {
		0, 3003, 10153, 13123,
	};
	int dupe[3], m;
	int uniq[6], n;
	int i;
	m = n = 0;
	for (i = 0; i < 6; i++)
		if (i+1 < 6 && c[i] == c[i+1])
			dupe[m++] = c[i++];
		else
			uniq[n++] = c[i]-m;
	return off[m]
		+ rank(dupe, m)*binom(14-m, n)
		+ rank(uniq, n);
}

static int
entry(void) {
	int c[6], i;
	for (i = 0; i < 6; i++)
		c[i] = INDEX(num[i]);
	return section(c)*899 + target-101;
}

static void
lehmer(int dec) {
	int fac[6];
	int i, j, x;
	for (i = 1; i <= 6; i++) {
		fac[6-i] = dec%i;
		dec /= i;
	}
	for (i = 0; i < 6; i++) {
		j = i+fac[i];
		x = num[j];
		for (; j > i; j--)
			num[j] = num[j-1];
		num[i] = x;
	}
}

static void
e(void) {
	int i, j, x;
	int bits;
	for (i = 1; i < 6; i++) {
		x = num[i];
		for (j = i; j && x < num[j-1]; j--)
			num[j] = num[j-1];
		num[j] = x;
	}
	bits = answer[entry()];
	seq = SLICE(bits, 0, 9)<<2;
	lehmer(SLICE(bits, 9, 10));
	ops = SLICE(bits, 19, 10);
}

static void
p(void) {
	int stk[6], i, j;
	int x, y, z;
	i = j = 0;
	for (; seq; seq >>= 1)
		if (seq & 1) {
			y = stk[--i];
			x = stk[--i];
			switch (ops & 3) {
			case 0: z = x + y; break;
			case 1: z = x - y; break;
			case 2: z = x * y; break;
			case 3: z = x / y; break;
			}
			stk[i++] = z;
			printf("%d %c %d = %d\n",
				x, "+-*/"[ops & 3], y, z);
			ops >>= 2;
		}
		else {
			stk[i++] = num[j++];
		}
	puts("");
}

int
main(void) {
	while (r()) {
		e();
		p();
	}
}
