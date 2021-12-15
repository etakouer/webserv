#!/usr/bin/python
import sys

print 'Content-type: text/html'
print

try:
    with open('/tmp/visit', 'r') as fichier:
        nbr_visiteurs = int(fichier.read())
        print nbr_visiteurs+1,'visites \o/'
    with open('/tmp/visit', 'w') as fichier:
        fichier.write(str(nbr_visiteurs+1))
except:
    with open('/tmp/visit', 'w') as fichier:
        print 1,'visites \o/'
        fichier.write(str(1))

try:
    sys.stdout.flush()
except:
    pass
