#ifndef FC_LEAKY_BUCKET_HPP
#define FC_LEAKY_BUCKET_HPP

#include <chrono>
#include <cstddef>
#include <string>
#include <set>

namespace fc {
    namespace http {
        class leaky_bucket_rules final {
        public:
            using clock = std::chrono::steady_clock;
            using millisecond = std::chrono::milliseconds;
            using second = std::chrono::milliseconds;

            leaky_bucket_rules(const leaky_bucket_rules &) = default;

            leaky_bucket_rules &operator=(const leaky_bucket_rules &)= default;

            leaky_bucket_rules(leaky_bucket_rules &&) = default;

            leaky_bucket_rules &operator=(leaky_bucket_rules &&)= default;

            ~leaky_bucket_rules() = default;

            leaky_bucket_rules();

            leaky_bucket_rules(const uint64_t limit, const uint64_t per_second);

            bool check() const;

            void increment();

            void update_time();

            void update_limit();

        private:
            bool init;
            clock::time_point current_time;
            const uint64_t limit;
            uint64_t counter;
            clock::time_point time;
            const uint64_t per_second;
        };
    }
}
#endif //FC_LEAKY_BUCKET_HPP
