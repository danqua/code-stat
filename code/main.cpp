#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

struct file_info
{
    int file_count;
    int line_count;
    int comment_count;
    int blank_count;
};

static file_info info_header;
static file_info info_implementation;

static std::string trim_whitespace(const std::string& str);
static void read_file(const std::filesystem::path& file);
static void read_directory(const std::filesystem::path& directory);
static void handle_file_types(const std::filesystem::path file_path);

int main(int arg_count, char** args)
{
    if (arg_count < 2) return 0;

    printf("--------------------------------------------------------------------------------\n");
    printf("%s %12s %15s %15s %26s\n", "Language", "Files", "Blanks", "Comments", "Lines");
    printf("--------------------------------------------------------------------------------\n");

    for (int arg_index = 1; arg_index < arg_count; arg_index++)
    {
        std::string arg = args[arg_index];
        std::filesystem::path path(arg);
        handle_file_types(path);
    }

    if (info_header.file_count > 0)
    {
        printf("%s %12d %15d %15d %26d\n",
            "Headers ",
            info_header.file_count,
            info_header.blank_count,
            info_header.comment_count,
            info_header.line_count);
    }
    if (info_implementation.file_count > 0)
    {
        printf("%s %12d %15d %15d %26d\n",
            "C/C++   ",
            info_implementation.file_count,
            info_implementation.blank_count,
            info_implementation.comment_count,
            info_implementation.line_count);
    }
    
    printf("--------------------------------------------------------------------------------\n");

    int total_file_count = info_header.file_count + info_implementation.file_count;
    int total_blank_count = info_header.blank_count + info_implementation.blank_count;
    int total_comment_count = info_header.comment_count + info_implementation.comment_count;
    int total_line_count = info_header.file_count + info_implementation.line_count;

    printf("%s %12d %15d %15d %26d\n",
            "Sum     ",
            total_file_count,
            total_blank_count,
            total_comment_count,
            total_line_count);
}

static std::string trim_whitespace(const std::string& str)
{
    const char* whitespace = " \t\n\r\f\v";
    size_t begin = str.find_first_not_of(whitespace);
    if (begin == std::string::npos)
    {
        return std::string{};
    }
    size_t end = str.find_last_not_of(whitespace);
    return std::string{ str.substr(begin, end - begin + 1) };
}

static void handle_file_types(const std::filesystem::path file_path)
{
    std::filesystem::file_status file_status = std::filesystem::status(file_path);
    switch (file_status.type())
    {
        case std::filesystem::file_type::regular:
        {
            read_file(file_path);
        } break;
        case std::filesystem::file_type::directory:
        {
            read_directory(file_path);
        } break;
    }
}

static file_info create_file_info(const std::filesystem::path& path)
{
    std::ifstream fs(path, std::ios::binary);
    std::string buffer;
    file_info result = {};
    bool inside_comment = false;
    
    while (std::getline(fs, buffer))
    {
        std::string line = trim_whitespace(buffer);
        if (line.length() == 0)
        {
            result.blank_count++;
        }
        else
        {
            if (line[0] == '/' && line[1] == '/')
            {
                result.comment_count++;
            }
            else if (line[0] == '/' && line[1] == '*')
            {
                /* Check if the comment is a single lined comment like this one */
                size_t end = line.find_last_of("*/");
                if (end != std::string::npos)
                {
                    result.comment_count++;
                }
                else
                {
                    // Move the file stream pointer until we find the closing comment literal
                    while (std::getline(fs, buffer))
                    {
                        std::string line = trim_whitespace(buffer);
                        size_t end = line.find_last_of("*/");
                    }
                    result.comment_count++;
                }
            }
            else
            {
                result.line_count++;
            }
        }
    }
    fs.close();
    return result;
}

static void update_file_info(const std::filesystem::path& path, file_info* out)
{
    file_info temp_info = create_file_info(path);
    out->line_count += temp_info.line_count;
    out->blank_count += temp_info.blank_count;
    out->comment_count += temp_info.comment_count;
}

static void read_file(const std::filesystem::path& path)
{    
    if (path.extension() == ".h" || path.extension() == ".hpp")
    {
        info_header.file_count++;
        update_file_info(path, &info_header);
    }
    else if(path.extension() == ".c" || path.extension() == ".cpp")
    {
        info_implementation.file_count++;
        update_file_info(path, &info_implementation);
    }
}

static void read_directory(const std::filesystem::path& directory)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        handle_file_types(entry);
    }
}