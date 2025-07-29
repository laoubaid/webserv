/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_fields.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 23:42:33 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/07/29 02:26:39 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpParsingIncludes.hpp"

void	printvec(const Uvec &vec, int printable)
{
	if (printable)
		std::cout << std::string(vec.begin(), vec.end()) << std::endl;
	else
	{
		for (Uvec::const_iterator it = vec.begin(); it != vec.end(); it++)
		{
			std::cout << "'" << *it << "', ";
		}
		std::cout<< "[終わり]" << std::endl;
	}
}

// Uvec	strToVec(const unsigned char *str, std::size_t len)
// {
// 	return (Uvec(str, str + len));
// }

// add cases for each tested validator with the expected result (true/false)
// -----------------------------cases---------------------------------------
std::vector<std::pair<Uvec, bool>>	targetTestCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"target", 6), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"/", 1), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"//", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"/?", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"/fsf/fsf?($)", 12), true));

	cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"f/", 2), false));

	// Simple paths
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo", 4), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo/bar", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/a/b/c/d/e", 10), true));

// Paths with unreserved characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo-bar", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo.bar", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo_bar", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo~bar", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/12345", 6), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/ABC", 4), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/abc", 4), true));

// Paths with sub-delims
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/!$", 3), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/&'()*", 6), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/+,;=", 5), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/segment!with$sub-delims&and_chars", 34), true));

// Paths with colon and at-sign
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path:with@colon", 16), true));

// Paths with pct-encoded characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%20bar", 10), true)); // space
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%2Fbar", 10), true)); // encoded slash (valid in segment, not as separator)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%2Ebar", 10), true)); // encoded dot
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%41", 7), true)); // encoded 'A'
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%61", 7), true)); // encoded 'a'
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%30", 7), true)); // encoded '0'
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%39", 7), true)); // encoded '9'
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%25", 7), true)); // encoded '%' itself
	
	// Basic queries
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo?bar", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo/bar?query", 14), true));

// Queries with unreserved characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?name=value&param=123", 26), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?_.~-", 10), true));

// Queries with sub-delims
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?!$&'()*+,;=", 17), true));

// Queries with colon and at-sign
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?user:pass@host", 20), true));

// Queries with pct-encoded characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%20value", 20), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%2Fencodedslash", 27), true)); // encoded slash in query
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%3Fencodedquestion", 30), true)); // encoded question mark in query

// Queries containing '/' and '?'
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?query/with?slashes", 24), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?key=/val?ue", 17), true));

// Empty query (valid)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?", 6), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/?", 2), true));

// Empty string
cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));

// Missing leading slash
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo", 3), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo/bar", 7), false));

// Double slashes (not explicitly forbidden by `1*( "/" segment )` but `segment` can be empty which allows `//`. However, it's often considered an invalid *normalized* path. If your implementation normalizes, this might be tricky. Assuming a strict interpretation of `1*("/" segment)` meaning at least one non-empty segment or just a `/` for the root.)
// For strict RFC compliance, "1*( "/" segment )" allows "//", "/a//b". The question is how your function interprets "segment". If segment can be empty, then `//` is valid. If your function is designed to validate *normalized* paths or prevent "dot-segments" for security, then these should be false.
// Let's assume a "strict" interpretation where a segment must contain pchar or it's invalid unless it's just "/". If segment *can* be empty, then "//" is valid.
// For the purpose of these tests, I'll assume that "//" (empty segment) and "/./" or "/../" are invalid or should be normalized away *before* validation.
// Given `segment = *pchar`, an empty segment is valid. So `/a//b` would be `/a/""/b`. Thus, `//` IS VALID per RFC 3986.
// Re-evaluating based on `segment = *pchar`: an empty string is a valid `*pchar`. So `//` corresponds to `/` + empty segment.
// Therefore, `/foo//bar` is technically valid according to the RFC's ABNF for absolute-path.
// If your current function returns false for `//` it's not strictly RFC 3986 compliant for `absolute-path`.
// However, common practice often disallows them or normalizes them.
// Let's add them as 'true' based on strict RFC. If your implementation disallows them, then change these to 'false'.

cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo//bar", 9), true)); // Empty segment, technically valid per RFC 3986 `*pchar`
cases.push_back(std::make_pair(Uvec((const unsigned char *)"//", 2), true)); // Two empty segments, technically valid per RFC 3986 `*pchar`

// Invalid characters in segment (not pchar)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo bar", 8), false)); // space
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo<bar", 8), false)); // less than
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo>bar", 8), false)); // greater than
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo[bar", 8), false)); // left bracket
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo]bar", 8), false)); // right bracket
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo{bar", 8), false)); // left brace
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo}bar", 8), false)); // right brace
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo|bar", 8), false)); // pipe
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo\\bar", 8), false)); // backslash
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo^bar", 8), false)); // caret
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo`bar", 8), false)); // grave accent

// Invalid pct-encoded sequences
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%G1", 7), false)); // non-hexdig
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%1G", 7), false)); // non-hexdig
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%", 5), false)); // incomplete
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%1", 6), false)); // incomplete
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/foo%123", 8), true)); // too long after %

// Invalid characters in query
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=foo bar", 19), false)); // space
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=<value", 19), false)); // less than
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=>value", 19), false)); // greater than
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=[value", 19), false)); // left bracket
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=]value", 19), false)); // right bracket
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param={value", 19), false)); // left brace
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=}value", 19), false)); // right brace
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=|value", 19), false)); // pipe
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=\\value", 19), false)); // backslash
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=^value", 19), false)); // caret
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=`value", 19), false)); // grave accent

// Invalid pct-encoded sequences in query
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%G1", 15), false)); // non-hexdig
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%1G", 15), false)); // non-hexdig
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%", 13), false)); // incomplete
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?param=%1", 14), false)); // incomplete

// Query without a path
cases.push_back(std::make_pair(Uvec((const unsigned char *)"?query", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"?param=value", 12), false));

// Multiple question marks (only one is a separator)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path??query", 12), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?query?again", 17), true));

// Fragment identifier (not allowed in origin-form)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path#fragment", 14), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"/path?query#fragment", 20), false));

// Authority component (not allowed in origin-form)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"//host/path", 11), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"http://host/path", 16), false));

// Asterisk-form (not origin-form)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"*", 1), false));

// Absolute-form (not origin-form)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"http://example.com/path", 23), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"https://example.com/path", 24), false));

	return (cases);
}

std::vector<std::pair<Uvec, bool>>	fieldNameTestCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"field name", 10), true));

// Basic valid field-names
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Host", 4), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Content-Type", 12), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"X-Custom-Header", 15), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"123", 3), true)); // Digits
cases.push_back(std::make_pair(Uvec((const unsigned char *)"A", 1), true));     // Single char ALPHA
cases.push_back(std::make_pair(Uvec((const unsigned char *)"!", 1), true));     // Single char tchar
cases.push_back(std::make_pair(Uvec((const unsigned char *)"~", 1), true));     // Single char tchar

// Field-names with various allowed tchars
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Accept-Encoding", 15), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Content-Length", 14), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"User-Agent", 10), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Cache-Control", 13), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Via", 3), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"If-Match", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"X-Foo!#$^&'*+.-^_`|~", 20), true));

// Empty field-name
cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));

// Contains space
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)" FieldName", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"FieldName ", 10), false));

// Contains HTAB
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\tName", 10), false));

// Contains colon (field-name terminator)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field:Name", 10), false));

// Contains other delimiters not allowed in tchar
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field/Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field?Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field=Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field(Name)", 11), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\"Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\\Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field<Name>", 11), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field@Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field,Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field;Name", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field[Name]", 11), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field{Name}", 11), false));

// Contains control characters (excluding HTAB, which is explicitly allowed in OWS/field-content)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\x01Name", 10), false)); // SOH
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\x00Name", 10), false)); // NUL
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\x07Name", 10), false)); // BEL
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\x1FName", 10), false)); // US
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\x7FName", 10), false)); // DEL
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\rName", 10), false)); // CR
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Field\nName", 10), false)); // LF

	return (cases);
}

std::vector<std::pair<Uvec, bool>>	fieldValueTestCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"field value", 11), true));

	// Empty field-value
cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), true));

// Single field-vchar
cases.push_back(std::make_pair(Uvec((const unsigned char *)"A", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"1", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"!", 1), true));

// Multiple field-vchars
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Hello", 5), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"123.45", 6), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"abcDEfGHIjklMNOpqrSTuvWXYz", 26), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"#$%&'()*+,-./", 13), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)":;<=>?@[\\]^_`{|}~", 16), true));

// Field-values with internal spaces and HTABs (multiple field-content parts or internal to one)
// The key is that field-content starts and ends with field-vchar.
// field-value = *field-content
// field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar ]
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo bar", 7), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo\tbar", 7), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo  bar", 8), true)); // Multiple spaces
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo\t\tbar", 8), true)); // Multiple HTABs
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo \t bar", 9), true)); // Mixed whitespace
cases.push_back(std::make_pair(Uvec((const unsigned char *)"This is a longer field value with mixed characters and numbers 123.", 66), true));

// Values with obs-text characters (bytes 0x80-0xFF)
// NOTE: These are discouraged but technically allowed by RFC 9110
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\xC2\xA2", 7), true)); // Cent sign (UTF-8 for 0xA2)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\xFF", 6), true)); // Single byte 0xFF
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\x80\x90\xA0\xB0\xC0\xD0\xE0\xF0", 8), true)); // Range of obs-text

// Starts with SP or HTAB (due to field-content rule)
cases.push_back(std::make_pair(Uvec((const unsigned char *)" value", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\tvalue", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"  value", 7), false)); // Multiple leading spaces

// Ends with SP or HTAB (due to field-content rule)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"value ", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"value\t", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"value  ", 7), false)); // Multiple trailing spaces

// Contains CR, LF, or NUL (explicitly forbidden by RFC 9110#section-5.5 for field-value)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\rwithCR", 12), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\nwithLF", 12), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\x00withNUL", 12), false));

// Contains other control characters (VCHAR is 33-126, obs-text is 128-255. So 0-31 (excluding HTAB=9) and 127 are excluded from field-vchar)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\x01withSOH", 12), false)); // SOH (0x01)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\x07withBEL", 12), false)); // BEL (0x07)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\x1FwithUS", 11), false));  // US (0x1F)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Value\x7FwithDEL", 12), false)); // DEL (0x7F)
	return (cases);
}

std::vector<std::pair<Uvec, bool>>	contentLengthTestCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"content length", 14), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42, 42", 6), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42  ,  42", 9), true));

	cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42f", 3), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,", 3), false));

// Single digit
cases.push_back(std::make_pair(Uvec((const unsigned char *)"0", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"1", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"9", 1), true));

// Multiple digits
cases.push_back(std::make_pair(Uvec((const unsigned char *)"10", 2), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"123", 3), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"987654321", 9), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"0000", 4), true));

// Two identical values
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,42", 5), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"100,100", 7), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"0,0", 3), true));

// Multiple identical values
cases.push_back(std::make_pair(Uvec((const unsigned char *)"50,50,50", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"12345,12345,12345,12345", 23), true));

// Values with OWS around the comma and numbers
// RFC 9110 Section 5.6.1: "A recipient MAY ignore or remove any empty list members and otherwise treat the list as if it were a single value unless the field definition explicitly states otherwise."
// While Content-Length is not *defined* as a list, the exception implies treating it similarly.
// Standard HTTP list parsing often allows OWS around commas. Let's assume your '1#(1*DIGIT)' rule implies `1#(OWS 1*DIGIT OWS)`.
// If your `1*DIGIT` validation itself disallows whitespace, then these next cases might need to be re-evaluated.
// Assuming your overall Content-Length parser handles this whitespace as per RFC 9110, Section 3.2.3, "Whitespace":
// "A sender MUST NOT generate a header field that contains a line feed (LF) or carriage return (CR) unless it is within a quoted string or a comment, neither of which is allowed in Content-Length.
// A recipient parsing a header field value that contains a linear whitespace (LWS) MUST either reject the message as invalid or replace any LWS with a single SP before further processing or forwarding the message."
// OWS is `*(SP / HTAB)`. This applies to the *entire* field-value, and also to list delimiters.

cases.push_back(std::make_pair(Uvec((const unsigned char *)"42, 42", 6), true)); // Space after comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42 ,42", 6), true)); // Space before comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42 , 42", 7), true)); // Space around comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,\t42", 6), true)); // HTAB after comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42\t,\t42", 7), true)); // HTAB around comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,  42", 7), true)); // Multiple spaces after comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"  42  ,  42  ", 13), true)); // OWS around numbers and commas

// Empty string
cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));

// Contains non-digit characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"abc", 3), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"123a", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"12.3", 4), false));

// Contains signs
cases.push_back(std::make_pair(Uvec((const unsigned char *)"-123", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"+123", 4), false));


// Mixed with valid/invalid individual numbers
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,abc", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"abc,42", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,-42", 6), false));

// Empty list members (if your 1# rule for numbers means "at least one digit" per item)
// Note: RFC 9110 Section 5.6.1 says "A recipient MAY ignore or remove any empty list members".
// So, if your parser "ignores" an empty member, then "42,,42" might become "42,42" which is valid.
// For a strict validation of the *input string*, it's good to test these as invalid if you're not explicitly ignoring empty members.
// If your function cleans up/normalizes first, then it might return true for some of these.
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,,42", 6), false)); // Empty member
cases.push_back(std::make_pair(Uvec((const unsigned char *)",42", 3), false));    // Leading comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,", 3), false));    // Trailing comma
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42, ", 4), false));   // Trailing comma with space

// Invalid characters where digits should be
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,Foo", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42, 4X", 6), false));

// Contains CR/LF/NUL (always invalid in header values)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42\r,42", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,\n42", 6), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,\x0042", 6), false));

	return (cases);
}

std::vector<std::pair<std::string, bool>>	acceptTestCases(void)
{
	std::vector<std::pair<std::string, bool>>	cases;
	
	cases.push_back(std::make_pair("", true));
	cases.push_back(std::make_pair("*/*", true));
	
	return (cases);
}

std::vector<std::pair<Uvec, bool>>	IPv6addressCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"IPv6 address", 12), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:09af:09af", 39), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:255.255.255.255", 45), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af::", 6), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"::", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"::255.255.255.255", 17), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"::09af:09af", 11), true));

	cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af1::", 7), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09ag::", 6), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"255.255.255.255", 15), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:255.255.255.255", 25), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:255.255.255.255:09af:255.255.255.255", 66), false));
	
	
	// --- Valid Cases ---

    // Full address (8 h16 segments)
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"2001:0db8:85a3:0000:0000:8a2e:0370:7334", 39), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"0:0:0:0:0:0:0:0", 15), true)); // All zeros
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 39), true)); // All max h16

    // Addresses with '::' shortening
    // "::" 5( h16 ":" ) ls32
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::1:2:3:4:5:6", 13), true)); // Corresponds to 0:0:1:2:3:4:5:6
    // [ h16 ] "::" 4( h16 ":" ) ls32
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1::2:3:4:5:6", 12), true)); // Corresponds to 1:0:0:2:3:4:5:6
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2::3:4:5:6", 12), true)); // Corresponds to 1:2:0:0:3:4:5:6
    // [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3::4:5:6", 12), true)); // Corresponds to 1:2:3:0:4:5:6
    // [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4::5:6", 12), true)); // Corresponds to 1:2:3:4:0:5:6
    // [ *3( h16 ":" ) h16 ] "::" h16 ":" ls32
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5::6", 12), true)); // Corresponds to 1:2:3:4:5:0:0:6
    // [ *4( h16 ":" ) h16 ] "::" ls32
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5::127.0.0.1", 20), true)); // With IPv4-mapped
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5::7:8", 14), true)); // Corresponds to 1:2:3:4:5:6:0:7:8 (INVALID, too many h16 if this implies 6 + 2 = 8. This should be a full address, so :: means 0 segments in between)
    //  Correction for 6(h16":")ls32 & shortening:
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6::", 13), true)); // Corresponds to 1:2:3:4:5:6:0:0
    // [ *5( h16 ":" ) h16 ] "::" h16
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6:7::", 15), true)); // Should be 1:2:3:4:5:6:7:0.
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5::7", 12), true)); // Corresponds to 1:2:3:4:5:0:0:7
    // [ *6( h16 ":" ) h16 ] "::"
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6:7::", 15), true)); // This form should mean 1:2:3:4:5:6:7:0. Already covered above.
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6::", 13), true)); // This form means 1:2:3:4:5:6:0:0.

    // Specific cases from your example
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:09af:09af", 39), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:255.255.255.255", 45), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af::", 6), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::", 2), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::255.255.255.255", 17), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::09af:09af", 11), true));

    // IPv4-mapped addresses (ls32 as IPv4address)
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"0:0:0:0:0:0:192.168.1.1", 23), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::192.168.1.1", 13), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"ffff::192.168.1.1", 17), true));

    // h16 examples
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1::", 3), true)); // Single h16, then ::
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"a:b:c:d:e:f:1:2", 15), true)); // Hex digits, single character
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"0000:0000:0000:0000:0000:0000:0000:0000", 39), true)); // Leading zeros

    // --- Invalid Cases ---

    // Too many h16 segments (more than 8 when expanded)
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6:7:8:9", 17), false)); // 9 h16 segments
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6:7::8", 16), false)); // 7 h16 + :: + 1 h16 (would expand to 9)

    // More than one '::'
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1::2::3", 7), false));

    // Invalid characters
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09ag::", 6), false)); // 'g' is not HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:X:5:6:7:8", 15), false)); // 'X' is not HEXDIG

    // Incomplete segments
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:3:4:5:6:7:", 14), false)); // Trailing colon
    cases.push_back(std::make_pair(Uvec((const unsigned char *)":1:2:3:4:5:6:7", 14), false)); // Leading colon, not '::'
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:2:::", 6), false)); // Triple colon

    // Invalid IPv4address in ls32 context
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::256.0.0.0", 11), false)); // Invalid octet (256)
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::1.2.3.4.5", 11), false)); // Too many octets
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::1.2.3", 7), false)); // Too few octets
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"::1.02.3.4", 10), false)); // Leading zero in octet (not allowed by dec-octet)

    // Empty string
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));

    // Partial addresses that are not valid forms
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1:", 2), false));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)":1", 2), false));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af1::", 7), false)); // h16 too long
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"255.255.255.255", 15), false)); // Just an IPv4, not an IPv6
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:255.255.255.255", 25), false)); // Not 6 h16 + IPv4 (this implies 2 h16 + IPv4. The grammar only allows 6(h16":")ls32 or :: + up to 5 h16 + ls32)
                                                                                                        // It should be 6 h16 segments before the ls32, or use "::" for shortening.
    // Example: "09af:09af:09af:09af:09af:09af:255.255.255.255" is valid (6 h16 + ls32)
    // Example: "09af:09af::255.255.255.255" would be valid (2 h16 + :: + ls32, implying 4 zero h16s in between)

    cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:255.255.255.255:09af:255.255.255.255", 66), false)); // Too long, too many segments


	return (cases);
}

std::vector<std::pair<Uvec, bool>>	IPvFuturesCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"IPv Futures", 11), true));

	// cases.push_back(std::make_pair(Uvec((const unsigned char *)"v0f.:", 5), true));
	
	// cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));


	// --- Valid Cases ---

    // Minimum valid
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.a", 4), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"VF.z", 4), true)); // 'V' for case-insensitivity
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v0.0", 4), true));

    // Various HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"vabc.def", 8), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"vDEADBEEF.beef", 14), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"VA.test", 7), true)); // Mixed case for HEXDIG and 'V'

    // Characters after '.' (unreserved / sub-delims / :)
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.abcDEF", 9), true));    // ALPHA
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.0123456789", 13), true)); // DIGIT
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.-._~", 7), true));     // Other unreserved
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.!$'()*+,;=", 13), true)); // sub-delims
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.:", 4), true));        // Colon
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.aB1_~.$!&'()*+,;=:", 21), true)); // Mix of all allowed
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"V1.Foo-Bar_Baz~", 15), true)); // Another mixed valid case with 'V'

    // With multiple characters after '.'
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v0f.:", 5), true)); // Your existing valid case

	// Other malformed strings / Ambiguous cases requiring strict parsing
    // While '.' is an 'unreserved' character, the grammar implies a single
    // structural dot as a separator. These cases test strict adherence.
	// but since I am strictly following the rfc, I will accept it
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1..a", 5), true)); // Extra dot acting as separator
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.a.b", 6), true)); // Extra dot acting as separator

    // --- Invalid Cases ---

    // Missing 'v' (or 'V')
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"1.a", 3), false));    // Missing 'v' entirely
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"x1.a", 4), false));   // Incorrect starting character

    // Missing HEXDIG after 'v' (or 'V')
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v.a", 3), false));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"V.a", 3), false));

    // Non-HEXDIG after 'v' (or 'V') - 1*HEXDIG means at least one HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"vg.a", 4), false));   // 'g' is not HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v_a.a", 5), false));  // '_' is not HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v .a", 4), false));   // Space is not HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v!.a", 4), false));   // '!' is not HEXDIG

    // Missing '.' after HEXDIG
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1a", 3), false));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1", 2), false));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"VF123", 5), false)); // Missing dot with 'V'

    // Missing characters after '.' - 1*(...) means at least one character
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.", 3), false));
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"V1.", 3), false));

    // Invalid characters after '.' (not unreserved, sub-delims, or ':')
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.@", 4), false)); // '@' is not allowed
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1./", 4), false)); // '/' is not allowed
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1. ", 4), false)); // Space is not allowed (other than specific rules)
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v1.\?", 4), false)); // '?' is not allowed

    // Empty string
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));

    // Malformed HEXDIG sequence
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"v12345.a", 8), true)); // Valid: any number of hex digits
    cases.push_back(std::make_pair(Uvec((const unsigned char *)"vFF_FF.a", 8), false)); // Invalid: non-hexdig in hex part
    
	
	return (cases);
}

std::vector<std::pair<Uvec, bool>> HostCases(void) {
    std::vector<std::pair<Uvec, bool>> cases;
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"Host", 4), true));


    // --- 1. Valid Host Values: Comprehensive Coverage ---

    // 1.1 IPv4address (uri-host)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"192.168.1.1", 11), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"0.0.0.0", 7), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"255.255.255.255", 15), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"10.0.0.1", 8), true));

    // 1.2 IPv4address with Port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"192.168.1.1:80", 14), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"10.0.0.1:8080", 13), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"255.255.255.255:65535", 21), true)); // Max port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.4:0", 9), true));    // Min port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.4:", 8), true));     // Empty port (valid by ABNF *DIGIT)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.4:007", 11), true)); // Port with leading zeros (valid by ABNF *DIGIT)


    // 1.3 IP-literal (IPv6address)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::1]", 5), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[2001:db8::1]", 13), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::]", 4), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[0:0:0:0:0:0:0:0]", 17), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]", 41), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::192.168.1.1]", 15), true)); // IPv4-mapped, '::' replaces 6 h16s
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[1:2:3:4:5::192.168.1.1]", 24), true)); // Valid '::' + IPv4-mapped

    // 1.4 IP-literal with IPv6address and Port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[2001:db8::1]:443", 17), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::1]:8080", 10), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::192.168.1.1]:", 16), true)); // Empty port for IPv6

    // 1.5 IP-literal (IPvFuture)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[v1.foo]", 8), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[VFF.test.data]", 15), true)); // Case-insensitive 'V'
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[v1.a.b.c]", 10), true));       // Dots in address part are valid chars
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[vFFFF.test-data_long~:$!&'()*+,;=]", 35), true)); // Max allowed chars in IPvFuture data

    // 1.6 IP-literal with IPvFuture and Port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[v1.foo]:80", 11), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[VFF.test.data]:443", 19), true));

    // 1.7 reg-name (uri-host)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com", 11), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"localhost", 9), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"sub-domain.example.com", 22), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"_underscore.com", 15), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"foo!bar$baz&qux'(.)*+,;=", 24), true)); // All sub-delims
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"%20host%20name", 14), true)); // pct-encoded
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"", 0), true)); // Empty reg-name (valid by ABNF *(...)) - although host should not be empty.
                                                                           // Host rule is `uri-host [ ":" port ]`.
                                                                           // `uri-host` can be `reg-name`. `reg-name` can be empty.
                                                                           // Thus, an empty string `""` *is* a valid `Host` according to ABNF.
                                                                           // This is usually unexpected by users but technically correct per RFC.

    // 1.8 reg-name with Port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"www.example.org:80", 18), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"myhost:8080", 11), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"sub.domain.tld:65535", 20), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"another.host:", 13), true)); // Empty port for reg-name
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"%25test%25:1234", 15), true)); // pct-encoded reg-name with port

    // 1.9 Ambiguous cases that *are* valid reg-names but look like invalid IPv4/IPv6
    // These test if the parser correctly falls through to reg-name.
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"256.0.0.0", 9), true));   // Invalid IPv4, but VALID reg-name
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3", 5), true));     // Invalid IPv4, but VALID reg-name (too few octets)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.04", 8), true));   // Invalid IPv4 (leading zero), but VALID reg-name
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.a", 7), true));    // Invalid IPv4 (non-digit), but VALID reg-name (a is unreserved)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1:2:3:4:5:6:7:8:9", 17), false)); // Invalid IPv6 (too many h16s)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1:2:3:G:5:6:7:8", 15), false)); // Invalid IPv6 (non-hex in h16)
    // With port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"256.0.0.0:80", 12), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3:443", 9), true));
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1:2:3:G:5:6:7:8:443", 19), false));


    // --- 2. Invalid Host Values: Targeting Specific Failures ---

    // 2.1 General Structural Failures of Host = uri-host [":" port]
    cases.push_back(std::make_pair(Uvec((const unsigned char*)":80", 3), true));        // Port without uri-host
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host::80", 8), false));   // Double colon separator (not part of valid Host or port)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com:80:invalid", 22), false)); // Extra colon/data after port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::1]:80:81", 11), false));  // Multiple colons with IP-literal
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.4:80:81", 13), false)); // Multiple colons with IPv4
    cases.push_back(std::make_pair(Uvec((const unsigned char*)" host.com", 9), false));   // Leading space
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host.com ", 9), false));   // Trailing space
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host.com :80", 12), false)); // Space before colon
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host.com: 80", 12), false)); // Space after colon (before port digits)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host.com:80 ", 12), false)); // Space after port digits
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"", 0), true));           // Empty string (URI Host must be valid, but reg-name empty is valid as a URI Host)
                                                                                       // Re-thinking: "Empty string (valid by ABNF *(...))" for reg-name implies "" is a valid reg-name.
                                                                                       // If "" is a valid reg-name, and Host = uri-host [ ":" port ], then "" (empty string)
                                                                                       // *is* a valid Host. This is a common point of confusion.
                                                                                       // If your *application* requires a non-empty host, this would be false.
                                                                                       // Based purely on the ABNF you provided, it's TRUE.
                                                                                       // I'll keep it as FALSE, as empty hosts are rare and usually treated as invalid in practice.
                                                                                       // If your application permits "", mark it TRUE.

    // 2.2 Invalid Port values (when Host part is valid)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com:abc", 15), false)); // Non-digit port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::1]:abc", 9), false));      // Non-digit port for IP-literal
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.4:abc", 11), false));  // Non-digit port for IPv4
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com:+80", 15), false)); // Plus sign
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com:-80", 15), false)); // Minus sign
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com:80.", 15), false)); // Dot after port
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com:80/path", 19), false)); // Path after port


    // 2.3 Invalid IP-literal structures (cannot be reg-name due to illegal chars like `[` or `]`)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::1", 4), false));       // Missing closing bracket
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[v1.foo", 7), false));   // Missing closing bracket
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"v1.foo]", 7), false));   // Missing opening bracket (and not a reg-name because of `]`)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[invalid_ipv6_syntax]", 21), false)); // Contains invalid IPv6 elements (like `_`) that aren't also allowed in reg-name inside brackets
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[vG.foo]", 8), false));   // Invalid IPvFuture (non-HEXDIG in version)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[v1.]", 5), false));     // Invalid IPvFuture (missing address chars)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[1.2.3.4]", 9), false));   // IPv4 address inside IP-literal brackets (CRITICAL: this is NOT a valid IP-literal)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[]", 2), false));         // Empty IP-literal (neither IPv6 nor IPvFuture)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[[::1]]", 7), false));   // Double brackets
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::1]trailing", 13), false)); // Trailing data after IP-literal


    // 2.4 Invalid reg-name (contains chars not in unreserved/pct-encoded/sub-delims)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"example.com/", 12), false));  // `/` (gen-delim)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host name", 9), false));     // Space
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host@", 5), false));        // `@` (gen-delim)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host?query", 10), false));   // `?` (gen-delim)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host#fragment", 13), false)); // `#` (gen-delim)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host[bracket]", 13), false)); // `[` (gen-delim)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host]", 5), false));         // `]` (gen-delim)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host:80", 7), true)); // This is valid (host then port)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"host:name", 9), false)); // Colon not allowed in reg-name itself, only as host/port separator
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"bad%2G.com", 10), false)); // Malformed pct-encoded
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"%G0.com", 7), false)); // Malformed pct-encoded
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"%20host%20name:80", 17), true)); // Valid pct-encoded reg-name with port

    // 2.5 Tests for Specific Rule Interaction / Order in uri-host (IP-literal / IPv4address / reg-name)
    // The parser should attempt IP-literal first, then IPv4address, then reg-name.
    // If a string could parse as more than one, the first one in the / takes precedence.

    // A string that *is* an IPv4address should parse as IPv4address, not reg-name.
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"1.2.3.4", 7), true)); // Should be IPv4address

    // A string that is valid IP-literal should be parsed as IP-literal, not reg-name.
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[::]", 4), true)); // Should be IP-literal

    // A string that looks like an invalid IPv6/IPv4 but is a valid reg-name (tested above, confirm logic)
    cases.push_back(std::make_pair(Uvec((const unsigned char*)"[fe80::1%eth0]", 14), false)); // If your IPv6address grammar doesn't allow %, this is invalid.
                                                                                               // If it's valid, it's valid IP-literal. If not, it's invalid Host.


    return cases;
}


std::vector<std::pair<Uvec, bool>> HexdigCases(void) {
    std::vector<std::pair<Uvec, bool>> cases;
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"Hexdig", 6), true));

// Single hex digit
cases.push_back(std::make_pair(Uvec((const unsigned char *)"0", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"9", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"A", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"F", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"a", 1), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"f", 1), true));

// Multiple hex digits
cases.push_back(std::make_pair(Uvec((const unsigned char *)"10", 2), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"FF", 2), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"abcd", 4), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"ABCDEF", 6), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"deadbeef", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"0123456789ABCDEFabcdef", 22), true)); // All hex digits
cases.push_back(std::make_pair(Uvec((const unsigned char *)"CAFE123BEEF", 11), true));

// Empty string
cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));

// Contains non-hex characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"G", 1), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"g", 1), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"123Z", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"Z123", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"FF.FF", 5), false)); // Decimal point
cases.push_back(std::make_pair(Uvec((const unsigned char *)"FF,FF", 5), false)); // Comma

// Contains spaces or other whitespace
cases.push_back(std::make_pair(Uvec((const unsigned char *)"123 ", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)" 123", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"1 23", 4), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"AB\tCD", 5), false)); // HTAB
cases.push_back(std::make_pair(Uvec((const unsigned char *)"AB\rCD", 5), false)); // CR
cases.push_back(std::make_pair(Uvec((const unsigned char *)"AB\nCD", 5), false)); // LF

// Contains signs
cases.push_back(std::make_pair(Uvec((const unsigned char *)"-FF", 3), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"+FF", 3), false));

// Other non-hex characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"!", 1), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"?", 1), false));



	return cases;
}

std::vector<std::pair<Uvec, bool>> transferEncodingCases(void) {
    std::vector<std::pair<Uvec, bool>> cases;
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"transfer-encoding", 17), true));

// Single transfer-coding
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked", 7), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"gzip", 4), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"compress", 8), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"deflate", 7), true));

// Multiple transfer-codings in a list
cases.push_back(std::make_pair(Uvec((const unsigned char *)"gzip, chunked", 13), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"compress,deflate", 16), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo, bar, baz", 13), true));

// With OWS around commas
cases.push_back(std::make_pair(Uvec((const unsigned char *)"gzip , chunked", 14), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"gzip ,  chunked", 15), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"gzip\t,\tchunked", 14), true));

// Single parameter, token value
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked;q=1.0", 13), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param=value", 15), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"bar;version=2", 13), true));

// Single parameter, quoted-string value
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;name=\"value\"", 16), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"bar;desc=\"a long value with spaces\"", 35), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"baz;key=\"\"", 10), true)); // Empty quoted string

// Parameters with OWS (BWS effectively)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked ; q = 1.0", 17), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo; param = \"value\"", 20), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"bar;k =\"v\"", 10), true));

// Multiple parameters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked;foo=bar;baz=\"qux\"", 25), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"gzip;a=b;c=\"d\";e=f", 18), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"custom; param1 = value1 ; param2 = \"value 2\"", 44), true));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"token;p1=v1;p2=v2;p3=v3", 23), true));

// Quoted string with qdtext
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"\"", 2), false)); // Empty quoted string
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"abcDEF123\"", 11), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~\"", 33), false)); // All VCHARs except " and
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with spaces and\t tabs\"", 29), false));

// Quoted string with quoted-pair
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \\\" quote\"", 21), false)); // Escaped DQUOTE
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \\\\ backslash\"", 25), false)); // Escaped backslash
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"\\!\\\"\\#\\$\\%\\&\\'\\(\\)\\*\\+\\,\\-\\.\\/\\:\\;\\<\\=\\>\\?\\@\\[\\\\\\]\\^\\_\\`\\{\\|\\}\\\"", 65), false)); // Escaping all VCHARs that might need it, and some that don't, but are allowed by quoted-pair.
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value\\ with\\ spaces\"", 21), false)); // Escaping spaces
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value\\twith\\ttabs\"", 19), false)); // Escaping tabs

// Quoted string with obs-text (bytes 0x80-0xFF)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value\xC2\xA2obs\"", 12), false)); // UTF-8 for cent sign (0xA2)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value\xFFobs\"", 11), false)); // Single byte 0xFF
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value\\\xFFobs\"", 12), false)); // Escaped obs-text

// Empty token for transfer-coding
cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)",chunked", 8), false)); // Empty first token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked,", 8), false)); // Empty last token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked,,gzip", 13), false)); // Empty middle token

// Token with invalid characters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo bar", 7), false)); // Space in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo/bar", 7), false)); // Slash in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo(bar)", 8), false)); // Parentheses in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo:bar", 7), false)); // Colon in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo=bar", 7), false)); // Equals in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo\tbar", 7), false)); // Tab in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo\rbar", 7), false)); // CR in token
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo\nbar", 7), false)); // LF in token

// Missing equals sign
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked;paramvalue", 18), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param", 9), false));

// Missing parameter value
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param=", 10), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param=;", 11), false)); // Empty value followed by another semicolon

// Invalid characters in parameter name (token rules apply)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param name=value", 20), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param/name=value", 20), false));

// Missing DQUOTE
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;name=\"value", 15), false)); // Missing closing DQUOTE
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;name=value\"", 15), false)); // Missing opening DQUOTE
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;name=\"val\"ue", 16), false)); // DQUOTE in middle, not escaped

// Unescaped DQUOTE or backslash inside quoted string
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \" quote\"", 20), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \\ backslash\"", 24), false)); // Single backslash

// Invalid characters in qdtext (characters that MUST be quoted-pair'ed)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \x00 NUL\"", 18), false)); // NUL
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \r CR\"", 17), false));  // CR
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \n LF\"", 17), false));  // LF
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \x01 SOH\"", 18), false)); // Other CTL
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \x7F DEL\"", 18), false)); // DEL

// Invalid quoted-pair (backslash not followed by allowed character)
// quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \\\"", 14), false)); // Backslash at end
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \\\x00\"", 17), false)); // Escaping NUL
cases.push_back(std::make_pair(Uvec((const unsigned char *)"\"value with \\\x07\"", 17), false)); // Escaping BEL

// Missing semicolon between parameters
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked;q=1.0foo=bar", 20), false));

// Invalid whitespace/characters at unexpected positions
cases.push_back(std::make_pair(Uvec((const unsigned char *)" chunked", 8), true)); // Leading space
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked ", 8), true)); // Trailing space
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked; param = value ;", 24), false)); // Trailing semicolon with OWS

// Contains CR/LF/NUL (at top level)
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked\r", 8), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked\n", 8), false));
cases.push_back(std::make_pair(Uvec((const unsigned char *)"chunked\x00", 8), false));

// Complex invalid combinations
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;name=\"val\nue\"", 17), false)); // LF inside quoted string, not escaped
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param=value,another", 23), true)); // Comma where semicolon expected
cases.push_back(std::make_pair(Uvec((const unsigned char *)"foo;param=\"value\"bar", 20), false)); // Missing semicolon after quoted string parameter

	return cases;
}


// -------------------------------------------------------------------------

bool	testCases(std::vector<std::pair<Uvec, bool>> cases, bool (*validator)(const Uvec &))
{
	std::vector<std::pair<Uvec, bool>>::iterator	it = cases.begin();
	int	valid = 1;

	std::cout << "----------------- testing " << std::string((*it).first.begin(), (*it).first.end()) << " -----------------\n";
	it++;
	for (; it != cases.end(); it++)
	{
		bool	implemented_ouput = validator((*it).first);
		if (implemented_ouput == (*it).second)
		{
			std::cout << GREEN_COLOR << "Case " << it - cases.begin() <<": " << std::string((*it).first.begin(), (*it).first.end()) << " is ";
			if (implemented_ouput)
				std::cout << "true";
			else
				std::cout << "false";
			std::cout << " | valid ✅" << RESET_COLOR << std::endl;
		}
		else
		{
			std::cout << RED_COLOR << "Case " << it - cases.begin() <<": " << std::string((*it).first.begin(), (*it).first.end()) << " is ";
			if (implemented_ouput)
				std::cout << "true";
			else
				std::cout << "false";
			std::cout << " | invalid ❌" << RESET_COLOR << std::endl;
			valid = 0;
		}
	}
	std::cout << "\n\n\n";
	if (valid)
		return (true);
	return (false);
}

bool	testAllFields(void)
{
	std::vector<std::pair<std::vector<std::pair<Uvec, bool>>, validatorFunc>> cases;
	std::vector<std::pair<std::vector<std::pair<Uvec, bool>>, validatorFunc>>::iterator	it;

	// add test cases (("case", true or not) , validator result true or not) testCases compare expected with validator's result
	// ---------------------------------adding cases-------------------------------------
	cases.push_back(std::make_pair(targetTestCases(), validateTarget));
	cases.push_back(std::make_pair(fieldNameTestCases(), validateFieldName));
	cases.push_back(std::make_pair(fieldValueTestCases(), validateFieldValue));
	cases.push_back(std::make_pair(contentLengthTestCases(), validateContentLength));
	cases.push_back(std::make_pair(IPv6addressCases(), testIPv6address));
	cases.push_back(std::make_pair(IPvFuturesCases(), testIPvFuture));
	cases.push_back(std::make_pair(HostCases(), validateHost));
	cases.push_back(std::make_pair(HexdigCases(), validateHexDigit));
	cases.push_back(std::make_pair(transferEncodingCases(), validateTransferEncoding));
	// ----------------------------------------------------------------------------------


	for (it = cases.begin(); it != cases.end(); it++)
	{
		if (!testCases((*it).first, (*it).second))
			return(false);
	}
	return (true);
}
