#include "big_integer.h"
#include <cstdio>

big_integer::big_integer() {
    a.push_back(0);
    sign = 0;
}

big_integer::~big_integer() {}

big_integer::big_integer(const big_integer& other) {
    if (*this != other)
        *this = other;
}

big_integer::big_integer(int x) {
    sign = 1;
    if (x < 0) {
        sign = -1;
        x = -x;
    } else if (x == 0)
        sign = 0;
    a.push_back(x % BASE);
    a.push_back((x / BASE) % BASE);
    if (a.back() == 0) {
        a.pop_back();
    }
}

big_integer::big_integer(std::string const& s) {
    sign = 0;
    int neg = (s[0] == '-' ? 1 : 0);
    a.push_back(0);
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

big_integer& big_integer::operator =(const big_integer& other) {
    if (*this == other)
        return *this;
    sign = other.sign;
    a.clear();
    a = other.a;
    return *this;
}

void big_integer::delete_nils() {
    while (a.size() > 1 && a.back() == 0)
        a.pop_back();
}

bool big_integer::is_zero() const {
    return (a.size() == 1 && a[0] == 0);
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
    if (a.size() != b.a.size())
        return ((int)a.size() * sign < (int)b.a.size() * b.sign ? -1 : 1);
    for (int i = (int)a.size() - 1; i >= 0; --i)
        if (a[i] != b.a[i])
            return (sign * a[i] < b.sign * b.a[i] ? -1 : 1);
    return 0;
}

int big_integer::get_length() const {
    return (int)a.size() * BSZE;
}

big_integer big_integer::operator -() const {
    big_integer res = big_integer(*this);
    res.sign *= (-1);
    return res;
}

big_integer big_integer::operator +() const {
    return *this;
}

big_integer& big_integer::operator ++() {
    big_integer b = 1;
    *this += b;
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer& big_integer::operator --() {
    big_integer b = 1;
    *this -= b;
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer big_integer::operator ++(int) {
    big_integer res = *this;
    ++(*this);
    if (is_zero())
        *this = 0;
    return res;
}

big_integer big_integer::operator --(int) {
    big_integer res = *this;
    --(*this);
    return res;
}


big_integer& big_integer::operator +=(const big_integer& b) {
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
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = 0; i < it_count || carry; ++i) {
        int f = (i < (int)a.size() ? a[i] : 0);
        int s = (i < (int)b.a.size() ? b.a[i] : 0);
        int cur = f + s + carry;
        if (i == (int)a.size())
            a.push_back(0);
        a[i] = cur % BASE;
        carry = cur / BASE;
    }
    delete_nils();
    return *this;
}

big_integer& big_integer::operator -=(const big_integer& b) {
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
    for (int i = 0; i < (int)b.a.size() || carry; ++i) {

        a[i] -= carry + (i < (int)b.a.size() ? b.a[i] : 0);
        carry = a[i] < 0;
        if (carry)
            a[i] += BASE;
    }
    delete_nils();
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer& big_integer::operator *=(const big_integer& b) {
    big_integer res = 0;
    res.sign = sign * b.sign;
    if (res.sign == 0) {
        *this = res;
        return *this;
    }

    res.a = std::vector<int>((int)a.size() + (int)b.a.size() + 5, 0); // ......

    for (int i = 0; i < (int)a.size(); ++i) {
        int carry = 0;
        for (int j = 0; j < (int)b.a.size() || carry; ++j) {
            long long cur = res.a[i + j] + 1LL * a[i] * (j < (int)b.a.size() ? b.a[j] : 0) + carry;
            carry = (int)(cur / BASE);
            res.a[i + j] = (int)(cur % BASE);
        }
    }

    *this = res;
    delete_nils();
    return *this;
}

big_integer& big_integer::operator /=(const big_integer& b) {
    big_integer mod;
    divmod(b, *this, mod);
    return *this;
}

big_integer& big_integer::operator %=(const big_integer& b) {
    big_integer div;
    divmod(b, div, *this);
    return *this;
}

void big_integer::divmod(const big_integer& b, big_integer& res_div, big_integer& res_mod) const {
    if (abs() < b.abs()) {
        res_div = 0;
        res_mod = 0;
        return;
    }

    big_integer arg = *this;
    big_integer div = b;
    big_integer res;

    res.sign = arg.sign * div.sign;;

    arg = arg.abs();
    div = div.abs();

    int shift = (int)arg.a.size() - (int)div.a.size();

    res.a = std::vector<int>(shift + 1, 0);

    big_integer shifted_arg;
    big_integer mult;

    for (int i = shift; i >= 0; --i) {
        // find current digit using binary search
        int l = 0;
        int r = BASE - 1;
        shifted_arg = arg >> (i * BSZE);
        for (int j = 0; j <= BSZE; ++j) {
            int m = (l + r) / 2;
            if (m * div <= shifted_arg)
                l = m;
            else
                r = m;
        }
        mult = l;
        mult *= div;

        mult <<= (i * BSZE);
        arg -= mult;
        res.a[i] = l;
    }
    if (arg.is_zero())
        arg.sign = 0;
    res.delete_nils();
    arg.delete_nils();
    arg.sign *= sign;
    res_div = res;
    res_mod = arg;
}

big_integer& big_integer::operator /=(int b) {
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
    for (int i = (int)other.a.size() - 1; i >= 0; --i) {
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

big_integer& big_integer::operator &=(const big_integer& b) {
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
        big_integer b_copy = -b;
        for (int i = 0; i < (int)b_copy.a.size(); ++i)
            b_copy.a[i] = ~b_copy.a[i];
        ++b_copy;
        *this &= b_copy;
        return *this;
    }
    if (*this < 0 && b > 0) {
        *this = b & *this;
        return *this;
    }
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = (int)a.size(); i < it_count; ++i)
        a.push_back(0);
    for (int i = 0; i < (int)a.size(); ++i) {
        a[i] &= (i < (int)b.a.size() ? b.a[i] : 0);
    }
    delete_nils();
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer& big_integer::operator |=(const big_integer& b) {
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
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = (int)a.size(); i < it_count; ++i)
        a.push_back(0);
    for (int i = 0; i < (int)a.size(); ++i) {
        a[i] |= (i < (int)b.a.size() ? b.a[i] : 0);
    }
    delete_nils();
    return *this;
}

big_integer& big_integer::operator ^=(const big_integer& b) {
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
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = (int)a.size(); i < it_count; ++i)
        a.push_back(0);
    for (int i = 0; i < (int)a.size(); ++i) {
        a[i] ^= (i < (int)b.a.size() ? b.a[i] : 0);
    }
    delete_nils();
    return *this;
}

big_integer& big_integer::operator >>=(int b) {
    if (b == 0 || is_zero()) {
        return *this;
    }
    if (sign < 0) {
        *this = -((abs() >> b) + 1);
        return *this;
    }

    int shift = b % BSZE; // real shift
    int nil_cnt = b / BSZE; // first nil_cnt digits will be deleted
                            // last nil_cnt digits will be nils

    for (int i = nil_cnt; i < (int)a.size(); ++i) {
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

    for (int i = nil_cnt; i < (int)a.size(); ++i)
        a[i - nil_cnt] = a[i];
    for (int i = (int)a.size() - nil_cnt; i < (int)a.size(); ++i)
        a[i] = 0;
    delete_nils();
    return *this;
}

big_integer& big_integer::operator <<=(int b) {
    if (b == 0 || is_zero()) {
        return *this;
    }
    int shift = b % BSZE; // real shift
    int nil_cnt = b / BSZE; // last digits will be nils
    a.push_back(0); // for last digit (a[n - 1] -> a[n])
    for (int i = (int)a.size() - 2; i >= 0; --i) {
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

    int prev_size = (int)a.size();
    for (int i = 0; i < nil_cnt; ++i)
        a.push_back(0);
    for (int i = prev_size + nil_cnt - 1; i >= nil_cnt; --i) {
        a[i] = a[i - nil_cnt];
    }
    for (int i = 0; i < nil_cnt; ++i) {
        a[i] = 0;
    }
    delete_nils();
    return *this;
}

bool operator <(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == -1;
}

bool operator >(const big_integer& a, const big_integer& b) {
    return b < a;
}

bool operator <=(const big_integer& a, const big_integer& b) {
    return !(b < a);
}

bool operator >=(const big_integer& a, const big_integer& b) {
    return !(a < b);
}

bool operator ==(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == 0;
}

bool operator !=(const big_integer& a, const big_integer& b) {
    return !(a == b);
}

big_integer big_integer::operator ~() const {
    /* ~A = -(A + 1) */
    big_integer res = *this;
    res++;
    res = -res;
    return res;
}

big_integer operator +(big_integer a, const big_integer& b) {
    a += b;
    return a;
}

big_integer operator -(big_integer a, const big_integer& b) {
    a -= b;
    return a;
}

big_integer operator *(big_integer a, const big_integer& b) {
    a *= b;
    return a;
}

big_integer operator /(big_integer a, const int& b) {
    int mod;
    big_integer res;
    a.divmod(b, res, mod);
    return res;
}

int operator %(big_integer a, const int& b) {
    int mod;
    big_integer res;
    a.divmod(b, res, mod);
    return mod;
}

big_integer operator /(big_integer a, const big_integer& b) {
    a /= b;
    return a;
}

big_integer operator %(big_integer a, const big_integer& b) {
    a %= b;
    return a;
}

big_integer operator &(big_integer a, const big_integer& b) {
    a &= b;
    return a;
}

big_integer operator |(big_integer a, const big_integer& b) {
    a |= b;
    return a;
}

big_integer operator ^(big_integer a, const big_integer& b) {
    a ^= b;
    return a;
}

big_integer operator >>(big_integer a, int b) {
    a >>= b;
    return a;
}

big_integer operator <<(big_integer a, int b) {
    a <<= b;
    return a;
}

std::string to_string(const big_integer &a) {
    int len = a.get_length();
    char *char_res = new char[len];
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

std::ostream& operator <<(std::ostream& s, big_integer& a) {
    return s << to_string(a);
}
