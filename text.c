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

#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#ifdef NOLIBC
#undef errno
int errno;
#endif

#define INDEX(x) ((x) > 10 ? 9 + (x)%8 : (x)-1)
#define SLICE(x, i, n) ((x)>>(i) & (1<<(n))-1)

static int num[6], target;
extern const int answer[];
static int seq, ops;
static int tty;
static char buf[4096];
static int len;

static int
next(int *p) {
	static char a[sizeof buf];
	static int n, i;
	int err, x;
	int ret, c;
	err = 1;
	x = 0;
	for (; ; i++) {
		if (i >= n) {
			do
				ret = read(0, a, sizeof a);
			while (ret == -1 && errno == EINTR);
			if (ret == -1)
				return 1;
			if (ret == 0) {
				err = -err;
				break;
			}
			n = ret;
			i = 0;
		}
		c = a[i];
		if (err && (c == ' ' || c == '\n' || c == '\t'))
			continue;
		if (c < '0' || c > '9')
			break;
		if (x > 127)
			return 1;
		x = x*10 + c-'0';
		err = 0;
	}
	if (!err)
		*p = x;
	return err;
}

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
	int i;
	for (i = 0; i < 6; i++)
		switch (next(&num[i])) {
		case -1:
			if (i == 0)
				return -1;
		case 1:
			return 1;
		}
	if (next(&target) != 0)
		return 1;
	if (!valid())
		return 1;
	return 0;
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
buffer(const char *s) {
	for (; *s; s++)
		buf[len++] = *s;
}

static const char *
string(int x) {
	static char a[16];
	char *p;
	p = &a[(sizeof a)-1];
	do {
		*--p = '0' + x%10;
		x /= 10;
	}
	while (x);
	return p;
}

static void
flush(void) {
	int i, ret;
	for (i = 0; i < len; i += ret) {
		do
			ret = write(1, &buf[i], len-i);
		while (ret == -1 && errno == EINTR);
		if (ret == -1)
			break;
	}
	len = 0;
}

static void
p(void) {
	const char *sym[] = {
		" + ", " - ", " * ", " / ",
	};
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
			buffer(string(x));
			buffer(sym[ops & 3]);
			buffer(string(y));
			buffer(" = ");
			buffer(string(z));
			buffer("\n");
			ops >>= 2;
		}
		else {
			stk[i++] = num[j++];
		}
	buffer("\n");
	if (tty || len > (sizeof buf)-128)
		flush();
}

int
main(void) {
	const char s[] = "bad input\n";
	struct termios t;
	int err;
	tty = ioctl(1, TCGETS, &t) == 0;
	while (!(err = r())) {
		e();
		p();
	}
	if (err != -1) {
		write(2, s, (sizeof s)-1);
		return 1;
	}
	flush();
}
