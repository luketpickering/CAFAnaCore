#pragma once

#include "CAFAnaCore/ISource.h"
#include "CAFAnaCore/IRecordSink.h"
#include "CAFAnaCore/IValueSource.h"

#include "CAFAnaCore/IDDict.h"
#include "CAFAnaCore/Cut.h"
#include "CAFAnaCore/Var.h"
#include "CAFAnaCore/Weight.h"

namespace ana
{
  class FitMultiverse;

  template<class RecT> class _IRecordEnsembleSource;
  template<class RecT> class _IRecordSource;


  template<class RecT> class _IRecordSourceDefaultImpl : public _ISource<RecordTag<RecT>>
  {
  public:
    using Record_t = RecT;

    virtual ~_IRecordSourceDefaultImpl() = default;

    IValueSource& GetVar(const _Var<RecT>& var);

    IValuePairSource& GetVars(const _Var<RecT>& varx,
                              const _Var<RecT>& vary);

    template<class SpillT> _IRecordSource<RecT>& GetCut(const _Cut<RecT, SpillT>& cut);

    _IRecordSource<RecT>& Weighted(const _Weight<RecT>& wei);

    IValueSource& operator[](const _Var<RecT>& var)
    {
      return GetVar(var);
    }

    template<class SpillT> _IRecordSource<RecT>& operator[](const _Cut<RecT, SpillT>& cut)
    {
      return GetCut(cut);
    }

    _IRecordEnsembleSource<RecT>& Ensemble(const std::vector<_Weight<RecT>>& weis,
                                           const FitMultiverse& multiverse);

  protected:
    IDDict<int, _IRecordSource<RecT>> fCutSources, fWeightSources;

    IDDict<const ana::FitMultiverse*, _IRecordEnsembleSource<RecT>> fEnsembleSources;

    IDDict<int, IValueSource> fVarSources;
    IDDict<int, IValuePairSource> fVarPairSources;
  };


  // By default, a RecordSource is implemented exactly as specified above. But
  // the user may choose to specialize it somehow.
  template<class RecT> class _IRecordSource : public _IRecordSourceDefaultImpl<RecT> {};

  // A source that will never provide anything
  template<class RecT> class NullSource : public _IRecordSource<RecT> {};
}


#include "CAFAnaCore/Cutter.h"
#include "CAFAnaCore/Valuer.h"
#include "CAFAnaCore/PairValuer.h"
#include "CAFAnaCore/Weighter.h"
#include "CAFAnaCore/EnsembleSource.h"

namespace ana
{
  template<class RecT> IValueSource&
  _IRecordSourceDefaultImpl<RecT>::GetVar(const _Var<RecT>& var)
  {
    return fVarSources.template Get<_Valuer<RecT>>(var.ID(), *this, var);
  }

  template<class RecT> IValuePairSource&
  _IRecordSourceDefaultImpl<RecT>::GetVars(const _Var<RecT>& varx,
                                           const _Var<RecT>& vary)
  {
    return fVarPairSources.template Get<_PairValuer<RecT>>((varx*vary).ID(), *this, varx, vary);
  }

  template<class RecT> template<class SpillT> _IRecordSource<RecT>&
  _IRecordSourceDefaultImpl<RecT>::GetCut(const _Cut<RecT, SpillT>& cut)
  {
    return fCutSources.template Get<_Cutter<RecT, SpillT>>(cut.ID(), *this, cut);
  }

  template<class RecT> _IRecordSource<RecT>&
  _IRecordSourceDefaultImpl<RecT>::Weighted(const _Weight<RecT>& wei)
  {
    return fWeightSources.template Get<_Weighter<RecT>>(wei.ID(), *this, wei);
  }

  template<class RecT> _IRecordEnsembleSource<RecT>& _IRecordSourceDefaultImpl<RecT>::
  Ensemble(const std::vector<_Weight<RecT>>& weis, const FitMultiverse& multiverse)
  {
    // TODO don't duplicate info between weis and multiverse
    return fEnsembleSources.template Get<_EnsembleSource<RecT>>(&multiverse, *this, weis, multiverse);
  }

}


// Sadly, we need to do this at some point in the file to generate a valid
// destructor. Otherwise we don't know how to delete fEnsembleSources
#include "CAFAnaCore/IRecordEnsembleSource.h"
