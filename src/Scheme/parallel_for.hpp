/*******************************************************************************
 * Copyright (C) 2022-2023 Theodore Chang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef DAMPING_DOLPHIN_PARALLEL_FOR_HPP
#define DAMPING_DOLPHIN_PARALLEL_FOR_HPP

#ifdef DD_TBB_ENABLED
#ifdef emit
#undef emit
#include <tbb/parallel_for.h>
#define emit
#else
#include <tbb/parallel_for.h>
#endif
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
