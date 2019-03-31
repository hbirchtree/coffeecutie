#pragma once

#define C_DEFAULT_CONSTRUCTOR(ctype) ctype() = default
#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete
#define C_MOVE_CONSTRUCTOR(ctype) ctype(ctype&& x) = default; ctype& operator=(ctype&& a) = default
