#ifndef __COLORFORMAT_HPP__
#define __COLORFORMAT_HPP__
#ifndef BEGIN_NAMESPACE
#include <namespacer.h>
#endif
BEGIN_NAMESPACE
enum ColorFormatCode : unsigned {
	ARGB = 0x03000102,
	RGBA = 0x00010203,
	BGRA = 0x02010003,
	ABGR = 0x03020100
};
typedef union ColorFormat {
	const ColorFormatCode code;
	struct ChannelIndices {
		const char r;
		const char g;
		const char b;
		const char a;
	} const index;
} ColorFormat;
ENDOF_NAMESPACE
#endif