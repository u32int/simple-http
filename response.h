#pragma once

#include <stddef.h>
#include "httpdef.h"

char *generate_response(char *str_request, size_t *bytes_total);
