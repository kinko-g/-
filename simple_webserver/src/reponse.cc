#include <response.h>

HtmlResponse::HtmlResponse(const std ::string filename) : HtmlResponse() {
    std ::fstream fs{};
    std ::string root_dir = "./Www/";
    auto evn_root_dir = std::getenv("root_dir");
    if (evn_root_dir)
    {
        root_dir = evn_root_dir;
        std::cout << " root dir :" << (root_dir + filename) << "\n";
        fs.open(root_dir + filename);
        if (fs.is_open())
        {
            fs.seekg(0, fs.end);
            auto sz = fs.tellg();
            fs.seekg(0, fs.beg);
            std ::string buf(sz, 0);
            fs.read(&*buf.begin(), sz);
            setResContent(buf);
        }
        else
        {
            std ::cout << " open file fault \n";
            std ::string content = "<h1>500 Server Error </>";
            setResContent(content);
        }
    }
}

ImageResponse::ImageResponse(std::string &filename) {
    proto_.insert_content("Content-Type", "image/png");
    std::fstream fs{};
    std::string root_dir = "./www/src/";
    auto evn_root_dir = std::getenv("source_dir");
    if (evn_root_dir) {
        root_dir = evn_root_dir;
        fs.open(root_dir + filename, std::ios::binary | std::ios::in);
        if (fs.is_open()) {
            fs.seekg(0, fs.end);
            auto sz = fs.tellg();
            fs.seekg(0, fs.beg);
            std ::string buf(sz, 0);
            fs.read(&*buf.begin(), sz);
            setResContent(buf);
        }
        else {
            std ::cout << " open file fault \n";
            std ::string content = "";
            setResContent(content);
        }
    }
}