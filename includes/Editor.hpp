#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Cursor.hpp"
#include "Keys.hpp"
#include "Terminal.hpp"
#include "posix/lib.hpp"

#include <cstddef>
#include <sys/ioctl.h>

#include <string>
#include <utility>
#include <filesystem>

class Editor : public Terminal {
public:
    Editor() = default;
    ~Editor();

    // Disable copy and move semantics
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

    static Editor& instance();

    /// Perform low-level keypress reading
    int readKey();

    /// Map keypresses to editor operations
    void processKeypress();

    void refreshScreen() const;

    void open(std::filesystem::path const& path);

private:
    static constexpr std::string_view KILO_VERSION { "0.0.1" };

    Cursor mCursor {};
    posix::winsize_t mWinsize {};

    std::size_t mNumRows {};
    std::string mRowOfText;

    void drawRows(std::string& buffer) const;
};

#endif