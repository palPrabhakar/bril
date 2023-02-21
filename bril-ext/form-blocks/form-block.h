#ifndef __FORM_BLOCK_H__
#define __FORM_BLOCK_H__

#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::string JMP = "jmp";
const std::string BR = "br";

json get_blocks(json &f);

#endif
