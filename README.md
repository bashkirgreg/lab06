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

Перемещаемся в нашу рабочую директорию:
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

Теперь создаём файлы `DESCRIPTION` и `ChangeLog.md` с записью о релизе `0.1.0.0`, используя текущую дату и наши данные, чтобы можно было собрать `RPM` пакет:
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

Создаём файл для `GitHub Actions`, а потом отправляем его на удалённый репозиторий через стандартные команды `Git`. Также незабываем создать файл `LICENSE`, который также отправляем на удалённый репозиторий:
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

set(CPACK_PACKAGE_CONTACT "student@example.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "student <student@example.com>")

set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_VERSION_TWEAK 0)
set(CPACK_PACKAGE_VERSION "0.1.0.0")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "solver library")

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

set(CPACK_RPM_PACKAGE_NAME "solver")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "solver")
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")

set(CPACK_DEBIAN_PACKAGE_NAME "solver")
set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "cmake >= 3.0")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)

set(CPACK_PACKAGE_NAME "solver")

if(WIN32)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_ARCHITECTURE "x64")
    set(CPACK_WIX_PRODUCT_GUID "3a7b8c2d-1e4f-5a6b-7c8d-9e0f1a2b3c4d")
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/LICENSE.rtf")
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "solver")
    set(CPACK_WIX_VERSION "4")
    set(CPACK_WIX_PROPERTY_ARPCONTACT "student@example.com")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "solver")
    set(CPACK_DMG_FORMAT "UDBZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ;ZIP")
endif()

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
    tags: [ "v*" ]
  workflow_dispatch:

permissions:
  contents: write

jobs:
  create-binary-packages:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]

    steps:
      - name: Get repo files
        uses: actions/checkout@v4
      
      - name: Install dependencies (Ubuntu)
        if: matrix.os == 'ubuntu-latest'
        run: |
          sudo apt-get update
          sudo apt-get install -y rpm
      
      - name: Setup .NET (Windows)
        if: matrix.os == 'windows-latest'
        uses: actions/setup-dotnet@v4
        with:
          dotnet-version: '8.x'
      
      - name: Install WIX (Windows)
        if: matrix.os == 'windows-latest'
        run: |
          dotnet tool install --global wix --version 4.0.0
          wix extension add --global WixToolset.UI.wixext/4.0.4
        shell: bash
      
      - name: Configure
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
        shell: bash
      
      - name: Build
        run: cmake --build build --config Release
        shell: bash
      
      - name: Create DEB and RPM (Ubuntu)
        if: matrix.os == 'ubuntu-latest'
        run: |
          cd build
          cpack -G DEB -C Release
          cpack -G RPM -C Release
        shell: bash
      
      - name: Create DMG (macOS)
        if: matrix.os == 'macos-latest'
        run: |
          cd build
          cpack -G DragNDrop -C Release
        shell: bash
      
      - name: Create MSI (Windows)
        if: matrix.os == 'windows-latest'
        run: |
          cd build
          cpack -G WIX -C Release
        shell: bash
      
      - name: Upload to Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            build/*.deb
            build/*.rpm
            build/*.dmg
            build/*.msi
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

Создаём специальный `LICENSE.rtf` для `MSI`, исправляем другие мелкие ошибки, а потом публикуем всё на `GitHub` через стандартные команды `Git`, проверяя вкладку `GitHub Actions` и `Releases`.

>Заранее приношу извинения за столько тэгов и релизов, уж очень `MSI` решил меня порадовать...


