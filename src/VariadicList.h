#ifndef VARIADICLIST_H
#define VARIADICLIST_H

#include <type_traits>

namespace vlist {

template <class...>
struct variadic_list {};

template <template <class> class predicate>
struct contrary_predicate {

		template <class Type>
		struct type : std::integral_constant<bool, !predicate<Type>::value> {};

};

// ============================================================================
//		first
// ============================================================================

template <class>
struct first;
template <template <class...> class list, class A, class... Args>
struct first<list<A, Args...>> {
		typedef A type;
};

// ============================================================================
//		last
// ============================================================================

template <class>
struct last;
template <template <class...> class list, class A>
struct last<list<A>> {
		typedef A type;
};
template <template <class...> class list, class A, class... Args>
struct last<list<A, Args...>> {
		typedef typename last<list<Args...>>::type type;
};

namespace priv {
template <class, class>
struct concat_2;

template <template <class...> class list, class ...ArgsA, class ...ArgsB>
struct concat_2<list<ArgsA...>, list<ArgsB...>> {
	typedef list<ArgsA..., ArgsB...> type;
};
}

// ============================================================================
//		concat
// ============================================================================

template <class...>
struct concat;
template <class A, class B, class... Args>
struct concat<A, B, Args...> {
	typedef typename concat<typename priv::concat_2<A, B>::type, Args...>::type type;
};
template <class A, class B>
struct concat<A, B> {
	typedef typename priv::concat_2<A, B>::type type;
};
template <class A>
struct concat<A> {
	typedef A type;
};

// ============================================================================
//		transform
// ============================================================================

template <class list, template <class> class lambda>
struct transform;
template <template <class...> class list, class A, template <class> class lambda, class... Args>
struct transform<list<A, Args...>, lambda> {
		typedef typename lambda<A>::type transformed;
		typedef typename concat<list<transformed>, typename transform<list<Args...>, lambda>::type>::type type;
};
template <template <class...> class list, template <class> class lambda>
struct transform<list<>, lambda> {
		typedef list<> type;
};



// ============================================================================
//		remove_first
// ============================================================================

template <class>
struct remove_first;
template <template <class...> class list, class A, class... Args>
struct remove_first<list<A, Args...>> {
		typedef list<Args...> type;
};

// ============================================================================
//		remove_last
// ============================================================================

template <class>
struct remove_last;
template <template <class...> class list, class A, class... Args>
struct remove_last<list<A, Args...>> {
		typedef typename concat<list<A>, typename remove_last<list<Args...>>::type>::type type;
};
template <template <class...> class list, class A>
struct remove_last<list<A>> {
		typedef list<> type;
};

// ============================================================================
//		remove_at
// ============================================================================

template <class, unsigned int>
struct remove_at;
template <template <class...> class list, class A, unsigned int N, class... Args>
struct remove_at<list<A, Args...>, N> {
		static_assert(N < sizeof...(Args)+1, "Invalid removal index.");
		typedef typename concat<list<A>, typename remove_at<list<Args...>, N-1>::type>::type type;
};
template <template <class...> class list, class A, class... Args>
struct remove_at<list<A, Args...>, 0u> {
		typedef list<Args...> type;
};

// ============================================================================
//		get_at
// ============================================================================

template <class, unsigned int>
struct get_at;
template <template <class...> class list, class A, unsigned int N, class... Args>
struct get_at<list<A, Args...>, N> {
		static_assert(N < sizeof...(Args)+1, "Invalid index.");
		typedef typename get_at<list<Args...>, N-1>::type type;
};
template <template <class...> class list, class A, class... Args>
struct get_at<list<A, Args...>, 0u> {
		typedef A type;
};

// ============================================================================
//		set_at
// ============================================================================

template <class, unsigned int, class>
struct set_at;
template <template <class...> class list, class A, unsigned int N, class Val, class... Args>
struct set_at<list<A, Args...>, N, Val> {
		static_assert(N < sizeof...(Args)+1, "Invalid index.");
		typedef typename concat<list<A>, typename set_at<list<Args...>, N-1, Val>::type>::type type;
};
template <template <class...> class list, class A, class Val, class... Args>
struct set_at<list<A, Args...>, 0u, Val> {
		typedef list<Val, Args...> type;
};

// ============================================================================
//		size
// ============================================================================

template <class>
struct size;
template <template <class...> class list, class... Args>
struct size<list<Args...>> : public std::integral_constant<unsigned int, sizeof...(Args)> {};

// ============================================================================
//		is_empty
// ============================================================================

template <class>
struct is_empty;
template <template <class...> class list, class... Args>
struct is_empty<list<Args...>> : public std::integral_constant<bool, sizeof...(Args) == 0u> {};

// ============================================================================
//		contains
// ============================================================================

template <class, class>
struct contains;
template <template <class...> class list, class toMatch, class A, class ...Args>
struct contains<list<A, Args...>, toMatch> :
		public std::integral_constant<bool, contains<list<Args...>, toMatch>::value> {};
template <template <class...> class list, class toMatch, class ...Args>
struct contains<list<toMatch, Args...>, toMatch> :
		public std::integral_constant<bool, true> {};
template <template <class...> class list, class toMatch>
struct contains<list<>, toMatch> :
		public std::integral_constant<bool, false> {};

namespace priv {
template <class, template <class> class, unsigned int, bool>
struct find_loop;
template <template <class...> class list, template <class> class predicate, unsigned int i, class A, class... Args>
struct find_loop<list<A, Args...>, predicate, i, true> :
		public std::integral_constant<unsigned int, i> {};
template <template <class...> class list, template <class> class predicate, unsigned int i, class A, class... Args>
struct find_loop<list<A, Args...>, predicate, i, false> :
		public std::integral_constant<unsigned int, find_loop<list<Args...>, predicate, i+1,
		predicate<typename first<list<Args...>>::type>::value>::value> {};
template <template <class...> class list, template <class> class predicate, unsigned int i, class A>
struct find_loop<list<A>, predicate, i, false> :
		public std::integral_constant<unsigned int, i+1> {};

template <template <class...> class list, template <class> class predicate, unsigned int i, bool b>
struct find_loop<list<>, predicate, i, b> : public std::integral_constant<unsigned int, i> {};


}

// ============================================================================
//		find_if
// ============================================================================

template <class, template <class> class>
struct find_if;
template <template <class...> class list, template <class> class predicate, class ...Args>
struct find_if<list<Args...>, predicate> :
		public std::integral_constant<unsigned int, priv::find_loop<list<Args...>, predicate, 0u,
		predicate<typename first<list<Args...>>::type>::value>::value> {};

// ============================================================================
//		any_of (checks if an item verifies the predicate)
// ============================================================================

template <class list, template <class> class predicate>
struct any_of : public std::integral_constant<bool, (find_if<list, predicate>::value < size<list>::value)> {};
template <template <class...> class list, template <class> class predicate>
struct any_of<list<>, predicate> : public std::false_type {};

// ============================================================================
//		all_of (checks if the entire list verifies the predicate)
// ============================================================================

template <class list, template <class> class predicate>
struct all_of : std::integral_constant<bool, !any_of<list, contrary_predicate<predicate>::template type>::value> {};

// ============================================================================
//		none_of (checks if the entire list verifies the predicate)
// ============================================================================

template <class list, template <class> class predicate>
struct none_of : std::integral_constant<bool, !any_of<list, predicate>::value> {};

// ============================================================================
//		truncate
// ============================================================================

template <class, unsigned int end_index> // end_index not included, will be the size of the end list
struct truncate;

namespace priv {

template <class, class, unsigned int size_of_left, unsigned int end_index, class>
struct truncate_loop;

template <template <class...> class list, class... ArgsL, class FirstR, class... ArgsR, unsigned int size_of_left, unsigned int end_index>
struct truncate_loop<list<ArgsL...>, list<FirstR, ArgsR...>, size_of_left, end_index, typename std::enable_if<(size_of_left < end_index)>::type> {
	typedef typename truncate_loop<list<ArgsL..., FirstR>, list<ArgsR...>, size_of_left+1, end_index, void>::type type;
};
template <template <class...> class list, class... ArgsL, class... ArgsR, unsigned int end_index>
struct truncate_loop<list<ArgsL...>, list<ArgsR...>, end_index, end_index, void> {
	typedef list<ArgsL...> type;
};

}

template <template <class...> class list, class... Args, unsigned int end_index>
struct truncate<list<Args...>, end_index> {
		static_assert(size<list<Args...>>::value >= end_index, "Invalid index to truncate.");
		typedef typename priv::truncate_loop<list<>, list<Args...>, 0u, end_index, void>::type type;
};

// ============================================================================
//		is_valid
// ============================================================================

template <class, unsigned int>
struct is_valid;
template <template <class...> class list, unsigned int i, class... Args>
struct is_valid<list<Args...>, i> : public std::integral_constant<bool, (i < sizeof...(Args))> {};

// ============================================================================
//		get
// ============================================================================

template <class, unsigned int, class = void>
struct get;
template <template <class...> class list, unsigned int i, class A, class... Args>
struct get<list<A, Args...>, i, std::enable_if<(i < sizeof...(Args)+1)>> {
	typedef typename get<list<Args...>, i-1>::type type;
};
template <template <class...> class list, class A, class... Args>
struct get<list<A, Args...>, 0u, void> {
	typedef A type;
};

// ============================================================================
//		reverse
// ============================================================================

namespace priv {

template <class, class>
struct reverse_t;

template <template <class...> class list, class FirstA, class... ArgsA, class... ArgsB>
struct reverse_t<list<FirstA, ArgsA...>, list<ArgsB...>> {
		typedef typename reverse_t<list<ArgsA...>, list<FirstA, ArgsB...>>::type type;
};
template <template <class...> class list, class... ArgsB>
struct reverse_t<list<>, list<ArgsB...>> {
		typedef list<ArgsB...> type;
};

}

template <class>
struct reverse;
template <template <class...> class list, class... Args>
struct reverse<list<Args...>> {
	typedef typename priv::reverse_t<list<Args...>, list<>>::type type;
};

// ============================================================================
//		map
// ============================================================================

template <template <class> class mapping_function, class vlist, class T, template <T...> class dst_list>
struct map_to_integral;

template <template <class> class mapping_function, template <class...> class list, class T, template <T...> class dst_list, class... Args>
struct map_to_integral<mapping_function, list<Args...>, T, dst_list> {
		typedef dst_list<mapping_function<Args>::value...> type;
};

template <template <class> class mapping_function, class vlist, template <class...> class dst_list>
struct map;

template <template <class> class mapping_function, template <class...> class list, template <class...> class dst_list, class... Args>
struct map<mapping_function, list<Args...>, dst_list> {
		typedef dst_list<typename mapping_function<Args>::type...> type;
};

template <class>
struct class_to_integral_mapping;
template <class T, T v>
struct class_to_integral_mapping<std::integral_constant<T, v>> {
	static constexpr T value = v;
};

template <class vlist, class T, template <T...> class dst_list>
struct class_to_integral {
		typedef typename map_to_integral<class_to_integral_mapping, vlist, T, dst_list>::type type;
};

template <class T, class dst_list, template <class...> class list>
struct integral_to_class;
template <class T, template <T...> class dst_list, template <class...> class list, T... Args>
struct integral_to_class<T, dst_list<Args...>, list> {
		typedef list<std::integral_constant<T, Args>...> type;
};

// ============================================================================
//		remove_if
// ============================================================================

template <class, template <class> class, class = void>
struct remove_if;
template <template <class...> class list, template <class> class predicate, class A, class... Args>
struct remove_if<list<A, Args...>, predicate, std::enable_if<predicate<A>::value>> {
		typedef typename remove_if<list<Args...>, predicate>::type type;
};
template <template <class...> class list, template <class> class predicate, class A, class... Args>
struct remove_if<list<A, Args...>, predicate, std::enable_if<!predicate<A>::value>> {
		typedef typename concat<list<A>, typename remove_if<list<Args...>, predicate>::type>::type type;
};
template <template <class...> class list, template <class> class predicate>
struct remove_if<list<>, predicate, void> { typedef list<> type; };

// ============================================================================
//		sort		--quick sort
// ============================================================================

template <class A, class B>
struct less : std::integral_constant<bool, (A::value < B::value)> {};

template <class A, class B>
struct greater : std::integral_constant<bool, (A::value > B::value)> {};

namespace priv {

struct same_type {};
struct before {};
struct after {};

template <class Left, class Right, template <class, class> class sorting_function>
struct test_apply {
		typedef typename std::conditional<sorting_function<Left, Right>::value, before, after>::type type;
};
template <class T, template <class, class> class sorting_function>
struct test_apply<T, T, sorting_function> {
		typedef same_type type;
};

template <class, class, class, class, bool remove_duplicate_b, template <class, class> class sorting_function>
struct insert_loop;
template <template <class...> class list, class T, bool remove_duplicate_b, template <class, class> class sorting_function, class... A_args, class... B_args>
struct insert_loop<list<A_args...>, T, list<B_args...>, after, remove_duplicate_b, sorting_function> {
		typedef list<A_args...> listA;
		typedef list<B_args...> listB;
		typedef typename first<listB>::type T_2;
		typedef typename concat<listA, list<T_2>>::type newLeft;
		typedef typename remove_first<listB>::type newRight;
		typedef typename test_apply<T_2, T, sorting_function>::type rslt;
		typedef typename insert_loop<newLeft, T, newRight, rslt, remove_duplicate_b, sorting_function>::type type;
};
template <template <class...> class list, class T, bool remove_duplicate_b, template <class, class> class sorting_function, class ...A_args>
struct insert_loop<list<A_args...>, T, list<>, after, remove_duplicate_b, sorting_function> {
		typedef typename concat<list<A_args...>, list<T>>::type type;
};
template <template <class...> class list, class T, bool remove_duplicate_b, template <class, class> class sorting_function, class... A_args, class... B_args>
struct insert_loop<list<A_args...>, T, list<B_args...>, before, remove_duplicate_b, sorting_function> {
		typedef list<A_args...> listA;
		typedef list<B_args...> listB;
		typedef typename remove_last<listA>::type head;
		typedef typename last<listA>::type T_0;
		typedef typename concat<head, list<T>, list<T_0>, listB>::type type;
};
template <template <class...> class list, class T, template <class, class> class sorting_function, class... A_args, class... B_args>
struct insert_loop<list<A_args...>, T, list<B_args...>, same_type, false, sorting_function> {
		typedef typename concat<list<A_args...>, list<T>, list<B_args...>>::type type;
};
template <template <class...> class list, class T, template <class, class> class sorting_function, class... A_args, class... B_args>
struct insert_loop<list<A_args...>, T, list<B_args...>, same_type, true, sorting_function> {
		typedef typename concat<list<A_args...>, list<B_args...>>::type type;
};

template <class, class, bool remove_duplicate_b, template <class, class> class sorting_function>
struct insert_item;

template <template <class...> class list, class T, class B, bool remove_duplicate_b, template <class, class> class sorting_function, class... Args>
struct insert_item<list<T, Args...>, B, remove_duplicate_b, sorting_function> {
		typedef typename test_apply<T, B, sorting_function>::type test_rlst;
		typedef typename insert_loop<list<T>, B, list<Args...>, test_rlst, remove_duplicate_b, sorting_function>::type type;
};
template <template <class...> class list, class B, bool remove_duplicate_b, template <class, class> class sorting_function>
struct insert_item<list<>, B, remove_duplicate_b, sorting_function> {
		typedef list<B> type;
};

template <class, class, bool remove_duplicate_b, template <class, class> class sorting_function>
struct insert;
template <template <class...> class list, class... Args, bool remove_duplicate_b, template <class, class> class sorting_function>
struct insert<list<Args...>, list<>, remove_duplicate_b, sorting_function> {
		typedef list<Args...> type;
};
template <class listA, class listB, bool remove_duplicate_b, template <class, class> class sorting_function>
struct insert {
		typedef typename first<listB>::type T;
		typedef typename insert<typename priv::insert_item<listA, T, remove_duplicate_b, sorting_function>::type,
		typename remove_first<listB>::type, remove_duplicate_b,
		sorting_function>::type type;
};

}

template <class list, template <class, class> class sorting_function = less>
struct quick_sort;
template <template <class...> class list, template <class, class> class sorting_function>
struct quick_sort<list<>, sorting_function> {
	typedef list<> type;
};
template <template <class...> class list, class A, class... Args, template <class, class> class sorting_function>
struct quick_sort<list<A, Args...>, sorting_function> {
	typedef typename priv::insert<list<A>, list<Args...>, false, sorting_function>::type type;
};

// ============================================================================
//		remove_duplicates
// ============================================================================

template <class list, template <class, class> class sorting_function = less>
struct remove_duplicates;
template <template <class...> class list, template <class, class> class sorting_function>
struct remove_duplicates<list<>, sorting_function> {
	typedef list<> type;
};
template <template <class...> class list, class A, template <class, class> class sorting_function, class... Args>
struct remove_duplicates<list<A, Args...>, sorting_function> {
	typedef typename priv::insert<list<A>, list<Args...>, true, sorting_function>::type type;
};

// ============================================================================
//		includes
// ============================================================================

// Returns true iff \forall x \in SmallList => x \in LargeList
// Naive implementation: O(N*M)
template <class LargeList, class SmallList>
struct includes {

		template <class SmallListItem>
		struct contains_item : contains<LargeList, SmallListItem> {};

		static constexpr bool value = all_of<SmallList, contains_item>::value;
};

// ============================================================================
//		difference
// ============================================================================

namespace priv {
	template <class LargeList, class SmallList, bool LargeListHead_In_SmallList>
	struct difference_loop;

	template <template <class...> class list, class LargeListItem, class SmallList, class... Litems>
	struct difference_loop<list<LargeListItem, Litems...>, SmallList, true> {
		typedef typename concat<
			list<>,
			typename difference_loop<
				list<Litems...>,
				SmallList,
				contains<SmallList, typename first<list<Litems...>>::type>::value
			>::type
		>::type type;
	};
	template <template <class...> class list, class LargeListItem, class SmallList, class... Litems>
	struct difference_loop<list<LargeListItem, Litems...>, SmallList, false> {
		typedef typename concat<
			list<LargeListItem>,
			typename difference_loop<
				list<Litems...>,
				SmallList,
				contains<SmallList, typename first<list<Litems...>>::type>::value
			>::type
		>::type type;
	};

	template <template <class...> class list, class LargeListItem, class SmallList>
	struct difference_loop<list<LargeListItem>, SmallList, true> {
		typedef list<> type;
	};
	template <template <class...> class list, class LargeListItem, class SmallList>
	struct difference_loop<list<LargeListItem>, SmallList, false> {
		typedef list<LargeListItem> type;
	};
}

// Returns true LargeList - SmallList
template <class LargeList, class SmallList>
struct difference;
template <template <class...> class list, class LargeListItem, class SmallList, class... Litems>
struct difference<list<LargeListItem, Litems...>, SmallList> {
		typedef typename priv::difference_loop<list<LargeListItem, Litems...>, SmallList, contains<SmallList, LargeListItem>::value>::type type;
};
template <template <class...> class list, class SmallList>
struct difference<list<>, SmallList> {
		typedef list<> type;
};

}

// ============================================================================
//		conditional_switch
// ============================================================================

template <unsigned int Choice, class... Options>
struct conditional_switch {
		static_assert(Choice < sizeof...(Options), "Invalid switch entry");
		typedef typename vlist::get_at<vlist::variadic_list<Options...>, Choice>::type type;
};

#endif // VARIADICLIST_H
