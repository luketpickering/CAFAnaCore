#pragma once

#include "CAFAnaCore/ISink.h"
#include "CAFAnaCore/IExposureSink.h"

namespace ana
{
  template<> class SinkInterface<double>: public IExposureSink
  {
  public:
    virtual void Fill(double x, double weight) = 0;
  };

  typedef _ISink<double> IValueSink;
}
