echo "Building course_os..." 

# Or if there's one argument, then set "hello.c"s contents equal to the test given
if (( $# == 1 )); then
	if [ -f "user/tests/${1}.c" ]; then
		echo "Going to execute user/tests/${1}.c"
		cp "user/tests/${1}.c" "user/hello/hello.c"
		make clean
		make
		make run
	else
		echo "Test ${1}.c does not exist. Exiting."
		exit 1
	fi

# Not one argument? Default to hello.c 
else
	echo "No test case given. Defaulting to the original hello.c"
	cp "user/hello/original_hello.c" "user/hello/hello.c"
	make clean
	make
	make run

fi
