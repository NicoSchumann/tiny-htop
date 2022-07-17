#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  ///
  /// Total CPU aggregation. With consideration of all CPU cores.
  /// RETURN: Range from (0.0f to 1.0f].
  ///
  float Utilization();

  ///
  /// Delta of total jiffies spended by CPU.
  ///
  long JiffiesDelta();

  ///
  ///
  ///
  long JiffiesActiveDelta();

  ///
  ///
  ///
  long JiffiesIdleDelta();

  ///
  ///
  ///
  void Update();

  ///
  ///
  ///
  Processor();

 private:
  struct Jiffies {
    long active{};  /// Time spended for computation.
    long idle{};    /// Time spended for twiddling thumbs of the CPU.
  };

  Jiffies jiffiesOld_;
  Jiffies jiffiesNew_;
};

#endif