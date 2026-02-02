#pragma once

#include "CAFAnaCore/IExposureSink.h"

#include "CAFAnaCore/Tags.h"

namespace ana
{
  template<class RecT> class SinkInterface<RecordTag<RecT>>: public IExposureSink
  {
  public:
    virtual void HandleRecord(const RecT* rec, double weight) = 0;
  };

  template<class RecT> using _IRecordSink = _ISink<RecordTag<RecT>>;
}
