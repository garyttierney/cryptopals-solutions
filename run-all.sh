#!/bin/sh

color() {
	printf '\033[%sm%s\033[m\n' "$@"
}

FAILED_CHALLENGES=0
PASSED_CHALLENGES=0

for challenge in $(find . -type f -executable -name 'challenge-solution');
do
	echo -e "\n"

	color '33' "Running: $challenge"
	color '33' '-------------------'

	LD_LIBRARY_PATH=./common $challenge
	RETCODE=$?

	color '33' '-------------------'

	if [ "$RETCODE" -eq "0" ];
	then
		color '32' "Passed: $challenge"
		PASSED_CHALLENGES=$((PASSED_CHALLENGES+1))
	else
		color '31' "Failed: $challenge"
		FAILED_CHALLENGES=$((FAILED_CHALLENGES+1))
	fi
done

TOTAL_CHALLENGES=$((PASSED_CHALLENGES+FAILED_CHALLENGES))

echo "Passed $PASSED_CHALLENGES out of a total $TOTAL_CHALLENGES challenges"
