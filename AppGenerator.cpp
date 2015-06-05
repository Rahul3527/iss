// main.cpp
// build with:
//g++ AppGenerator.cpp -std=c++11
#include <random>
#include <fstream>
#include <iostream>
#include "Instruction.h"

using namespace std;

class AppGenerator
{
  public:
    AppGenerator(string f)
    {
      filename = f;
    }  
    ~AppGenerator()
    {
    }
    void generate(int32_t block_size, int32_t num_blocks);
    
  private:
    string filename;
};

void AppGenerator::generate(int32_t block_size, int32_t num_blocks)
{
  ofstream outfile;
  outfile.open(filename.c_str(), ios::out|ios::binary); 
  if (outfile.good()) {
    cout << "Is good" << endl;
  }
  else {
    cout << "Is bad" << endl;
  }
  OPCODE op;
  int32_t dst_reg; 
  int32_t src0_reg;
  int32_t src1_reg;
  for (int i = 0; i < num_blocks; i++) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1, OPCODE::JMP+1);
    for (int j = 0; j < block_size; j++) {
      op = (OPCODE)((int32_t)dist(mt) - 1);
      dst_reg = (int32_t)(dist(mt)) % 16;
      src0_reg = (int32_t)(dist(mt)) % 16;
      src1_reg = (int32_t)(dist(mt)) % 16;
      Instruction inst(op, dst_reg, src0_reg, src1_reg); 
      outfile.write((char*)(&inst), sizeof(Instruction));
    }
    if (i % 30 == 0) {
      src0_reg = i - 10; 
      Instruction inst(OPCODE::JMP, 0, src0_reg, 0); 
      outfile.write((char*)(&inst), sizeof(Instruction));
    }
  }
  outfile.close();  
}

int32_t main(int32_t args, char* argv[])
{
  AppGenerator *app_gen = new AppGenerator("t.log");
  app_gen->generate(15, 10000);
}


