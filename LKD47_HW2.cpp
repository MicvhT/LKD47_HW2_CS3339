#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using namespace std;

void print_usage() {
    cout << "usage:\n"
         << "./fp_overflow_checker loop_bound loop_counter\n\n"
         << "  loop_bound is a positive floating-point value\n"
         << "  loop_counter is a positive floating-point value\n";
}

string grouped_bits(float x) {
    static_assert(sizeof(float) == 4, "Requires IEEE-754 single-precision.");
    uint32_t u;
    memcpy(&u, &x, sizeof(float));
    string s = bitset<32>(u).to_string(); // 32 bits total
    return s.substr(0,1) + " " + s.substr(1,8) + " " + s.substr(9,23);
}

// Find smallest T such that T + inc == T
float overflow_threshold(float inc) {
    const int mantissa_bits = numeric_limits<float>::digits; // 24
    float T = ldexp(inc, mantissa_bits); // inc * 2^24
    for (int i = 0; i < 4 && (T + inc) != T; ++i)
        T = nextafter(T, numeric_limits<float>::infinity());
    return T;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage();
        return 1;
    }

    float loop_bound, increment;
    try {
        loop_bound = stof(argv[1]);
        increment  = stof(argv[2]);
    } catch (...) {
        print_usage();
        return 1;
    }

    if (!(loop_bound > 0.0f && increment > 0.0f)) {
        print_usage();
        return 1;
    }

    cout << "\nLoop bound:  " << grouped_bits(loop_bound) << "\n";
    cout << "Loop counter: " << grouped_bits(increment) << "\n\n";

    float T = overflow_threshold(increment);
    if (loop_bound >= T) {
        cout << "Warning: Possible overflow!\n";
        cout << "Overflow threshold:\n";
        cout << T << "\n";
        cout << grouped_bits(T) << "\n";
    } else {
        cout << "There is no overflow!\n\n";
    }

    return 0;
}
