# ignore vcpkg and emsdk folders and format all files
find . -not \( -path ./vcpkg -prune \) -not \( -path ./emsdk -prune \) -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\|c\)' -exec clang-format -style=file -i {} \;
