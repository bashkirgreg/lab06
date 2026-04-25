## Laboratory work V

Данная лабораторная работа посвещена изучению фреймворков для тестирования на примере GTest

## Tutorial

Готовим окружение виртуальной машины к работе:
```sh
$ export GITHUB_USERNAME=<имя_пользователя>
$ alias gsed=sed # for *-nix system
$ cd ${GITHUB_USERNAME}/workspace
$ pushd .
$ source scripts/activate
```

Клонируем данные из прошлой работы:
```sh
$ git clone https://github.com/${GITHUB_USERNAME}/lab04 projects/lab05
$ cd projects/lab05
$ git remote remove origin
$ git remote add origin https://github.com/${GITHUB_USERNAME}/lab05
```

Добавляем фреймворк `Google Test (gtest)` в проект как подмодуль `Git`:
```sh
$ mkdir third-party
$ git submodule add https://github.com/google/googletest third-party/gtest
$ cd third-party/gtest && git checkout release-1.8.1 && cd ../..
$ git add third-party/gtest
$ git commit -m"added gtest framework"
```
После второй команды получаем:
```sh
Cloning into '/home/user1/bashkirgreg/workspace/projects/lab05/third-party/gtest'...
remote: Enumerating objects: 28627, done.
remote: Counting objects: 100% (61/61), done.
remote: Compressing objects: 100% (46/46), done.
remote: Total 28627 (delta 32), reused 15 (delta 15), pack-reused 28566 (from 2)
Receiving objects: 100% (28627/28627), 13.74 MiB | 5.26 MiB/s, done.
Resolving deltas: 100% (21273/21273), done.
```
А после третьей:
```sh
Note: switching to 'release-1.8.1'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by switching back to a branch.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -c with the switch command. Example:

  git switch -c <new-branch-name>

Or undo this operation with:

  git switch -

Turn off this advice by setting config variable advice.detachedHead to false

HEAD is now at 2fe3bd99 Merge pull request #1433 from dsacre/fix-clang-warnings
```
И наконец после пятой:
```sh
[main 73db13a] added gtest framework
 2 files changed, 4 insertions(+)
 create mode 100644 .gitmodules
 create mode 160000 third-party/gtest
```

Добавляем настройки в `CMakeLists.txt` для тестирования:
```sh
$ gsed -i '/option(BUILD_EXAMPLES "Build examples" OFF)/a\
option(BUILD_TESTS "Build tests" OFF)
' CMakeLists.txt
$ cat >> CMakeLists.txt <<EOF

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)
  
  target_compile_options(gtest PRIVATE -Wno-error)
  target_compile_options(gtest_main PRIVATE -Wno-error)
  target_compile_options(gmock PRIVATE -Wno-error)
  
  add_executable(check tests/test1.cpp sources/print.cpp)
  target_include_directories(check PRIVATE include)
  target_link_libraries(check gtest_main)
  add_test(NAME check COMMAND check)
endif()
EOF
```

Прописываем код для теста:
```sh
$ mkdir tests
$ cat > tests/test1.cpp <<EOF
#include <print.hpp>
#include <gtest/gtest.h>
#include <fstream>
#include <string>

TEST(Print, InFileStream)
{
  std::string filepath = "file.txt";
  std::string text = "hello";
  std::ofstream out{filepath};

  print(text, out);
  out.close();

  std::string result;
  std::ifstream in{filepath};
  in >> result;

  EXPECT_EQ(result, text);
}
EOF
```

Производим сборку проекта, а затем запускаем тест:
```sh
$ cmake -H. -B_build -DBUILD_TESTS=ON
$ cmake --build _build
$ cmake --build _build --target test
```
После первой команды выдало:
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
CMake Deprecation Warning at third-party/gtest/CMakeLists.txt:1 (cmake_minimum_required):
  Compatibility with CMake < 3.5 will be removed from a future version of
  CMake.

  Update the VERSION argument <min> value or use a ...<max> suffix to tell
  CMake that the project does not need compatibility with older versions.


CMake Deprecation Warning at third-party/gtest/googlemock/CMakeLists.txt:42 (cmake_minimum_required):
  Compatibility with CMake < 3.5 will be removed from a future version of
  CMake.

  Update the VERSION argument <min> value or use a ...<max> suffix to tell
  CMake that the project does not need compatibility with older versions.


CMake Deprecation Warning at third-party/gtest/googletest/CMakeLists.txt:49 (cmake_minimum_required):
  Compatibility with CMake < 3.5 will be removed from a future version of
  CMake.

  Update the VERSION argument <min> value or use a ...<max> suffix to tell
  CMake that the project does not need compatibility with older versions.


CMake Warning (dev) at third-party/gtest/googletest/cmake/internal_utils.cmake:239 (find_package):
  Policy CMP0148 is not set: The FindPythonInterp and FindPythonLibs modules
  are removed.  Run "cmake --help-policy CMP0148" for policy details.  Use
  the cmake_policy command to set the policy and suppress this warning.

Call Stack (most recent call first):
  third-party/gtest/googletest/CMakeLists.txt:84 (include)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Found PythonInterp: /usr/bin/python3 (found version "3.12.3") 
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE  
-- Configuring done (0.7s)
-- Generating done (0.0s)
-- Build files have been written to: /home/user1/bashkirgreg/workspace/projects/lab05/_build
```
После второй команды:
```sh
[  9%] Linking CXX static library libgtest.a
[  9%] Built target gtest
[ 14%] Building CXX object third-party/gtest/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
[ 19%] Linking CXX static library libgtest_main.a
[ 19%] Built target gtest_main
[ 23%] Building CXX object CMakeFiles/check.dir/tests/test1.cpp.o
[ 28%] Building CXX object CMakeFiles/check.dir/sources/print.cpp.o
[ 33%] Linking CXX executable check
[ 33%] Built target check
[ 38%] Building CXX object formatter_lib/CMakeFiles/formatter.dir/formatter.cpp.o
[ 42%] Linking CXX static library libformatter.a
[ 42%] Built target formatter
[ 47%] Building CXX object formatter_ex_lib/CMakeFiles/formatter_ex.dir/formatter_ex.cpp.o
[ 52%] Linking CXX static library libformatter_ex.a
[ 52%] Built target formatter_ex
[ 57%] Building CXX object hello_world/CMakeFiles/hello_world.dir/hello_world.cpp.o
[ 61%] Linking CXX executable hello_world
[ 61%] Built target hello_world
[ 66%] Building CXX object solver/solver_lib/CMakeFiles/solver_lib.dir/solver.cpp.o
[ 71%] Linking CXX static library libsolver_lib.a
[ 71%] Built target solver_lib
[ 76%] Building CXX object solver/CMakeFiles/solver.dir/equation.cpp.o
[ 80%] Linking CXX executable solver
[ 80%] Built target solver
[ 85%] Building CXX object third-party/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o
[ 90%] Linking CXX static library libgmock.a
[ 90%] Built target gmock
[ 95%] Building CXX object third-party/gtest/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
[100%] Linking CXX static library libgmock_main.a
[100%] Built target gmock_main
```
И наконец после третьей команды:
```sh
Running tests...
Test project /home/user1/bashkirgreg/workspace/projects/lab05/_build
    Start 1: check
1/1 Test #1: check ............................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

Запускаем тест напрямую для более детального вывода:
```sh
$ _build/check
$ cmake --build _build --target test -- ARGS=--verbose
```
После первой команды выдало:
```sh
Running main() from /home/user1/bashkirgreg/workspace/projects/lab05/third-party/gtest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test case.
[----------] Global test environment set-up.
[----------] 1 test from Print
[ RUN      ] Print.InFileStream
[       OK ] Print.InFileStream (0 ms)
[----------] 1 test from Print (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test case ran. (0 ms total)
[  PASSED  ] 1 test.
```
После второй:
```sh
Running tests...
UpdateCTestConfiguration  from :/home/user1/bashkirgreg/workspace/projects/lab05/_build/DartConfiguration.tcl
UpdateCTestConfiguration  from :/home/user1/bashkirgreg/workspace/projects/lab05/_build/DartConfiguration.tcl
Test project /home/user1/bashkirgreg/workspace/projects/lab05/_build
Constructing a list of tests
Done constructing a list of tests
Updating test list for fixtures
Added 0 tests to meet fixture requirements
Checking test dependency graph...
Checking test dependency graph end
test 1
    Start 1: check

1: Test command: /home/user1/bashkirgreg/workspace/projects/lab05/_build/check
1: Working Directory: /home/user1/bashkirgreg/workspace/projects/lab05/_build
1: Test timeout computed to be: 10000000
1: Running main() from /home/user1/bashkirgreg/workspace/projects/lab05/third-party/gtest/googletest/src/gtest_main.cc
1: [==========] Running 1 test from 1 test case.
1: [----------] Global test environment set-up.
1: [----------] 1 test from Print
1: [ RUN      ] Print.InFileStream
1: [       OK ] Print.InFileStream (0 ms)
1: [----------] 1 test from Print (0 ms total)
1: 
1: [----------] Global test environment tear-down
1: [==========] 1 test from 1 test case ran. (1 ms total)
1: [  PASSED  ] 1 test.
1/1 Test #1: check ............................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 seс
```

Начинаем настройку необходимых файлов для будущей работы с `GitHub Actions`. Для начала переписываем наш существующий `CMakeLists.txt` через `nano`, заполняя его такой информацией:
```sh
cmake_minimum_required(VERSION 3.10)
project(TestLab03)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)

add_library(print STATIC sources/print.cpp)
target_include_directories(print PUBLIC include)

if(BUILD_TESTS)
  enable_testing()
  
  include(FetchContent)
  FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.14.0)
  FetchContent_MakeAvailable(googletest)
  
  add_executable(check tests/test1.cpp)
  target_link_libraries(check PRIVATE print gtest_main)
  target_include_directories(check PRIVATE include)
  add_test(NAME check COMMAND check)
endif()
```
Потом добавляем директорию `mkdir -p .github/workflows` и через `cat > .github/workflows/ci.yml <<'EOF'` наполянем нужный файл для работы с `GitHub Actions`:
```sh
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Configure
      run: cmake -B build -DBUILD_TESTS=ON
    
    - name: Build
      run: cmake --build build
    
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
EOF
```
Теперь хочется проверить работоспособность этих файлов. Для начала `rm -rf build`, потом `cmake -B build -DBUILD_TESTS=ON`, которая выдаёт:
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
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE  
-- Configuring done (6.3s)
-- Generating done (0.0s)
-- Build files have been written to: /home/user1/bashkirgreg/workspace/projects/lab05/build
```
Вскоре делаем `cmake --build build`, получая:
```sh
[  8%] Building CXX object CMakeFiles/print.dir/sources/print.cpp.o
[ 16%] Linking CXX static library libprint.a
[ 16%] Built target print
[ 25%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
[ 33%] Linking CXX static library ../../../lib/libgtest.a
[ 33%] Built target gtest
[ 41%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
[ 50%] Linking CXX static library ../../../lib/libgtest_main.a
[ 50%] Built target gtest_main
[ 58%] Building CXX object CMakeFiles/check.dir/tests/test1.cpp.o
[ 66%] Linking CXX executable check
[ 66%] Built target check
[ 75%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o
[ 83%] Linking CXX static library ../../../lib/libgmock.a
[ 83%] Built target gmock
[ 91%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
[100%] Linking CXX static library ../../../lib/libgmock_main.a
[100%] Built target gmock_main
```
И наконец проверяем всё через `cd build && ctest --output-on-failure && cd ..`:
```sh
Test project /home/user1/bashkirgreg/workspace/projects/lab05/build
    Start 1: check
1/1 Test #1: check ............................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```
Для красоты и удобства удаляем лишние файлы `rm -rf _build third-party formatter_lib formatter_ex_lib hello_world solver examples`. Обязательно создаём `cat > .gitignore <<EOF`, чтобы временные файлы и папки сборки не попадали в репозиторий:
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
EOF
```
Добавляем все изменения в Git и отправляем на удалённый репозиторий:
```sh
git add .
git commit -m "Add new files"
git push origin main
```
После пуша переходим на страницу `GitHub Actions`, наблюдая чудную картину из выполненной работы. 
> После бесчиленных проб и ошибок я отказался от использования `.gitmodules` и `third-party` в пользу `FetchContent`, с которым я слегка был знаком после работы с `CMakeLists.txt`, поскольку он автоматически скачивает актуальную версию `gtest (v1.14.0)` во время конфигурации `CMakeLists.txt`и решает множество других проблем. Чтобы не возникало ошибок, я решил на всякий случай удалить `third-party`, оставив `.gitmodules` для подтверждения своей работы с командами из туториала. 

Создаём снимок экрана:
```sh
$ mkdir artifacts
$ sleep 20s && gnome-screenshot --file artifacts/screenshot.png
```
Он лежит в папке `artifacts`, и сразу хочу сказать, что `GitHub` у меня запущен на хостовой машине, а на виртуальной машине я работаю только с терминалом, поэтому и получилась фотография терминала. 
