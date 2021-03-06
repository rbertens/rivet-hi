// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/UnstableFinalState.hh"

namespace Rivet {


  /// @brief Add a short analysis description here
  class BELLE_2011_I878990 : public Analysis {
  public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(BELLE_2011_I878990);


    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Initialise and register projections
      declare(UnstableFinalState(), "UFS");

      // Book histograms
      _h_q2 = bookHisto1D(1, 1, 1);
    }

    // Calculate the Q2 using mother and daugher meson
    double q2(const Particle& B, int mesonID) {
      FourMomentum q = B.mom() - filter_select(B.children(), Cuts::pid==mesonID)[0];
      return q*q;
    }

    // Check for explicit decay into pdgids
    bool isSemileptonicDecay(const Particle& mother, vector<int> ids) {
      // Trivial check to ignore any other decays but the one in question modulo photons
      const Particles children = mother.children(Cuts::pid!=PID::PHOTON);
      if (children.size()!=ids.size()) return false;
      // Check for the explicit decay
      return all(ids, [&](int i){return count(children, hasPID(i))==1;});
    }

    /// Perform the per-event analysis
    void analyze(const Event& event) {
      // Loop over B0 mesons 
      foreach(const Particle& p, apply<UnstableFinalState>(event, "UFS").particles(Cuts::pid==PID::B0)) {
        if (isSemileptonicDecay(p, {PID::PIMINUS, PID::POSITRON, PID::NU_E}) ||
            isSemileptonicDecay(p, {PID::PIMINUS, PID::ANTIMUON, PID::NU_MU})) {
            _h_q2->fill(q2(p, PID::PIMINUS), event.weight());
        }
      }
    }


    /// Normalise histograms etc., after the run
    void finalize() {

      normalize(_h_q2, 3000.86); // normalize to BF*dQ2

    }

    //@}


  private:


    /// @name Histograms
    //@{
    Histo1DPtr _h_q2;
    //@}


  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(BELLE_2011_I878990);


}
