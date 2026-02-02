#pragma once

#include "CAFAnaCore/PassthroughExposure.h"

#include "CAFAnaCore/IRecordEnsembleSink.h"
#include "CAFAnaCore/IRecordEnsembleSource.h"

namespace ana
{
  /// Implementation detail for various source/sink classes
  template<class RecT> class PassthroughEnsemble: public PassthroughExposure<_IRecordEnsembleSource<RecT>, _IRecordEnsembleSink<RecT>>
  {
  public:
    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      for(_IRecordEnsembleSink<RecT>* sink: _IRecordEnsembleSource<RecT>::fSinks) sink->HandleSingleRecord(rec, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& weights) override
    {
      for(_IRecordEnsembleSink<RecT>* sink: _IRecordEnsembleSource<RecT>::fSinks) sink->HandleEnsemble(rec, weights);
    }

    virtual const FitMultiverse* GetMultiverse() const override
    {
      return fSource->GetMultiverse();
    }

  protected:
    PassthroughEnsemble(const _IEnsembleSource<RecT>& src)
      : fSource(&src)
    {
    }

    const _IEnsembleSource<RecT>* fSource;
  };
}
