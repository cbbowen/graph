#pragma once

#if defined(__has_include) && !__has_include(<omp.h>)
#	undef _OPENMP
#endif

#ifdef _OPENMP
#	include <omp.h>
#endif

namespace graph {
	inline namespace v1 {
		namespace impl {
#ifdef _OPENMP
			inline int omp_get_max_threads() { return ::omp_get_max_threads(); }
			inline int omp_get_num_threads() { return ::omp_get_num_threads(); }
			inline int omp_get_thread_num() { return ::omp_get_thread_num(); }
#else
			inline int omp_get_max_threads() { return 1; }
			inline int omp_get_num_threads() { return 1; }
			inline int omp_get_thread_num() { return 0; }
#endif
		}
	}
}
