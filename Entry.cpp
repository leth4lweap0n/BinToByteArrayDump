#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <string>

_STD vector<uint8_t> ReadFileContent(const _STD wstring& file_name) {
    _STD ifstream file(file_name, _STD ios::binary);
    if (!file) {
        _STD wcerr << "Error: Unable to open file: " << file_name << _STD endl;
        return {};
    }
    file.seekg(0, _STD ios::end);
    const _STD streampos file_size = file.tellg();
    file.seekg(0, _STD ios::beg);

    _STD vector<uint8_t> bytes(file_size);
    file.read(reinterpret_cast<char*>(bytes.data()), file_size);

    file.close();
    return bytes;
}

void ProcessFolderFiles(const _STD wstring& folder_path, const _STD wstring& output_file) {
    WIN32_FIND_DATAW find_data;
    const HANDLE h_find = FindFirstFileW((folder_path + L"\\*").c_str(), &find_data);
    if (h_find != INVALID_HANDLE_VALUE) {
        do {
            if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                _STD wstring file_name = find_data.cFileName;

                const size_t dot_pos = file_name.find_last_of(L'.');
                if (dot_pos != _STD wstring::npos) {
                    file_name = file_name.substr(0, dot_pos);
                }

                _STD vector<uint8_t> byteArray = ReadFileContent(folder_path + L"\\" + find_data.cFileName);
                if (!byteArray.empty()) {
                    _STD wofstream output(output_file, _STD ios_base::app);
                    if (output.is_open()) {
                        output << L"const _STD uint8_t " << file_name << L"[" << byteArray.size() << L"] = {";
                        for (size_t i = 0; i < byteArray.size(); ++i) {
                            if (i != 0)
                                output << L",";
                            output << L"0x" << _STD hex << byteArray[i];
                        }
                        output << L"};" << '\n';
                        output.close();
                    }
                }
            }
        } while (FindNextFileW(h_find, &find_data) != 0);
        FindClose(h_find);
    }
}

int main() {
	const _STD wstring folder_path = L"FolderPath";

	const _STD wstring output_file = L"output_file.h";

    ProcessFolderFiles(folder_path, output_file);

    _STD wcout << L"Process completed. Byte arrays written to file: " << output_file << '\n';

    return EXIT_SUCCESS;
}
