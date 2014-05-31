#include "big_integer.h"
#include <cstdio>

big_integer::big_integer() {
    a.push_back(0);
    sign = 0;
}

big_integer::~big_integer() {}

big_integer::big_integer(const big_integer& other) {
    int comp = cmp(other);
    if (comp != 0)
        copy_to(other);
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
        multiply(10);
        add(s[i] - '0');
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
    int comp = cmp(other);
    if (comp != 0)
        copy_to(other);
    return *this;
}

void big_integer::copy_to(const big_integer& other) {
    sign = other.sign;
    a = other.a;
}

void big_integer::delete_nils() {
    while (a.size() > 1 && a.back() == 0)
        a.pop_back();
}

bool big_integer::is_zero() const {
    return ((int)a.size() == 1 && a[0] == 0);
}

big_integer big_integer::abs() const {
    big_integer res;
    res.copy_to(*this);
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
    big_integer res;
    res.copy_to(*this);
    res.negate();
    return res;
}

big_integer big_integer::operator +() const {
    return *this;
}

big_integer& big_integer::operator ++() {
    big_integer one;
    one.sign = 1;
    one.a[0] = 1;
    add(one);
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer& big_integer::operator --() {
    big_integer one;
    one.sign = 1;
    one.a[0] = 1;
    subtract(one);
    if (is_zero())
        *this = 0;
    return *this;
}

big_integer big_integer::operator ++(int) {
    big_integer res = *this;
    big_integer one;
    one.sign = 1;
    one.a[0] = 1;
    add(one);
    if (is_zero())
        *this = 0;
    return res;
}

big_integer big_integer::operator --(int) {
    big_integer res = *this;
    big_integer one;
    one.sign = 1;
    one.a[0] = 1;
    subtract(one);
    return res;
}

void big_integer::negate() {
    sign *= (-1);
}

void big_integer::absolute() {
    if (sign < 1)
        sign = 1;
}

void big_integer::add(const big_integer& b) {
    if (b.is_zero()) {
        return;
    }
    if (is_zero()) {
        copy_to(b);
        return;
    }
    if (sign != b.sign) {
        big_integer copy_b;
        copy_b.copy_to(b);
        copy_b.negate();
        subtract(copy_b);
        return;
    }
    a.push_back(0); a.push_back(0);
    unsigned int carry = 0;
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = 0; i < it_count || carry; ++i) {
        int f = (i < (int)a.size() ? a[i] : 0);
        int s = (i < (int)b.a.size() ? b.a[i] : 0);
        unsigned int cur = f + s + carry;

        a[i] = cur % BASE;
        carry = cur / BASE;
    }
    delete_nils();
}

void big_integer::subtract(const big_integer& b) {
    if (b.is_zero()) {
        return;
    }
    if (is_zero()) {
        big_integer res;
        res.copy_to(b);
        res.negate();
        copy_to(res);
        return;
    }
    if (sign != b.sign) {
        big_integer neg_b;
        neg_b.copy_to(b);
        neg_b.negate();
        add(neg_b);
        return;
    }
    /////////////////////////////////////////
    /// abs() < b.abs()
    /////////////////////////////////////////
    big_integer copy_b;
    copy_b.copy_to(b);
    bool this_was_changed = (sign == -1);
    bool b_was_changed = (copy_b.sign == -1);
    absolute();
    copy_b.absolute();
    int comp = cmp(copy_b);
    if (comp < 0) {
        if (this_was_changed)
            negate();
        if (b_was_changed)
            copy_b.negate();

        copy_b.subtract(*this);
        copy_b.negate();
        copy_to(copy_b);
        return;
    }
    if (this_was_changed)
        negate();
    if (b_was_changed)
        copy_b.negate();
    ///////////////////////////////////////
    int carry = 0;
    for (int i = 0; i < (int)b.a.size() || carry; ++i) {
        int cur = a[i];
        cur -= carry + (i < (int)b.a.size() ? b.a[i] : 0);
        carry = cur < 0;
        if (carry)
            cur += BASE;
        a[i] = cur;
    }

    delete_nils();
    if (is_zero()) {
        big_integer res;
        copy_to(res);
    }
}

void big_integer::multiply(const big_integer& b) {
    big_integer res;
    res.sign = sign * b.sign;
    if (res.sign == 0) {
        copy_to(res);
        return;
    }

    res.a.resize((int)a.size() + (int)b.a.size() + 1);
    for (int i = 0; i < (int)a.size(); ++i) {
        int carry = 0;
        for (int j = 0; j < (int)b.a.size() || carry; ++j) {
            long long cur = res.a[i + j] + 1LL * a[i] * (j < (int)b.a.size() ? b.a[j] : 0) + carry;
            carry = (int)(cur / BASE);
            res.a[i + j] = (int)(cur % BASE);
        }
    }
    copy_to(res);
    delete_nils();
}

void big_integer::multiply(int b) {
    if (b == 0 || is_zero()) {
        big_integer res;
        copy_to(res);
        return;
    }
    int carry = 0;
    if (b < 0) {
        sign *= (-1);
        b = -b;
    }
    for (int i = 0; i < (int)a.size() || carry; ++i) {
        if (i == (int)a.size())
            a.push_back(0);
        long long cur = carry + a[i] * 1ll * b;
        a[i] = int(cur % BASE);
        carry = int(cur / BASE);
    }
    delete_nils();
}

big_integer& big_integer::operator +=(const big_integer& b) {
    add(b);
    return *this;
}

big_integer& big_integer::operator -=(const big_integer& b) {
    subtract(b);
    return *this;
}

big_integer& big_integer::operator *=(const big_integer& b) {
    multiply(b);
    return *this;
}

big_integer& big_integer::operator /=(const big_integer& b) {
    big_integer mod;
    divmod(b, *this, mod, false);
    return *this;
}

big_integer& big_integer::operator %=(const big_integer& b) {
    big_integer div;
    divmod(b, div, *this, true);
    return *this;
}


void big_integer::divmod(const big_integer& b, big_integer& res_div, big_integer& res_mod, bool need_mod) const {
    if (abs() < b.abs()) {
        big_integer res1;
        res_div = res1;
        res_mod.copy_to(*this);
        return;
    }

    big_integer arg, div, res;

    arg.copy_to(*this);
    div.copy_to(b);

    res.sign = arg.sign * div.sign;

    arg.absolute();
    div.absolute();

    int ext_mul = 1, ext_shift = 0;
    while (ext_mul * div.a.back() < BASE / 2) {
        ext_mul *= 2;
        ext_shift++;
    }
    if (ext_mul != 1) {
        arg.lsh(ext_shift);
        div.lsh(ext_shift);
    }

    int n, m;
    n = div.a.size();
    m = arg.a.size() - div.a.size();

    res.a.resize(m + 1);

    big_integer shifted, sub;
    shifted.copy_to(div);
    shifted.lsh(m * BSZE);
    if (arg >= shifted) {
        res.a[m] = 1;
        arg.subtract(shifted);
    } else {
        res.a[m] = 0;
    }

    for (int j = m - 1; j >= 0; --j) {
        long long cur = 1LL * (n + j < (int)arg.a.size() ? arg.a[n + j] : 0) * BASE +
                (n + j - 1 < (int)arg.a.size() ? arg.a[n + j - 1] : 0);

        cur /= div.a[n - 1];
        if (BASE - 1 < cur)
            cur = BASE - 1;
        res.a[j] = cur;
        shifted.rsh(BSZE);
        sub.copy_to(shifted);
        sub.multiply(res.a[j]);
        arg.subtract(sub);
        while (arg.sign == -1) {
            res.a[j]--;
            arg.add(shifted);
        }
    }

    if (arg.is_zero())
        arg.sign = 0;
    res.delete_nils();
    arg.delete_nils();
    if (need_mod && ext_mul != 1) {
        arg.rsh(ext_shift); // normalization of remainder
        arg.delete_nils();
    }
    arg.sign *= sign;
    res_div.copy_to(res);
    res_mod.copy_to(arg);
}

big_integer& big_integer::operator /=(int b) {
    int mod;
    divmod(b, *this, mod);
    return *this;
}

void big_integer::divmod(int b, big_integer& div, int& mod) const {
    big_integer other;
    other.copy_to(*this);
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
    if (other.is_zero()) {
        big_integer res;
        other.copy_to(res);
    }
    mod = carry;
    div.copy_to(other);
}

/// binary operations !!!

void big_integer::my_and(const big_integer& b) {
    /*
     * a & 0 = 0
     * 0 & b = 0
     * a > 0 && b > 0
     * (-a) & (-b) = -(((a - 1) | (b - 1)) + 1)
     * (a)  & (-b) = a & ~(b - 1)
     * (-a) & (b)  = (b) & (-a)
    */
    if (is_zero() || b.is_zero()) {
        big_integer res;
        copy_to(res);
        return;
    }
    if (*this < 0 && b < 0) {
        *this = -(((abs() - 1) | (b.abs() - 1)) + 1);
        return;
    }
    if (*this > 0 && b < 0) {
        big_integer b_copy;
        b_copy.copy_to(b);
        b_copy.negate();
        for (int i = 0; i < (int)b_copy.a.size(); ++i)
            b_copy.a[i] = ~b_copy.a[i];
        ++b_copy;
        my_and(b_copy);
        return;
    }
    if (*this < 0 && b > 0) {
        *this = b & *this;
        return;
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
}

void big_integer::my_or(const big_integer& b) {
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
        return;
    }
    if (b.is_zero()) {
        return;
    }
    if (*this < 0 && b < 0) {
        *this = -(((abs() - 1) & (b.abs() - 1)) + 1);
        return;
    }
    if (*this > 0 && b < 0) {
        *this = -((~*this & (b.abs() - 1)) + 1);
        return;
    }
    if (*this < 0 && b > 0) {
        *this = b | *this;
        return;
    }
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = (int)a.size(); i < it_count; ++i)
        a.push_back(0);
    for (int i = 0; i < (int)a.size(); ++i) {
        a[i] |= (i < (int)b.a.size() ? b.a[i] : 0);
    }
    delete_nils();
}

void big_integer::my_xor(const big_integer& b) {
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
        return;
    }
    if (b.is_zero()) {
        return;
    }
    if (*this < 0 && b < 0) {
        *this = (abs() - 1) ^ (b.abs() - 1);
        return;
    }
    if (*this > 0 && b < 0) {
        *this = -((*this ^ (b.abs() - 1)) + 1);
        return;
    }
    if (*this < 0 && b > 0) {
        *this = b ^ *this;
        return;
    }
    int it_count = std::max((int)a.size(), (int)b.a.size());
    for (int i = (int)a.size(); i < it_count; ++i)
        a.push_back(0);
    for (int i = 0; i < (int)a.size(); ++i) {
        a[i] ^= (i < (int)b.a.size() ? b.a[i] : 0);
    }
    delete_nils();
}

void big_integer::lsh(int b) {
    if (b == 0 || is_zero()) {
        return;
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
}

void big_integer::rsh(int b) {
    if (b == 0 || is_zero()) {
        return;
    }
    if (sign < 0) {
        absolute();
        rsh(b);
        add(1);
        negate();
        return;
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
}

big_integer& big_integer::operator &=(const big_integer& b) {
    my_and(b);
    return *this;
}

big_integer& big_integer::operator |=(const big_integer& b) {
    my_or(b);
    return *this;
}

big_integer& big_integer::operator ^=(const big_integer& b) {
    my_xor(b);
    return *this;
}

big_integer& big_integer::operator >>=(int b) {
    rsh(b);
    return *this;
}

big_integer& big_integer::operator <<=(int b) {
    lsh(b);
    return *this;
}

bool operator <(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == -1;
}

bool operator >(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == 1;
}

bool operator <=(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res <= 0;
}

bool operator >=(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res >= 0;
}

bool operator ==(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res == 0;
}

bool operator !=(const big_integer& a, const big_integer& b) {
    int res = a.cmp(b);
    return res != 0;
}

big_integer big_integer::operator ~() const {
    /* ~A = -(A + 1) */
    big_integer res = *this;
    res.add(1);
    res.negate();
    return res;
}

big_integer operator +(big_integer a, const big_integer& b) {
    a.add(b);
    return a;
}

big_integer operator -(big_integer a, const big_integer& b) {
    a.subtract(b);
    return a;
}

big_integer operator *(big_integer a, const big_integer& b) {
    a.multiply(b);
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
    big_integer mod;
    a.divmod(b, a, mod, false);
    return a;
}

big_integer operator %(big_integer a, const big_integer& b) {
    big_integer div;
    a.divmod(b, div, a, true);
    return a;
}

big_integer operator &(big_integer a, const big_integer& b) {
    a.my_and(b);
    return a;
}

big_integer operator |(big_integer a, const big_integer& b) {
    a.my_or(b);
    return a;
}

big_integer operator ^(big_integer a, const big_integer& b) {
    a.my_xor(b);
    return a;
}

big_integer operator >>(big_integer a, int b) {
    a.rsh(b);
    return a;
}

big_integer operator <<(big_integer a, int b) {
    a.lsh(b);
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
    for (int i = last - 1; i >= 0; --i)
        res += char_res[i];
    return res;
}

std::ostream& operator <<(std::ostream& s, big_integer& a) {
    return s << to_string(a);
}
