#include <fc/network/http/leaky_bucket.hpp>

namespace fc {
    namespace http {

        leaky_bucket_rules::leaky_bucket_rules() :
                init(false),
                current_time(clock::now()),
                limit(-1),
                counter(0),
                time(clock::now()),
                per_second(1){}

        leaky_bucket_rules::leaky_bucket_rules(
                const uint64_t limit,
                const uint64_t per_second
        ) :
                init(true),
                current_time(clock::now()),
                limit(limit),
                counter(0),
                time(clock::now()),
                per_second(per_second){}

        bool leaky_bucket_rules::check() const {
            if (init) {
               return limit > counter;
            }
            return true;
        }

        void leaky_bucket_rules::update_limit() {
            auto delta = std::chrono::duration_cast<std::chrono::seconds>(current_time - time);

            if (delta >= std::chrono::seconds(0) && delta < std::chrono::seconds(per_second)) {
                time = current_time;
            } else {
                counter = 0;
                time = current_time;
            }
        }

        void leaky_bucket_rules::increment() {
            ++counter;
        }

        void leaky_bucket_rules::update_time() {
            current_time = clock::now();
        }
    }
}