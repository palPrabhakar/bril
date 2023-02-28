#ifndef __FORM_BLOCK_H__
#define __FORM_BLOCK_H__

#include "../json.hpp"
using json = nlohmann::json;

const std::string JMP = "jmp";
const std::string BR = "br";
const std::string RET = "ret";

json get_blocks(json &f);

json get_named_blocks(json &f);

#endif
