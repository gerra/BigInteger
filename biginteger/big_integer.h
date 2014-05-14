#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <iostream>
#include <vector>

class big_integer {
private:
    const static int BSZE = 30; // base size
    const static int ISZE = 32; // size of a[]'s element
    const static int BASE = (int)(1 << BSZE);
    int sign;
    std::vector<int> a;

public:
    big_integer();
    ~big_integer();
    big_integer(const big_integer&);
    big_integer(int);
    explicit big_integer(const std::string&);

    big_integer& operator=(const big_integer&);

    void ensure_capacity();
    void set_nils();
    void delete_nils();

    bool is_zero() const;
    int cmp(const big_integer&) const;
    int get_length() const;

    big_integer abs() const;

    big_integer operator +() const;
    big_integer operator -() const;
    big_integer operator ~() const;

    big_integer& operator ++();
    big_integer& operator --();
    big_integer operator ++(int);
    big_integer operator --(int);

    big_integer& operator +=(const big_integer&);
    big_integer& operator -=(const big_integer&);
    big_integer& operator *=(const big_integer&);
    big_integer& operator /=(const big_integer&);
    big_integer& operator %=(const big_integer&);
    big_integer& operator /=(int);

    big_integer& operator &=(const big_integer&);
    big_integer& operator |=(const big_integer&);
    big_integer& operator ^=(const big_integer&);
    big_integer& operator >>=(int);
    big_integer& operator <<=(int);

    void divmod(int, big_integer&, int&) const;
    void divmod(const big_integer&, big_integer&, big_integer&) const;
};

bool operator <(const big_integer&, const big_integer&);
bool operator >(const big_integer&, const big_integer&);
bool operator <=(const big_integer&, const big_integer&);
bool operator >=(const big_integer&, const big_integer&);
bool operator ==(const big_integer&, const big_integer&);
bool operator !=(const big_integer&, const big_integer&);

big_integer operator +(big_integer, const big_integer&);
big_integer operator -(big_integer, const big_integer&);
big_integer operator *(big_integer, const big_integer&);
big_integer operator /(big_integer, const big_integer&);
big_integer operator %(big_integer, const big_integer&);

big_integer operator &(big_integer, const big_integer&);
big_integer operator |(big_integer, const big_integer&);
big_integer operator ^(big_integer, const big_integer&);
big_integer operator >>(big_integer, int);
big_integer operator <<(big_integer, int);

std::string to_string(const big_integer&);
std::ostream& operator <<(std::ostream&, big_integer&);

#endif // BIG_INTEGER_H
