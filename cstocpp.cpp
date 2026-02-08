// cstocpp.cpp
// Transpilador simple C# -> C++ para el flujo tiny2d/unify
// - using ns.sub -> #include "ns/sub.h" (comillas, librería local)
// - static int/void Main() -> int WINAPI WinMain(...)
// - cuenta llaves y cierra bloques abiertos al final si hace falta
// - incluye únicos y ordenados

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <set>
#include <algorithm>

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Uso: cstocpp <input.cs> <output.cpp>\n";
        return 1;
    }

    std::ifstream fin(argv[1]);
    if (!fin.is_open()) {
        std::cerr << "No se pudo abrir: " << argv[1] << "\n";
        return 1;
    }

    std::ofstream fout(argv[2]);
    if (!fout.is_open()) {
        std::cerr << "No se pudo crear: " << argv[2] << "\n";
        return 1;
    }

    std::string line;
    std::vector<std::string> body;
    std::set<std::string> includeSet;
    bool inMain = false;
    int mainBraceCount = 0;
    bool insideClass = false;
    int classBraceCount = 0;

    // Regexes
    std::regex usingRx(R"(^\s*using\s+([A-Za-z0-9_\.]+)\s*;)");
    std::regex mainRx(R"(^\s*(?:static\s+)?(void|int)\s+Main\s*\()");
    std::regex classRx(R"(^\s*class\s+([A-Za-z0-9_]+)\s*(\{|$))");

    while (std::getline(fin, line)) {
        std::string t = trim(line);

        // 1) using -> include (local library with quotes)
        std::smatch m;
        if (std::regex_search(t, m, usingRx)) {
            std::string name = m[1].str();
            std::replace(name.begin(), name.end(), '.', '/');
            std::string inc = "#include \"" + name + ".h\"";
            includeSet.insert(inc);
            continue; // no escribir la línea original
        }

        // 2) Detectar inicio de Main (static int/void Main)
        if (std::regex_search(t, m, mainRx)) {
            inMain = true;
            mainBraceCount = 0;
            body.push_back("int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {");
            continue;
        }
        // 3) Detectar inicio de clase
        else if (std::regex_search(t, m, classRx)) {
            insideClass = true;
            classBraceCount = 0;
        }

        // 4) Contar llaves en la línea para main y clase
        for (char c : t) {
            if (inMain) {
                if (c == '{') mainBraceCount++;
                else if (c == '}') mainBraceCount--;
            }
            if (insideClass) {
                if (c == '{') classBraceCount++;
                else if (c == '}') classBraceCount--;
            }
        }

        // 5) Si estamos dentro de main, procesar su contenido
        if (inMain) {
            if (t == "{") continue;
            if (t == "}" && mainBraceCount <= 0) {
                body.push_back("}");
                inMain = false;
                mainBraceCount = 0;
                continue;
            }
            body.push_back(line);
            continue;
        }

        // 6) Si estamos dentro de clase, omitimos la declaración de clase y su cuerpo
        if (insideClass) {
            if (t == "}" && classBraceCount <= 0) {
                insideClass = false;
                classBraceCount = 0;
                continue;
            }
            continue;
        }

        // 7) Fuera de main y fuera de clase: añadir la línea tal cual
        body.push_back(line);
    }

    // Cierre de main si quedó abierto
    if (inMain) {
        while (mainBraceCount > 0) {
            body.push_back("}");
            mainBraceCount--;
        }
        body.push_back("}");
        inMain = false;
    }

    // Cierre silencioso de clase si quedó abierta
    if (insideClass) {
        insideClass = false;
    }

    // Escribir includes (ordenados y únicos)
    std::vector<std::string> includes(includeSet.begin(), includeSet.end());
    std::sort(includes.begin(), includes.end());
    for (const auto& inc : includes) fout << inc << "\n";
    if (!includes.empty()) fout << "\n";

    // Escribir cuerpo, evitando líneas vacías duplicadas
    bool lastEmpty = false;
    for (auto& l : body) {
        std::string t = trim(l);
        if (t.empty()) {
            if (!lastEmpty) {
                fout << "\n";
                lastEmpty = true;
            }
        } else {
            fout << l << "\n";
            lastEmpty = false;
        }
    }

    fout.close();
    fin.close();

    return 0;
}