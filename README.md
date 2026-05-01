## Laboratory work VI

Данная лабораторная работа посвещена изучению средств пакетирования на примере **CPack**

```sh
$ open https://cmake.org/Wiki/CMake:CPackPackageGenerators
```

## Tutorial

Настраиваем окружение для дальнейшей работы:
```sh
$ export GITHUB_USERNAME=<имя_пользователя>
$ export GITHUB_EMAIL=<адрес_почтового_ящика>
$ alias edit=<nano|vi|vim|subl>
$ alias gsed=sed # for *-nix system
```

Перемещаемся в рабочую директорию:
```sh
$ cd ${GITHUB_USERNAME}/workspace
$ pushd .
$ source scripts/activate
```

Клонируем прошлую работу для начало новой работы:
```sh
$ git clone https://github.com/${GITHUB_USERNAME}/lab05 projects/lab06
$ cd projects/lab06
$ git remote remove origin
$ git remote add origin https://github.com/${GITHUB_USERNAME}/lab06
```

С помощью команд `gsed` добавляем в `CMakeLists.txt` после строки `project(print)` переменные версии: `MAJOR=0`, `MINOR=1`, `PATCH=0`, `TWEAK=0`, составную `PRINT_VERSION` и строковую `PRINT_VERSION_STRING="v${PRINT_VERSION}"`. Из-за последовательной вставки переменные располагаются в файле в обратном порядке. С помощью команды `git diff` получаем все изменения:
```sh
$ gsed -i '/project(print)/a\
set(PRINT_VERSION_STRING "v\${PRINT_VERSION}")
' CMakeLists.txt
$ gsed -i '/project(print)/a\
set(PRINT_VERSION\
  \${PRINT_VERSION_MAJOR}.\${PRINT_VERSION_MINOR}.\${PRINT_VERSION_PATCH}.\${PRINT_VERSION_TWEAK})
' CMakeLists.txt
$ gsed -i '/project(print)/a\
set(PRINT_VERSION_TWEAK 0)
' CMakeLists.txt
$ gsed -i '/project(print)/a\
set(PRINT_VERSION_PATCH 0)
' CMakeLists.txt
$ gsed -i '/project(print)/a\
set(PRINT_VERSION_MINOR 1)
' CMakeLists.txt
$ gsed -i '/project(print)/a\
set(PRINT_VERSION_MAJOR 0)
' CMakeLists.txt
$ git diff
```
Наши изменения:
```sh
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 164d4b4..27b18e3 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -1,5 +1,12 @@
 cmake_minimum_required(VERSION 3.10)
-project(TestLab03)
+project(print)
+set(PRINT_VERSION_MAJOR 0)
+set(PRINT_VERSION_MINOR 1)
+set(PRINT_VERSION_PATCH 0)
+set(PRINT_VERSION_TWEAK 0)
+set(PRINT_VERSION
+  ${PRINT_VERSION_MAJOR}.${PRINT_VERSION_MINOR}.${PRINT_VERSION_PATCH}.${PRINT_VERSION_TWEAK})
+set(PRINT_VERSION_STRING "v${PRINT_VERSION}")
 
 set(CMAKE_CXX_STANDARD 11)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Теперь создаём файлы `DESCRIPTION` и `ChangeLog.md` с записью о релизе `0.1.0.0`, используя текущую дату и наши данные,чтобы можно было собрать `RPM` пакет:
```sh
$ touch DESCRIPTION && edit DESCRIPTION
$ touch ChangeLog.md
$ export DATE="`LANG=en_US date +'%a %b %d %Y'`"
$ cat > ChangeLog.md <<EOF
* ${DATE} ${GITHUB_USERNAME} <${GITHUB_EMAIL}> 0.1.0.0
- Initial RPM release
EOF
```

Начинаем создавать конфигурацию для `CPack`:
```sh
$ cat > CPackConfig.cmake <<EOF
include(InstallRequiredSystemLibraries)
EOF
```
```sh
$ cat >> CPackConfig.cmake <<EOF
set(CPACK_PACKAGE_CONTACT ${GITHUB_EMAIL})
set(CPACK_PACKAGE_VERSION_MAJOR \${PRINT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR \${PRINT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH \${PRINT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK \${PRINT_VERSION_TWEAK})
set(CPACK_PACKAGE_VERSION \${PRINT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_FILE \${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "static C++ library for printing")
EOF
```
```sh
$ cat >> CPackConfig.cmake <<EOF

set(CPACK_RESOURCE_FILE_LICENSE \${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README \${CMAKE_CURRENT_SOURCE_DIR}/README.md)
EOF
```
```sh
$ cat >> CPackConfig.cmake <<EOF

set(CPACK_RPM_PACKAGE_NAME "print-devel")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "print")
set(CPACK_RPM_CHANGELOG_FILE \${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md)
set(CPACK_RPM_PACKAGE_RELEASE 1)
EOF
```
```sh
$ cat >> CPackConfig.cmake <<EOF

set(CPACK_DEBIAN_PACKAGE_NAME "libprint-dev")
set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "cmake >= 3.0")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
EOF
```
```sh
$ cat >> CPackConfig.cmake <<EOF

include(CPack)
EOF
```
```sh
$ cat >> CMakeLists.txt <<EOF

include(CPackConfig.cmake)
EOF
```

Обновляем ссылки и упоминания старой лабораторной работы на новую:
```sh
$ gsed -i 's/lab05/lab06/g' README.md
```

Отправляем все данные на удалённый репозиторий, ставя нужную метку версии:
```sh
$ git add .
$ git commit -m"added cpack config"
$ git tag v0.1.0.0
git push origin main --tags
```

Создаём файл для `GitHyb Actions`, а потом отправляем его на удалённый репозиторий через стандартные команды `Git`. Также незабываем создать файл `LICENSE`, который также отправляем на удалённый репозиторий:
```sh
name: CI

on:
  push:
    branches: [ master, main ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ master, main ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
      with:
        fetch-depth: 0
    
    - name: Configure CMake
      run: cmake -H. -B_build -DCPACK_GENERATOR="TGZ"
    
    - name: Build
      run: cmake --build _build
    
    - name: Package
      run: |
        cd _build
        cpack -G "TGZ"
        cd ..
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v4
      with:
        name: packages
        path: _build/*.tar.gz
```

Осуществляем сборку и создаём архив-пакет:
```sh
$ cmake -H. -B_build
$ cmake --build _build
$ cd _build
$ cpack -G "TGZ"
$ cd ..
```
После первой команды получаем:
```sh
-- Configuring done (0.0s)
-- Generating done (0.0s)
-- Build files have been written to: /home/user1/bashkirgreg/workspace/projects/lab06/_build
```
После второй команды:
```sh
[ 50%] Building CXX object CMakeFiles/print.dir/sources/print.cpp.o
[100%] Linking CXX static library libprint.a
[100%] Built target print
```
После четвёртой команды:
```sh
CPack: Create package using TGZ
CPack: Install projects
CPack: - Run preinstall target for: print
CPack: - Install project: print []
CPack: Create package
CPack: - package: /home/user1/bashkirgreg/workspace/projects/lab06/_build/print-0.1.0.0-Linux.tar.gz generated.
```

Также осуществляем сборку и создаём архив-пакет, но короче и быстрее:
```sh
$ cmake -H. -B_build -DCPACK_GENERATOR="TGZ"
$ cmake --build _build --target package
```
После первой команды:
```sh
-- Configuring done (0.0s)
-- Generating done (0.0s)
-- Build files have been written to: /home/user1/bashkirgreg/workspace/projects/lab06/_build
```
После второй команды:
```sh
[100%] Built target print
Run CPack packaging tool...
CPack: Create package using TGZ
CPack: Install projects
CPack: - Run preinstall target for: print
CPack: - Install project: print []
CPack: Create package
CPack: - package: /home/user1/bashkirgreg/workspace/projects/lab06/_build/print-0.1.0.0-Linux.tar.gz generated.
```

Перемещаем созданный архив в "новую" папку:
```sh
$ mkdir artifacts
$ mv _build/*.tar.gz artifacts
$ tree artifacts
```
После третьей команды:
```sh
artifacts
├── print-0.1.0.0-Linux.tar.gz
└── screenshot.png

1 directory, 2 files
```
> Данная папка была создана ещё в туториале прошлой лабораторной работы, поэтому она уже была в директории `lab06`, храня в себе фотографию пятой лабораторной работы.

Проверяем вкладку `GitHub Actions`, в которой, к счастью, работа успешно выполнилась.


## Homework

После того, как вы настроили взаимодействие с системой непрерывной интеграции,</br>
обеспечив автоматическую сборку и тестирование ваших изменений, стоит задуматься</br>
о создание пакетов для измениний, которые помечаются тэгами (см. вкладку [releases](https://github.com/tp-labs/lab06/releases)).</br>
Пакет должен содержать приложение _solver_ из [предыдущего задания](https://github.com/tp-labs/lab03#задание-1)
Таким образом, каждый новый релиз будет состоять из следующих компонентов:
- архивы с файлами исходного кода (`.tar.gz`, `.zip`)
- пакеты с бинарным файлом _solver_ (`.deb`, `.rpm`, `.msi`, `.dmg`)


Для начала добавляем несколько строк в конце общего `CMakeLists.txt` , чтобы `solver` работал нормально:
```sh
add_subdirectory(formatter_lib)
add_subdirectory(formatter_ex_lib)
add_subdirectory(solver)

install(TARGETS solver RUNTIME DESTINATION bin)
install(TARGETS print ARCHIVE DESTINATION lib)

include(CPackConfig.cmake)
```

Потом полностью переделываем `CPackConfig.cmake`:
```sh
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_CONTACT "${GITHUB_EMAIL}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${GITHUB_USERNAME} <${GITHUB_EMAIL}>")

set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_VERSION_TWEAK 0)
set(CPACK_PACKAGE_VERSION "0.1.0.0")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Solver app")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_NAME "solver")

set(CPACK_DEBIAN_PACKAGE_NAME "solver")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)

set(CPACK_RPM_PACKAGE_NAME "solver")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_RELEASE 1)

include(CPack)
```

Переписываем файл `DESCRIPTION`:
```sh
Solver application for mathematical problems.
```

Дополняем `.gitignore`:
```sh
build/
_build/
file.txt
*.o
*.a
*.so
*.exe
.deps/
*.swp

artifacts/
*.tar.gz
*.deb
*.rpm
*.zip
*.msi
*.dmg
.vscode/
.idea/
*.swp
*.swo
*~
.DS_Store
Thumbs.db
```

Теперь нужно обновить `.github/workflows/ci.yml`:
```sh
name: CI

on:
  push:
    branches: [ master, main ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ master, main ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
      with:
        fetch-depth: 0
    
    - name: Configure CMake
      run: cmake -H. -B_build -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: cmake --build _build
    
    - name: Package
      run: |
        cd _build
        cpack -G "TGZ"
        cpack -G "DEB"
        cd ..
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v4
      with:
        name: packages
        path: |
          _build/*.tar.gz
          _build/*.deb
```

Отправляем все наши видоизменённые файлы в репозиторий на `GitHub`:
```sh
git add .
git commit -m "Add files from HomeWork"
git push origin main
```

Создаём и отправляем тег для релиза
```sh
git tag v1.0.0.0 -m "Release v1.0.0.0"
git push origin main --tags
```

Теперь очищаем `rm -rf _build artifacts` и осуществляем сборку вместе с созданием архивов. 

Сначала `cmake -H. -B_build -DCMAKE_BUILD_TYPE=Release`, которая выдаёт:
```sh
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.5s)
-- Generating done (0.0s)
-- Build files have been written to: /home/user1/bashkirgreg/workspace/projects/lab06/_build
```

Потом прописываем `cmake --build _build`, получая:
```sh
[ 10%] Building CXX object CMakeFiles/print.dir/sources/print.cpp.o
[ 20%] Linking CXX static library libprint.a
[ 20%] Built target print
[ 30%] Building CXX object formatter_lib/CMakeFiles/formatter.dir/formatter.cpp.o
[ 40%] Linking CXX static library libformatter.a
[ 40%] Built target formatter
[ 50%] Building CXX object formatter_ex_lib/CMakeFiles/formatter_ex.dir/formatter_ex.cpp.o
[ 60%] Linking CXX static library libformatter_ex.a
[ 60%] Built target formatter_ex
[ 70%] Building CXX object solver/solver_lib/CMakeFiles/solver_lib.dir/solver.cpp.o
[ 80%] Linking CXX static library libsolver_lib.a
[ 80%] Built target solver_lib
[ 90%] Building CXX object solver/CMakeFiles/solver.dir/equation.cpp.o
[100%] Linking CXX executable solver
[100%] Built target solver
```

Перемещаемся `cd _build` и пишем `cpack -G "TGZ"`:
```sh
CPack: Create package using TGZ
CPack: Install projects
CPack: - Run preinstall target for: print
CPack: - Install project: print []
CPack: Create package
CPack: - package: /home/user1/bashkirgreg/workspace/projects/lab06/_build/solver-0.1.0.0-Linux.tar.gz generated.
```

А после `cpack -G "DEB"`:
```sh
CPack: Create package using DEB
CPack: Install projects
CPack: - Run preinstall target for: print
CPack: - Install project: print []
CPack: Create package
CPack: - package: /home/user1/bashkirgreg/workspace/projects/lab06/_build/solver-0.1.0.0-Linux.deb generated.
```

Создаём `mkdir artifacts`, поскольку ранее мы её удалили, а затем перемещаем туда наши архивы через `mv _build/*.tar.gz _build/*.deb artifacts/ 2>/dev/null`. Проверяем содержимое через `tree artifacts`:
```sh
artifacts
├── solver-0.1.0.0-Linux.deb
└── solver-0.1.0.0-Linux.tar.gz

1 directory, 2 files
```

Теперь публикуем `Release` по нашему тэгу, загружая файлы из папки `artifacts`. Наконец проверяем `GitHub Actions` на корректность работы, по пути исправляя появившиеся ошибки.

>Для красоты решил удалить лишнюю папку `artifacts` с ненужной для этой работы фотографией. Другие файлы уже побоялся удалять...
