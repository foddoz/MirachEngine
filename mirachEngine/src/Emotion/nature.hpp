#pragma once
#include <string>
#include <unordered_map>
#include <map>

struct NatureWeight
{
    float Ppos, Pneg;
    float Apos, Aneg;
    float Dpos, Dneg;
};

class Nature 
{
public:
    Nature() = default;

    static const NatureWeight& GetNatureWeight(const std::string& name);

    static void CreateNature(const std::string& name,
        float Pp, float Pn, float Ap, float An, float Dp, float Dn);
private:
    inline static std::unordered_map<std::string, NatureWeight> m_natures;
};