#include "big_integer.h"
#include <cstdio>

big_integer::big_integer() {
    n = 1;
    a_size = 1;
    a = new int[a_size];
    a[0] = 0;
    sign = 0;
}

big_integer::big_integer(const big_integer& other) {
    *this = other;
}

big_integer::big_integer(int x) {
    sign = 1;
    if (x < 0) {
        sign = -1;
        x = -x;
    } else if (x == 0)
        sign = 0;
    n = 2;
    a_size = 2;
    a = new int[a_size];
    a[0] = x % BASE;
    a[1] = (x / BASE) % BASE;
    if (a[1] == 0) {
        n--;
    }
}

big_integer::big_integer(std::string const& s) {
    sign = 0;
    n = 0;
    a_size = s.length();
    a = new int[a_size];
    int neg = (s[0] == '-' ? 1 : 0);
    for (int i = 0; i < a_size; ++i)
        a[i] = 0;
    for (int i = neg; i < (int)s.length(); i++) {
        *this *= 10;
        *this += (s[i] - '0');
    }
    delete_nils();
    if (is_zero())
        sign = 0;
    else if (s[0] == '-')
        sign = -1;
    else
        sign = 1;
}

big_integer& big_integer::operator=(const big_integer& other) {
    n = other.n;
    a_size = other.a_size;
    sign = other.sign;
    a = new int[a_size];
    for (int i = 0; i < a_size; ++i)
        a[i] = other.a[i];
    return *this;
}

void big_integer::ensure_capacity() {
    if (n >= a_size || n * 4 <= a_size) {
        a_size = n * 2;
        int * b = new int[a_size];
        for (int i = 0; i < a_size; ++i)
            b[i] = 0;
        for (int i = 0; i < n; ++i)
            b[i] = a[i];
        delete[] a;
        a = b;
    }
}

void big_integer::delete_nils() {
    while (n > 1 && a[n - 1] == 0)
        n--;
}

bool big_integer::is_zero() const {
    return (n == 1 && a[0] == 0);
}

big_integer big_integer::abs() const {
    big_integer res = *this;
    if (res.sign < 0)
        res.sign = 1;
    return res;
}

/**
 (*this) < b : res = -1
 (*this) > b : res = 1
 (*this) == b : res = 0
 */
int big_integer::cmp(const big_integer& b) const {
    if (sign != b.sign)
        return (sign < b.sign ? -1 : 1);
    if (n != b.n)
        return (n * sign < b.n * b.sign ? -1 : 1);
    for (int i = n - 1; i >= 0; --i)
        if (a[i] != b.a[i])
            return (sign * a[i] < b.sign * b.a[i] ? -1 : 1);
    return 0;
}

int big_integer::get_length() const {
    return n * BSZE;
}

big_integer big_integer::operator-() const {
    big_integer res = big_integer(*this);
    res.sign *= (-1);
    return res;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer& big_integer::operator++() {
    big_integer b = 1;
    *this += b;
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer& big_integer::operator--() {
    big_integer b = 1;
    *this -= b;
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    ++(*this);
    if (is_zero())
        *this = 0;
    return res;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    --(*this);
    return res;
}


big_integer& big_integer::operator+=(const big_integer& b) {
    if (b.is_zero()) {
        return *this;
    }
    if (is_zero()) {
        *this = b;
        return *this;
    }
    if (sign != b.sign) {
        *this = *this - (-b);
        return *this;
    }
    int carry = 0;
    for (int i = 0; i < std::max(n, b.n) || carry; ++i) {
        int f = (i < n ? a[i] : 0);
        int s = (i < b.n ? b.a[i] : 0);
        int cur = f + s + carry;
        if (i >= n)
            n++;
        ensure_capacity();
        a[i] = cur % BASE;
        carry = cur / BASE;
    }
    delete_nils();
    ensure_capacity();
    return *this;
}

big_integer& big_integer::operator-=(const big_integer& b) {
    if (b.is_zero()) {
        return *this;
    }
    if (is_zero()) {
        *this = -b;
        return *this;
    }
    if (sign != b.sign) {
        *this = *this + (-b);
        return *this;
    }
    if (abs() < b.abs()) {
        *this = -(b - *this);
        return *this;
    }
    int carry = 0;
    for (int i = 0; i < b.n || carry; ++i) {
        a[i] -= carry + (i < b.n ? b.a[i] : 0);
        carry = a[i] < 0;
        if (carry)
            a[i] += BASE;
    }
    delete_nils();
    if (is_zero())
        *this = 0;
    ensure_capacity();
    return *this;
}

big_integer& big_integer::operator*=(const big_integer& b) {
    big_integer res = 0;
    res.sign = sign * b.sign;
    if (res.sign == 0) {
        *this = res;
        return *this;
    }
    res.n = n + b.n + 1;
    res.ensure_capacity();
    for (int i = 0; i < res.n; ++i)
        res.a[i] = 0;
    for (int i = 0; i < n; ++i) {
        int carry = 0;
        for (int j = 0; j < b.n || carry; ++j) {
            long long cur = res.a[i + j] + 1LL * a[i] * (j < b.n ? b.a[j] : 0) + carry;
            carry = (int)(cur / BASE);
            res.a[i + j] = (int)(cur % BASE);
        }
    }
    *this = res;
    delete_nils();
    ensure_capacity();
    return *this;
}

// b =/= 0 !!
//big_integer& big_integer::operator/=(const big_integer& b) {
//    if (abs() < b.abs()) {
//        *this = 0;
//        return *this;
//    }
//}

big_integer& big_integer::operator/=(int b) {
    int mod;
    divmod(b, *this, mod);
    return *this;
}

void big_integer::divmod(int b, big_integer& div, int& mod) const {
    big_integer other = *this;
    if (b < 0) {
        other.sign *= (-1);
        b *= (-1);
    }

    int carry = 0;
    for (int i = other.n - 1; i >= 0; --i) {
        long long cur = other.a[i] + carry * 1LL * BASE;
        other.a[i] = int(cur / b);
        carry = int(cur % b);
    }
    other.delete_nils();
    if (other.is_zero())
        other = 0;
    mod = carry;
    div = other;
}

big_integer& big_integer::operator&=(const big_integer& b) {
    /*
     * a & 0 = 0
     * 0 & b = 0
     * a > 0 && b > 0
     * (-a) & (-b) = -(((a - 1) | (b - 1)) + 1)
     * (a)  & (-b) = a & ~(b - 1)
     * (-a) & (b)  = (b) & (-a)
    */
    if (is_zero() || b.is_zero()) {
        *this = 0;
        return *this;
    }
    if (*this < 0 && b < 0) {
        *this = -(((abs() - 1) | (b.abs() - 1)) + 1);
        return *this;
    }
    if (*this > 0 && b < 0) {
        *this = *this & (~(b - 1)).abs();
        return *this;
    }
    if (*this < 0 && b > 0) {
        *this = b & *this;
        return *this;
    }
    n = std::max(n, b.n);
    ensure_capacity();
    for (int i = 0; i < n; ++i) {
        a[i] &= (i < b.n ? b.a[i] : 0);
    }
    delete_nils();
    return *this;
}

big_integer& big_integer::operator|=(const big_integer& b) {
    /*
     * a | 0 = a
     * 0 | b = b
     * a > 0 && b > 0
     * (-a) | (-b) = -(((a - 1) & (b - 1)) + 1)
     * (a)  | (-b) = -((~a & (b - 1)) + 1)
     * (-a) | (b)  = (b) | (-a)
    */
    if (is_zero()) {
        *this = b;
        return *this;
    }
    if (b.is_zero()) {
        return *this;
    }
    if (*this < 0 && b < 0) {
        *this = -(((abs() - 1) & (b.abs() - 1)) + 1);
        return *this;
    }
    if (*this > 0 && b < 0) {
        *this = -((~*this & (b.abs() - 1)) + 1);
        return *this;
    }
    if (*this < 0 && b > 0) {
        *this = b | *this;
        return *this;
    }
    n = std::max(n, b.n);
    ensure_capacity();
    for (int i = 0; i < n; ++i) {
        a[i] |= (i < b.n ? b.a[i] : 0);
    }
    delete_nils();
    return *this;
}

big_integer& big_integer::operator^=(const big_integer& b) {
    /*
     * a ^ 0 = a
     * 0 ^ b = b
     * a > 0 && b > 0
     * (-a) ^ (-b) = (a - 1) ^ (b - 1)
     * (a)  ^ (-b) = -((a ^ (b - 1)) + 1)
     * (-a) ^ (b)  = b ^ (-1)
    */
    if (is_zero()) {
        *this = b;
        return *this;
    }
    if (b.is_zero()) {
        return *this;
    }
    if (*this < 0 && b < 0) {
        *this = (abs() - 1) ^ (b.abs() - 1);
        return *this;
    }
    if (*this > 0 && b < 0) {
        *this = -((*this ^ (b.abs() - 1)) + 1);
        return *this;
    }
    if (*this < 0 && b > 0) {
        *this = b ^ *this;
        return *this;
    }
    n = std::max(n, b.n);
    ensure_capacity();
    for (int i = 0; i < n; ++i) {
        a[i] ^= (i < b.n ? b.a[i] : 0);
    }
    delete_nils();
    return *this;
}

big_integer& big_integer::operator>>=(int b) {
    // negative !!!
    //std::cout<<a[1];
    int shift = b % BSZE; // real shift
    int nil_cnt = b / BSZE; // first nil_cnt digits will be deleted
                            // last nil_cnt digits will be nils

    for (int i = nil_cnt; i < n; ++i) {
        unsigned int cur, to_prev;

        to_prev = 0;
        for (int j = 0; j < shift; ++j) {
            to_prev |= (1 << j); // first bits to prev digits
        }
        to_prev &= a[i]; // bits of current digit, sure
        to_prev <<= (BSZE - shift); // in next digits last bits will be first bits

        cur = a[i];
        cur >>= shift;

        a[i] = cur;
        if (i > 0)
           a[i - 1] |= to_prev;
    }
    //std::cout << a[1] << " !\n";
    for (int i = nil_cnt; i < n; ++i)
        a[i - nil_cnt] = a[i];
    for (int i = n - nil_cnt; i < n; ++i)
        a[i] = 0;

    delete_nils();
    ensure_capacity();
    return *this;
}

big_integer& big_integer::operator<<=(int b) {
    int shift = b % BSZE; // real shift
    int nil_cnt = b / BSZE; // last digits will be nils
    n++; // for last digit (a[n - 1] -> a[n])
    ensure_capacity();
    for (int i = n - 2; i >= 0; --i) {
        unsigned int cur, to_next;

        to_next = 0;
        for (int j = BSZE - shift; j < BSZE; ++j) {
            to_next |= (1 << j); // last bits to next digits
        }
        to_next &= a[i]; // bits of current digint, sure
        to_next >>= (BSZE - shift); // in next digits last bits will be first bits

        cur = a[i];
        cur <<= shift;
        for (int j = BSZE; j < ISZE; ++j) {
            cur &= ~(1U << j); // last bits = 0
        }

        a[i] = cur;
        a[i + 1] |= to_next;
    }
    n += nil_cnt;
    ensure_capacity();
    for (int i = n + nil_cnt - 1; i >= nil_cnt; --i) {
        a[i] = a[i - nil_cnt];
    }
    for (int i = 0; i < nil_cnt; ++i) {
        a[i] = 0;
    }
    delete_nils();
    return *this;
}

bool operator<(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == -1;
}

bool operator>(const big_integer& a, const big_integer& b) {
    return b < a;
}

bool operator<=(const big_integer& a, const big_integer& b) {
    return !(b < a);
}

bool operator>=(const big_integer& a, const big_integer& b) {
    return !(a < b);
}

bool operator==(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == 0;
}

bool operator!=(const big_integer& a, const big_integer& b) {
    return !(a == b);
}

big_integer big_integer::operator~() const {
    /* ~A = -(A + 1) */
    big_integer res = *this;
    res++;
    res = -res;
    return res;
}

big_integer operator+(big_integer a, const big_integer& b) {
    a += b;
    return a;
}

big_integer operator-(big_integer a, const big_integer& b) {
    a -= b;
    return a;
}

big_integer operator*(big_integer a, const big_integer& b) {
    a *= b;
    return a;
}

big_integer operator/(big_integer a, const int& b) {
    int mod;
    big_integer res;
    a.divmod(b, res, mod);
    return res;
}

int operator%(big_integer a, const int& b) {
    int mod;
    big_integer res;
    a.divmod(b, res, mod);
    return mod;
}

//big_integer operator/(big_integer a, const big_integer& b) {
//    a /= b;
//    return a;
//}

big_integer operator&(big_integer a, const big_integer& b) {
    a &= b;
    return a;
}

big_integer operator|(big_integer a, const big_integer& b) {
    a |= b;
    return a;
}

big_integer operator^(big_integer a, const big_integer& b) {
    a ^= b;
    return a;
}

big_integer operator>>(big_integer a, int b) {
    a >>= b;
    return a;
}

big_integer operator<<(big_integer a, int b) {
    a <<= b;
    return a;
}

std::string to_string(const big_integer &a) {
    char char_res[a.get_length()];
    int last = 0;
    std::string res;
    if (a.is_zero()) {
        char_res[last++] = '0';
    }
    big_integer other = a;
    while (!other.is_zero()) {
        int cur;
        other.divmod(10, other, cur);
        char_res[last++] = '0' + cur;
    }
    if (a < 0) {
        char_res[last++] = '-';
    }
    for (int i = last - 1; i >=0; --i)
        res += char_res[i];
    return res;
}

std::ostream& operator<<(std::ostream& s, big_integer& a) {
    return s << to_string(a);
}
