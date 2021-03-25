#!/home/waff/workspace/waffsh/waffsh
# NOTE: See parser.c behaviour
#         |
#         v
sleep 10& this should be ignored

echo line count in source/header files: > test.txt
# shellcheck disable=SC2062
# shellcheck disable=SC2010
ls -1 | grep -E \.[ch]$ |xargs wc -l | sort >> test.txt
cat test.txt # this is an inline comment

# shellcheck disable=SC1012
printf \n
echo I use arch, btw. > test.txt
 cat test.txt

# shellcheck disable=SC2038
# shellcheck disable=SC2061
# shellcheck disable=SC2035
find . -iname *.txt -type f | xargs rm

echo This should fail:
it_fails