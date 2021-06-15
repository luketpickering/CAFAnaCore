#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Cut.h"

namespace ana
{
  // TODO is this the best name?
  template<class RecT, class SpillT> class _CutApplier: public _IRecordSink<RecT>, public _IRecordSource<RecT>
  {
  public:
    _CutApplier(const _Cut<RecT, SpillT>& cut) : fCut(cut) {}

    virtual ~_CutApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      if(fCut(rec)){
        for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->Fill(rec, weight);
      }
    }

    virtual void HandlePOT(double pot) override
    {
      // TODO make use of the SpillT part of fCut to modify the exposure where
      // necessary.
      for(IValueSink* sink: _IRecordSource<RecT>::fSinks) sink->FillPOT(pot);
    }

    virtual void HandleLivetime(double livetime) override
    {
      for(IValueSink* sink: _IRecordSource<RecT>::fSinks) sink->FillLivetime(livetime);
    }

  protected:
    _Cut<RecT, SpillT> fCut;
  };
}
