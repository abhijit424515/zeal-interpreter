#ifndef BASE_HH
#define BASE_HH

#include <map>
#include <memory>
#include <argp.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

void repl_print(const string& s);

template<typename TO, typename FROM>
unique_ptr<TO> static_unique_ptr_cast (unique_ptr<FROM>&& old) {
    return unique_ptr<TO>{static_cast<TO*>(old.release())};
}

#endif