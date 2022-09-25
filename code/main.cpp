#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

enum class file_type
{
    header,
    implementation
};

struct file_info
{
    file_type type;
    int file_count;
    int line_count;
    int comment_count;
    int blank_count;
};

static file_info info_header;
static file_info info_implementation;

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

static file_info create_file_info(const std::filesystem::path& path, file_type type)
{
    std::ifstream fs(path, std::ios::binary);
    std::string line;

    file_info result = {};
    result.type = type;
    
    while (std::getline(fs, line))
    {
        result.line_count++;
        result.comment_count++;
        result.blank_count++;
    }

    fs.close();
    return result;
}

static void read_file(const std::filesystem::path& file)
{
    std::vector<std::filesystem::path> header_paths;
    std::vector<std::filesystem::path> implementation_paths;
    header_paths.reserve(256);
    implementation_paths.reserve(256);
    
    if (file.extension() == ".h" || file.extension() == ".hpp")
    {
        info_header.file_count++;
        header_paths.push_back(file);
    }
    else if(file.extension() == ".c" || file.extension() == ".cpp")
    {
        info_implementation.file_count++;
        implementation_paths.push_back(file);
    }

    for (const auto& path : header_paths)
    {
        file_info temp_info = create_file_info(path, file_type::header);
        info_header.line_count += temp_info.line_count;
    }

    for (const auto& path : implementation_paths)
    {
        file_info temp_info = create_file_info(path, file_type::implementation);
        info_implementation.line_count += temp_info.line_count;
    }
}

static void read_directory(const std::filesystem::path& directory)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        handle_file_types(entry);
    }
}