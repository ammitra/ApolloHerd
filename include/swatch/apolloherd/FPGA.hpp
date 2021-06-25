#ifndef __FPGA_HPP__
#define __FPGA_HPP__

// determine which FPGA the ApolloCMFPGA class represents
// might need to change order depending on which FPGA has which CM ID
enum class FPGA {
  KINTEX = 0,
  VIRTEX = 1
};

#endif  // __FPGA_HPP__