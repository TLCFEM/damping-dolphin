//
// Created by theodore on 6/16/22.
//

#ifndef DAMPING_DOLPHIN_PARALLEL_FOR_HPP
#define DAMPING_DOLPHIN_PARALLEL_FOR_HPP

#ifdef DD_TBB_ENABLED
#include <tbb/parallel_for.h>
#endif

namespace dd {
    template<typename index, typename lmd> void parallel_for(index begin, index end, lmd&& lambda) {
#ifdef DD_TBB_ENABLED
        tbb::parallel_for(begin, end, std::forward<lmd>(lambda));
#else
        for(index I = begin; I < end; ++I) lambda(I);
#endif
    }
} // namespace dd

#endif // DAMPING_DOLPHIN_PARALLEL_FOR_HPP
