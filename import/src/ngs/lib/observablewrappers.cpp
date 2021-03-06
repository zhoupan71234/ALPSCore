/*
 * Copyright (C) 1998-2014 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

#include <alps/ngs/observablewrappers.hpp>
// #ifdef ALPS_NGS_USE_NEW_ALEA
//     #include <alps/ngs/alea.hpp>
// #endif

namespace alps {

    namespace ngs {

        namespace detail {

            std::string ObservableWapper::getName() const {
                return _name;
            }

            uint32_t ObservableWapper::getBinnum() const {
                return _binnum;
            }

            std::string SignedObservableWapper::getSign() const {
                return _sign;
            }

        }
        
        //TODO
        alps::mcobservables & operator<< (alps::mcobservables & set, RealObservable const & obs) {
            set.create_RealObservable(obs.getName(), obs.getBinnum());
            return set;
        }

        // #ifdef ALPS_NGS_USE_NEW_ALEA
        //     alps::accumulator::accumulator_set & operator<< (alps::accumulator::accumulator_set & set, RealObservable const & obs) {
        //         using namespace alps::accumulator::tag;
                
        //         typedef accumulator::accumulator<double, accumulator::features<mean, error, max_num_binning> > accum_type;
        //         typedef accumulator::detail::accumulator_wrapper wrapper_type;
                
        //         set.insert(obs.getName(), boost::shared_ptr<wrapper_type>(new wrapper_type(accum_type(accumulator::bin_num = obs.getBinnum()))));

        //         return set;
        //     }
            
        //     alps::accumulator::accumulator_set & operator<< (alps::accumulator::accumulator_set & set, RealVectorObservable const & obs) {
        //         using namespace alps::accumulator::tag;
                
        //         typedef accumulator::accumulator<std::vector<double>, accumulator::features<mean, error, max_num_binning> > accum_type;
        //         typedef accumulator::detail::accumulator_wrapper wrapper_type;
                
        //         set.insert(obs.getName(), boost::shared_ptr<wrapper_type>(new wrapper_type(accum_type(accumulator::bin_num = obs.getBinnum()))));

        //         return set;
        //     }
            
        //     alps::accumulator::accumulator_set & operator<< (alps::accumulator::accumulator_set & set, SimpleRealObservable const & obs) {
        //         using namespace alps::accumulator::tag;
                
        //         typedef accumulator::accumulator<double, accumulator::features<mean, error> > accum_type;
        //         typedef accumulator::detail::accumulator_wrapper wrapper_type;
                
        //         set.insert(obs.getName(), boost::shared_ptr<wrapper_type>(new wrapper_type(accum_type(accumulator::bin_num = obs.getBinnum()))));
                
        //         return set;
        //     }
            
        //     alps::accumulator::accumulator_set & operator<< (alps::accumulator::accumulator_set & set, SimpleRealVectorObservable const & obs) {
        //         using namespace alps::accumulator::tag;
                
        //         typedef accumulator::accumulator<std::vector<double>, accumulator::features<mean, error> > accum_type;
        //         typedef accumulator::detail::accumulator_wrapper wrapper_type;
                
        //         set.insert(obs.getName(), boost::shared_ptr<wrapper_type>(new wrapper_type(accum_type(accumulator::bin_num = obs.getBinnum()))));
                
        //         return set;
        //     }
        // #endif        

        alps::mcobservables & operator<< (alps::mcobservables & set, RealVectorObservable const & obs) {
            set.create_RealVectorObservable(obs.getName(), obs.getBinnum());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, SimpleRealObservable const & obs) {
            set.create_SimpleRealObservable(obs.getName());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, SimpleRealVectorObservable const & obs) {
            set.create_SimpleRealVectorObservable(obs.getName());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, SignedRealObservable const & obs) {
            set.create_SignedRealObservable(obs.getName(), obs.getSign(), obs.getBinnum());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, SignedRealVectorObservable const & obs) {
            set.create_SignedRealVectorObservable(obs.getName(), obs.getSign(), obs.getBinnum());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, SignedSimpleRealObservable const & obs) {
            set.create_SignedSimpleRealObservable(obs.getName(), obs.getSign());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, SignedSimpleRealVectorObservable const & obs) {
            set.create_SignedSimpleRealVectorObservable(obs.getName(), obs.getSign());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, RealTimeSeriesObservable const & obs) {
            set.create_RealTimeSeriesObservable(obs.getName());
            return set;
        }

        alps::mcobservables & operator<< (alps::mcobservables & set, RealVectorTimeSeriesObservable const & obs) {
          set.create_RealTimeSeriesObservable(obs.getName());
          return set;
        }
    };

}
