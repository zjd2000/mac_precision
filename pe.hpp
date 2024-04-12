#ifndef _PE_HPP
#define _PE_HPP

#include <assert.h>

class pe_unit {
public:
    int loc_x;
    int loc_y;
    float a;
    float b;
    float m = -1;
    float c_in = -1;
    float c_out;
    float sum;

    pe_unit(int x, int y) : loc_x(x), loc_y(y){};
    virtual void update() = 0;
    virtual const void print_unit(){
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        // ...
    };
};

class yellow_pe :public pe_unit {
public:
    yellow_pe(int x, int y) : pe_unit(x, y){};
    void update(){
        assert(m != -1);
        c_out = (1 - a - b + a * b) + m - m * (1 - a - b + a * b);
        sum = (1 - a - b + a * b) * m + (1 - (1 - a - b + a * b)) * (1 - m);
    };
    const void print_unit() override{
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        std::cout << "TYPE = yellow_pe\n";
        std::cout << "P(a) =\t" << a << std::endl;
        std::cout << "P(b) =\t" << b << std::endl;
        std::cout << "P(m) =\t" << m << std::endl;
        std::cout << "P(c_out) =\t" << c_out << std::endl;
        std::cout << "P(sum) =\t" << sum << std::endl;
    };
};

class blue_pe :public pe_unit {
public:
    blue_pe(int x, int y) : pe_unit(x, y){};
    void update(){
        assert(m != -1);
        c_out = m + (a + b - a * b) - m * (a + b - a * b);
        sum = m * (a + b - a * b) + (1 - m) * (1 - (a + b - a * b));
    };
    const void print_unit() override{
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        std::cout << "TYPE = blue_pe\n";
        std::cout << "P(a) =\t" << a << std::endl;
        std::cout << "P(b) =\t" << b << std::endl;
        std::cout << "P(m) =\t" << m << std::endl;
        std::cout << "P(c_out) =\t" << c_out << std::endl;
        std::cout << "P(sum) =\t" << sum << std::endl;
    };
};

class red_pe :public pe_unit {
public:
    red_pe(int x, int y) : pe_unit(x, y){};
    void update(){
        assert(m != -1 && c_in != -1);
        float d = 1 - a - b + a * b;
        c_out = (c_in + d - c_in * d) * (m + (c_in * d + (1 - c_in) * (1 - d)) - m * (c_in * d + (1 - c_in) * (1 - d)));
        sum = m * (c_in * d + (1 - c_in) * (1 - d)) + (1 - m) * (1 - (c_in * d + (1 - c_in) * (1 - d)));
    };
    const void print_unit() override{
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        std::cout << "TYPE = red_pe\n";
        std::cout << "P(a) =\t" << a << std::endl;
        std::cout << "P(b) =\t" << b << std::endl;
        std::cout << "P(m) =\t" << m << std::endl;
        std::cout << "P(c_in) =\t" << c_in << std::endl;
        std::cout << "P(c_out) =\t" << c_out << std::endl;
        std::cout << "P(sum) =\t" << sum << std::endl;
    };
};

class black_pe :public pe_unit {
public:
    black_pe(int x, int y) : pe_unit(x, y){};
    void update(){
        assert(m != -1 && c_in != -1);
        float d = a + b - a * b;
        c_out = (c_in + d - c_in * d) * (m + (c_in * d + (1 - c_in) * (1 - d)) - m * (c_in * d + (1 - c_in) * (1 - d)));
        sum = m * (c_in * d + (1 - c_in) * (1 - d)) + (1 - m) * (1 - (c_in * d + (1 - c_in) * (1 - d)));
    };
    const void print_unit() override{
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        std::cout << "TYPE = black_pe";
        std::cout << "P(a) =\t" << a << std::endl;
        std::cout << "P(b) =\t" << b << std::endl;
        std::cout << "P(m) =\t" << m << std::endl;
        std::cout << "P(c_in) =\t" << c_in << std::endl;
        std::cout << "P(c_out) =\t" << c_out << std::endl;
        std::cout << "P(sum) =\t" << sum << std::endl;
    };
};

class and_gate :public pe_unit {
public:
    and_gate(int x, int y) : pe_unit(x, y){};
    void update(){
        c_out = a + b - a * b;
    };
    const void print_unit() override{
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        std::cout << "TYPE = AND_gate";
        std::cout << "P(a) =\t" << a << std::endl;
        std::cout << "P(b) =\t" << b << std::endl;
        std::cout << "P(c_out) =\t" << c_out << std::endl;
    };
};

class nand_gate :public pe_unit {
public:
    nand_gate(int x, int y) : pe_unit(x, y){};
    void update(){
        c_out = 1 - (a + b - a * b);
    };
    const void print_unit() override{
        std::cout << "pe[" << loc_x << ", " << loc_y << "]\n";
        std::cout << "--------------------------\n";
        std::cout << "TYPE = NAND_gate";
        std::cout << "P(a) =\t" << a << std::endl;
        std::cout << "P(b) =\t" << b << std::endl;
        std::cout << "P(c_out) =\t" << c_out << std::endl;
    };
};

#endif