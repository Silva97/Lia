#!/usr/bin/env bash

make test name=lexer || exit
make test name=parser || exit
make test name=compiler || exit
make test name=cmd || exit
make test name=procedure || exit
make test name=macros || exit
bash tests/test_modules.sh || exit

echo "* Test finished without errors *"
exit 0