#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <iostream>

class big_integer {
private:
    const static int BSZE = 30;
    const static int BASE = (int)(1 << BSZE);
    int sign;
    int * a, a_size;
    int n;

public:
    big_integer();
    big_integer(const big_integer&);
    big_integer(int);
    explicit big_integer(const std::string&);

    big_integer& operator=(const big_integer&);

    void ensure_capacity();
    void delete_nils();

    bool is_zero() const;
    int cmp(const big_integer&) const;

    big_integer abs() const;

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer& operator--();
    big_integer operator++(int);
    big_integer operator--(int);

    big_integer& operator+=(const big_integer&);
    big_integer& operator-=(const big_integer&);
    big_integer& operator*=(const big_integer&);
    big_integer& operator/=(const big_integer&);
    big_integer& operator/=(int);

    big_integer& operator&=(const big_integer&);
    big_integer& operator|=(const big_integer&);
    big_integer& operator^=(const big_integer&);
    big_integer& operator>>=(int);
    big_integer& operator<<=(int);

    void divmod(int, big_integer&, int&) const;
};

bool operator<(const big_integer&, const big_integer&);
bool operator>(const big_integer&, const big_integer&);
bool operator<=(const big_integer&, const big_integer&);
bool operator>=(const big_integer&, const big_integer&);
bool operator==(const big_integer&, const big_integer&);
bool operator!=(const big_integer&, const big_integer&);

big_integer operator+(big_integer, const big_integer&);
big_integer operator-(big_integer, const big_integer&);
big_integer operator*(big_integer, const big_integer&);
big_integer operator/(big_integer, const big_integer&);

big_integer operator&(big_integer, const big_integer&);
big_integer operator|(big_integer, const big_integer&);
big_integer operator^(big_integer, const big_integer&);
big_integer operator>>(big_integer, int);
big_integer operator<<(big_integer, int);

std::string to_string(const big_integer&);
std::ostream& operator<<(std::ostream&, big_integer&);

#endif // BIG_INTEGER_H
