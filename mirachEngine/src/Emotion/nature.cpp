#include "nature.hpp"

const NatureWeight& Nature::GetNatureWeight(const std::string& name) 
{
    static const NatureWeight defaultNW{}; 
    auto it = m_natures.find(name);
    return (it == m_natures.end()) ? defaultNW : it->second;
}

void Nature::CreateNature(const std::string& name,
    float Pp, float Pn, float Ap, float An, float Dp, float Dn)
{
    m_natures[name] = NatureWeight{ Pp, Pn, Ap, An, Dp, Dn };
}