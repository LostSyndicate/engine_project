//
// Created by losts_n1cs3jj on 11/1/2022.
//

#ifndef SM3_REFERENCE_H
#define SM3_REFERENCE_H

#include "Defs.h"

class Reference
{
public:
    static int ReportActiveReferences();
    
    Reference();
    virtual ~Reference();

#ifdef GENERATE_CLASS_NAMES
    virtual const std::string& GetClassName() const;
#endif
    
    void Ref();
    void Unref();
    int GetReferenceCount() const;
private:
#ifdef STORE_ACTIVE_REFS
    Reference* ref_next_{nullptr};
    Reference* ref_prev_{nullptr};
#endif
    int ref_count_{1};
};

#endif //SM3_REFERENCE_H
