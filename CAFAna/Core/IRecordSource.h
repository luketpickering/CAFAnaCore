#pragma once

#include "CAFAna/Core/ISource.h"
#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IValueSource.h"
#include "CAFAna/Core/Cut.h"
#include "CAFAna/Core/Var.h"
#include "CAFAna/Core/Weight.h"

#include <numeric>
#include <unordered_map>

namespace ana{class FitMultiverse;} // TODO TODO import fully

namespace ana::beta
{
  /// Helper for implementation of _IRecordSourceDefaultImpl
  template<class KeyT, class ElemT> class IDDict
  {
  public:
    /// Return the element with ID 'id' or construct a new element of type
    /// ConsT with arguments cons_args
    template<class ConsT, class... Ts> ElemT& Get(KeyT key, Ts&&... cons_args)
    {
      auto it = fElems.find(key);

      if(it != fElems.end()) return *it->second;

      return *fElems.emplace(key, new ConsT(cons_args...)).first->second;
    }

  protected:
    std::unordered_map<KeyT, std::unique_ptr<ElemT>> fElems;
  };


  template<class RecT> class _IRecordEnsembleSource;
  template<class RecT> class _IRecordSource;


  template<class RecT> class _IRecordEnsembleSourceDefaultImpl : public _IEnsembleSource<RecT>
  {
  public:
    using Record_t = RecT;

    virtual ~_IRecordEnsembleSourceDefaultImpl() {}

    IValueEnsembleSource& GetVar(const _Var<RecT>& var);

    IValuePairEnsembleSource& GetVars(const _Var<RecT>& varx,
                                      const _Var<RecT>& vary);


    template<class SpillT> _IRecordEnsembleSource<RecT>& GetCut(const _Cut<RecT, SpillT>& cut);

    _IRecordEnsembleSource<RecT>& Weighted(const _Weight<RecT>& wei);

    IValueEnsembleSource& operator[](const _Var<RecT>& var)
    {
      return GetVar(var);
    }

    template<class SpillT> _IRecordEnsembleSource<RecT>& operator[](const _Cut<RecT, SpillT>& cut)
    {
      return GetCut(cut);
    }

  protected:
    IDDict<int, _IRecordEnsembleSource<RecT>> fCutSources, fWeightSources;

    IDDict<int, IValueEnsembleSource> fVarSources;
    IDDict<int, IValuePairEnsembleSource> fVarPairSources;
  };


  template<class RecT> class _IRecordSourceDefaultImpl : public _ISource<RecordTag<RecT>>
  {
  public:
    using Record_t = RecT;

    virtual ~_IRecordSourceDefaultImpl() {}

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
  template<class RecT> class _IRecordEnsembleSource : public _IRecordEnsembleSourceDefaultImpl<RecT> {};
  template<class RecT> class _IRecordSource : public _IRecordSourceDefaultImpl<RecT> {};

  // A source that will never provide anything
  template<class RecT> class NullSource : public _IRecordSource<RecT> {};
  template<class RecT> class NullEnsembleSource : public _IRecordEnsembleSource<RecT> {};
}


#include "CAFAna/Core/Cutter.h"
#include "CAFAna/Core/EnsembleCutter.h"
#include "CAFAna/Core/Valuer.h"
#include "CAFAna/Core/PairValuer.h"
#include "CAFAna/Core/EnsembleValuer.h"
#include "CAFAna/Core/EnsemblePairValuer.h"
#include "CAFAna/Core/Weighter.h"
#include "CAFAna/Core/EnsembleWeighter.h"
#include "CAFAna/Core/EnsembleSource.h"

namespace ana::beta
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


  template<class RecT> IValueEnsembleSource&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetVar(const _Var<RecT>& var)
  {
    return fVarSources.template Get<_EnsembleValuer<RecT>>(var.ID(), *this, var);
  }

  template<class RecT> IValuePairEnsembleSource&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetVars(const _Var<RecT>& varx,
                                                   const _Var<RecT>& vary)
  {
    return fVarPairSources.template Get<_EnsemblePairValuer<RecT>>((varx*vary).ID(), *this, varx, vary);
  }

  template<class RecT> template<class SpillT> _IRecordEnsembleSource<RecT>&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetCut(const _Cut<RecT, SpillT>& cut)
  {
    return fCutSources.template Get<_EnsembleCutter<RecT, SpillT>>(cut.ID(), *this, cut);
  }


  template<class RecT> _IRecordEnsembleSource<RecT>&
  _IRecordEnsembleSourceDefaultImpl<RecT>::Weighted(const _Weight<RecT>& wei)
  {
    return fWeightSources.template Get<_EnsembleWeighter<RecT>>(wei.ID(), *this, wei);
  }

}
