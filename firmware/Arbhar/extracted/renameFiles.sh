DIR=$1/

#FILES=$(find $DIR -type f -not -name '*.txt' -not -name '.*')
FILES=$(find $DIR -type f -not -name '.*')

for FILE in $FILES
do	
	echo "$FILE"
	name="${FILE##*/}"
	echo "$name"
	echo ""$DIR"z"$name""
	sudo mv "$FILE" $DIR"z""$name"
	sync
done
