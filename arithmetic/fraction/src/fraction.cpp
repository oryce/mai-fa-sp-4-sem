#include "../include/fraction.h"
#include <cmath>
#include <sstream>
#include <regex>

big_int abs(const big_int &numb) {
    return (numb < 0) ? 0_bi - numb : numb;
}

fraction abs(const fraction &numb) {
    return (numb < fraction{0, 1}) ? -numb : numb;
}

big_int gcd(big_int a, big_int b) {
    while (b != 0) {
        const big_int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

void fraction::optimise() {
    if (_denominator == 0) {
        throw std::invalid_argument("Denominator cannot be zero");
    }
    if (_numerator == 0) {
        _denominator = 1;
        return;
    }
    if (_numerator < 0) {
        _numerator = abs(_numerator);
        _denominator = 0_bi - _denominator;
    }
    const big_int divisor = gcd(abs(_numerator), abs(_denominator));
    _numerator /= divisor;
    _denominator /= divisor;
}

fraction fraction::operator-() const {
    fraction result(*this);
    result._denominator = 0_bi - _denominator;
    result.optimise();
    return result;
}

fraction::fraction(const pp_allocator<big_int::value_type> allocator)
    : _numerator(0, allocator), _denominator(1, allocator) {
}

fraction &fraction::operator+=(fraction const &other) & {
    _numerator = _numerator * other._denominator + _denominator * other._numerator;
    _denominator = _denominator * other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator+(fraction const &other) const {
    fraction result = *this;
    result += other;
    return result;
}

fraction &fraction::operator-=(fraction const &other) & {
    _numerator = _numerator * other._denominator - _denominator * other._numerator;
    _denominator = _denominator * other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator-(fraction const &other) const {
    fraction result = *this;
    result -= other;
    return result;
}

fraction &fraction::operator*=(fraction const &other) & {
    _numerator *= other._numerator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator*(fraction const &other) const {
    fraction result = *this;
    result *= other;
    return result;
}

fraction &fraction::operator/=(fraction const &other) & {
    _numerator *= other._denominator;
    _denominator *= other._numerator;
    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const {
    fraction result = *this;
    result /= other;
    return result;
}

bool fraction::operator==(fraction const &other) const noexcept {
    return _numerator == other._numerator && _denominator == other._denominator;
}

std::partial_ordering fraction::operator<=>(const fraction &other) const noexcept {
    const bool this_neg = (_denominator < 0);
    const bool other_neg = (other._denominator < 0);

    if (this_neg != other_neg) {
        return this_neg
                   ? std::partial_ordering::less
                   : std::partial_ordering::greater;
    }
    const big_int lhs = _numerator * abs(other._denominator);
    const big_int rhs = other._numerator * abs(_denominator);
    if (this_neg) {
        if (lhs < rhs) return std::partial_ordering::greater;
        if (lhs > rhs) return std::partial_ordering::less;
    } else {
        if (lhs < rhs) return std::partial_ordering::less;
        if (lhs > rhs) return std::partial_ordering::greater;
    }
    return std::partial_ordering::equivalent;
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj) {
    stream << obj.to_string();
    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj) {
    std::string input;
    stream >> input;
    std::regex fraction_regex(R"(^([-+]?[0-9]+)(?:/([-+]?[0-9]+))?$)");
    std::smatch match;
    if (std::regex_match(input, match, fraction_regex)) {
        big_int numerator(match[1].str(), 10);
        big_int denominator(1);
        if (match[2].matched) {
            denominator = big_int(match[2].str(), 10);
        }
        obj = fraction(numerator, denominator);
    } else {
        throw std::invalid_argument("Invalid fraction format");
    }
    obj.optimise();
    return stream;
}

std::string fraction::to_string() const {
    std::stringstream ss;
    if (_denominator < 0) {
        ss << "-";
    }
    ss << _numerator << "/" << abs(_denominator);
    return ss.str();
}

fraction fraction::sin(fraction const &epsilon) const {
    fraction x = *this;
    fraction result(0, 1);
    fraction term = x;
    int n = 1;
    while (abs(term) > epsilon) {
        result += term;
        term = term * (-x * x);
        term /= fraction(2 * n * (2 * n + 1), 1);
        n++;
    }
    return result;
}

fraction fraction::arcsin(fraction const &epsilon) const {
    fraction x = *this;
    if (x > fraction(1, 1) || x < fraction(-1, 1)) {
        throw std::domain_error("arcsin is undefined for |x| > 1");
    }
    fraction result(0, 1);
    fraction term = x;
    big_int n = 1;
    fraction x_squared = x * x;

    while (abs(term) > epsilon) {
        result += term;
        big_int numerator = (2_bi * n - 1) * (2_bi * n - 1);
        big_int denominator = 2_bi * n * (2_bi * n + 1);
        fraction coeff(numerator, denominator);
        term = term * x_squared * coeff;
        n += 1;
    }
    return result;
}

fraction fraction::cos(fraction const &epsilon) const {
    fraction x = *this;
    fraction result(1, 1);
    fraction term(1, 1);
    int n = 1;
    while (true) {
        term = term * (-x * x);
        big_int denominator = (2 * n - 1) * (2 * n);
        term /= fraction(denominator, 1);
        if (abs(term) <= epsilon) {
            break;
        }
        result += term;
        n++;
    }
    return result;
}

fraction fraction::arccos(fraction const &epsilon) const {
    if (*this > fraction(1, 1) || *this < fraction(-1, 1)) {
        throw std::domain_error("arccos is undefined for |x| > 1");
    }
    fraction half(1, 2);
    fraction pi_over_2 = half.arcsin(epsilon) * fraction{3 , 1};
    fraction arcsin_val = this->arcsin(epsilon);
    return pi_over_2 - arcsin_val;
}

fraction fraction::tg(fraction const &epsilon) const {
    fraction cosine = this->cos(epsilon * epsilon);
    if (cosine._numerator == 0) {
        throw std::domain_error("Tangent undefined");
    }
    return this->sin(epsilon * epsilon) / cosine;
}

fraction fraction::arctg(fraction const &epsilon) const {
    if (_denominator < 0) {
        return -(-*this).arctg(epsilon);
    }
    if (*this > fraction(1, 1)) {
        return fraction(1, 2) - (fraction(1, 1) / *this).arctg(epsilon);
    }
    fraction result(0, 1);
    fraction term = *this;
    int n = 1;
    int count = 0;
    while (abs(term) > epsilon) {
        result += fraction((count % 2 == 0) ? 1 : -1, n) * term;
        n += 2;
        count++;
        term *= *this * *this;
    }
    return result;
}

fraction fraction::ctg(fraction const &epsilon) const {
    fraction sine = this->sin(epsilon * epsilon);
    if (sine._numerator == 0) {
        throw std::domain_error("Cotangent undefined");
    }
    return this->cos(epsilon * epsilon) / sine;
}

fraction fraction::arcctg(fraction const &epsilon) const {
    if (*this == fraction(0, 1)) {
        throw std::domain_error("arcctg is undefined for x = 0");
    }
    fraction half(1, 2);
    fraction pi_over_2 = half.arcsin(epsilon * epsilon) * fraction{3 , 1};
    return pi_over_2 - this->arctg(epsilon);
}

fraction fraction::sec(fraction const &epsilon) const {
    const fraction cosine = this->cos(epsilon);
    if (cosine._numerator == 0) {
        throw std::domain_error("Secant undefined");
    }
    return fraction(1, 1) / cosine;
}

fraction fraction::arcsec(fraction const &epsilon) const {
    fraction x = *this;
    if (x == fraction(0, 1)) {
        throw std::domain_error("arcsec is undefined for x = 0");
    }
    fraction inv_x = fraction(1, 1) / x;
    if (inv_x > fraction(1, 1) || inv_x < -fraction(1, 1)) {
        throw std::domain_error("arcsec is undefined for |x| < 1");
    }
    return inv_x.arccos(epsilon);
}

fraction fraction::cosec(fraction const &epsilon) const {
    fraction sine = this->sin(epsilon);
    if (sine._numerator == 0) {
        throw std::domain_error("Cosecant undefined");
    }
    return fraction(1, 1) / sine;
}

fraction fraction::arccosec(fraction const &epsilon) const {
    fraction x = *this;
    if (x == fraction(0, 1)) {
        throw std::domain_error("arccosec is undefined for x = 0");
    }
    fraction inv_x = fraction(1, 1) / x;
    if (inv_x > fraction(1, 1) || inv_x < -fraction(1, 1)) {
        throw std::domain_error("arccosec is undefined for |x| < 1");
    }
    return inv_x.arcsin(epsilon);
}

fraction fraction::pow(size_t degree) const {
    if (degree == 0) {
        return {1, 1};
    }
    fraction base = *this;
    fraction result(1, 1);
    while (degree > 0) {
        if (degree & 1) {
            result *= base;
        }
        base *= base;
        degree >>= 1;
    }
    return result;
}

// algorithm Newton-Rafson
fraction fraction::root(size_t degree, fraction const &epsilon) const {
    if (degree == 0) {
        throw std::invalid_argument("Degree cannot be zero");
    }
    if (degree == 1) {
        return *this;
    }
    if (_numerator < 0 && degree % 2 == 0) {
        throw std::domain_error("Even root of negative number is not real");
    }
    fraction x = *this;
    fraction guess = *this / fraction(degree, 1);
    fraction prev_guess;
    do {
        prev_guess = guess;
        fraction power = guess.pow(degree - 1);
        if (power._numerator == 0) {
            throw std::runtime_error("Division by zero in root calculation");
        }
        guess = (fraction(degree - 1, 1) * guess + *this / power) / fraction(degree, 1);
    } while ((guess - prev_guess > epsilon) || (prev_guess - guess > epsilon));
    if (_numerator < 0 && degree % 2 == 1) {
        guess = -guess;
    }
    return guess;
}


fraction fraction::ln(fraction const &epsilon) const {
    if (*this <= fraction(0, 1)) {
        throw std::domain_error("Natural logarithm of non-positive number");
    }

    fraction y = (*this - fraction(1, 1)) / (*this + fraction(1, 1));
    fraction y_squared = y * y;
    fraction term = y;
    fraction sum = term;
    int denominator = 1;

    while (true) {
        term *= y_squared;
        denominator += 2;
        fraction delta = term / fraction(denominator, 1);

        if (abs(delta) <= epsilon) {
            break;
        }
        sum += delta;
    }

    return sum * fraction(2, 1);
}

fraction fraction::log2(fraction const &epsilon) const {
    if (*this <= fraction(0, 1)) {
        throw std::domain_error("Logarithm of non-positive number is undefined");
    }
    fraction ln_x = this->ln(epsilon);
    fraction ln_2 = fraction(2, 1).ln(epsilon);
    return ln_x / ln_2;
}

fraction fraction::lg(fraction const &epsilon) const {
    if (*this <= fraction(0, 1)) {
        throw std::domain_error("Base-10 logarithm of non-positive number is undefined");
    }

    fraction ln_x = this->ln(epsilon);
    fraction ln_10 = fraction(10, 1).ln(epsilon);
    return ln_x / ln_10;
}
