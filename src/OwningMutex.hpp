#include <mutex>
#include <type_traits>

// Mutex that requires you to go through it before using state protected by it.
template <typename T>
class OwningMutex {
    std::mutex mutex;
    T value;

public:
  template <typename... Args>
  OwningMutex(Args &&...val) : value(std::forward<Args>(val)...) {}

  // Trying to save reference provided to function outside of it is almost
  // certenly wrong.
  template <typename Fn> auto with_lock(Fn fn) {
    static_assert(std::is_invocable_v<Fn, T &>);
    std::lock_guard<std::mutex> lock(mutex);
    return fn(value);
  }

  // This class is similar to lock_guard, except it also provides access to
  // value that is protected by mutex.
  class MutexGuard {
    OwningMutex &mutex_ref;

  public:
    explicit MutexGuard(OwningMutex &mutex_r) : mutex_ref(mutex_r) {
      mutex_ref.mutex.lock();
    }

    // Probably better to use shared_ptr<MutexGuard<T>> instead of trying to
    // implement this.
    MutexGuard(MutexGuard &) = delete;
    MutexGuard &operator=(MutexGuard &) = delete;

    // Supporting move ctor would require introducing extra state, so that we
    // don't unlock the mutex using instance that we move out of.
    MutexGuard(MutexGuard &&) = delete;
    MutexGuard &operator=(MutexGuard &&) = delete;

    // Trying to save provided reference somewhere is almost certenly wrong.
    T &get() const noexcept { return mutex_ref.value; }

    ~MutexGuard() { mutex_ref.mutex.unlock(); }
  };

  // Provides a way to get access to value under lock.
  [[nodiscard]] MutexGuard lock() {
    return MutexGuard(*this);
  }

  OwningMutex(OwningMutex &) = delete;
  OwningMutex &operator=(OwningMutex &) = delete;

  OwningMutex(OwningMutex &&) = delete;
  OwningMutex &operator=(OwningMutex &&) = delete;
};
