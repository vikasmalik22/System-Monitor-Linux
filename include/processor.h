#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
  Processor() : m_total(0), m_active(0) {};
 private:
    float m_total;
    float m_active;
};

#endif