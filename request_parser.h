#pragma once

#include "httpdef.h"

int request_parse(const char *req, struct httprequest *ret);
