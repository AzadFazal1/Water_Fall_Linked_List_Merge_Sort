

#include "dl_list.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>

namespace cs225 {

template <class T>
dl_list<T>::dl_list() : head_{nullptr}, tail_{nullptr}, size_{0} {
  /// @todo mp_list.1
}

template <class T>
dl_list<T>::dl_list(const dl_list &other)
    : head_{nullptr}, tail_{nullptr}, size_{0} {
  for (auto &elem : other) {
    push_back(elem);
  }
  /// @todo mp_list.1
  /// hint: re-use your push_X function(s)!
}

template <class T>
dl_list<T>::dl_list(dl_list &&other)
    : head_{nullptr}, tail_{nullptr}, size_{0} {

  swap(other);
  
  /// @todo mp_list.1
}

template <class T> dl_list<T> &dl_list<T>::operator=(dl_list rhs) {
  swap(rhs);
  return *this;

  /// @todo mp_list.1
}

template <class T> void dl_list<T>::swap(dl_list &other) {
  std::swap(head_, other.head_);
  std::swap(tail_, other.tail_);
  std::swap(size_, other.size_);
  /// @todo mp_list.1
}

template <class T> void dl_list<T>::push_back(T data) {

  if (size_ == 0) {
    head_ = std::make_unique<node>(std::move(data));
    tail_ = head_.get();
  } else {
    tail_->next = std::make_unique<node>(std::move(data));
    tail_->next->prev = tail_;
    tail_ = tail_->next.get();
  }

  ++size_;
  /// @todo mp_list.
}

template <class T> void dl_list<T>::push_front(T data) {

  if (size_ == 0) {
    head_ = std::make_unique<node>(std::move(data));
    head_.get()->next = nullptr;
    head_.get()->prev = nullptr;
    tail_ = head_.get();
  } else {
    auto n_head = std::make_unique<node>(std::move(data));
    n_head.get()->prev = nullptr;
    head_.get()->prev = n_head.get();
    n_head.get()->next = std::move(head_);
    head_ = std::move(n_head);
  }

  ++size_;

  /// @todo mp_list.1
}

template <class T> void dl_list<T>::reverse() { reverse(head_, tail_); }

template <class T>
void dl_list<T>::reverse(std::unique_ptr<node> &first, node *&last) {
  if (first.get() == last) {
    return;
  }
  auto o_first = first.get();
  auto curr = std::move(first->next);
  auto o_prev = std::move(first);
  o_prev->prev = curr.get();
  while (curr.get() != last) {
    // swap the ownership
    curr->next.swap(o_prev);
    // set the previous pointer
    curr->prev = o_prev.get();
    // move the current pointer
    curr.swap(o_prev);
  }
  curr->next.swap(o_prev);
  curr->prev = nullptr;
  first.swap(curr);
  last = o_first;
  /// @todo mp_list.1
}

template <class T> void dl_list<T>::reverse_nth(unsigned n) {

  if (n <= 1) {
    return;
  }

  if (n >= size_) {
    return reverse();
  }

  auto cur = head_.get();
  auto begin = std::move(head_);
  while (cur->next) {
    for (unsigned i = 1; i < n; ++i) {
      cur = cur->next.get();
      if (!cur->next) {
        break;
      }
    }
    // Unstiching the list on both sides.
    auto left = begin->prev;
    begin->prev = nullptr;
    auto right = std::move(cur->next);
    if (right) {
      right->prev = nullptr;
    }
    // Reversing the unstiched sub list.
    reverse(begin, cur);
    if (!head_) {
      head_ = std::move(begin);
    }
    // Restiching the reversed list on both left and right sides.
    if (left) {
      begin->prev = left;
      left->next = std::move(begin);
    }
    if (right) {
      right->prev = cur;
      cur->next = std::move(right);
    }
    if (cur->next) {
      begin = std::move(cur->next);
      cur = begin.get();
    }
  }
  tail_ = cur;
}

template <class T> void dl_list<T>::waterfall() {

  if (size_ < 3) {
    return;
  }

  auto cur = head_.get();
  while (cur != tail_ && cur->next.get() != tail_) {
    auto owner = std::move(cur->next);
    owner->next->prev = cur;
    cur->next = std::move(owner->next);
    owner->prev = tail_;
    tail_->next = std::move(owner);
    tail_ = tail_->next.get();
    cur = cur->next.get();
  }
  /// @todo mp_list.1
}

template <class T> auto dl_list<T>::split(unsigned split_point) -> dl_list {
  if (split_point >= size_)
    return {};

  if (split_point == 0) {
    dl_list lst;
    swap(*this);
    return lst;
  }

  auto old_size = size_;
  auto new_head = split(head_.get(), split_point);

  // set up current list
  size_ = split_point;

  for (tail_ = head_.get(); tail_->next; tail_ = tail_->next.get())
    ;

  // set up returned list
  dl_list ret;
  ret.head_ = std::move(new_head);
  for (ret.tail_ = ret.head_.get(); ret.tail_->next;
       ret.tail_ = ret.tail_->next.get())
    ;
  ret.size_ = old_size - split_point;
  return ret;
}

template <class T>
auto dl_list<T>::split(node *start, unsigned split_point)
    -> std::unique_ptr<node> {
  assert (split_point > 0);
  for (unsigned i = 0; i < split_point; ++i) {
    start = start->next.get();
  }
  tail_ = start->prev;
  auto n_head = std::move(tail_->next);
  n_head->prev = nullptr;
  tail_->next = nullptr;
  return n_head;

  /// @todo mp_list.2
}

template <class T> void dl_list<T>::merge(dl_list &to_merge) {
  head_ = merge(std::move(head_), std::move(to_merge.head_));

  for (tail_ = head_.get(); tail_->next; tail_ = tail_->next.get())
    ;
  size_ += to_merge.size_;

  to_merge.tail_ = nullptr;
  to_merge.size_ = 0;
}

template <class T>
auto dl_list<T>::merge(std::unique_ptr<node> first,
                       std::unique_ptr<node> second) -> std::unique_ptr<node> {
  
  if (!first) {     // if only second list exists.
    return second;
  }
  if (!second) {    // if only first list exists.
    return first;
  }
  // Setting the primary list.
  if(second->data < first->data) {
      first.swap(second);
  }
  auto cur = first.get();
   // Sorting and inserting the elements in primary list before 
  // reaching the last element in either of the list(whichever comes first).
  while(cur->next && second) {
    if(cur->next->data < second->data) {
      cur = cur->next.get();
    }
    else {
      auto temp = std::move(second->next);
      temp.swap(second);
      temp->prev = cur;
      temp->next = std::move(cur->next);
      temp->next->prev = temp.get();
      cur->next = std::move(temp);
    }
  }
  if (second) {
    cur->next = std::move(second);
  }

  return first;
}
  /// @todo mp_list.2

template <class T> void dl_list<T>::sort() {
  if (!head_)
    return;

  head_ = mergesort(std::move(head_), size_);
  for (tail_ = head_.get(); tail_->next; tail_ = tail_->next.get())
    ;
}

template <class T>
auto dl_list<T>::mergesort(std::unique_ptr<node> start, uint64_t length)
    -> std::unique_ptr<node> {
      if(length == 1) {
        return start;
      }
  auto len_2 = length / 2;
  auto len_1 = len_2;
  if(length % 2 != 0) {
    ++len_1;
  }
  assert(len_1 + len_2 == length);
  
  auto cur = start.get();
  auto second = split(cur, len_1);
  auto first = mergesort(std::move(start), len_1);
  auto sec = mergesort(std::move(second), len_2);
 // std::cout << print(first.get()) << " : " << print(sec.get());
  auto new_head = merge(std::move(first), std::move(sec));
 // std::cout << " : " << print(new_head.get()) << std::endl;

  return new_head;

  /// @todo mp_list.2
}

template <class T>
std::string dl_list<T>::print(node* beg) {
  std::stringstream ss;
  while(beg) {
    ss << beg->data << " ";
    beg = beg->next.get(); 
  }
  return ss.str();
}


} // namespace cs225
