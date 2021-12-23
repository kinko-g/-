#include <iostream>
#include "family_financial_manager.h"
#include "saving.h"

int main() {

    ffms::family_financial_manager service{};
    service.loop();
    return 0;
}