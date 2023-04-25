//
// Created by losts_n1cs3jj on 11/1/2022.
//

#include "Core/Reference.h"

#include <typeinfo>
#include <cstdio>

#ifdef STORE_ACTIVE_REFS
static Reference* s_ref_last = nullptr;
static Reference* s_ref_first = nullptr;
static int s_ref_count = 0;
#endif

int Reference::ReportActiveReferences()
{
#ifdef STORE_ACTIVE_REFS
    int count = 0;
    Reference* ref = s_ref_first;
    while (ref != nullptr)
    {
        if (count == 0)
            printf("Reference::ReportActiveReferences(): Listing references...\n");
    
        printf("Warning! Active Reference: %s[%p] at index %d\n",
#ifdef GENERATE_CLASS_NAMES
               ref->GetClassName().c_str()
#else
               typeid(*ref).name()
#endif
               , ref, count);
        ref = ref->ref_next_;
        count++;
    }
    if (count == 0)
        printf("Reference::ReportActiveReferences(): Did not find any active references.\n");
    return count;
#else
    return 0;
#endif
}

Reference::Reference()
{
    ++s_ref_count;
#ifdef STORE_ACTIVE_REFS
    if (s_ref_first == nullptr)
        s_ref_first = this;
    ref_prev_ = s_ref_last;
    if (s_ref_last != nullptr)
        s_ref_last->ref_next_ = this;
    s_ref_last = this;
#endif
}

Reference::~Reference()
{
#ifdef STORE_ACTIVE_REFS
    if (ref_prev_ != nullptr)
        ref_prev_->ref_next_ = ref_next_;
    if (ref_next_ != nullptr)
        ref_next_->ref_prev_ = ref_prev_;
    if (s_ref_first == this)
        s_ref_first = ref_next_;
    if (s_ref_last == this)
        s_ref_last = ref_prev_;
#endif
    --s_ref_count;
}

#ifdef GENERATE_CLASS_NAMES
const std::string& Reference::GetClassName() const
{
    static const std::string name{"Reference"};
    return name;
}
#endif

void Reference::Ref()
{
    ++ref_count_;
}

void Reference::Unref()
{
    if (--ref_count_ <= 0)
        delete this;
}

int Reference::GetReferenceCount() const
{
    return ref_count_;
}
