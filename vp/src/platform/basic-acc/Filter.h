#ifndef FILTER_H_
#define FILTER_H_
#include <cmath>
#include <iomanip>
#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def2.h"

class Filter : public sc_module {
public:
  tlm_utils::simple_target_socket<Filter> tsock;

  sc_fifo<bool> i_next_row;
  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<unsigned char> o_r;
  sc_fifo<unsigned char> o_g;
  sc_fifo<unsigned char> o_b;

  SC_HAS_PROCESS(Filter);

  Filter(sc_module_name n) 
      : sc_module(n), tsock("tsock"), base_offset(0) {

    SC_THREAD(do_filter);
    tsock.register_b_transport(this, &Filter::blocking_transport);
  }

  ~Filter() = default;

private:
  
  unsigned char val_r[MASK_N];
  unsigned char val_g[MASK_N];
  unsigned char val_b[MASK_N];
  unsigned char in_buf_r[MASK_Y][MASK_X];
  unsigned char in_buf_g[MASK_Y][MASK_X];
  unsigned char in_buf_b[MASK_Y][MASK_X];
  unsigned char median_buf_r[MASK_Y][MASK_X];
  unsigned char median_buf_g[MASK_Y][MASK_X];
  unsigned char median_buf_b[MASK_Y][MASK_X];
  float mean_r;
  float mean_g;
  float mean_b;

  unsigned int base_offset;

  void do_filter() {
    while (true) {

      if (i_next_row.read()) {

        int l = 0; // median buffer x index
        int m = 0; // midian buffer y index

        // row 1
        int i = 0;
        for (int w = 0; w < MASK_Y; ++w) {
          for (int v = 0; v < MASK_X; ++v) {
            in_buf_r[w][v] = i_r.read();
            in_buf_g[w][v] = i_g.read();
            in_buf_b[w][v] = i_b.read();
            val_r[3 * w + v] = in_buf_r[w][v];
            val_g[3 * w + v] = in_buf_g[w][v];
            val_b[3 * w + v] = in_buf_b[w][v];
            wait(1 * CLOCK_PERIOD, SC_NS);
          }
        }
        std::sort(val_r, val_r + 9);
        std::sort(val_g, val_g + 9);
        std::sort(val_b, val_b + 9);

        median_buf_r[m][l] = val_r[4];
        median_buf_g[m][l] = val_g[4];
        median_buf_b[m][l] = val_b[4];
        
        i = i + 1;
        l++;

        while (i < 3) {
          for (int w = 0; w < MASK_Y; ++w) {
            for (int v = 0; v < MASK_X; ++v) {
              if (v == MASK_X - 1) {
                in_buf_r[w][v] = i_r.read();
                in_buf_g[w][v] = i_g.read();
                in_buf_b[w][v] = i_b.read();
                wait(1 * CLOCK_PERIOD, SC_NS);
              } else {
                in_buf_r[w][v] = in_buf_r[w][v + 1];
                in_buf_g[w][v] = in_buf_g[w][v + 1];
                in_buf_b[w][v] = in_buf_b[w][v + 1];
              }
              val_r[3 * w + v] = in_buf_r[w][v];
              val_g[3 * w + v] = in_buf_g[w][v];
              val_b[3 * w + v] = in_buf_b[w][v];
            }
          }
          std::sort(val_r, val_r + 9);
          std::sort(val_g, val_g + 9);
          std::sort(val_b, val_b + 9);

          median_buf_r[m][l] = val_r[4];
          median_buf_g[m][l] = val_g[4];
          median_buf_b[m][l] = val_b[4];

          i = i + 1;
          l++;
        }

        m++;
        l--;
        // row 2
        for (int w = 0; w < MASK_Y; ++w) {
          for (int v = 0; v < MASK_X; ++v) {
              if (w == MASK_Y - 1) {
                in_buf_r[w][v] = i_r.read();
                in_buf_g[w][v] = i_g.read();
                in_buf_b[w][v] = i_b.read();
                wait(1 * CLOCK_PERIOD, SC_NS);
              } else {
                in_buf_r[w][v] = in_buf_r[w + 1][v];
                in_buf_g[w][v] = in_buf_g[w + 1][v];
                in_buf_b[w][v] = in_buf_b[w + 1][v];
              }
              val_r[3 * w + v] = in_buf_r[w][v];
              val_g[3 * w + v] = in_buf_g[w][v];
              val_b[3 * w + v] = in_buf_b[w][v];
          }
        }
        std::sort(val_r, val_r + 9);
        std::sort(val_g, val_g + 9);
        std::sort(val_b, val_b + 9);

        median_buf_r[m][l] = val_r[4];
        median_buf_g[m][l] = val_g[4];
        median_buf_b[m][l] = val_b[4];

        i = i + 1; // i = 4
        l--;

        while (i < 6) {
          for (int w = 0; w < MASK_Y; ++w) {
            for (int v = MASK_X - 1; v >= 0; --v) {
              if (v == 0) {
                in_buf_r[w][v] = i_r.read();
                in_buf_g[w][v] = i_g.read();
                in_buf_b[w][v] = i_b.read();
                wait(1 * CLOCK_PERIOD, SC_NS);
              } else {
                in_buf_r[w][v] = in_buf_r[w][v - 1];
                in_buf_g[w][v] = in_buf_g[w][v - 1];
                in_buf_b[w][v] = in_buf_b[w][v - 1];
              }
              
              val_r[3 * w + v] = in_buf_r[w][v];
              val_g[3 * w + v] = in_buf_g[w][v];
              val_b[3 * w + v] = in_buf_b[w][v];
                    
            }
          }
          std::sort(val_r, val_r + 9);
          std::sort(val_g, val_g + 9);
          std::sort(val_b, val_b + 9);

          median_buf_r[m][l] = val_r[4];
          median_buf_g[m][l] = val_g[4];
          median_buf_b[m][l] = val_b[4];

          i = i + 1;
          l--;
        }

        m++;
        l++;

        // row 3
        for (int w = 0; w < MASK_Y; ++w) {
          for (int v = 0; v < MASK_X; ++v) {
              if (w == MASK_Y - 1) {
                in_buf_r[w][v] = i_r.read();
                in_buf_g[w][v] = i_g.read();
                in_buf_b[w][v] = i_b.read();
                wait(1 * CLOCK_PERIOD, SC_NS);
              } else {
                in_buf_r[w][v] = in_buf_r[w + 1][v];
                in_buf_g[w][v] = in_buf_g[w + 1][v];
                in_buf_b[w][v] = in_buf_b[w + 1][v];
              }
              val_r[3 * w + v] = in_buf_r[w][v];
              val_g[3 * w + v] = in_buf_g[w][v];
              val_b[3 * w + v] = in_buf_b[w][v];
          }
        }
        std::sort(val_r, val_r + 9);
        std::sort(val_g, val_g + 9);
        std::sort(val_b, val_b + 9);

        median_buf_r[m][l] = val_r[4];
        median_buf_g[m][l] = val_g[4];
        median_buf_b[m][l] = val_b[4];

        i = i + 1; // i = 7
        l++;

        while (i < 9) {
          for (int w = 0; w < MASK_Y; ++w) {
            for (int v = 0; v < MASK_X; ++v) {
              if (v == MASK_X - 1) {
                in_buf_r[w][v] = i_r.read();
                in_buf_g[w][v] = i_g.read();
                in_buf_b[w][v] = i_b.read();
                wait(1 * CLOCK_PERIOD, SC_NS);
              } else {
                in_buf_r[w][v] = in_buf_r[w][v + 1];
                in_buf_g[w][v] = in_buf_g[w][v + 1];
                in_buf_b[w][v] = in_buf_b[w][v + 1];
              }
              val_r[3 * w + v] = in_buf_r[w][v];
              val_g[3 * w + v] = in_buf_g[w][v];
              val_b[3 * w + v] = in_buf_b[w][v];
            }
          }
          std::sort(val_r, val_r + 9);
          std::sort(val_g, val_g + 9);
          std::sort(val_b, val_b + 9);

          median_buf_r[m][l] = val_r[4];
          median_buf_g[m][l] = val_g[4];
          median_buf_b[m][l] = val_b[4];
          i = i + 1;
          l++;
        }
      } else {
        for (int w = 0; w < MASK_Y; ++w) {
          for (int v = 0; v < MASK_X - 1; ++v) {
            median_buf_r[w][v] = median_buf_r[w][v + 1];
            median_buf_g[w][v] = median_buf_g[w][v + 1];
            median_buf_b[w][v] = median_buf_b[w][v + 1];
          }
        }
        int l = MASK_X - 1; // median buffer x index
        int m = 0; // midian buffer y index

        for (m = 0; m < MASK_Y; ++m) {
          if (m == 0) {
            for (int w = 0; w < MASK_Y; ++w) {
              for (int v = 0; v < MASK_X; ++v) {
                in_buf_r[w][v] = i_r.read();
                in_buf_g[w][v] = i_g.read();
                in_buf_b[w][v] = i_b.read();
                wait(1 * CLOCK_PERIOD, SC_NS);
                val_r[3 * w + v] = in_buf_r[w][v];
                val_g[3 * w + v] = in_buf_g[w][v];
                val_b[3 * w + v] = in_buf_b[w][v];
              }
            }
          } else {
            for (int w = 0; w < MASK_Y; ++w) {
              for (int v = 0; v < MASK_X; ++v) {
                  if (w == MASK_Y - 1) {
                    in_buf_r[w][v] = i_r.read();
                    in_buf_g[w][v] = i_g.read();
                    in_buf_b[w][v] = i_b.read();
                    wait(1 * CLOCK_PERIOD, SC_NS);
                  } else {
                    in_buf_r[w][v] = in_buf_r[w + 1][v];
                    in_buf_g[w][v] = in_buf_g[w + 1][v];
                    in_buf_b[w][v] = in_buf_b[w + 1][v];
                  }
                  val_r[3 * w + v] = in_buf_r[w][v];
                  val_g[3 * w + v] = in_buf_g[w][v];
                  val_b[3 * w + v] = in_buf_b[w][v];
              }
            }
          }
          std::sort(val_r, val_r + 9);
          std::sort(val_g, val_g + 9);
          std::sort(val_b, val_b + 9);

          median_buf_r[m][l] = val_r[4];
          median_buf_g[m][l] = val_g[4];
          median_buf_b[m][l] = val_b[4];
        }
      }

      mean_r = 0;
      mean_g = 0;
      mean_b = 0;
      for (int w = 0; w < MASK_Y; ++w) {
        for (int v = 0; v < MASK_X; ++v) {
          mean_r = mean_r + (median_buf_r[w][v] * mean_mask[w][v]);
          mean_g = mean_g + (median_buf_g[w][v] * mean_mask[w][v]);
          mean_b = mean_b + (median_buf_b[w][v] * mean_mask[w][v]);
        }
      }

      o_r.write(floor(mean_r));
      o_g.write(floor(mean_g));
      o_b.write(floor(mean_b));
    }
    wait(1 * CLOCK_PERIOD, SC_NS); 
  }

  void blocking_transport(tlm::tlm_generic_payload &payload,
                                    sc_core::sc_time &delay) {
    sc_dt::uint64 addr = payload.get_address();
    addr -= base_offset;
    unsigned char *data_ptr = payload.get_data_ptr();
    word buffer;
    switch (payload.get_command()) {
    case tlm::TLM_READ_COMMAND:
      switch (addr) {
      case FILTER_RESULT_ADDR:
        buffer.uc[0] = o_r.read();
        buffer.uc[1] = o_g.read();
        buffer.uc[2] = o_b.read();
        break;
      case FILTER_CHECK_ADDR:
        buffer.uint = o_r.num_available();
      break;
      default:
        std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                  << std::setfill('0') << std::setw(8) << std::hex << addr
                  << std::dec << " is not valid" << std::endl;
      }
      data_ptr[0] = buffer.uc[0];
      data_ptr[1] = buffer.uc[1];
      data_ptr[2] = buffer.uc[2];
      data_ptr[3] = buffer.uc[3];
      break;
    case tlm::TLM_WRITE_COMMAND:
      switch (addr) {
      case FILTER_R_ADDR:
        if (data_ptr[0] == 1) {
          i_next_row.write(true);
        } else if (data_ptr[0] == 0){
          i_next_row.write(false);
        }
        i_r.write(data_ptr[1]);
        i_g.write(data_ptr[2]);
        i_b.write(data_ptr[3]);
        break;
      default:
        std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                  << std::setfill('0') << std::setw(8) << std::hex << addr
                  << std::dec << " is not valid" << std::endl;
      }
      break;
    case tlm::TLM_IGNORE_COMMAND:
      payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
      return;
    default:
      payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
      return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
