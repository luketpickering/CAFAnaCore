#include "CAFAnaCore/Registry.txx"

#include "CAFAnaCore/IFittable.h"
#include "CAFAnaCore/IFitSyst.h"

namespace ana
{
  // Instantiate the registries we know we need. Experiment code should add a
  // similar file instatiating any additional registries.
  template class Registry<IFittable>;
  template class Registry<IFitSyst>;
}
