#include "Concept.hpp"

Concept::Concept(const std::string& k, const std::vector<ARule *>& vals)
    : key(k), values(vals) {}

Concept::~Concept() {}