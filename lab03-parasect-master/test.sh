function do_test() {
    ./main "$1" "$2"
    student=$?
    echo "$2" | grep "^$1$" > /dev/null
    ref=$?
    if [ $student -ne $ref ]; then
        echo "Failed Test: pattern=\"$1\", text=\"$2\"";
        exit 1;
    fi
}

# Write Your Tests Here
# The do_test function tests if your program gives the
# same result as the reference implementation on the pattern
# and text provided.

# Test literals
do_test 'aaa' 'aaa'
do_test 'cat' 'cat'
do_test 'givemeAonthisClass' 'givemeAonthisClass'
do_test 'Software' 'Software'
do_test 'TeStInGcAsE' 'TeStInGcAsE'

# Test dot
do_test 'a.a' 'aPa'
do_test '.................' 'triskaidekaphobia'
do_test 'TesT.C.SE' 'TesTaCASE'
do_test '..nished' 'Finished'
do_test 'Here.sareallylong.tringthatwillbe.eplacedw.thdots' 'Hereisareallylongstringthatwillbereplacedwithdots'


# Test star
do_test 'a*b*c*' 'aaaaaaaaaabbbbccccccc'
do_test 'a*bc*' 'abccc'
do_test 'a*b' 'b'
do_test 'f*b*k*' 'fbk'
do_test 'a*' 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'

# Test dot
do_test '.*.*.*' 'aaaaaaaaaabbbbccccccc'
do_test 'a*b.*' 'abccc'
do_test 'f*.*k*' 'fbk'
do_test '.*' 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
do_test '.*' 'ZZZZZZZZZZZZZZZZZZZZZ'

echo '-----------------';
echo 'All Tests Passed!';
