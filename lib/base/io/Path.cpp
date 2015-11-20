/* vim: set ts=4 sw=4 tw=79 et :*/
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Path.h"

std::string Path::normalize(const std::string aPath) {
    std::string copy = aPath;
    normalizeInPlace(&copy);
    return copy;
}

void Path::normalizeInPlace(std::string* aPath) {

    if (aPath->empty()) {
        aPath->push_back('.');
        return;
    }

    uint32_t length = aPath->size();
    uint32_t index = 0;
    char /* byte */ sawDots = 0;

    while(index < length) {
        char currentChar = aPath->at(index);

        if (currentChar == '/' or currentChar == '\\') {
            if (sawDots == 2) {
                length -= 3;
                index -= 2;
                aPath->erase(index, 3);
            }
        }

        if (currentChar == '.') {
            sawDots = 0;
        } else {
            sawDots++;
        }

        index++;
    }

    return;
}

bool Path::isAbsolute(const std::string aPath) {
    if (aPath.empty()) {
        return false;
    }

    const char* data = aPath.data();

    #ifdef POSIX

    return data[0] == '/';

    #else

    // On Windows an absolute path (or more accurately, non-relative path) is something
	// that falls under any one of these definitions:
    // 1. A UNC name of any format, which always start with two backslash
    // characters ("\\").
    // 2. A disk designator with a backslash, for example "C:\" or "d:\".
    // 3. A single backslash, for example, "\directory" or "\file.txt". This is
    // also referred to as an absolute path.
    // (https://msdn.microsoft.com/en-gb/library/windows/desktop/aa365247%28v=vs.85%29.aspx#paths)

    // This covers 1 and 3, every path that begins with a '\' must be absolute
    // - we're not verifying validity of paths here..
    if (data[0] == '\\') {
        return true;
    }

    // This is safe to do - we've already checked that the string isn't empty so
    // we know that data[0] has a real character, and data[1] will be at least
    // a NULL terminator so we won't read bad memory.
    if (data[0] == '/' && data[1] == '/') {
        return true;
    }

    // By this point, all absolute paths need to be qualified by at least 3
    // characters

    if (aPath.size() >= 3) {

        // TODO: Do we really need to verify the drive letter is correct?
        if ((data[0] >= 'a' && data[0] <= 'z') ||
            (data[0] >= 'A' && data[0] <= 'Z')) {

            return data[1] == ':' && (data[2] == '\\' || data[2] == '/') ;
        }
    }

    return false;
    #endif
}
