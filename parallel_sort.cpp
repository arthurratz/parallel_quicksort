#include "stdafx.h"

#include "parallel_sort.h"

#ifndef PARALLEL_STABLE_SORT_STL_CPP
#define PARALLEL_STABLE_SORT_STL_CPP

namespace parallel_sort_impl
{
#if defined( _WIN32 )
	static HANDLE hStdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
	const WORD wdRed = FOREGROUND_RED | FOREGROUND_INTENSITY;
	const WORD wdWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
#endif

	void stress_test(void)
	{
		while (true)
		{
			std::size_t count = 0L;
			int sort_type = std::rand() % 5;
			std::vector<std::int64_t> array, array_copy;
			// Generate the values of data items in the array to be sorted
			misc::init(array, std::make_pair(std::pow(10, misc::minval_radix), \
				std::pow(10, misc::maxval_radix)), count, sort_type);

			// Resize the array and copy all items to the second array to be sorted in parallel
			array_copy.resize(array.size());
			std::copy(array.begin(), array.end(), array_copy.begin());

			// Obtain the value of walltime prior to performing the sequential sorting
			std::chrono::system_clock::time_point \
				time_s = std::chrono::system_clock::now();

			std::cout << "sorting an array... (sort type: " << misc::sorttype2string(sort_type) << ")\n";

			// Perform the sequental sort by using std::sort function
			std::sort(array.begin(), array.end(),
				[](std::int64_t first, std::int64_t end) { return first < end; });

			// Obtain the value of walltime after to performing the sequential sorting
			std::chrono::system_clock::time_point \
				time_f = std::chrono::system_clock::now();

			// Compute the overall execution walltime
			std::chrono::system_clock::duration \
				std_sort_time_elapsed = time_f - time_s;

			std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(4)
				<< "array size = " << count << " execution time (std::sort): " << std_sort_time_elapsed.count() << " ms ";

			// Obtain the value of walltime prior to performing the parallel sorting
			time_s = std::chrono::system_clock::now();

			// Perform the parallel sorting
			internal::parallel_sort(array_copy.begin(), array_copy.end(),
				[](std::int64_t first, std::int64_t end) { return first < end; });

			// Obtain the value of walltime after to performing the parallel sorting
			time_f = std::chrono::system_clock::now();

			std::size_t position = 0L;
			// Compute the overall execution walltime spent for parallel sorting
			std::chrono::system_clock::duration \
				intro_sort_time_elapsed = time_f - time_s;

			// Perform a verification if the array is properly sorted
			bool is_sorted = misc::sorted(array_copy.begin(), array_copy.end(), position,
				[](std::int64_t first, std::int64_t end) { return first < end; });

			// Compute the actual performance gain as the difference of execution walltime values
			std::double_t time_diff = \
				std_sort_time_elapsed.count() - intro_sort_time_elapsed.count();

			#if defined( _WIN32 )
			::SetConsoleTextAttribute(hStdout, \
				(is_sorted == true) ? wdWhite : wdRed);
			#else
			if (is_sorted == false)
				std::cout << "\033[1;31m";
			#endif	

			std::cout << "<--> (internal::parallel_sort): " << intro_sort_time_elapsed.count() << " ms " << "\n";

			std::cout << "verification: ";

			if (is_sorted == false) {
				std::cout << "failed at pos: " << position << "\n";
				std::cin.get();
				// Print out the array if the sorting has failed
				misc::print_out(array_copy.begin() + position, array_copy.end() + position + 10);
			}

			else {
				// Print out the statistics
				std::double_t ratio = intro_sort_time_elapsed.count() / \
					(std::double_t)std_sort_time_elapsed.count();
				std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2)
					<< "passed... [ time_diff: " << std::fabs(time_diff)
					<< " ms (" << "ratio: " << (ratio - 1.0) * 100 << "%) recursion depth = "
					<< internal::g_depth << " ]" << "\n";
			}

			std::cout << "\n";

			#if !defined ( _WIN32 )
			if (is_sorted == false)
				std::cout << "\033[0m";
			#endif	
		}
	}

	void parallel_sort_demo(void)
	{
		std::size_t count = 0L;
		std::cout << "Enter the number of data items N = "; std::cin >> count;

		std::vector<std::int64_t> array;
		misc::init(array, std::make_pair(std::pow(10, misc::minval_radix), \
			std::pow(10, misc::maxval_radix)), count, 0);

		std::chrono::system_clock::time_point \
			time_s = std::chrono::system_clock::now();

		internal::parallel_sort(array.begin(), array.end(),
			[](std::int64_t first, std::int64_t end) { return first < end; });

		std::chrono::system_clock::time_point \
			time_f = std::chrono::system_clock::now();

		std::size_t position = 0L;
		std::chrono::system_clock::duration \
			intro_sort_time_elapsed = time_f - time_s;

		std::cout << "Execution Time: " << intro_sort_time_elapsed.count()
				  << " ms " << "recursion depth = " << internal::g_depth << " ";

		bool is_sorted = misc::sorted(array.begin(), array.end(), position,
			[](std::int64_t first, std::int64_t end) { return first < end; });

		std::cout << "(verification: ";

		if (is_sorted == false) {
			std::cout << "failed at pos: " << position << "\n";
			std::cin.get();
			misc::print_out(array.begin() + position, array.end() + position + 10);
		}

		else {
			std::cout << "passed...)" << "\n";
		}

		char option = '\0';
		std::cout << "Do you want to output the array [Y/N]?"; std::cin >> option;

		if (option == 'y' || option == 'Y')
			misc::print_out(array.begin(), array.end());
	}
}

int main()
{
	std::string logo = "Parallel Sort v.1.00 (C)MIT by Arthur V. Ratz";
	std::cout << logo << "\n\n";

	std::cout << "1. Run Sorting Reliability Stress Test (SRST)\n";
	std::cout << "2. Sort Random Sequence of N Integers\n";

	std::cout << "\nChoose: ";

	char option = '\0';
	while (option != '1' && option != '2')
		option = std::getchar();
	
	std::cout << "\n";

	if (option == '1')
		parallel_sort_impl::stress_test();
	if (option == '2')
		parallel_sort_impl::parallel_sort_demo();

	return 0;
}

#endif // PARALLEL_STABLE_SORT_STL_CPP
