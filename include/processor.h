#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>

class Processor {
 public:
  ///
  /// Total CPU aggregation. With consideration of all CPU cores.
  /// RETURN: Range from (0.0f to 1.0f].
  ///
  float Utilization();

  Processor();

 private:
  struct Jiffies {
    long active{};  /// Time spended for computation.
    long idle{};    /// Time spended for twiddling thumbs of the CPU.
  };

  /// This array holds old and new jiffy objects, for calculating the delta.
  std::array<Jiffies, 2> jiffies_;
};

#endif