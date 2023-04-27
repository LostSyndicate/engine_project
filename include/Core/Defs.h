//
// Created by losts_n1cs3jj on 11/1/2022.
//

#ifndef SM3_DEFS_H
#define SM3_DEFS_H

#include <string>

#define SM_DEPTH_SIZE 24

// define to store active references, which consumes more memory but 
// allows you to print out the references that still are referenced.
#define STORE_ACTIVE_REFS

#define GENERATE_CLASS_NAMES
#   ifdef GENERATE_CLASS_NAMES
#   define CLASS_NAME(class_name)                           \
        const std::string& GetClassName() const override {  \
            static const std::string name{class_name};      \
            return name;                                    \
        }
#else
#   define CLASS_NAME(class_name)
#endif

#endif //SM3_DEFS_H
