#include "OwningMutex.hpp"
#include <cassert>

#include <thread>
#include <vector>

constexpr size_t SET_VALUE = 10000;

template <typename Fn> void run_across_threads(Fn fn) {

  std::vector<std::thread> threads;
  threads.reserve(SET_VALUE);

  for (unsigned i = 0; i < SET_VALUE; ++i) {
    threads.emplace_back(fn);
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

static void test_with_lock() {
  OwningMutex<int> mutex(0);

  run_across_threads([&mutex] { mutex.with_lock([](int &val) { val += 1; }); });

  assert(mutex.lock().get() == SET_VALUE);
}

static void test_guard() {
  OwningMutex<int> mutex(0);

  run_across_threads([&mutex] {
    auto guard = mutex.lock();
    guard.get() += 1;
  });

  assert(mutex.lock().get() == SET_VALUE);
}

static void test_guard_manual_create() {
  OwningMutex<int> mutex(0);

  run_across_threads([&mutex] {
    OwningMutex<int>::MutexGuard guard(mutex);
    guard.get() += 1;
  });

  assert(mutex.lock().get() == SET_VALUE);
}

static void test_lock() {
  OwningMutex<int> mutex(0);

  run_across_threads([&mutex] { mutex.lock().get() += 1; });

  assert(mutex.lock().get() == SET_VALUE);
}

static void test_with_lock_return() {
  OwningMutex<int> mutex(5);

  assert(mutex.with_lock([](int& val){return val + 5;}) == 10);
}

int main() {
  test_with_lock();
  test_guard();
  test_guard_manual_create();
  test_lock();
  test_with_lock_return();

  return 0;
}
