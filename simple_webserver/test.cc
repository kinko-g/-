#include "service.h"
#include "response.h"
#include "http.h"

int main() {

    AppService app{};
    std::cout << app.contains_pattern("/image/1") << "\n";
    std::cout << app.contains_pattern("/home") << "\n";
    return 0;
}