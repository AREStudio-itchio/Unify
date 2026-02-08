# Unify
**Unify es un motor que usa un gestionador de proyectos para facilitar la creación de videojuegos usando Tiny2D**, pero no de la forma habitual, Unify permite al usuario programar en C#, para luego ser convertido a código C++, y luego con G++, ser compilado a código de máquina (EXE).

# Requerimientos
Unify requiere tener instalado G++ desde MSYS2 (MinGW), y añadir el bin de G++ en el PATH del sistema.
Añadir la carpeta del proyecto Unify en el PATH del sistema.

# Consejo
Como usamos un compilador para traducirlo todo a C++, si algo no funciona bien porque el compilador no conoce esa funcionalidad, puedes escribirlo tal cual como en C++, porque las funciones que no son detectadas como C#, son copiadas tal cual, lo que significa que puedes ejecutarlo como código C++.

# Uso
Usa Unify de la siguiente manera:
```cmd
.\unify [comando]
```

# Software de terceros
Unify distribuye 2 software nuestros:
Tiny2D: librería C para el desarrollo de videojuegos.
C# To C++ Compiler: compilador C++ para compilar C# a C++ sin depender del compilador de dotnet (.NET).
**No te preocupes por su licencia, están incluidas en thirdparty**, menos Tiny2D, que dentro del comentario del inicio ya se mantiene la licencia.

# Licencia
Unify está licenciado bajo MIT, **pero esto no se aplica a tu EXE final, ni tampoco Tiny2D, ni C# To C++ compiler, ni tampoco G++**. Tiny2D y G++ tienen excepciones, Tiny2D por su licencia y G++ por su GPL con excepción, y C# To C++ compiler solo genera código, y Unify solo es el gestor de proyectos.
