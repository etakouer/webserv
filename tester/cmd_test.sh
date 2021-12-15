#!/bin/null

# faites vos tests a partir de goinfre svp

echo $(python -c "print('A' * 100000)") > data && echo start && curl -X POST http://localhost/directory/youpi.bla --data-binary "@data" > out

mkfifo piper
nc -vlk 8000 < piper | tee -a in.http | nc localhost 80 | tee -a out.http > piper

socat -r req.http -R res.http TCP-LISTEN:8000,fork,reuseaddr TCP:localhost:80

split -p "Test" req.http
