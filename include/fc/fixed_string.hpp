#pragma once

#include <fc/io/raw_fwd.hpp>
#include <array>

namespace fc {
    template<typename Storage = std::pair<uint64_t, uint64_t>>
    fc::fixed_string<Storage> to_fixed_string(const std::string &a) {
        fc::fixed_string<Storage> result;
        if (a.size() < result.max_size()) {
            std::copy(a.begin(), a.end(), result.begin());
        } else {
            std::copy(a.begin(), a.begin() + result.max_size(), result.begin());
        }

        return result;
    }

    namespace raw {
        template<typename Stream, typename Storage>
        inline void pack(Stream &s, const fc::fixed_string<Storage> &u) {
            unsigned_int size = u.size();
            pack(s, size);
            s.write((const char *) &u.data, size);
        }

        template<typename Stream, typename Storage>
        inline void unpack(Stream &s, fc::fixed_string<Storage> &u) {
            unsigned_int size;
            fc::raw::unpack(s, size);
            if (size.value > 0) {
                if (size.value > sizeof(Storage)) {
                    s.read((char *) &u.data, sizeof(Storage));
                    char buf[1024];
                    size_t left = size.value - sizeof(Storage);
                    while (left >= 1024) {
                        s.read(buf, 1024);
                        left -= 1024;
                    }
                    s.read(buf, left);

                    /*
                    s.seekp( s.tellp() + (size.value - sizeof(Storage)) );
                    char tmp;
                    size.value -= sizeof(storage);
                    while( size.value ){ s.read( &tmp, 1 ); --size.value; }
                    */
                    //  s.skip( size.value - sizeof(Storage) );
                } else {
                    s.read((char *) &u.data, size.value);
                }
            }
        }

        /*
        template<typename Stream, typename... Args>
        inline void pack( Stream& s, const boost::multiprecision::number<Args...>& d ) {
           s.write( (const char*)&d, sizeof(d) );
        }

        template<typename Stream, typename... Args>
        inline void unpack( Stream& s, boost::multiprecision::number<Args...>& u ) {
           s.read( (const char*)&u, sizeof(u) );
        }
        */
    }
}

#include <fc/variant.hpp>

namespace fc {
    template<typename Storage = std::pair<uint64_t, uint64_t>>
    void to_variant(const fixed_string<Storage> &s, variant &v) {
        v = fc::to_string(s);
    }

    template<typename Storage = std::pair<uint64_t, uint64_t>>
    void from_variant(const variant &v, fixed_string<Storage> &s) {
        s = fc::to_fixed_string<Storage>(v.as_string());
    }
}