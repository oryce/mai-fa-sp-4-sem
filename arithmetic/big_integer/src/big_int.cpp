#include "../include/big_int.h"

#include <algorithm>
#include <cmath>
#include <mutex>
#include <sstream>
#include <string>

// Auxiliary functions
bool big_int::is_zero() const {
    return this->_digits.size() == 1 && this->_digits[0] == 0;
}

void big_int::optimise() {
    while (this->_digits.size() > 1 && this->_digits.back() == 0) {
        this->_digits.pop_back();
    }
    if (this->is_zero()) {
        _sign = true;
    }
}

bool big_int::is_valid_digit(char c, unsigned radix) {
    const int value = [&] {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
        if (c >= 'a' && c <= 'z') return c - 'a' + 10;
        return -1;
    }();
    return value >= 0 && static_cast<unsigned>(value) < radix;
}

unsigned big_int::char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return 10 + c - 'A';
    if (c >= 'a' && c <= 'z') return 10 + c - 'a';
    throw std::invalid_argument(std::string("Invalid character: ") + c);
}

// Constructors
big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int> > &digits, bool sign) : _sign(sign),
    _digits(digits) {
    _digits.push_back(0);
    optimise();
}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int> > &&digits, bool sign) noexcept : _sign(sign),
    _digits(std::move(digits)) {
    _digits.push_back(0);
    optimise();
}

big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int> allocator) : _sign(true),
    _digits(allocator) {
    if (radix < 2 || radix > 36) {
        throw std::invalid_argument("Radix must be between 2 and 36");
    }
    if (num.empty()) {
        _digits.push_back(0);
        return;
    }
    std::string_view num_view(num);
    size_t start = 0;
    if (num_view[0] == '-') {
        _sign = false;
        start = 1;
    } else if (num_view[0] == '+') {
        start = 1;
    }
    while (start < num_view.size() - 1 && num_view[start] == '0') {
        ++start;
    }
    num_view = num_view.substr(start);

    if (num_view.empty() || num_view == "0") {
        _digits.push_back(0);
        _sign = true;
        return;
    }
    for (char c: num_view) {
        // string view only for using this range based for
        if (!is_valid_digit(c, radix)) {
            throw std::invalid_argument(
                std::string("Invalid character '") + c +
                "' for radix " + std::to_string(radix));
        }
        const unsigned digit = char_to_value(c);
        unsigned carry = digit;

        for (auto &d: _digits) {
            const auto product = static_cast<unsigned long long int>(d) * radix + carry;
            d = static_cast<unsigned int>(product & UINT_MAX);
            carry = static_cast<unsigned int>(product >> (8 * sizeof(unsigned int)));
        }

        if (carry > 0) {
            _digits.push_back(carry);
        }
    }
}

big_int::big_int(pp_allocator<unsigned int> allocator) : _sign(true), _digits(allocator) {
    _digits.push_back(0);
}


// Comparisons
big_int::operator bool() const noexcept {
    return !this->is_zero();
}

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept {
    if (_sign != other._sign) return _sign ? std::strong_ordering::greater : std::strong_ordering::less;

    bool is_pos = _sign;
    if (_digits.size() != other._digits.size()) {
        return is_pos ? _digits.size() <=> other._digits.size() : other._digits.size() <=> _digits.size();
    }

    for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
        if (_digits[i] != other._digits[i]) {
            return is_pos ? _digits[i] <=> other._digits[i] : other._digits[i] <=> _digits[i];
        }
    }

    return std::strong_ordering::equal;
}

bool big_int::operator==(const big_int &other) const noexcept {
    return std::strong_ordering::equal == (*this <=> other);
}


// Binary operations
big_int big_int::operator~() const {
    big_int result(*this);
    for (auto &digit: result._digits) {
        digit = ~digit;
    }
    result.optimise();
    return result;
}

big_int &big_int::operator&=(const big_int &other) & {
    const size_t min_size = std::min(_digits.size(), other._digits.size());
    _digits.resize(min_size, 0);
    for (size_t i = 0; i < min_size; ++i) {
        _digits[i] &= other._digits[i];
    }
    optimise();
    return *this;
}

big_int big_int::operator&(const big_int &other) const {
    big_int tmp = *this;
    return tmp &= other;
}

big_int &big_int::operator|=(const big_int &other) & {
    const size_t max_size = std::max(_digits.size(), other._digits.size());
    _digits.resize(max_size, 0);
    for (size_t i = 0; i < max_size; ++i) {
        _digits[i] |= (i < other._digits.size()) ? other._digits[i] : 0;
    }
    optimise();
    return *this;
}

big_int big_int::operator|(const big_int &other) const {
    big_int tmp = *this;
    return tmp |= other;
}

big_int &big_int::operator^=(const big_int &other) & {
    const size_t max_size = std::max(_digits.size(), other._digits.size());
    _digits.resize(max_size, 0);
    for (size_t i = 0; i < max_size; ++i) {
        _digits[i] ^= (i < other._digits.size()) ? other._digits[i] : 0;
    }
    optimise();
    return *this;
}

big_int big_int::operator^(const big_int &other) const {
    big_int tmp = *this;
    return tmp ^= other;
}

big_int &big_int::operator<<=(size_t shift) & {
    if (shift == 0 || (_digits.size() == 1 && _digits[0] == 0)) return *this;

    size_t int_shift = shift / (8 * sizeof(unsigned int));
    size_t bit_shift = shift % (8 * sizeof(unsigned int));
    if (int_shift > 0) {
        _digits.insert(_digits.begin(), int_shift, 0);
    }
    if (bit_shift > 0) {
        unsigned long long carry = 0;
        for (unsigned int &_digit: _digits) {
            unsigned long long value = (static_cast<unsigned long long>(_digit) << bit_shift) | carry;
            _digit = static_cast<unsigned int>(value % BASE);
            carry = value / BASE;
        }

        if (carry > 0) {
            _digits.push_back(static_cast<unsigned int>(carry));
        }
    }
    optimise();
    return *this;
}

big_int big_int::operator<<(size_t shift) const {
    big_int tmp = *this;
    return tmp <<= shift;
}

big_int &big_int::operator>>=(size_t shift) & {
    if (shift == 0 || (_digits.size() == 1 && _digits[0] == 0)) return *this;

    size_t int_shift = shift / (8 * sizeof(unsigned int));
    size_t bit_shift = shift % (8 * sizeof(unsigned int));
    if (int_shift >= _digits.size()) {
        _digits.clear();
        _digits.push_back(0);
        optimise();
        return *this;
    }

    if (int_shift > 0) {
        _digits.erase(_digits.begin(), _digits.begin() + static_cast<unsigned int>(int_shift));
    }

    if (bit_shift > 0) {
        unsigned long long carry = 0;
        for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
            unsigned long long value = (carry << (8 * sizeof(unsigned int))) | _digits[i];
            _digits[i] = static_cast<unsigned int>(value >> bit_shift);
            carry = (value & ((1ULL << bit_shift) - 1));
        }
    }
    optimise();
    return *this;
}

big_int big_int::operator>>(size_t shift) const {
    big_int tmp = *this;
    return tmp >>= shift;
}


// Input / Output
std::string big_int::to_string() const {
    if (this->is_zero()) return "0";
    std::string res;
    big_int tmp = *this;
    tmp._sign = true;
    while (tmp) {
        auto val = tmp % 10;
        res += static_cast<char>('0' + val._digits[0]);
        tmp /= 10;
    }
    if (!_sign) {
        res += '-';
    }
    std::ranges::reverse(res);
    return res;
}

std::ostream &operator<<(std::ostream &stream, const big_int &value) {
    stream << value.to_string();
    return stream;
}

std::istream &operator>>(std::istream &stream, big_int &value) {
    std::string val;
    stream >> val;
    value = big_int(val, 10, value._digits.get_allocator());
    return stream;
}


// Arithmetic operations + -
big_int &big_int::minus_assign(const big_int &other, size_t shift) & {
    if (other.is_zero()) return *this;

    if (_sign != other._sign) {
        big_int temp(other);
        temp._sign = _sign;
        return plus_assign(temp, shift);
    }

    big_int abs_this(*this);
    abs_this._sign = true;
    big_int abs_other(other);
    abs_other._sign = true;
    abs_other <<= shift;

    bool result_sign = _sign;
    if ((abs_this <=> abs_other) == std::strong_ordering::less) {
        result_sign = !result_sign;
        std::swap(abs_this._digits, abs_other._digits);
    }

    const size_t max_size = std::max(abs_this._digits.size(), abs_other._digits.size());
    std::vector<unsigned int, pp_allocator<unsigned int> > result(max_size, 0, _digits.get_allocator());
    long long borrow = 0;

    for (size_t i = 0; i < max_size; ++i) {
        long long difference = borrow;
        if (i < abs_this._digits.size()) {
            difference += abs_this._digits[i];
        }

        if (i < abs_other._digits.size()) {
            difference -= abs_other._digits[i];
        }

        if (difference < 0) {
            difference += static_cast<long long>(BASE);
            borrow = -1;
        } else {
            borrow = 0;
        }
        result[i] = static_cast<unsigned int>(difference);
    }
    _digits = std::move(result);
    _sign = result_sign;
    optimise();
    if (this->is_zero()) {
        _sign = true;
    }
    return *this;
}

big_int &big_int::operator-=(const big_int &other) & {
    return minus_assign(other, 0);
}

big_int big_int::operator-(const big_int &other) const {
    big_int tmp = *this;
    return tmp -= other;
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) & {
    if (other._digits.size() == 1 && other._digits[0] == 0) return *this;
    if (_sign == other._sign) {
        size_t max_size = std::max(_digits.size(), other._digits.size() + shift);
        _digits.resize(max_size, 0);

        unsigned long long carry = 0;
        for (size_t i = 0; i < max_size; ++i) {
            unsigned long long sum = carry;
            if (i < _digits.size()) {
                sum += _digits[i];
            }

            if (i >= shift && (i - shift) < other._digits.size()) {
                sum += other._digits[i - shift];
            }

            _digits[i] = static_cast<unsigned int>(sum % BASE);
            carry = sum / BASE;
        }

        if (carry > 0) {
            _digits.push_back(static_cast<unsigned int>(carry));
        }
    } else {
        big_int temp(other);
        temp._sign = _sign;
        minus_assign(temp, shift);
    }
    optimise();
    if (this->is_zero()) {
        _sign = true;
    }
    return *this;
}

big_int &big_int::operator+=(const big_int &other) & {
    return plus_assign(other, 0);
}

big_int big_int::operator+(const big_int &other) const {
    big_int tmp = *this;
    return tmp += other;
}

big_int &big_int::operator++() & {
    *this += big_int(1);
    return *this;
}

big_int big_int::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

big_int &big_int::operator--() & {
    *this -= big_int(1, _digits.get_allocator());
    return *this;
}

big_int big_int::operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
}


// Arithmetic operations * / %
big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) & {
    if (other.is_zero() || this->is_zero()) {
        _digits.clear();
        _digits.push_back(0);
        optimise();
        return *this;
    }

    if (rule == multiplication_rule::trivial) {
        big_int result(_digits.get_allocator());
        result._digits.resize(_digits.size() + other._digits.size(), 0);
        for (size_t i = 0; i < _digits.size(); ++i) {
            unsigned long long carry = 0;
            for (size_t j = 0; j < other._digits.size() || carry; ++j) {
                unsigned long long prod = result._digits[i + j] + carry;
                if (j < other._digits.size()) {
                    prod += static_cast<unsigned long long>(_digits[i]) * other._digits[j];
                }

                result._digits[i + j] = static_cast<unsigned int>(prod % BASE);
                carry = prod / BASE;
            }
        }

        _sign = (_sign == other._sign);
        _digits = std::move(result._digits);
        optimise();
        return *this;
    }
    if (rule == multiplication_rule::Karatsuba) {
        big_int result = multiply_karatsuba(*this, other);
        _digits = std::move(result._digits);
        _sign = (_sign == other._sign);
        optimise();
        return *this;
    }
    return *this;
}

big_int &big_int::operator*=(const big_int &other) & {
    return multiply_assign(other, decide_mult((other._digits.size())));
}

big_int big_int::operator*(const big_int &other) const {
    big_int tmp = *this;
    return tmp *= other;
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) & {
    if (this->is_zero()) return *this;
    if (other.is_zero()) throw std::logic_error("Division by zero");

    big_int abs_this(*this);
    abs_this._sign = true;
    big_int abs_other(other);
    abs_other._sign = true;
    if (abs_this < abs_other) {
        _digits.clear();
        _digits.push_back(0);
        optimise();
        return *this;
    }

    std::vector<unsigned int, pp_allocator<unsigned int> > quotient(_digits.size(), 0, _digits.get_allocator());
    big_int remain(0, _digits.get_allocator());
    for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; i--) {
        remain._digits.insert(remain._digits.begin(), _digits[i]);
        remain.optimise();

        unsigned long long left = 0, q = 0, right = BASE;
        while (left <= right) {
            const unsigned long long mid = left + (right - left) / 2;
            big_int temp = abs_other * big_int(static_cast<long long>(mid), _digits.get_allocator());
            if (remain >= temp) {
                q = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        } // Binsearch for find the whole result

        if (q > 0) {
            const big_int temp = abs_other * big_int(static_cast<long long>(q), _digits.get_allocator());
            remain -= temp;
        }
        quotient[i] = static_cast<unsigned int>(q);
    }

    _sign = (_sign == other._sign);
    _digits = std::move(quotient);
    optimise();
    return *this;
}

big_int &big_int::operator/=(const big_int &other) & {
    return divide_assign(other, decide_div(other._digits.size()));
}

big_int big_int::operator/(const big_int &other) const {
    big_int tmp = *this;
    return tmp /= other;
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) & {
    if (this->is_zero()) return *this;
    if (other.is_zero()) throw std::logic_error("Division by zero");

    big_int abs_this(*this);
    abs_this._sign = true;
    big_int abs_other(other);
    abs_other._sign = true;
    if (abs_this < abs_other) {
        _sign = true;
        return *this;
    }

    big_int remain(0, _digits.get_allocator());
    for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
        remain._digits.insert(remain._digits.begin(), _digits[i]);
        remain.optimise();
        unsigned long long left = 0, right = BASE;
        unsigned long long q = 0;
        while (left <= right) {
            const unsigned long long mid = left + (right - left) / 2;
            big_int temp = abs_other * big_int(static_cast<long long>(mid), _digits.get_allocator());
            if (remain >= temp) {
                q = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        if (q > 0) {
            big_int temp = abs_other * big_int(static_cast<long long>(q), _digits.get_allocator());
            remain -= temp;
        }
    }

    _digits = std::move(remain._digits);
    _sign = true;
    optimise();
    return *this;
}

big_int &big_int::operator%=(const big_int &other) & {
    return modulo_assign(other, decide_div(other._digits.size()));
}

big_int big_int::operator%(const big_int &other) const {
    big_int tmp = *this;
    return tmp %= other;
}

//  Schizophrenia
big_int::multiplication_rule big_int::decide_mult(size_t rhs) const noexcept {
    return (rhs > 32) ? multiplication_rule::Karatsuba : multiplication_rule::trivial;
} // This tsiferka was made up out of my head

big_int::division_rule big_int::decide_div(size_t rhs) const noexcept {
    return division_rule::trivial;
}

big_int operator""_bi(unsigned long long n) {
    return {n};
} // The thing that calls the bigint constructor if you write 228_bi

big_int multiply_karatsuba(const big_int &a, const big_int &b) {
    if (a._digits.size() <= 8 || b._digits.size() <= 8) {
        big_int result = a;
        result.multiply_assign(b, big_int::multiplication_rule::trivial);
        return result;
    }
    const size_t max_size = (std::max(a._digits.size(), b._digits.size()) + 1) / 2;

    big_int low1(a._digits.get_allocator()), high1(a._digits.get_allocator());
    low1._digits.assign(
        a._digits.begin(),
        a._digits.begin() + std::min(max_size, a._digits.size())
    );
    if (a._digits.size() > max_size) {
        high1._digits.assign(a._digits.begin() + max_size, a._digits.end());
    }
    low1.optimise();
    high1.optimise();
    // Sawing the first number

    big_int low2(b._digits.get_allocator()), high2(b._digits.get_allocator());
    low2._digits.assign(
        b._digits.begin(),
        b._digits.begin() + std::min(max_size, b._digits.size())
    );
    if (b._digits.size() > max_size) {
        high2._digits.assign(b._digits.begin() + max_size, b._digits.end());
    }
    low2.optimise();
    high2.optimise();
    // Sawing the second number

    big_int z0 = multiply_karatsuba(low1, low2);
    big_int z2 = multiply_karatsuba(high1, high2);

    big_int sum1 = low1 + high1;
    big_int sum2 = low2 + high2;
    big_int z1 = multiply_karatsuba(sum1, sum2);
    z1 -= z0;
    z1 -= z2;

    z2 <<= (max_size * 2); // this operation is equivalent to  *base^2
    z1 <<= max_size; // this operation is equivalent to  *base

    big_int result = z2 + z1 + z0;
    // (h1*Base + l1)(h2*Base +l2) = h1*h2*Base^2 + ((h1 + l1)(h2 + l2) - h1*h2 - l1*l2)*Base + l1*l2
    result._sign = (a._sign == b._sign);
    result.optimise();
    return result;
}
