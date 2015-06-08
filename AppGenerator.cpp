// main.cpp
// build with:
//g++ AppGenerator.cpp -std=c++11 -o appgen
#include <random>
#include <fstream>
#include <iostream>
#include "Inst.h"

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
    for (int j = 0; j < block_size-1; j++) {
      op = (OPCODE)((int32_t)dist(mt) - 1);
      dst_reg = (int32_t)(dist(mt)) % 32;
      src0_reg = (int32_t)(dist(mt)) % 32;
      src1_reg = (int32_t)(dist(mt)) % 32;
      CInst inst(op, dst_reg, src0_reg, src1_reg); 
      outfile.write((char*)(&inst), sizeof(CInst));
    }
    src0_reg = i - 10; 
    CInst inst(OPCODE::JMP, 0, src0_reg, 0); 
    outfile.write((char*)(&inst), sizeof(CInst));
  }
  outfile.close();  
}

int32_t main(int32_t args, char* argv[])
{
  AppGenerator *app_gen = new AppGenerator("t.bin");
  int32_t block_count = 200;
  int32_t inst_per_block = 10;
  if(args > 2) {
    block_count = atoi(argv[1]);
    inst_per_block = atoi(argv[2]);
  }
  else if (args > 1){
    block_count = atoi(argv[1]);
  }

  app_gen->generate(inst_per_block, block_count);
}


