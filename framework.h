#pragma once
#include "PDIFParser.h"
#include "SCHParser.h"

#include <msclr\marshal_cppstd.h>
#include <string>
#include <codecvt>


using namespace System;

void convertString(String^ string, std::string& stdstr);