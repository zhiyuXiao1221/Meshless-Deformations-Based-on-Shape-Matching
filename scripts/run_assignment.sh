set -e

if [ -z "$1" ]
then
	echo >&2 "No assignment given.";
	exit 1;
fi

tes=$2

if [ -z "$tes" ]
then
	tes=1;
fi

assignment=$1
assignment_number=${assignment: -1}

echo "Building project $assignment"

if [ "$assignment" != "tech_proj" ] && [ "$assignment" != "dirt_proj" ]
then
	cmake --build ./build --config Release --target $assignment_name

	cd ./assignments/$assignment
	./../../build/assignments/$assignment/assignment$assignment_number $tes
else
	cmake --build ./build --config Release --target $assignment

	cd ./assignments/$assignment
	./../../build/assignments/$assignment/$assignment $tes
fi

cd ../..