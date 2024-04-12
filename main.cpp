#include <iostream>
#include <vector>
#include "pe.hpp"

#define SIZE_A 8
#define SIZE_B 12

class mac {
private:
    const int pe_layout[12][8] = {
        4, 4, 4, 4, 4, 4, 4, 5,
        1, 1, 1, 1, 1, 1, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 2,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        3, 3, 3, 3, 3, 3, 3, 0,
        2, 2, 2, 2, 2, 2, 2, 1};
    std::vector<std::vector<pe_unit*> > pe;
    std::vector<float> result;

    std::vector<float> _8bit_adder(std::vector<float> &a_in, std::vector<float> &b_in);
public:
    mac();
    pe_unit* at(const int &x, const int &y);
    void compute(std::vector<float> &a_in, std::vector<float> &b_in);
    const void print_result();
    const void print_unit(const int &x, const int &y);
    ~mac(){};
};

mac::mac() {
    for (int i = 0 ; i < SIZE_B; i++) {
        std::vector<pe_unit*> tmp;
        for (int j = 0 ; j < SIZE_A; j++) {
            pe_unit *p;
            switch (pe_layout[i][j]) {
                case 0: p = new yellow_pe(i, j); break;
                case 1: p = new blue_pe(i, j); break;
                case 2: p = new red_pe(i, j); break;
                case 3: p = new black_pe(i, j); break;
                case 4: p = new and_gate(i, j); break;
                case 5: p = new nand_gate(i, j); break;
                default:
                    throw "Empty PE Layout Exists!";
                    break;
            }
            tmp.emplace_back(p);
        }
        pe.emplace_back(tmp);
    }

    result.reserve(20);
}

std::vector<float> mac::_8bit_adder(std::vector<float> &a_in, std::vector<float> &b_in) {
    int len_a = a_in.size();
    int len_b = b_in.size();
    assert(len_a ==len_b);
    std::vector<float> c_in(8+1, 1);
    std::vector<float> sum(8, 1);
    for (int i = 0; i < 8; i++) {
        float a = a_in[i];
        float b = b_in[i];
        float c = c_in[i];
        c_in[i + 1] = (a + b - a * b) * (c + (a * b + (1 - a) * (1 - b)) - c * (c + (a * b + (1 - a) * (1 - b))));
        sum[i] = c * (a * b + (1 - a) * (1 - b)) + (1 - c) * (1- (a * b + (1 - a) * (1 - b)));
    }

    return sum;
}

pe_unit*  mac::at(const int &x, const int &y) {
    if (y >= SIZE_A || x >= SIZE_B) {
        throw "Out of bound!";
    }
    return pe[x][y];
}

void mac::compute(std::vector<float> &a_in, std::vector<float> &b_in) {
    for (int i = 0; i < SIZE_B; i++) {
        for (int j = 0; j < SIZE_A; j++) {
            pe[i][j]->a = a_in[j];
            pe[i][j]->b = b_in[i];
        }
    }

    //layer 0
    for (int j = 0; j < SIZE_A; j++) {
        pe[0][j]->update();
    }
    
    //layer 1
    pe[1][0]->m = pe[0][1]->c_out;
    pe[1][1]->m = pe[0][2]->c_out;
    pe[1][2]->m = pe[0][3]->c_out;
    pe[1][3]->m = pe[0][4]->c_out;
    pe[1][4]->m = pe[0][5]->c_out;
    pe[1][5]->m = pe[0][6]->c_out;
    pe[1][6]->m = pe[0][7]->c_out;
    pe[1][6]->c_in = 0;
    pe[1][7]->m = 1;
    for (int j = 0; j < SIZE_A; j++) {
        pe[1][j]->update();
    }

    //layer 2
    pe[2][0]->m = pe[1][1]->sum;
    pe[2][1]->m = pe[1][2]->sum;
    pe[2][2]->m = pe[1][3]->sum;
    pe[2][3]->m = pe[1][4]->sum;
    pe[2][4]->m = pe[1][5]->sum;
    pe[2][5]->m = pe[1][6]->sum;
    pe[2][6]->m = pe[1][7]->sum;
    pe[2][7]->m = pe[1][7]->c_out;
    pe[2][0]->c_in = pe[1][0]->sum;
    pe[2][1]->c_in = pe[1][1]->sum;
    pe[2][2]->c_in = pe[1][2]->sum;
    pe[2][3]->c_in = pe[1][3]->sum;
    pe[2][4]->c_in = pe[1][4]->sum;
    pe[2][5]->c_in = pe[1][5]->sum;
    pe[2][6]->c_in = pe[1][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[2][j]->update();
    }

    //layer 3
    pe[3][0]->m = pe[2][1]->sum;
    pe[3][1]->m = pe[2][2]->sum;
    pe[3][2]->m = pe[2][3]->sum;
    pe[3][3]->m = pe[2][4]->sum;
    pe[3][4]->m = pe[2][5]->sum;
    pe[3][5]->m = pe[2][6]->sum;
    pe[3][6]->m = pe[2][7]->sum;
    pe[3][7]->m = pe[2][7]->c_out;
    pe[3][0]->c_in = pe[2][0]->sum;
    pe[3][1]->c_in = pe[2][1]->sum;
    pe[3][2]->c_in = pe[2][2]->sum;
    pe[3][3]->c_in = pe[2][3]->sum;
    pe[3][4]->c_in = pe[2][4]->sum;
    pe[3][5]->c_in = pe[2][5]->sum;
    pe[3][6]->c_in = pe[2][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[3][j]->update();
    }

    //layer 4
    pe[4][0]->m = pe[3][1]->sum;
    pe[4][1]->m = pe[3][2]->sum;
    pe[4][2]->m = pe[3][3]->sum;
    pe[4][3]->m = pe[3][4]->sum;
    pe[4][4]->m = pe[3][5]->sum;
    pe[4][5]->m = pe[3][6]->sum;
    pe[4][6]->m = pe[3][7]->sum;
    pe[4][7]->m = pe[3][7]->c_out;
    pe[4][0]->c_in = pe[3][0]->sum;
    pe[4][1]->c_in = pe[3][1]->sum;
    pe[4][2]->c_in = pe[3][2]->sum;
    pe[4][3]->c_in = pe[3][3]->sum;
    pe[4][4]->c_in = pe[3][4]->sum;
    pe[4][5]->c_in = pe[3][5]->sum;
    pe[4][6]->c_in = pe[3][6]->sum;
    pe[4][7]->c_in = 0;
    for (int j = 0; j < SIZE_A; j++) {
        pe[4][j]->update();
    }

    //layer 5
    pe[5][0]->m = pe[4][1]->sum;
    pe[5][1]->m = pe[4][2]->sum;
    pe[5][2]->m = pe[4][3]->sum;
    pe[5][3]->m = pe[4][4]->sum;
    pe[5][4]->m = pe[4][5]->sum;
    pe[5][5]->m = pe[4][6]->sum;
    pe[5][6]->m = pe[4][7]->sum;
    pe[5][7]->m = pe[4][7]->c_out;
    pe[5][0]->c_in = pe[4][0]->sum;
    pe[5][1]->c_in = pe[4][1]->sum;
    pe[5][2]->c_in = pe[4][2]->sum;
    pe[5][3]->c_in = pe[4][3]->sum;
    pe[5][4]->c_in = pe[4][4]->sum;
    pe[5][5]->c_in = pe[4][5]->sum;
    pe[5][6]->c_in = pe[4][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[5][j]->update();
    }

    //layer 6
    pe[6][0]->m = pe[5][1]->sum;
    pe[6][1]->m = pe[5][2]->sum;
    pe[6][2]->m = pe[5][3]->sum;
    pe[6][3]->m = pe[5][4]->sum;
    pe[6][4]->m = pe[5][5]->sum;
    pe[6][5]->m = pe[5][6]->sum;
    pe[6][6]->m = pe[5][7]->sum;
    pe[6][7]->m = pe[5][7]->c_out;
    pe[6][0]->c_in = pe[5][0]->sum;
    pe[6][1]->c_in = pe[5][1]->sum;
    pe[6][2]->c_in = pe[5][2]->sum;
    pe[6][3]->c_in = pe[5][3]->sum;
    pe[6][4]->c_in = pe[5][4]->sum;
    pe[6][5]->c_in = pe[5][5]->sum;
    pe[6][6]->c_in = pe[5][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[6][j]->update();
    }

    //layer 7
    pe[7][0]->m = pe[6][1]->sum;
    pe[7][1]->m = pe[6][2]->sum;
    pe[7][2]->m = pe[6][3]->sum;
    pe[7][3]->m = pe[6][4]->sum;
    pe[7][4]->m = pe[6][5]->sum;
    pe[7][5]->m = pe[6][6]->sum;
    pe[7][6]->m = pe[6][7]->sum;
    pe[7][7]->m = pe[6][7]->c_out;
    pe[7][0]->c_in = pe[6][0]->sum;
    pe[7][1]->c_in = pe[6][1]->sum;
    pe[7][2]->c_in = pe[6][2]->sum;
    pe[7][3]->c_in = pe[6][3]->sum;
    pe[7][4]->c_in = pe[6][4]->sum;
    pe[7][5]->c_in = pe[6][5]->sum;
    pe[7][6]->c_in = pe[6][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[7][j]->update();
    }

    //layer 8
    pe[8][0]->m = pe[7][1]->sum;
    pe[8][1]->m = pe[7][2]->sum;
    pe[8][2]->m = pe[7][3]->sum;
    pe[8][3]->m = pe[7][4]->sum;
    pe[8][4]->m = pe[7][5]->sum;
    pe[8][5]->m = pe[7][6]->sum;
    pe[8][6]->m = pe[7][7]->sum;
    pe[8][7]->m = pe[7][7]->c_out;
    pe[8][0]->c_in = pe[7][0]->sum;
    pe[8][1]->c_in = pe[7][1]->sum;
    pe[8][2]->c_in = pe[7][2]->sum;
    pe[8][3]->c_in = pe[7][3]->sum;
    pe[8][4]->c_in = pe[7][4]->sum;
    pe[8][5]->c_in = pe[7][5]->sum;
    pe[8][6]->c_in = pe[7][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[8][j]->update();
    }

    //layer 9
    pe[9][0]->m = pe[8][1]->sum;
    pe[9][1]->m = pe[8][2]->sum;
    pe[9][2]->m = pe[8][3]->sum;
    pe[9][3]->m = pe[8][4]->sum;
    pe[9][4]->m = pe[8][5]->sum;
    pe[9][5]->m = pe[8][6]->sum;
    pe[9][6]->m = pe[8][7]->sum;
    pe[9][7]->m = pe[8][7]->c_out;
    pe[9][0]->c_in = pe[8][0]->sum;
    pe[9][1]->c_in = pe[8][1]->sum;
    pe[9][2]->c_in = pe[8][2]->sum;
    pe[9][3]->c_in = pe[8][3]->sum;
    pe[9][4]->c_in = pe[8][4]->sum;
    pe[9][5]->c_in = pe[8][5]->sum;
    pe[9][6]->c_in = pe[8][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[9][j]->update();
    }

    //layer 10
    pe[10][0]->m = pe[9][1]->sum;
    pe[10][1]->m = pe[9][2]->sum;
    pe[10][2]->m = pe[9][3]->sum;
    pe[10][3]->m = pe[9][4]->sum;
    pe[10][4]->m = pe[9][5]->sum;
    pe[10][5]->m = pe[9][6]->sum;
    pe[10][6]->m = pe[9][7]->sum;
    pe[10][7]->m = pe[9][7]->c_out;
    pe[10][0]->c_in = pe[9][0]->sum;
    pe[10][1]->c_in = pe[9][1]->sum;
    pe[10][2]->c_in = pe[9][2]->sum;
    pe[10][3]->c_in = pe[9][3]->sum;
    pe[10][4]->c_in = pe[9][4]->sum;
    pe[10][5]->c_in = pe[9][5]->sum;
    pe[10][6]->c_in = pe[9][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[10][j]->update();
    }

    //layer 11
    pe[11][0]->m = pe[10][1]->sum;
    pe[11][1]->m = pe[10][2]->sum;
    pe[11][2]->m = pe[10][3]->sum;
    pe[11][3]->m = pe[10][4]->sum;
    pe[11][4]->m = pe[10][5]->sum;
    pe[11][5]->m = pe[10][6]->sum;
    pe[11][6]->m = pe[10][7]->sum;
    pe[11][7]->m = pe[10][7]->c_out;
    pe[11][0]->c_in = pe[10][0]->sum;
    pe[11][1]->c_in = pe[10][1]->sum;
    pe[11][2]->c_in = pe[10][2]->sum;
    pe[11][3]->c_in = pe[10][3]->sum;
    pe[11][4]->c_in = pe[10][4]->sum;
    pe[11][5]->c_in = pe[10][5]->sum;
    pe[11][6]->c_in = pe[10][6]->sum;
    for (int j = 0; j < SIZE_A; j++) {
        pe[11][j]->update();
    }

    // adder
    std::vector<float> adder_a_in(8);
    std::vector<float> adder_b_in(8);
    for (int i = 0; i < 8; i++) {
        adder_a_in[i] = pe[11][i]->c_out;
    }
    for (int i = 0; i < 7; i++) {
        adder_b_in[i] = pe[11][i+1]->sum;
    }
    adder_b_in[7] = 0;
    std::vector<float> adder_out;
    adder_out = _8bit_adder(adder_a_in, adder_b_in);

    // result
    for (int i = 0; i < SIZE_B; i++) {
        result.emplace_back(pe[i][0]->sum);
    }
    for (int k = 0; k < 8; k++) {
        result.emplace_back(adder_out[k]);
    }
}

const void mac::print_result(){
    std::cout.precision(4);
    int i = 0;
    for (auto it = result.begin(); it != result.end(); it++) {
        std::cout << "P[" << i++ << "] =\t" << *it << "\n" ;
    }
}

const void mac::print_unit(const int &x, const int &y) {
    if (y >= SIZE_A || x >= SIZE_B) {
        throw "Out of bound!";
    }
    pe[x][y]->print_unit();
}

int main() {
    mac mymac;
    std::vector<float> A = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    std::vector<float> B = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0.5};
    mymac.compute(A, B);
    mymac.print_result();   //print result
    mymac.print_unit(6,6); // print pe[6][6]
    mymac.print_unit(6,8); // out of bound
    return 0;
}
