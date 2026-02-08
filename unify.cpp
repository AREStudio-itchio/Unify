#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

bool PathExists(const std::string& path) {
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES);
}

std::string GetCurrentDir() {
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    return std::string(buffer);
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (argc < 2) {
        std::cout << "Uso: .\\unify.exe [comando]\n";
        return 0;
    }

    std::string cmd = argv[1];

    // ---------------- HELP ----------------
    if (cmd == "help") {
        std::cout << "Comandos disponibles:\n";
        std::cout << "  help            Muestra esta ayuda\n";
        std::cout << "  version         Muestra la versión del proyecto\n";
        std::cout << "  new [nombre]    Crea un nuevo proyecto\n";
        std::cout << "  delete [nombre] Elimina un proyecto\n";
        std::cout << "  build           Compila el proyecto actual\n";
        std::cout << "  run             Compila y ejecuta el proyecto actual\n";
        return 0;
    }

    // ---------------- VERSION ----------------
    if (cmd == "version") {
        std::cout << "Unify Version 1.0.0\n";
        return 0;
    }

    // ---------------- NEW PROJECT ----------------
        if (cmd == "new") {

            if (argc < 3) {
                std::cerr << "Debes especificar un nombre.\n";
                return 0;
            }

            std::string name = argv[2];

            if (PathExists(name)) {
                std::cerr << "El proyecto ya existe.\n";
                return 0;
            }

            // Crear carpeta del proyecto
            CreateDirectoryA(name.c_str(), NULL);

            // Copiar tiny2d.h desde la carpeta de unify.exe
            char exePath[MAX_PATH];
            GetModuleFileNameA(NULL, exePath, MAX_PATH);
            std::string exeDir = exePath;
            size_t pos = exeDir.find_last_of("\\/");
            exeDir = exeDir.substr(0, pos);

            std::string tinySrc = exeDir + "\\tiny2d.h";
            std::string tinyDst = name + "\\tiny2d.h";

            if (!CopyFileA(tinySrc.c_str(), tinyDst.c_str(), FALSE)) {
                std::cerr << "Error: no se pudo copiar tiny2d.h desde unify.exe\n";
                return 1;
            }

            // Crear main.cs
            std::string cs =
        R"(using tiny2d;

bool tiny2D_Init() {
    return true;
}

bool tiny2D_Update(float dt) {
    DrawText2D("Hello from PROYECTO!", 20, 20, RGB(255,255,255));
    return true;
}

static int Main() {
    return tiny2D_Run(800, 600, "PROYECTO");
}
        )";

            while (cs.find("PROYECTO") != std::string::npos)
                cs.replace(cs.find("PROYECTO"), 8, name);

            std::ofstream f(name + "\\main.cs");
            f << cs;
            f.close();

            std::cout << "Proyecto '" << name << "' creado.\n";
            return 0;
        }


    // ---------------- DELETE PROJECT ----------------
    if (cmd == "delete") {

        if (argc < 3) {
            std::cerr << "Debes especificar un nombre.\n";
            return 0;
        }

        std::string name = argv[2];

        if (!PathExists(name)) {
            std::cerr << "El proyecto no existe.\n";
            return 0;
        }

        system(("rmdir /S /Q \"" + name + "\"").c_str());
        std::cout << "Proyecto '" << name << "' eliminado.\n";
        return 0;
    }

    // ---------------- BUILD ----------------
    if (cmd == "build") {

        std::string cwd = GetCurrentDir();

        size_t pos = cwd.find_last_of("\\/");
        std::string projectName = cwd.substr(pos + 1);

        // Asegurarse de trabajar desde el proyecto
        SetCurrentDirectoryA(cwd.c_str());

        std::string convertCmd = "cstocpp main.cs main.cpp";
        int result = system(convertCmd.c_str());
        if (result != 0) {
            std::cerr << "Error al convertir main.cs a main.cpp\n";
            return 1;
        }

        // Compilar
        std::string buildCmd =
            "g++ main.cpp -o \"" + projectName +
            ".exe\" -static -I. -mwindows -lwinmm -lgdi32 -lmsimg32";

        if (system(buildCmd.c_str()) != 0) {
            std::cerr << "Error al compilar el proyecto.\n";
            return 1;
        }

        std::cout << "Proyecto compilado: " << projectName << ".exe\n";
        return 0;
    }

    // ---------------- RUN ----------------
    if (cmd == "run") {

        std::string cwd = GetCurrentDir();

        size_t pos = cwd.find_last_of("\\/");
        std::string projectName = cwd.substr(pos + 1);

        SetCurrentDirectoryA(cwd.c_str());
        
        std::string convertCmd = "cstocpp main.cs main.cpp";
        int result = system(convertCmd.c_str());
        if (result != 0) {
            std::cerr << "Error al convertir main.cs a main.cpp\n";
            return 1;
        }

        std::string buildCmd =
            "g++ main.cpp -o \"" + projectName +
            ".exe\" -static -I. -mwindows -lwinmm -lgdi32 -lmsimg32";

        if (system(buildCmd.c_str()) != 0) {
            std::cerr << "Error al compilar el proyecto.\n";
            return 1;
        }

        system((projectName + ".exe").c_str());
        return 0;
    }

    return 0;
}
