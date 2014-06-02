#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

template <class T>
struct my_vector {
private:
    struct vector_value {
        int ptr_cnt;
        const static int S_SIZE = 15;
        int sze;
        vector<T> * store;
        T small[S_SIZE];

        vector_value() {
            vector_value(0);
        }

        ~vector_value() {
           delete store;
        }

        vector_value(int n_size) {
            ptr_cnt = 0;
            if (n_size < 0) {
                /// exception !!!
            } else if (n_size <= S_SIZE) {
                for (int i = 0; i < S_SIZE; ++i)
                    small[i] = 0;
                store = NULL;
            } else {
                store = new vector<T>(n_size);
            }
            sze = n_size;
        }

        vector_value(const vector_value * other) {
            ptr_cnt = 0;
            sze = other->sze;
            if (sze <= S_SIZE) {
                /*cout << "we're copying small obj ...\n";*/
                memcpy(small, other->small, sizeof(other->small));
                store = NULL;
            } else {
                store = new vector<T>(sze + 1);
                *store = *other->store;
            }
        }

        void push_back(T x) {
            // we're usng small object
            if (sze < S_SIZE) {
                /*cout << "we're pushing back to small obj ...\n";*/
                small[sze] = x;
                /*cout << "small object exists: \n";
                for (int i = 0; i < S_SIZE; ++i)
                    cout << small[i] << " ";
                cout << "\n";*/
            } else if (sze == S_SIZE) {
                /*cout << "we're creating big obj ... \n";
                cout << "small object exists: \n";
                for (int i = 0; i < S_SIZE; ++i)
                    cout << small[i] << " ";
                cout << "\n";*/

                // we should use big object
                store = new vector<T>(S_SIZE);
                for (int i = 0; i < S_SIZE; ++i) {
                    (*store)[i] = small[i];
                    /*cout << i << " " << "\n";
                    cout << "sizeof store " << sizeof(store) << "\n";
                    cout << "sizeof small " << sizeof(small) << "\n";
                    cout << i << " small object exists: \n";
                    for (int i = 0; i < S_SIZE; ++i)
                        cout << small[i] << " ";
                    cout << "\n";*/
                }
                (*store).push_back(x);
                /*cout << "current size of big obj after PB is " << (*store).size() << "\n";

                cout << "big object exists: \n";
                for (int i = 0; i < (int)(*store).size(); ++i)
                    cout << (*store)[i] << " ";
                cout << "\n";*/
            }
            // we're using big object
            else
                (*store).push_back(x);
            ++sze;
        }

        void pop_back() {
            // we're using big object
            if (sze > S_SIZE) {
                (*store).pop_back();
                // we can use small object
                if (sze - 1 == S_SIZE) {
                    /*cout << "we're copying everything to big obj ...\n";*/
                    // copying to small object
                    for (int i = 0; i < S_SIZE; ++i)
                        small[i] = (*store)[i];
                    // we don't need big object anymore
                    delete store;
                    store = NULL;
                    /*cout << "we've done with it ...\n";*/
                }
            } else if (sze == 0) {
                /// exception !!!!!!!!!!!!!
            }
            --sze;
        }

        T back() const {
            if (sze <= S_SIZE && sze > 0)
                return small[sze - 1];
            else if (sze > S_SIZE) {
                /*cout << "we're extracting from big obj ... \n";*/
                return (*store).back();
            } else {
                /// exception !!!!!!!!!!!!!
                throw "not enough elements";
            }
        }
    };

    vector_value * value;

public:
    my_vector() {
        value = new vector_value(0);
        ++value->ptr_cnt;
    }

    my_vector(const my_vector& other) {
        value = other.value;
        ++value->ptr_cnt;
    }

    ~my_vector() {
        /*cout << "we're in destructor\n";*/
        if (--value->ptr_cnt == 0)
            delete value;
    }

    my_vector& operator = (const my_vector& other) {
        if (value == other.value)
            return *this;
        if (--value->ptr_cnt == 0)
            delete value;
        value = other.value;
        ++value->ptr_cnt;
        return *this;
    }

    const T& operator[] (int i) const {
        if (i > value->sze || i < 0) {
            /// exception !!!!!!1
        }
        return (value->sze <= value->S_SIZE ? value->small[i] : (*value->store)[i]);
    }

    T & operator[] (int i) {
        if (i > value->sze || i < 0) {
            /// exception !!!!!!1
        }
        if (value->ptr_cnt > 1) {
            --value->ptr_cnt;
            value = new vector_value(value);
            ++value->ptr_cnt;
        }
        return (value->sze <= value->S_SIZE ? value->small[i] : (*value->store)[i]);
    }

    size_t size() const {
        return (size_t)value->sze;
    }

    void resize(int n_size) {
        delete value;
        value = new vector_value(n_size);
        value->ptr_cnt++;
    }

    void push_back(T x) {
        if (value->ptr_cnt > 1) {
            --value->ptr_cnt;
            value = new vector_value(value);
            ++value->ptr_cnt;
        }
        value->push_back(x);
    }
    void pop_back() {
        if (value->ptr_cnt > 1) {
            --value->ptr_cnt;
            value = new vector_value(value);
            ++value->ptr_cnt;
        }
        value->pop_back();
    }

    T back() {
        return value->back();
    }
};

typedef unsigned int UI;

class big_integer {
private:
    const static int BSZE = 31; // base size
    const static int ISZE = 32; // size of a[]'s element
    const static UI BASE = (UI)(1U << BSZE);
    int sign;
    vector<UI> a;
    //my_vector<UI> a;
public:
    big_integer();
    ~big_integer();
    big_integer(const big_integer&);
    big_integer(int);
    explicit big_integer(const string&);

    big_integer& operator=(const big_integer&);

    void copy_to(const big_integer&);
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

    void negate();
    void absolute();

    void add(const big_integer&);
    void subtract(const big_integer&);
    void multiply(const big_integer&);
    void multiply(int);
    void divide(const big_integer&);
    void modulo(const big_integer&);
    void divide_int(int);

    void my_and(const big_integer&);
    void my_or(const big_integer&);
    void my_xor(const big_integer&);
    void lsh(int);
    void rsh(int);

    void divmod(int, big_integer&, int&) const;
    void divmod(const big_integer&, big_integer&, big_integer&, bool) const;
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

string to_string(const big_integer&);
ostream& operator <<(ostream&, big_integer&);

#endif // BIG_INTEGER_H
