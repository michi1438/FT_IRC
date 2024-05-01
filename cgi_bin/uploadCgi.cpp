
int main(int argc, char **argv) {
    // Lire req_str du fichier temporaire
    std::string req_str;
    std::ifstream temp_file(argv[1]);
    if (temp_file.is_open()) {
        getline(temp_file, req_str);
        temp_file.close();
    } else {
        std::cerr << "Unable to open file";
        return 1;
    }

    // Convertir req_str en Req
    RequestParser Req = RequestParser::fromString(req_str);

    // Appeler handleFileUpload avec Req
    handleFileUpload(Req);

    return 0;
}