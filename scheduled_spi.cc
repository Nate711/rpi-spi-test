#include <gflags/gflags.h>
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>

/*
    # BUILD
    mkdir build
    cd build
    cmake ..
    make

    # RUN
    sudo ./spi-pigpio-speed [--help]
*/

DEFINE_int32(loops, 10000, "number of messages to send");
DEFINE_int32(speed, 1000000, "SPI bus frequency in hz");
DEFINE_int32(bytes, 16, "number of bytes to send per transfer");
DEFINE_int32(period_ns, 1000000, "number of nanoseconds between loops");

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  int loops = FLAGS_loops;
  int speed = FLAGS_speed;
  int bytes = FLAGS_bytes;
  int period_ns = FLAGS_period_ns;
  int i;
  int h;
  double start, diff, sps;
  char buf[16384];
  char buf_in[16384];

  // Initialize the out buffer with some data
  for (int i = 0; i < bytes; i++) {
    buf[i] = static_cast<uint8_t>(i);
  }

  if (gpioInitialise() < 0) {
    return 1;
  }

  uint32_t spi_flags = 1 + 2;
  h = spiOpen(0, speed, spi_flags);

  if (h < 0) {
    return 2;
  }
  start = time_time();

  double average_exchange_duration = 0.0;
  for (i = 0; i < loops; i++) {
    if (i % 100 == 0) {
      printf("count: %d\n", i);
    }
    auto now = std::chrono::high_resolution_clock::now();
    spiXfer(h, buf, buf_in, bytes);
    auto transfer_end = std::chrono::high_resolution_clock::now();
    average_exchange_duration +=
        std::chrono::duration_cast<std::chrono::nanoseconds>(transfer_end - now)
            .count() /
        static_cast<double>(loops);

    std::this_thread::sleep_until(now + std::chrono::nanoseconds(period_ns));
  }

  diff = time_time() - start;

  printf("sps=%.1f: %d bytes @ %d bps (loops=%d time=%.1f)\n",
         (double)loops / diff, bytes, speed, loops, diff);
  printf("average exchange duration (us): %f\n",
         average_exchange_duration / 1000.0);

  spiClose(h);
  gpioTerminate();
  return 0;
}
