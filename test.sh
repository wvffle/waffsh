#!/home/waff/workspace/waffsh/waffsh
ls -1 | grep -E '.[ch]$'|xargs wc -l
sleep 10& this should be ignored
echo test > /tmp/test
cat /tmp/test
# some comment
sleep 1 #inline comment
echo 2 >> /tmp/test
cat /tmp/test

non_existant_command 3
sleep 1
echo test 3 > /tmp/test
cat /tmp/test
 # comment with space