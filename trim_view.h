#pragma once
//we will create a view called trim, that, given a range and a unary predicate, returns a new range
//without the front or back elements that satisfies the predicate
#include <ranges>
template <std::ranges::input_range Range, typename Predicate>
requires std::ranges::view<Range>
class trim_view : public std::ranges::view_interface<trim_view<Range, Predicate>>
{
private://data members
	Range base_{};
	Predicate pred_;
	mutable std::ranges::iterator_t<Range> begin_{ std::begin(base_) };
	mutable std::ranges::iterator_t<Range> end_{ std::end(base_) };
	mutable bool evaluated_ = false;
private: //functions
	void ensure_evaluated() const {
		if (!evaluated_) {
			while (begin_ != std::end(base_) && pred_(*begin_)) {
				begin_ = std::next(begin_);
			}
			while (end_ != begin_ && pred_(*std::prev(end_))) {
				end_ = std::prev(end_);
			}
		}
	}
public:
	trim_view() = default;
	constexpr trim_view(Range base, Predicate pred)
		: base_(std::move(base))
		, pred_(std::move(pred))
		, begin_(std::begin(base_))
		, end_(std::end(base_))
	{}
	constexpr Range base() const& {
		return base_;
	}
	constexpr Range base()&& {
		return std::move(base_);
	}
	constexpr Predicate const& pred() const {
		return pred_;
	}
	constexpr auto size() requires std::ranges::sized_range<Range>{
		return std::distance(begin_, end_);
	}
	constexpr auto size() const requires std::ranges::sized_range<const Range>{
		return std::distance(begin_, end_);
	}
	constexpr auto begin() const {
		ensure_evaluated();
		return begin_;
	}
	constexpr auto end() const {
		ensure_evaluated();
		return end_;
	}
};

//a template argument deduction guide
template <typename Range, typename Predicate>
trim_view(Range&& base, Predicate pred)->trim_view<std::ranges::views::all_t<Range>, Predicate>;

//a function object that can instatiate the trim_view adaptor with the proper arguments.
namespace details {
	template <typename Predicate>
	struct trim_view_range_adaptor_closure {
		Predicate pred_;
		constexpr trim_view_range_adaptor_closure(Predicate pred)
			: pred_(pred) {}
		template <std::ranges::viewable_range Range>
		constexpr auto operator() (Range&& r) {
			return trim_view(std::forward<Range>(r), pred_);
		}
	};

	struct trim_view_range_adaptor {
		template <std::ranges::viewable_range Range, typename Predicate>
		constexpr auto operator() (Range&& r, Predicate pred) {
			return trim_view(std::forward<Range>(r), pred);
		}
		template <typename Predicate>
		constexpr auto operator() (Predicate pred) {
			return trim_view_range_adaptor_closure(pred);
		}
	};

	template <std::ranges::viewable_range Range, typename Predicate>
	constexpr auto operator| (Range&& r, trim_view_range_adaptor_closure<Predicate> const& a) {
		return a(std::forward<Range>(r));
	}
}

namespace views {
	inline static details::trim_view_range_adaptor trim;
}