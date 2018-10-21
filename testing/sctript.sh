#abrir un com para ver las respuestas
while true; do
   for file in ./*.bin; do
      echo -n "file="
      echo $file
      sleep 1
      for i in {0..1000}; do
         cat $file > /dev/ttyUSB1;
#         sleep 0.01;
      done;
   done;
done;
