#include "Asynchronous.h"

int main() {
    std::string api_url = std::string(DOMEN) + "api/geographical_object/";

    Asynchronous asynchronous;
    asynchronous.processJsonResponseGeographicalObject(api_url);

    return 0;
}
